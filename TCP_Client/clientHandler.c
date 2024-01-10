#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include "clientHandler.h"
#include "../common/socketp2p.h"
#include "helper.h"

#define BUFF_SIZE 256
void RegisterShareFileHandler(int client_socket, uint32_t id)
{
    char path[BUFF_SIZE];
    char filename[BUFF_SIZE];

    printf("Nhap duong dan file muon chia se: ");
    scanf("%s", path);

    getFileName(path, filename);
    int success = registerShareFile(client_socket, id, filename);
    if (success)
    {
        saveFile(path, filename);
    }
}

void CancelShareFileHandler(int client_socket, uint32_t id)
{
    char path[BUFF_SIZE];
    char filename[BUFF_SIZE];

    printf("Nhap duong dan file muon huy chia se: ");
    scanf("%s", path);

    getFileName(path, filename);
    int success = cancelShareFile(client_socket, id, filename);
    if (success)
    {
        deleteFile(path);
    }
}

void FindFileHandler(int client_socket, uint32_t id)
{
    char buffer[BUFF_SIZE];
    char fileName[BUFF_SIZE - 56];
    printf("Enter file name: ");
    scanf("%s", fileName);
    memset(buffer, '\0', BUFF_SIZE);
    sprintf(buffer, "FI %s\r\n", fileName);
    send(client_socket, buffer, BUFF_SIZE, 0);
    memset(buffer, '\0', BUFF_SIZE);
    recv(client_socket, buffer, BUFF_SIZE, 0);
    if (strcmp(buffer, "130") == 0)
    {
        while (strcmp(buffer, "131") != 0)
        {
            memset(buffer, '\0', BUFF_SIZE);
            recv(client_socket, buffer, BUFF_SIZE, 0);
            if (strcmp(buffer, "131") != 0)
            {
                printf("%s\n", buffer);
            }
        }
    }
    else if (strcmp(buffer, "230") == 0)
    {
        printf("file not found!\n");
    }
    else
    {
        printf("command is wrong!\n");
    }
}

void DownloadFileHandler(int client_socket, uint32_t id)
{
    char fileName[200], supplierIP[16];
    int supplierPort;
    printf("Input <#File_name> <#Supplier_IP> <#Supplier_Port>: ");
    int r = scanf("%s %s %d", fileName, supplierIP, &supplierPort);
    if (r != 3)
    {
        printf("Ussage: <#File_name> <#Supplier_IP> <#Supplier_Port>\n");
        return;
    }
    char buffer[BUFF_SIZE];
    memset(buffer, '\0', BUFF_SIZE);
    sprintf(buffer, "CH %s %s %d\r\n", fileName, supplierIP, supplierPort);
    send(client_socket, buffer, BUFF_SIZE, 0);
    memset(buffer, '\0', BUFF_SIZE);
    int ret = recv(client_socket, buffer, BUFF_SIZE, 0);
    if (ret <= 0)
    {
        printf("-Error: Server not response! Check file error!\n");
        return;
    }
    if (strcmp(buffer, "160") == 0)
    {
        downloadFile(client_socket, fileName, supplierIP, supplierPort);
    }
    else if (strcmp(buffer, "260") == 0)
    {
        printf("-Error: file is not shared by this client!\n");
        return;
    }
    else
    {
        printf("-Error: Not understand response!\n");
        return;
    }
}
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
        printf("Sign up is success.");

        FILE *file = fopen("config.txt", "w+");
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
        perror("An error occurred!\n");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 110)
    {
        printf("Login is success.\n");
    }
    else if (status == 210)
    {
        printf("Unsignuped!\n");
        exit(EXIT_FAILURE);
    }
    else if (status == 211)
    {
        printf("Login yet!\n");
        exit(EXIT_FAILURE);
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        perror("An unknown error\n");
        exit(EXIT_FAILURE);
    }
}

int registerShareFile(int socket, uint32_t id, char *file_name)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;

    // request send protocol register share file
    sprintf(buffer, "SH %u %s\r\n", id, file_name);
    printf("%s\n", buffer);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!\n");
        return 0;
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 120)
    {
        printf("Register share file is success.\n");
        return 1;
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!\n");
    }
    else
    {
        perror("An unknown error!\n");
    }

    return 0;
}

int cancelShareFile(int socket, uint32_t id, char *file_name)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;

    sprintf(buffer, "DF %u %s\r\n", id, file_name);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!\n");
        return 0;
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 150)
    {
        printf("Successfully unsubscribed file sharing\n");
        return 1;
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!\n");
    }
    else
    {
        perror("An unknown error!\n");
    }

    return 0;
}

void findFile(int socket)
{
    // char buffer[BUFF_SIZE];
    // int bytes_received;
    // sprintf(buffer, "FI %")
}

void downloadFile(int socket, char *fileName, char *clientIP, int port)
{
    char buffer[BUFF_SIZE];
    char bufferFile[4096];
    sprintf(buffer, "DL %s\r\n", fileName);
    int client_sock = createSocket();
    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(port);
    src_addr.sin_addr.s_addr = inet_addr(clientIP);
    if (connect(client_sock, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0)
    {
        printf("-Error can't connect to source client!\n");
        return;
    }
    send(client_sock, buffer, BUFF_SIZE, 0);
    memset(buffer, '\0', BUFF_SIZE);
    int ret = recv(client_sock, buffer, BUFF_SIZE, 0);
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
            sscanf(buffer, "140 %lld", &fileSize);
            storeFile(client_sock, fileName, fileSize);
            memset(buffer, '\0', BUFF_SIZE);
            sprintf(buffer, "141");
            send(client_sock, buffer, BUFF_SIZE, 0);
            printf("+OK download file success.\n");
        }
        else if (strstr(buffer, "240") == buffer)
        {
            printf("-ERR file is not exist in this client!\n");
            memset(buffer, '\0', BUFF_SIZE);
            sprintf(buffer, "UD %s %s %d\r\n", fileName, clientIP, port);
            send(socket, buffer, BUFF_SIZE, 0);
            memset(buffer, '\0', BUFF_SIZE);
            recv(socket, buffer, BUFF_SIZE, 0);
            if (strcmp(buffer, "170") == 0)
            {
                printf("+OK update file index in server success!\n");
            }
            else
            {
                printf("-ERROR update file index in server fail!\n");
            }
        }
    }
}

void sendFile(int socket, char *filePath)
{
    char buffer[4096];
    FILE *file = fopen(filePath, "rb");
    if (file == NULL)
    {
        return;
    }
    else
    {
        memset(buffer, '\0', 4096);
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, 4096, file)) > 0)
        {
            send(socket, buffer, 4096, 0);
        }
        fclose(file);
    }
}
/**
 * @function storeFile: write file content from client to a file.
 * @param conn_sock: connect socket's descriptor
 * @param filePath: path of file to write in server
 * @param fileSize: size of file
 */
void storeFile(int conn_sock, char *fileName, long long fileSize)
{
    int ret = 0;
    char bufferFile[4096];
    char filePath[500];
    memset(filePath, '\0', 500);
    sprintf(filePath, "storage/%s", fileName);
    FILE *file = fopen(filePath, "wb"); // "wb" để mở tập tin để ghi dữ liệu nhị phân
    if (file == NULL)
    {
        perror("Khong the mo file");
        return;
    }
    while (fileSize > 0)
    {
        ret = recv(conn_sock, bufferFile, BUFF_SIZE, 0);
        if (ret > fileSize)
        {
            ret = fileSize;
        }
        size_t elements_written = fwrite(bufferFile, ret, 1, file);
        if (elements_written != 1)
        {
            perror("Loi khi ghi vao file");
            fclose(file);
            return;
        }
        fileSize = fileSize - ret;
    }
    fclose(file);
}