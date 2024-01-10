#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H
#include "Client.h"

void loadClients();
void SignUpHandler(int connfd, struct Client **client, char *command, int *loginStatus);
void SignInHandler(int connfd, struct Client **client, char *command, int *loginStatus);
void RegisterShareHandler(int connfd, char *command, int *loginStatus);
void CancelShareHandler(int connfd, char *command, int *loginStatus);
void FindShareFileHandler(int connfd, char *command, int *loginStatus);
void CheckFileHandler(int connfd, char *command, int *loginStatus);
void removeBugFile(int connfd, char *command, int *loginStatus);
#endif