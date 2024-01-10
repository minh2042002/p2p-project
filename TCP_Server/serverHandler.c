#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "serverHandler.h"
#include "serverUtils.h"
#include "Client.h"

#define BUFF_SIZE 256
struct Client *clientList = NULL;

/**
 * @brief load client info from client file
 */
void loadClients()
{
    loadFromFile(&clientList);
}

/**
 * @brief Handle registration requests from clients
 * @param connfd sock_fd
 * @param client client list
 * @param command command
 * @param loginStatus status of login (to change status of login)
 */
void SignUpHandler(int connfd, struct Client **client, char *command, int *loginStatus)
{
    char buffer[BUFF_SIZE];
    char client_ip[16];
    uint16_t client_port;
    int clientListenPort;
    sscanf(command, "SU %d", &clientListenPort);
    int success = getInfoClient(connfd, client_ip, &client_port);
    write_log(&client_port, client_ip, command);

    if (success)
    {
        uint32_t id;
        while (1)
        {
            id = generateClientID();
            struct Client *current = clientList;
            int duplicate = 0;
            while (current != NULL)
            {
                if (current->id == id)
                {
                    duplicate = 1;
                    break;
                }
                current = current->next;
            }
            if (duplicate == 0)
            {
                break;
            }
        }

        struct Client *newClient = create(id, client_ip, clientListenPort);
        add(&clientList, newClient);

        *client = newClient;
        if (login(newClient) == 1)
        {
            *loginStatus = 1;
        }
        saveAll(clientList);

        sprintf(buffer, "%d - %u", 100, id);
        send(connfd, buffer, 256, 0);
    }
    else
    {
        sprintf(buffer, "%d", 300);
        send(connfd, buffer, 256, 0);
    }
}

/**
 * @brief Handle login requests from clients
 * @param connfd sock_fd
 * @param client client list
 * @param command command
 * @param loginStatus status of login
 */
void SignInHandler(int connfd, struct Client **client, char *command, int *loginStatus)
{
    char buffer[BUFF_SIZE];
    uint32_t clientID = 0;
    char client_ip[16];
    uint16_t client_port;
    int clientListenPort;
    memset(client_ip, '\0', 16);
    sscanf(command, "SI %u %d", &clientID, &clientListenPort);
    getInfoClient(connfd, client_ip, &client_port);
    printf("%u %s %s", client_port, client_ip, command);

    write_log(&client_port, client_ip, command);

    *client = find(clientList, clientID);
    if (*client != NULL)
    {
        if (login(*client) == 1)
        {
            *loginStatus = 1;
            update(*client, client_ip);
            (*client)->_addr.sin_port = clientListenPort;
            sprintf(buffer, "%d", 110);
            saveAll(clientList);
        }
        else
        {
            sprintf(buffer, "%d", 211);
        }
    }
    else
    {
        sprintf(buffer, "%d", 210);
    }
    send(connfd, buffer, 256, 0);
}

/**
 * @brief Handle register share file requests from clients
 * @param connfd sock_fd
 * @param command command
 * @param loginStatus status of login
 */
void RegisterShareHandler(int connfd, char *command, int *loginStatus)
{
    char buffer[BUFF_SIZE];
    if (loginStatus == 0)
    {
        memset(buffer, '\0', BUFF_SIZE);
        sprintf(buffer, "212");
        send(connfd, buffer, BUFF_SIZE, 0);
        return;
    }
    char client_ip[BUFF_SIZE];
    uint16_t client_port;
    getInfoClient(connfd, client_ip, &client_port);
    write_log(&client_port, client_ip, command);

    uint32_t id;
    char filename[BUFF_SIZE];
    if (sscanf(command, "SH %u %s", &id, filename) == 2)
    {
        struct Client *existClient = find(clientList, id);
        char client_ip[BUFF_SIZE];
        uint16_t client_port;

        strcpy(client_ip, inet_ntoa(existClient->_addr.sin_addr));
        client_port = existClient->_addr.sin_port;
        update(existClient, client_ip);
        saveAll(clientList);
        int success = updateIndex(id, client_ip, client_port, filename);
        if (success)
        {
            sprintf(buffer, "%d", 120);
            send(connfd, buffer, 256, 0);
        }
        else
        {
            sprintf(buffer, "%d", 300);
            send(connfd, buffer, 256, 0);
        }
    }
    else
    {
        sprintf(buffer, "%d", 300);
        send(connfd, buffer, 256, 0);
    }
}

/**
 * @brief Handle cancel register share file requests from clients
 * @param connfd sock_fd
 * @param command command
 * @param loginStatus status of login
 */
void CancelShareHandler(int connfd, char *buffer, int *loginStatus)
{
    char sendData[BUFF_SIZE];
    if (loginStatus == 0)
    {
        memset(sendData, '\0', BUFF_SIZE);
        sprintf(sendData, "212");
        send(connfd, sendData, BUFF_SIZE, 0);
        return;
    }
    char client_ip[BUFF_SIZE];
    uint16_t client_port;
    getInfoClient(connfd, client_ip, &client_port);
    write_log(&client_port, client_ip, buffer);

    char type[3];
    uint32_t id;
    char filename[BUFF_SIZE];
    if (sscanf(buffer, "%2s %u %s", type, &id, filename) == 3)
    {
        int success = deleteIndex(id, filename);
        if (success == 1)
        {
            sprintf(buffer, "%d", 150);
            send(connfd, buffer, 256, 0);
        }
        else if (success == 0)
        {
            sprintf(buffer, "%d", 250);
            send(connfd, buffer, 256, 0);
        }
        else
        {
            sprintf(buffer, "%d", 300);
            send(connfd, buffer, 256, 0);
        }
    }
    else
    {
        sprintf(buffer, "%d", 300);
        send(connfd, buffer, 256, 0);
    }
}

/**
 * @brief Handle find file requests from clients
 * @param connfd sock_fd
 * @param command command
 * @param loginStatus status of login
 */
void FindShareFileHandler(int connfd, char *command, int *loginStatus)
{
    char buffer[BUFF_SIZE];
    if (loginStatus == 0)
    {
        memset(buffer, '\0', BUFF_SIZE);
        sprintf(buffer, "212");
        send(connfd, buffer, BUFF_SIZE, 0);
        return;
    }
    memset(buffer, '\0', BUFF_SIZE);
    char fileName[50], fileNameTmp[50];
    uint32_t id;
    char ip[16];
    int port;
    sscanf(command, "FI %s", fileName);
    char client_ip[BUFF_SIZE];
    uint16_t client_port;
    getInfoClient(connfd, client_ip, &client_port);
    write_log(&client_port, client_ip, command);

    char line[BUFF_SIZE];
    int count = 0;
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("ERROR: Can not open file!\n");
        exit(EXIT_FAILURE);
    }
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%u %15s %d %[^\n]", &id, ip, &port, fileNameTmp) == 4)
        {
            if (strcmp(fileName, fileNameTmp) == 0)
            {
                struct Client *clientTmp = find(clientList, id);
                if (clientTmp->isLogin == 1)
                {
                    count++;
                    if (count == 1)
                    {
                        sprintf(buffer, "%d", 130);
                        send(connfd, buffer, BUFF_SIZE, 0);
                    }
                    memset(buffer, '\0', BUFF_SIZE);
                    sprintf(buffer, "%s %d %s", ip, port, fileName);
                    send(connfd, buffer, BUFF_SIZE, 0);
                }
            }
        }
    }
    fclose(file);
    if (count > 0)
    {
        memset(buffer, '\0', BUFF_SIZE);
        sprintf(buffer, "%d", 131);
        send(connfd, buffer, BUFF_SIZE, 0);
    }
    else
    {
        memset(buffer, '\0', BUFF_SIZE);
        sprintf(buffer, "%d", 230);
        send(connfd, buffer, BUFF_SIZE, 0);
    }
}

/**
 * @brief Check if this file is shared by a client with this ip and port
 * @param fileName file name
 * @param clientIP client's ip
 * @param clientPort client's port
 * @return 1 if true, 0 if false
 */
int checkFile(char *fileName, char *clientIP, int clientPort)
{
    char fileNameTmp[50];
    uint32_t id;
    char ip[16];
    int port;
    char line[BUFF_SIZE];
    int count = 0;
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("ERROR: Can not open file!\n");
        exit(EXIT_FAILURE);
    }
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%u %15s %d %[^\n]", &id, ip, &port, fileNameTmp) == 4)
        {
            if (strcmp(fileName, fileNameTmp) == 0 && strcmp(clientIP, ip) == 0 && clientPort == port)
            {
                struct Client *clientTmp = find(clientList, id);
                if (clientTmp->isLogin == 1)
                {
                    return 1;
                }
            }
        }
    }
    fclose(file);
    return 0;
}

/**
 * @brief Handle the request from the client to check if this file is shared by the client with this ip and port
 * @param connfd sock_fd
 * @param command command
 * @param loginStatus status of login
 */
void CheckFileHandler(int connfd, char *command, int *loginStatus)
{
    char buffer[BUFF_SIZE];
    if (loginStatus == 0)
    {
        memset(buffer, '\0', BUFF_SIZE);
        sprintf(buffer, "212");
        send(connfd, buffer, BUFF_SIZE, 0);
        return;
    }
    memset(buffer, '\0', BUFF_SIZE);
    char fileName[200], clientIP[16];
    int port;

    char client_ip[BUFF_SIZE];
    uint16_t client_port;
    getInfoClient(connfd, client_ip, &client_port);
    write_log(&client_port, client_ip, command);

    sscanf(command, "CH %s %s %d", fileName, clientIP, &port);
    if (checkFile(fileName, clientIP, port))
    {
        sprintf(buffer, "160");
    }
    else
    {
        sprintf(buffer, "260");
    }
    send(connfd, buffer, BUFF_SIZE, 0);
}

/**
 * @brief Handle the request remove file cannot be downloaded from index file
 * @param connfd sock_fd
 * @param command command
 * @param loginStatus status of login
 */
void removeBugFile(int connfd, char *command, int *loginStatus)
{
    char buffer[BUFF_SIZE];
    char client_ip[BUFF_SIZE];
    uint16_t client_port;
    getInfoClient(connfd, client_ip, &client_port);
    write_log(&client_port, client_ip, command);

    if (loginStatus == 0)
    {
        memset(buffer, '\0', BUFF_SIZE);
        sprintf(buffer, "212");
        send(connfd, buffer, BUFF_SIZE, 0);
        return;
    }
    memset(buffer, '\0', BUFF_SIZE);
    char fileName[200], clientIP[16];
    int port;
    sscanf(command, "UD %s %s %d", fileName, clientIP, &port);
    struct Client *clientA = findByIPAndPort(clientList, clientIP, port);
    int r = deleteIndex(clientA->id, fileName);
    if (r == 1)
    {
        sprintf(buffer, "170");
    }
    else
    {
        sprintf(buffer, "270");
    }
    send(connfd, buffer, BUFF_SIZE, 0);
};
