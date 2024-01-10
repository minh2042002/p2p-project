#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "../common/socketp2p.h"
#include "Client.h"
#include "serverUtils.h"
#include "serverHandler.h"

#define BUFF_SIZE 256
void *handleThread(void *);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: ./server <#Port_Number>");
        return 1;
    }

    // Load list client from file
    loadClients();
    int port_number = atoi(argv[1]);
    int *connfd;
    pthread_t tid;
    struct sockaddr_in *client_addr;
    int sin_size;
    int server_sock = createSocket();
    bindSocket(server_sock, port_number);
    listenSocket(server_sock, 10);

    char buffer[256];
    int bytes_received;
    char message[256];
    sin_size = sizeof(struct sockaddr_in);
    client_addr = malloc(sin_size);
    while (1)
    {
        connfd = malloc(sizeof(int));

        // connect with a client
        *connfd = acceptSocket(server_sock, (struct sockaddr *)client_addr, &sin_size);
        printf("[+] connect to client %s:%d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
        pthread_create(&tid, NULL, &handleThread, (void *)connfd);
    }

    close(server_sock);

    return 0;
}

void *handleThread(void *arg)
{
    int loginStatus = 0;
    int ret;
    int connfd = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());

    int l = -1;
    char buffer[256];
    char command[BUFF_SIZE];
    char send_data[BUFF_SIZE];
    struct Client *client = NULL;
    while (1)
    {
        ret = recv(connfd, buffer, 256, 0);
        if (ret <= 0)
        {
            // logout and close thread
            if (loginStatus == 1)
            {
                logout(client);
                loginStatus = 0;
            }
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
                    if (l >= BUFF_SIZE)
                    {
                        memset(send_data, '\0', BUFF_SIZE);
                        strcpy(send_data, "310");
                        ret = send(connfd, send_data, BUFF_SIZE, 0);
                        l = 0;
                    }
                    else
                    {
                        command[l - 1] = '\0';
                        if (strstr(command, "SU") == command)
                        {
                            SignUpHandler(connfd, &client, command, &loginStatus);
                        }
                        else if (strstr(command, "SI ") == command)
                        {
                            SignInHandler(connfd, &client, command, &loginStatus);
                        }
                        else if (strstr(command, "SH ") == command)
                        {
                            RegisterShareHandler(connfd, command, &loginStatus);
                        }
                        else if (strstr(command, "DF ") == command)
                        {
                            CancelShareHandler(connfd, command, &loginStatus);
                        }
                        else if (strstr(command, "FI ") == command)
                        {
                            FindShareFileHandler(connfd, command, &loginStatus);
                        }
                        else if (strstr(command, "CH ") == command)
                        {
                            CheckFileHandler(connfd, command, &loginStatus);
                        }
                        else if (strstr(command, "UD ") == command)
                        {
                            removeBugFile(connfd, command, &loginStatus);
                        }
                        else
                        {
                            sprintf(buffer, "%d", 300);
                            send(connfd, buffer, 256, 0);
                        }
                        l = -1;
                    }
                }
            }
        }
    }
    close(connfd);
}
