#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "../common/socketp2p.h"
#include "Client.h"
#include "serverHandler.h"

#define BUFF_SIZE 256
void *handleThread(void *);
struct Client *clientList = NULL;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: ./server <#Port_Number>");
        return 1;
    }

    // Load list client
    loadFromFile(&clientList);

    int port_number = atoi(argv[1]);
    int *connfd;
    pthread_t tid;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int server_sock = createSocket();
    bindSocket(port_number, server_sock);
    listenSocket(server_sock, 5);


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
                            // Get IP and PORT of client from connfd
                            struct sockaddr_in peer_addr;
                            socklen_t peer_len = sizeof(peer_addr);
                            if (getpeername(connfd, (struct sockaddr *)&peer_addr, &peer_len) == -1)
                            {
                                perror("Unable to get client information!");
                                close(connfd);
                                exit(EXIT_FAILURE);
                            }

                            char client_ip[INET_ADDRSTRLEN];
                            inet_ntop(AF_INET, &(peer_addr.sin_addr), client_ip, sizeof(client_ip));
                            uint16_t client_port = ntohs(peer_addr.sin_port);

                            uint32_t id = generateClientID();

                            struct Client *newClient = create(id, client_ip, client_port);
                            add(&clientList, newClient);
                            login(newClient);
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