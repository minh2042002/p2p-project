int createSocket();
void connectSocket(int client_socket, char *server_ip, int port_number);
void bindSocket(int port_number, int server_socket);
void listenSocket(int listen_sock, int backlog);
int acceptSocket(int server_sock, struct sockaddr *client_addr, socklen_t *client_len);