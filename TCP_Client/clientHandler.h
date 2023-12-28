int createSocket();
int connectServer(int client_socket, char *server_ip, int port_number);
int readAndSendFile(int client_socket, char *file_path);
void register(int socket);
void login(int socket, uint32_t id, int port);
void regisShareFile(int socket, uint32_t id, char* file_name);