#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

int createSocket();
int connectServer(char *server_ip, int port_number, int client_socket);
void received(int client_socket, char *buffer, int* bytes_received);

#endif