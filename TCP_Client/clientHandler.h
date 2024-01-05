#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H
#include <stdint.h>

int createSocket();
int readAndSendFile(int client_socket, char *file_path);
void signup(int socket);
void login(int socket, uint32_t id, int port);
int registerShareFile(int socket, uint32_t id, char *file_name);
int cancelShareFile(int socket, uint32_t id, char *file_name);
void findFile(int socket);
void downloadFile(int socket);
void saveFile(char *path);
void deleteFile(char *path);
#endif