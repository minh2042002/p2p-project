#include <stdlib.h>
#include <stdio.h>
#include "socketp2p.h"

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
/// @param client_sock a socket descriptor
/// @return 0 - if connect fail, 1 - if connect is success
void connectSocket(int client_socket, char *server_ip, int port_number)
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
}

/// @brief bind server with port and socket descriptor
/// @param port_number port server
/// @param listen_sock  a socket descriptor
/// @return 0 nnn
void bindSocket(int server_socket, int port_number)
{
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("\nError bind(): ");
        exit(EXIT_FAILURE);
    }
}

void listenSocket(int listen_socket, int backlog)
{
    if (listen(listen_socket, backlog) == -1)
    {
        perror("\nError listen(): ");
        exit(EXIT_FAILURE);
    }
}

int acceptSocket(int server_socket, struct sockaddr *client_addr, socklen_t *client_len)
{
    int conn_sock = accept(server_socket, (struct sockaddr *)&client_addr, client_len);
    if (conn_sock < 0)
    {
        perror("\nError accept(): ");
        exit(EXIT_FAILURE);
    }
    return conn_sock;
}