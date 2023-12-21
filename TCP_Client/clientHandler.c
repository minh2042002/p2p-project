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
        perror("Không thể tạo socket");
    }

    return client_socket;
}

/// @brief connect server with socket
/// @param server_ip server address
/// @param port_number port of server
/// @param client_socket a socket descriptor
/// @return 0 - if connect fail, 1 - if connect is success
int connectServer(char *server_ip, int port_number, int client_socket)
{
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Không thể kết nối đến server");
        return 0;
    }

    return 1;
}

/// @brief receive response from server
/// @param client_socket a socket descriptor
/// @param buffer response from server
void received(int client_socket, char* buffer, int* bytes_received)
{
    *bytes_received = recv(client_socket, buffer, 256, 0);
        if (*bytes_received > 0) 
        {
            printf("%s\n", buffer);
        }
}
