#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "clientHandler.h"

/// @brief receive response from server
/// @param client_socket a socket descriptor
/// @param buffer response from server
// void receive(int client_socket, char *buffer, int *bytes_received)
// {
//     *bytes_received = recv(client_socket, buffer, 256, 0);
//     if (*bytes_received > 0)
//     {
//         printf("%s\n", buffer);
//     }
// }

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
