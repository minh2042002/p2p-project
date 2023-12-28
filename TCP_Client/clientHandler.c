#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "clientHandler.h"

/// @brief create new socket
/// @return a socket descriptor return -1 if create fail)
int createSocket()
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("\nError socket(): ");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

/// @brief connect server with socket
/// @param server_ip server address
/// @param port_number port of server
/// @param client_socket a socket descriptor
/// @return 0 - if connect fail, 1 - if connect is success
int connectServer(int client_socket, char *server_ip, int port_number)
{
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("\nError connect(): ");
        exit(EXIT_FAILURE);
    }

    return 1;
}

void registerIndex(int socket)
{
    char buffer[256];
    int bytes_received;
    int status;
    uint32_t index;

    // request send protocol register with format "SU"
    sprintf(buffer, "SU");
    send(socket, buffer, 256, 0);

    bytes_received = recv(socket, buffer, 256, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    int result = sscanf(buffer, "%d - %u", &status, index); // status: 100 and index
    if (result == 2)
    {
        printf("Sign up is succes.");

        FILE *file = fopen("config.txt", "w");
        if (file == NULL)
        {
            perror("Not open file and save index!");
        }
        else
        {
            fprintf(file, index);
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
        else {
            perror("An unknown error");
        }
    }
}

void login(int socket, uint32_t index, int port)
{
    char buffer[256];
    int bytes_received;
    int status;
    char data[250];

    // request send protocol login with format "SI <ID> <LISTEN_PORT>"
    sprintf(buffer, "SI %u %d", index, port);
    send(socket, buffer, 256, 0);

    bytes_received = recv(socket, buffer, 256, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", status);
    if (status == 110)
    {
        printf("Login is success.");
    }
    else if (status == 211)
    {
        printf("Unregistered!");
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
    }
    else {
        perror("An unknown error");
    }
}

int readAndSendFile(int client_socket, char *file_path)
{
    char buffer[256];
    int bytes_received;
    int file_size;

    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        perror("Không thể mở file");
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

    receive(client_socket, buffer, &bytes_received);

    // read file and send file
    while (!feof(file))
    {
        bytes_received = fread(buffer, 1, sizeof(buffer), file);
        send(client_socket, buffer, bytes_received, 0);
    }

    fclose(file);
    return 1;
}
