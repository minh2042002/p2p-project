#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H
#include <stdint.h>

int createSocket();
int bindServer(int port_number, int server_socket);
uint32_t generateClientID(int client_socket);
void write_log(uint16_t port, char *ip_address, const char *buffer);
void logout();
#endif