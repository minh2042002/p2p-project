#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "../common/socketp2p.h";
#include "Client.h";

#define BUFF_SIZE 256
void *handleThread(void *);

int main(int argc, char *argv[])
{

    struct Client *clientList = NULL;

    if (argc != 2)
    {
        printf("usage: ./server <#Port_Number>");
        return 1;
    }

    int port_number = atoi(argv[1]);
    int *connfd;
    pthread_t tid;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int server_sock = createSocket();
    bindSocket(port_number, server_sock);
    listenSocket(server_sock, 5);

    struct Client *clientList = NULL;

    char buffer[256];
    int bytes_received;
    char message[256];

    while (1)
    {
        connfd = malloc(sizeof(int));
        // connect with a client
        *connfd = acceptSocket(server_sock, (struct sockaddr *)&client_addr, &client_len);
        printf("[+] Đã kết nối với %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        pthread_create(&tid, NULL, &handleThread, (void *)connfd);
    }

    close(server_sock);

    return 0;
}

void *handleThread(void *arg)
{
    int ret;
    int connfd = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());

    int l = 0;
    char buffer[256];
    char command[BUFF_SIZE];
    char send_data[BUFF_SIZE];
    while (1)
    {
        ret = recv(connfd, buffer, 256, 0);
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
                        ret = send(connfd, send_data, BUFF_SIZE, 0);
                        l = 0;
                    }
                    else
                    {
                        command[l - 1] = '\0';
                        if (strcmp(command, "SU") == 0)
                        {
                            si
                        }
                        else if (strstr(command, "SI ") == command)
                        {
                            // Todo sign in
                        }
                        else if (strstr(command, "SH ") == command)
                        {
                            // Todo share file
                        }
                        else if (strstr(command, "DF") == command)
                        {
                            // Todo unshare file
                        }
                        l = -1;
                    }
                }
            }
        }
    }
    close(connfd);
}