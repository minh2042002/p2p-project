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
pthread_mutex_t loginMutex = PTHREAD_MUTEX_INITIALIZER;
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
        printf("[+] Đã kết nối với %s:%d\n", inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
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
            logout(client);
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
                        if (strcmp(command, "SU") == 0)
                        {
                            char client_ip[BUFF_SIZE];
                            uint16_t client_port;
                            int succes = getInfoClient(connfd, client_ip, &client_port);

                            if (succes)
                            {
                                uint32_t id = generateClientID();

                                struct Client *newClient = create(id, client_ip, client_port);
                                add(&clientList, newClient);
                                client = newClient;
                                login(newClient);
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
                        else if (strstr(command, "SI ") == command)
                        {
                            uint32_t clientID = 0;
                            char clientIP[201];
                            memset(clientIP, '\0', 201);
                            sscanf(buffer, "SI %u %200s", &clientID, clientIP);
                            clientIP[200] = '\0';
                            client = find(clientList, clientID);
                            if (client != NULL)
                            {
                                pthread_mutex_lock(&loginMutex);
                                if (client->isLogin == 1)
                                {
                                    sprintf(buffer, "212");
                                }
                                else
                                {
                                    login(client);
                                    update(client, clientIP);
                                    sprintf(buffer, "110");
                                    saveAll(clientList);
                                }
                                pthread_mutex_unlock(&loginMutex);
                            }
                            else
                            {
                                sprintf(buffer, "210");
                            }
                            send(connfd, buffer, 256, 0);
                        }
                        else if (strstr(command, "SH ") == command)
                        {
                            char client_ip[BUFF_SIZE];
                            uint16_t client_port;
                            getInfoClient(connfd, client_ip, &client_port);

                            char type[3];
                            uint32_t id;
                            char filename[BUFF_SIZE];
                            if (sscanf(command, "%2s %u %s", type, &id, filename) == 3)
                            {
                                struct Client *existClient = find(clientList, id);
                                char client_ip[BUFF_SIZE];
                                uint16_t client_port;

                                strcpy(client_ip, inet_ntoa(existClient->_addr.sin_addr));
                                client_port = ntohs(existClient->_addr.sin_port);
                                update(existClient, client_ip);
                                saveAll(clientList);
                                int succes = updateIndex(id, client_ip, client_port, filename);
                                if (succes)
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
                        else if (strstr(command, "DF") == command)
                        {
                            char client_ip[BUFF_SIZE];
                            uint16_t client_port;
                            getInfoClient(connfd, client_ip, &client_port);

                            char type[3];
                            uint32_t id;
                            char filename[BUFF_SIZE];
                            if (sscanf(command, "%2s %u %s", type, &id, filename) == 3)
                            {
                                int succes = deleteIndex(id, filename);
                                if (succes == 1)
                                {
                                    sprintf(buffer, "%d", 150);
                                    send(connfd, buffer, 256, 0);
                                }
                                else if (succes == 0)
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