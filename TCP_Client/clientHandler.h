#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H
#include <stdint.h>

void RegisterShareFileHandler(int client_socket, uint32_t id);
void CancelShareFileHandler(int client_socket, uint32_t id);
void FindFileHandler(int client_socket);
void DownloadFileHandler(int client_socket);

int createSocket();
void signup(int socket, int listen_port);
void login(int socket, uint32_t id, int port);
int registerShareFile(int socket, uint32_t id, char *file_name);
int cancelShareFile(int socket, uint32_t id, char *file_name);
void downloadFile(int socket, char *fileName, char *clientIP, int port);
void sendFile(int socket, char *filePath);
void storeFile(int conn_sock, char *fileName, long long fileSize);

#endif