int readAndSendFile(int client_socket, char *file_path);
void registerIndex(int socket);
void login(int socket, int id);
void shareFile(int socket);
void cancelShareFile(int socket);
void findFile(int socket);
void downloadFile(int socket);