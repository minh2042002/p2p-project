int createSocket();
int connectServer(int client_socket, char *server_ip, int port_number);
// void receive(int client_socket, char *buffer, int *bytes_received);
int readAndSendFile(int client_socket, char *file_path);
void registerIndex(int socket);
void login(int socket, int id);
