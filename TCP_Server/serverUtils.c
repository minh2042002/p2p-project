#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include "serverUtils.h"
#include "Client.h"
#define BUFF_SIZE 256

pthread_mutex_t indexFileMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief generate client's id random
 * @return random id
 */
uint32_t generateClientID()
{
    uint32_t random_id = (uint32_t)rand();
    return random_id;
}

/**
 * @brief delete shared file from index file
 * @param client_id client's id
 * @param file_name file name to delete
 * @return 1 if success, 0 if fail
 */
int deleteIndex(uint32_t client_id, const char *file_name)
{
    int status = 0;
    pthread_mutex_lock(&indexFileMutex);
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("Cannot open file index.txt!");
        exit(EXIT_FAILURE);
    }
    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        perror("Error: can not create temp file!\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    char line[BUFF_SIZE];
    uint32_t id;
    char ip[16];
    uint16_t port;
    char filename[BUFF_SIZE];

    int existFile = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%u %15s %hu %[^\n]", &id, ip, &port, filename) == 4)
        {
            if (id == client_id && strcmp(filename, file_name) == 0)
            {
                existFile = 1;
            }
            else
            {
                fprintf(tempFile, "%s", line);
            }
        }
    }
    fclose(file);
    if (existFile)
    {
        file = fopen("index.txt", "w");
        if (file == NULL)
        {
            perror("Error: Can not open file!\n");
            exit(EXIT_FAILURE);
        }
        rewind(tempFile);
        char c;
        while ((c = fgetc(tempFile)) != EOF)
        {
            fputc(c, file);
        }
        fclose(file);
        fclose(tempFile);
        status = 1;
    }
    else
    {
        status = 0;
    }
    pthread_mutex_unlock(&indexFileMutex);
    return status;
}

/**
 * @brief update share file index
 * @param client_ip client's ip
 * @param client_port client's port
 * @param file_name file name
 * @return 1 if success
 *
 */
int updateIndex(uint32_t client_id, char *client_ip, uint16_t client_port, char *file_name)
{

    pthread_mutex_lock(&indexFileMutex);
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("Error: Can not open file!\n");
        exit(EXIT_FAILURE);
    }
    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        perror("Error: Can not create temp file!\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    char line[BUFF_SIZE];
    uint32_t id;
    char ip[BUFF_SIZE];
    uint16_t port;
    char filename[BUFF_SIZE];

    int recordUpdated = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%u %15s %hu %[^\n]", &id, ip, &port, filename) == 4)
        {
            if (id == client_id && strcmp(filename, file_name) == 0)
            {
                snprintf(line, sizeof(line), "%u %s %u %s\n", client_id, client_ip, client_port, file_name);
                recordUpdated = 1;
            }
        }

        fprintf(tempFile, "%s", line);
    }

    if (!recordUpdated)
    {
        fprintf(tempFile, "%u %s %u %s\n", client_id, client_ip, client_port, file_name);
    }
    fclose(file);
    file = fopen("index.txt", "w");
    if (file == NULL)
    {
        perror("ERROR: Can not open file!\n");
        exit(EXIT_FAILURE);
    }
    rewind(tempFile);
    char c;
    while ((c = fgetc(tempFile)) != EOF)
    {
        fputc(c, file);
    }
    fclose(file);
    fclose(tempFile);
    pthread_mutex_unlock(&indexFileMutex);
    return 1;
}

/**
 * @brief get client info from socket
 * @param socket client's socket description
 * @param client_ip buffer to store client's ip
 * @param client_port buffer to store client's port
 * @return 1 if success, 0 if fail
 */
int getInfoClient(int socket, char *client_ip, uint16_t *client_port)
{
    // Get IP and PORT of client from connfd
    struct sockaddr_in peer_addr;
    socklen_t peer_len = sizeof(peer_addr);
    if (getpeername(socket, (struct sockaddr *)&peer_addr, &peer_len) == -1)
    {
        perror("Unable to get client information!");
        return 0;
    }

    inet_ntop(AF_INET, &(peer_addr.sin_addr), client_ip, 16);
    *client_port = ntohs(peer_addr.sin_port);

    return 1;
}

/// @brief write log to file
/// @param ip_address contain ip address and port of client
/// @param buffer information
void write_log(uint16_t port, char *ip_address, const char *buffer)
{
    char log_entry[BUFF_SIZE];
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