#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "clientHandler.h"

void signup(int socket)
{
    char buffer[256];
    int bytes_received;
    int status;
    uint32_t id;

    // request send protocol signup with format "SU"
    sprintf(buffer, "SU\r\n");
    send(socket, buffer, 256, 0);

    bytes_received = recv(socket, buffer, 256, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    int result = sscanf(buffer, "%d - %u", &status, &id); // status: 100 and id
    if (result == 2)
    {
        printf("Sign up is succes.");

        FILE *file = fopen("config.txt", "w");
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
            perror("An unknown error");
        }
    }
}

void login(int socket, uint32_t id, int port)
{
    char buffer[256];
    int bytes_received;
    int status;
    char data[250];

    // request send protocol login with format "SI <ID> <LISTEN_PORT>"
    sprintf(buffer, "SI %u %d\r\n", id, port);
    send(socket, buffer, 256, 0);

    bytes_received = recv(socket, buffer, 256, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 110)
    {
        printf("Login is success.");
    }
    else if (status == 211)
    {
        printf("Unsignuped!");
        exit(EXIT_FAILURE);
    }
    else if (status == 212)
    {
        printf("Login yet!");
        exit(EXIT_FAILURE);
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
        exit(EXIT_FAILURE);
    }
    else
    {
        perror("An unknown error");
        exit(EXIT_FAILURE);
    }
}

void registerShareFile(int socket, uint32_t id, char *file_name)
{
}

int readAndSendFile(int client_socket, char *file_path)
{
    char buffer[256];
    int bytes_received;
    int file_size;

    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        perror("Error");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *token = strtok(file_path, "/");

    // Loop to find the last file name in the path
    char *file_name = NULL;
    while (token != NULL)
    {
        file_name = token; // Lưu giá trị token vào fileName
        token = strtok(NULL, "/");
    }

    // request send file with format " <name_file> <file_size> "
    sprintf(buffer, "%s %d", file_name, file_size);
    send(client_socket, buffer, 256, 0);

    recv(client_socket, buffer, bytes_received, 0);

    // read file and send file
    while (!feof(file))
    {
        bytes_received = fread(buffer, 1, sizeof(buffer), file);
        send(client_socket, buffer, bytes_received, 0);
    }

    fclose(file);
    return 1;
}

void shareFile(int socket)
{
}

void cancelShareFile(int socket)
{
}

void findFile(int socket)
{
}

void downloadFile(int socket)
{
}
