#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "../common/socketp2p.h";
#include "./helper.h";
#include "./clientHandler.h";
struct ServerInfo
{
    char *ip;
    int port;
};

int listen_port;
int server_port;

void *listenThread(void *);
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
    pthread_create(&serverTid, NULL, connectServerThread, &serverInfo);

    // create listen socket thread
    pthread_t listenTid;
    pthread_create(&listenTid, NULL, listenThread, listen_port);
    return 0;
}
void printMenu()
{
    printf("Menu:\n");
    printf("1. Chia sẻ file\n");
    printf("2. Hủy chia sẻ file\n");
    printf("3. Tìm kiếm file\n");
    printf("4. Download file\n");
    printf("Nhập chức năng: ");
}
void *connectServerThread(void *arg)
{
    pthread_detach(pthread_self());
    struct ServerInfo *serverInfo = (struct ServerInfo *)arg;
    int client_socket = createSocket();
    connectServer(client_socket, serverInfo->ip, serverInfo->port);
    uint32_t id = getID(); // kiểm tra client có id chưa. -1
    if (id == -1)
    {
        register(client_socket);
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
        if (r != 1 || function < 1 || function > 4)
        {
            printf("Nhập chức năng từ 1 - 4!\n");
            continue;
        }
        if (function == 1)
        {
            /* code */
        }
        else if (function == 2)
        {
            /* code */
        }
        else if (function == 3)
        {
            /* code */
        }
        else
        {
            /* code */
        }
    }

    close(client_socket);
}
void *requestThread(void *arg)
{
    pthread_detach(pthread_self());
};
void *connectServerThread(void *arg)
{
    pthread_detach(pthread_self());
}