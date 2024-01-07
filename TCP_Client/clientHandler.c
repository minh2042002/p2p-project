#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/stat.h>
#include "clientHandler.h"

#define BUFF_SIZE 256
#include "socketp2p.h"

void signup(int socket)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;
    uint32_t id;

    // request send protocol signup with format "SU"
    sprintf(buffer, "SU\r\n");
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    int result = sscanf(buffer, "%d - %u", &status, &id); // status: 100 and id
    if (result == 2)
    {
        printf("Sign up is succes.");

        FILE *file = fopen("config.txt", "w");
        if (file == NULL)
        {
            perror("Not open file and save id!");
        }
        else
        {
            fprintf(file, "%d", id);
        }

        fclose(file);
    }
    else
    {
        sscanf(buffer, "%d", &status); // status: 300
        if (status == 300)
        {
            printf("Protocol is wrong!");
        }
        else
        {
            printf("%s\n", buffer);
            perror("An unknown error");
        }
    }
}

void login(int socket, uint32_t id, int port)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;
    char data[250];

    // request send protocol login with format "SI <ID> <LISTEN_PORT>"
    sprintf(buffer, "SI %u %d\r\n", id, port);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 110)
    {
        printf("Login is success.");
    }
    else if (status == 211)
    {
        printf("Unsignuped!");
        exit(EXIT_FAILURE);
    }
    else if (status == 212)
    {
        printf("Login yet!");
        exit(EXIT_FAILURE);
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
        exit(EXIT_FAILURE);
    }
    else
    {
        perror("An unknown error");
        exit(EXIT_FAILURE);
    }
}

int registerShareFile(int socket, uint32_t id, char *file_name)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;

    // request send protocol register share file
    sprintf(buffer, "SH %u %s", id, file_name);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        return 0;
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 120)
    {
        printf("Register share file is success.");
        return 1;
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
    }
    else
    {
        perror("An unknown error!");
    }

    return 0;
}

int cancelShareFile(int socket, uint32_t id, char *file_name)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;

    sprintf(buffer, "DF %u %s", id, file_name);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        return 0;
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 150)
    {
        printf("Successfully unsubscribed file sharing");
        return 1;
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
    }
    else
    {
        perror("An unknown error!");
    }

    return 0;
}

void findFile(int socket)
{
}

void downloadFile(int socket)
{
    int port;
    char fileName[100];
    char clientIP[9];
    char buffer[4096];
    printf("Input:<#fileName> <#client_IP> <#client_PORT>\n");
    int r = scanf("%s %s %d", fileName, clientIP, &port);
    if (r != 3)
    {
        printf("Error: format input must be <#fileName> <#client_IP> <#client_PORT> \n");
        return;
    }
    sprintf(buffer, "DL %s\r\n", fileName);
    int client_sock = createSocket();
    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(port);
    src_addr.sin_addr.s_addr = inet_addr(clientIP);
    if (connect(client_sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0)
    {
        printf("\n-Error can't connect to source client!\n");
        return;
    }
    send(client_sock, buffer, 4096, 0);
    memset(buffer, '\0', 4096);
    int ret = recv(client_sock, buffer, 4096, 0);
    if (ret <= 0)
    {
        printf("-ERROR can connect source client!\n");
        return;
    }
    else
    {
        long long fileSize = 0;
        buffer[ret] = '\0';
        if (strstr(buffer, "140") == buffer)
        {
            sscanf(buffer, "140 %ld", &fileSize);
        }
        else if (strstr(buffer, "240") == buffer)
        {
            printf("+OK file is not exist in this client!\n");
        }
    }
}

long long getFileSize(char *filePath)
{
    struct stat file_info;
    if (stat(filePath, &file_info) == 0)
    {
        return (long long)file_info.st_size;
    }
    else
    {
        printf("%s\n", filePath);
        printf("-Error Can not get file info\n");
        return -1;
    }
}

void sendFile(int socket, char *filePath)
{
    char buffer[4096];
    FILE *file = fopen(filePath, "rb");
    if (file == NULL)
    {
        memset(buffer, '\0', 4096);
        sprintf(buffer, "240 fail");
        send(socket, buffer, 4096, 0);
        return;
    }
    else
    {
        long long fileSize = getFileSize(filePath);
        memset(buffer, '\0', 4096);
        sprintf(buffer, "140 %ld", fileSize);
        send(socket, buffer, 4096, 0);
        memset(buffer, '\0', 4096);
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, 4096, file)) > 0)
        {
            send(socket, buffer, 4096, 0);
        }
        fclose(file);
    }
}