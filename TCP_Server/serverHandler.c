#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#include "serverHandler.h"

/// @brief create new socket
/// @return a socket nnn)
int createSocket()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("\nError socket(): ");
        exit(EXIT_FAILURE);
    }
    return server_socket;
}

uint32_t generateClientID(int client_socket)
{
    uint32_t random_id = arc4ramdom(); // Use arc4random()
    return random_id;
}

uint32_t findClient(char *ip_address, uint16_t port)
{
    FILE *file;
    char line[256];
    char search_string
}

void saveClientId(uint32_t client_id, char *ip_address, uint16_t port)
{
    char log_entry[256];
    sprintf(log_entry, "[%s:%d] $ %d", ip_address, port, client_id);

    FILE *log_file = fopen("clientIds.txt", "a");
    if (log_file)
    {
        time_t t = time(NULL);
        struct tm ltm = *localtime(&t);

        fprintf(log_file, "%s\n", log_entry);
        fclose(log_file);
    }
}
/// @brief write log to file
/// @param client_addr contain ip address and port of client
/// @param buffer information
void write_log(uint16_t port, char *ip_address, const char *buffer)
{
    char log_entry[256];
    sprintf(log_entry, "[%s:%d]$%s", ip_address, port, buffer);

    FILE *log_file = fopen("log.txt", "a");
    if (log_file)
    {
        time_t t = time(NULL);
        struct tm ltm = *localtime(&t);

        fprintf(log_file, "[%02d/%02d/%04d %02d:%02d:%02d] $ %s\n",
                ltm.tm_mday,
                ltm.tm_mon + 1,
                ltm.tm_year + 1900,
                ltm.tm_hour,
                ltm.tm_min,
                ltm.tm_sec,
                log_entry);
        fclose(log_file);
    }
}