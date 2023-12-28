#include "Client.h"

struct Client create(uint32_t index, char *ip, int port)
{
    struct Client client;
    client.index = index;
    client._addr.sin_family = AF_INET;
    client._addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &(client._addr.sin_addr));

    return client;
}

struct Client saveClient(struct Client *client)
{
    FILE *file = fopen("client.txt", "wb");
    if (file == NULL)
    {
        perror("Cannot open file to save client!");
        exit(EXIT_FAILURE);
    }

    fwrite(client, sizeof(struct Client), 1, file);

    fclose(file);
};

struct Client loadClients()
{
    struct Client clientArray[]
    FILE *file = fopen("client.txt", "rb");
    if (file == NULL) {
        perror("Cannot open file to read");
        exit(EXIT_FAILURE);
    }

    while(!feof(file)) {
        fread()
    }
    fread()
}

struct Client getClientFromFile(uint32_t)