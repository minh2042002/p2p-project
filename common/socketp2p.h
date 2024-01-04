#ifndef SOCKET_P2P_H
#define SOCKET_P2P_H
int createSocket();
void connectSocket(int client_socket, char *server_ip, int port_number);
void bindSocket(int server_socket, int port_number);
void listenSocket(int listen_socket, int backlog);
int acceptSocket(int server_socket, struct sockaddr *client_addr, socklen_t *client_len);
#endif