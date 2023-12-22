#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

int createSocket();
int connectServer(char *server_ip, int port_number, int client_socket);
void receive(int client_socket, char *buffer, int *bytes_received);
int readAndSendFile(int client_socket, char *file_path);

#endif