#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "serverHandler.h"
#include "serverUtils.h"
#include "Client.h"

#define BUFF_SIZE 256
struct Client *clientList = NULL;

void loadClients()
{
    loadFromFile(&clientList);
}

void SignUpHandler(int connfd, struct Client **client, char *command, int *loginStatus)
{
    char buffer[BUFF_SIZE];
    char client_ip[16];
    uint16_t client_port;
    int clientListenPort;
    sscanf(command, "SU %d", &clientListenPort);
    int success = getInfoClient(connfd, client_ip, &client_port);
    printf("%s", command);
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
    FILE *file = fopen("index.txt", "r+"); // Mở file để đọc
    if (file == NULL)
    {
        perror("Không thể mở file");
        exit(EXIT_FAILURE);
    }
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Sử dụng sscanf để đọc từng trường từ dòng
        if (sscanf(line, "%u %15s %d %[^\n]", &id, ip, &port, fileNameTmp) == 4)
        {
            // So sánh id và filename với giá trị mong muốn
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
int checkFile(char *fileName, char *clientIP, int clientPort)
{
    char fileNameTmp[50];
    uint32_t id;
    char ip[16];
    int port;
    char line[BUFF_SIZE];
    int count = 0;
    FILE *file = fopen("index.txt", "r+"); // Mở file để đọc
    if (file == NULL)
    {
        perror("Không thể mở file");
        exit(EXIT_FAILURE);
    }
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Sử dụng sscanf để đọc từng trường từ dòng
        if (sscanf(line, "%u %15s %d %[^\n]", &id, ip, &port, fileNameTmp) == 4)
        {
            // So sánh id và filename với giá trị mong muốn
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
