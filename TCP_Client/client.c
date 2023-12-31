#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "../common/socketp2p.h"
#include "./helper.h"
#include "./clientHandler.h"
#define BUFF_SIZE 256
struct ServerInfo
{
    char *ip;
    int port;
};

int listen_port;
int server_port;

void *listenThread(void *);
void *sendFileThread(void *);
void *requestThread(void *);
void *connectServerThread(void *);

void RegisterShareFileHandler(int client_socket, uint32_t id);
void CancelShareFileHandler(int client_socket, uint32_t id);
void FindFileHandler(int client_socket, uint32_t id);
void DownloadFileHandler(int client_socket, uint32_t id);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("usage: ./client <#Server_IP> <#Server_PORT> <#Listen_PORT>");
        return 1;
    }

    server_port = atoi(argv[2]);
    listen_port = atoi(argv[3]);

    struct ServerInfo serverInfo;
    serverInfo.ip = argv[1];
    serverInfo.port = server_port;

    // create connect server socket thread
    pthread_t serverTid;
    pthread_create(&serverTid, NULL, &connectServerThread, (void *)&serverInfo);
    // create listen socket thread
    pthread_t listenTid;
    pthread_create(&listenTid, NULL, &listenThread, (void *)&listen_port);
    pthread_join(serverTid, NULL);
    return 0;
}

void printMenu()
{
    printf("Menu:\n");
    printf("1. Chia sẻ file\n");
    printf("2. Hủy chia sẻ file\n");
    printf("3. Tìm kiếm file\n");
    printf("4. Download file\n");
    printf("5. Exit\n");
    printf("Nhập chức năng: ");
}
void *connectServerThread(void *arg)
{
    struct ServerInfo *serverInfo = (struct ServerInfo *)arg;
    int client_socket = createSocket();
    connectSocket(client_socket, serverInfo->ip, serverInfo->port);
    uint32_t id = getID(); // kiểm tra client có id chưa. -1
    if (id == -1)
    {
        signup(client_socket);
    }
    else
    {
        login(client_socket, id, listen_port);
    }
    while (1)
    {
        int function = 0;
        printMenu();
        int r = scanf("%d", &function);
        if (r != 1 || function < 1 || function > 5)
        {
            printf("Nhập chức năng từ 1 - 5!\n");
            continue;
        }
        if (function == 1)
        {
            RegisterShareFileHandler(client_socket, id);
        }
        else if (function == 2)
        {
            CancelShareFileHandler(client_socket, id);
        }
        else if (function == 3)
        {
            FindFileHandler(client_socket, id);
        }
        else if (function == 4)
        {
            DownloadFileHandler(client_socket, id);
        }
        else if (function == 5)
        {
            break;
        }
    }
    close(client_socket);
}
void *requestThread(void *arg)
{
    pthread_detach(pthread_self());
};
void *listenThread(void *arg)
{

    pthread_detach(pthread_self());
    int port = *((int *)arg);
    pthread_t tid;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int listen_sock = createSocket();
    bindSocket(listen_sock, port);
    listenSocket(listen_sock, 5);
    int *conn_sock;
    while (1)
    {
        conn_sock = malloc(sizeof(int));
        *conn_sock = acceptSocket(listen_sock, (struct sockaddr *)&client_addr, &client_len);
        printf("[+] Đã kết nối với %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        pthread_create(&tid, NULL, &sendFileThread, (void *)conn_sock);
    }
}
void *sendFileThread(void *arg)
{
    pthread_detach(pthread_self());
    int conn_sock = *((int *)arg);
    int l = 0, ret = 0;
    char command[BUFF_SIZE];
    char send_data[BUFF_SIZE];
    char buffer[BUFF_SIZE];
    while (1)
    {
        ret = recv(conn_sock, buffer, 256, 0);
        if (ret <= 0)
        {
            // close
            break;
        }
        else
        {
            int len = strlen(buffer);
            for (int i = 0; i < len; i++)
            {
                if (l < BUFF_SIZE)
                {
                    l++;
                }
                if (l < BUFF_SIZE)
                {
                    command[l] = buffer[i];
                }
                else
                {
                    command[l - 2] = command[l - 1];
                    command[l - 1] = buffer[i];
                }
                if (l > 1 && command[l - 1] == '\r' && command[l] == '\n')
                {
                    if (l == 2 * BUFF_SIZE)
                    {
                        memset(send_data, '\0', BUFF_SIZE);
                        strcpy(send_data, "310");
                        ret = send(conn_sock, send_data, BUFF_SIZE, 0);
                        l = 0;
                    }
                    else
                    {
                        command[l - 1] = '\0';
                        if (strcmp(command, "DL") == 0)
                        {
                            // Sendfile
                            char fileName[100];
                            sscanf(command, "DL %s", fileName);
                            sendFile(conn_sock, fileName);
                        }
                        l = -1;
                    }
                }
            }
        }
    }
    close(conn_sock);
}

void RegisterShareFileHandler(int client_socket, uint32_t id)
{
    char path[BUFF_SIZE];
    char filename[BUFF_SIZE];

    printf("Nhap duong dan file muon chia se: ");
    scanf("%s", path);

    getFileName(path, filename);
    int succes = registerShareFile(client_socket, id, filename);
    if (succes)
    {
        saveFile(path);
    }
}

void CancelShareFileHandler(int client_socket, uint32_t id)
{
    char path[BUFF_SIZE];
    char filename[BUFF_SIZE];

    printf("Nhap duong dan file muon huy chia se: ");
    scanf("%s", path);

    getFileName(path, filename);
    int succes = cancelShareFile(client_socket, id, filename);
    if (succes)
    {
        deleteFile(path);
    }
}

void FindFileHandler(int client_socket, uint32_t id)
{
}

void DownloadFileHandler(int client_socket, uint32_t id)
{
}