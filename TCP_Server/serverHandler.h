#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H
#include <stdint.h>

void signup(int connfd);
uint32_t generateClientID();
int updateIndex(uint32_t client_id, char *client_ip, uint16_t client_port, char *file_name);
int deleteIndex(uint32_t client_id, const char *file_name);
int getInfoClient(int socket, char *client_ip, uint16_t *client_port);
void write_log(uint16_t port, char *ip_address, const char *buffer);
void logout();
#endif