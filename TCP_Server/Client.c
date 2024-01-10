#include "Client.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
pthread_mutex_t loginMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clientFileMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief create new client
 * @param id client's id
 * @param ip client's ip
 * @param port client's port
 * @return new client
 */
struct Client *create(uint32_t id, char *ip, uint16_t port)
{
    struct Client *client = (struct Client *)malloc(sizeof(struct Client));
    if (client == NULL)
    {
        perror("Unable to allocate memory!");
        exit(EXIT_FAILURE);
    }
    client->id = id;
    client->_addr.sin_family = AF_INET;
    client->_addr.sin_port = port;
    inet_pton(AF_INET, ip, &(client->_addr.sin_addr));

    client->next = NULL;
    client->prev = NULL;

    return client;
}

/**
 * @brief add a client into client list
 * @param head client list
 * @param newClient new client
 */
void add(struct Client **head, struct Client *newClient)
{
    if (*head == NULL)
    {
        *head = newClient;
    }
    else
    {
        newClient->next = *head;
        (*head)->prev = newClient;
        *head = newClient;
    }
}

/**
 * @brief find a client by id
 * @param head client list
 * @param id client's id
 * @return a client
 */
struct Client *find(struct Client *head, uint32_t id)
{
    struct Client *current = head;

    while (current != NULL)
    {
        if (current->id == id)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

/**
 * @brief find a client by ip and port
 * @param head client list
 * @param ip client's ip
 * @param port client's port
 * @return a client
 */
struct Client *findByIPAndPort(struct Client *head, char *ip, int port)
{
    struct Client *current = head;
    char ipTmp[16];
    while (current != NULL)
    {
        inet_pton(AF_INET, ipTmp, &(current->_addr.sin_addr));
        if (strcmp(ip, ipTmp) && current->_addr.sin_port == port)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

/**
 * @brief delete client by id
 * @param head client list
 * @param id client's id
 */
void deleteByID(struct Client **head, uint32_t id)
{
    struct Client *delClient = find(*head, id);
    if (delClient == NULL)
        return;

    if (delClient->prev != NULL)
    {
        delClient->prev->next = delClient->next;
    }
    else
    {
        *head = delClient->next;
    }

    if (delClient->next != NULL)
    {
        delClient->next->prev = delClient->prev;
    }

    free(delClient);
}

/**
 * @brief update client ip
 * @param client client need to update ip
 * @param newIP new ip
 */
void update(struct Client *client, const char *newIP)
{
    if (client != NULL)
    {
        inet_pton(AF_INET, newIP, &(client->_addr.sin_addr));
    }
}

/**
 * @brief login
 * @param client client need to login
 * @return 1 if login success, 0 is login fail
 */
int login(struct Client *client)
{
    int status = 0;
    if (client != NULL)
    {
        pthread_mutex_lock(&loginMutex);
        if (client->isLogin == 0)
        {
            client->isLogin = 1;
            status = 1;
        }
        else
        {
            status = 0;
        }
        pthread_mutex_unlock(&loginMutex);
        return status;
    }
    else
    {
        printf("Client is null\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief logout
 * @param client client need to logout
 */
void logout(struct Client *client)
{
    pthread_mutex_lock(&loginMutex);
    if (client != NULL)
    {
        client->isLogin = 0;
    }
    pthread_mutex_unlock(&loginMutex);
}

/**
 * @brief save all client into client file
 * @param head client list
 */
void saveAll(struct Client *head)
{
    pthread_mutex_lock(&clientFileMutex);
    FILE *file = fopen("client.txt", "wb");
    if (file == NULL)
    {
        perror("Cannot open file to save client!");
        exit(EXIT_FAILURE);
    }

    int numberOfClients = count(head);
    // Save number of clients
    fwrite(&numberOfClients, sizeof(int), 1, file);
    struct Client *current = head;
    while (current != NULL)
    {
        fwrite(current, sizeof(struct Client), 1, file);
        current = current->next;
    }
    fclose(file);
    pthread_mutex_unlock(&clientFileMutex);
};

/**
 * @brief load all client from file
 * @param head buffer to store clients
 */
void loadFromFile(struct Client **head)
{
    FILE *file = fopen("client.txt", "rb");
    if (file == NULL)
    {
        perror("Cannot open file to load client!");
        exit(EXIT_FAILURE);
    }

    int numberOfClients;
    fread(&numberOfClients, sizeof(int), 1, file);

    for (int i = 0; i < numberOfClients; i++)
    {
        struct Client *client = (struct Client *)malloc(sizeof(struct Client));
        if (client == NULL)
        {
            perror("Unable to allocate memory!");
            exit(EXIT_FAILURE);
        }

        fread(client, sizeof(struct Client), 1, file);
        client->next = NULL;
        client->prev = NULL;
        client->isLogin = 0;
        add(head, client);
    }

    fclose(file);
}

/**
 * @brief count number of clients
 * @param head client list
 * @return number of client
 */
int count(struct Client *head)
{
    int numberOfClients = 0;
    struct Client *current = head;
    while (current != NULL)
    {
        numberOfClients++;
        current = current->next;
    }

    return numberOfClients;
}