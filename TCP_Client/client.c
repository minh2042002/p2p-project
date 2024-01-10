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
/**
 * @brief print menu of main program.
 */
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
/**
 * @brief Thread use to connect and request to server
 */
void *connectServerThread(void *arg)
{
    struct ServerInfo *serverInfo = (struct ServerInfo *)arg;
    int client_socket = createSocket();
    connectSocket(client_socket, serverInfo->ip, serverInfo->port);
    uint32_t id = getID();
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
            int character;
            while ((character = getchar()) != '\n' && character != EOF)
                ;
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
            FindFileHandler(client_socket);
        }
        else if (function == 4)
        {
            DownloadFileHandler(client_socket);
        }
        else if (function == 5)
        {
            break;
        }
    }
    close(client_socket);
}
/**
 * @brief this thread use to accept connect from other client, create a send file thread per connection.
 */
void *listenThread(void *arg)
{

    pthread_detach(pthread_self());
    int port = *((int *)arg);
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
        pthread_t tid;
        pthread_create(&tid, NULL, &sendFileThread, (void *)conn_sock);
    }
    close(listen_sock);
}
/**
 * @brief this thread use to request to analys download file request.
 */
void *sendFileThread(void *arg)
{
    int conn_sock = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());
    int l = -1, ret = 0;
    char command[BUFF_SIZE];
    char send_data[BUFF_SIZE];
    char buffer[BUFF_SIZE];
    while (1)
    {
        ret = recv(conn_sock, buffer, 256, 0);
        if (ret <= 0)
        {
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
                        if (strstr(command, "DL ") == command)
                        {
                            // Sendfile
                            char fileName[100];
                            sscanf(command, "DL %s", fileName);
                            FILE *file = fopen("index.txt", "r");
                            if (file == NULL)
                            {
                                printf("Error: can not open index.txt!");
                                exit(EXIT_FAILURE);
                            }
                            int status = 0;
                            char line[300];
                            char fileNameCMP[100];
                            char filePath[200];
                            while (fgets(line, 300, file) != NULL)
                            {
                                sscanf(line, "%s : %s", fileNameCMP, filePath);
                                if (strcmp(fileName, fileNameCMP) == 0)
                                {
                                    status = 1;
                                    break;
                                }
                            }

                            if (status == 0)
                            {
                                memset(buffer, '\0', BUFF_SIZE);
                                sprintf(buffer, "241");
                                send(conn_sock, buffer, BUFF_SIZE, 0);
                            }
                            else
                            {
                                file = fopen(filePath, "rb");
                                if (file != NULL)
                                {
                                    fseek(file, 0, SEEK_END);
                                    long file_size = ftell(file);
                                    if (file_size != -1)
                                    {
                                        memset(buffer, '\0', BUFF_SIZE);
                                        sprintf(buffer, "140 %ld", file_size);
                                        send(conn_sock, buffer, BUFF_SIZE, 0);
                                        sendFile(conn_sock, filePath);
                                        memset(buffer, '\0', BUFF_SIZE);
                                        recv(conn_sock, buffer, BUFF_SIZE, 0);
                                    }
                                    else
                                    {
                                        memset(buffer, '\0', BUFF_SIZE);
                                        sprintf(buffer, "242");
                                        send(conn_sock, buffer, BUFF_SIZE, 0);
                                    }

                                    fclose(file);
                                }
                                else
                                {
                                    memset(buffer, '\0', BUFF_SIZE);
                                    sprintf(buffer, "240");
                                    send(conn_sock, buffer, BUFF_SIZE, 0);
                                }
                            }
                        }
                        l = -1;
                    }
                }
            }
        }
    }
    close(conn_sock);
}
