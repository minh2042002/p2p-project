int createSocket();
int connectServer(int client_socket, char *server_ip, int port_number);
int readAndSendFile(int client_socket, char *file_path);
void registerIndex(int socket);
void login(int socket, int index, int port);
