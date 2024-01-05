#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "clientHandler.h"

#define BUFF_SIZE 256

void signup(int socket)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;
    uint32_t id;

    // request send protocol signup with format "SU"
    sprintf(buffer, "SU\r\n");
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    int result = sscanf(buffer, "%d - %u", &status, &id); // status: 100 and id
    if (result == 2)
    {
        printf("Sign up is succes.");

        FILE *file = fopen("config.txt", "w");
        if (file == NULL)
        {
            perror("Not open file and save id!");
        }
        else
        {
            fprintf(file, "%d", id);
        }

        fclose(file);
    }
    else
    {
        sscanf(buffer, "%d", &status); // status: 300
        if (status == 300)
        {
            printf("Protocol is wrong!");
        }
        else
        {
            printf("%s\n", buffer);
            perror("An unknown error");
        }
    }
}

void login(int socket, uint32_t id, int port)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;
    char data[250];

    // request send protocol login with format "SI <ID> <LISTEN_PORT>"
    sprintf(buffer, "SI %u %d\r\n", id, port);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 110)
    {
        printf("Login is success.");
    }
    else if (status == 211)
    {
        printf("Unsignuped!");
        exit(EXIT_FAILURE);
    }
    else if (status == 212)
    {
        printf("Login yet!");
        exit(EXIT_FAILURE);
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
        exit(EXIT_FAILURE);
    }
    else
    {
        perror("An unknown error");
        exit(EXIT_FAILURE);
    }
}

int registerShareFile(int socket, uint32_t id, char *file_name)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;

    // request send protocol register share file
    sprintf(buffer, "SH %u %s", id, file_name);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        return 0;
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 120)
    {
        printf("Register share file is success.");
        return 1;
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
    }
    else
    {
        perror("An unknown error!");
    }

    return 0;
}

int cancelShareFile(int socket, uint32_t id, char *file_name)
{
    char buffer[BUFF_SIZE];
    int bytes_received;
    int status;

    sprintf(buffer, "DF %u %s", id, file_name);
    send(socket, buffer, BUFF_SIZE, 0);

    bytes_received = recv(socket, buffer, BUFF_SIZE, 0);
    if (bytes_received == 0)
    {
        perror("An error occurred!");
        return 0;
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &status);
    if (status == 150)
    {
        printf("Successfully unsubscribed file sharing");
        return 1;
    }
    else if (status == 300)
    {
        printf("Protocol is wrong!");
    }
    else
    {
        perror("An unknown error!");
    }

    return 0;
}

void findFile(int socket)
{
}

void downloadFile(int socket)
{
}

void saveFile(char *path)
{
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("Cannot open file index.txt!");
        return;
    }

    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        perror("Cannot open temp file!");
        fclose(file);
        return;
    }

    char line[BUFF_SIZE];
    char pathInFile[BUFF_SIZE];
    int exists = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%[^\n]", pathInFile))
        {
            if (strcmp(path, pathInFile) == 0)
            {
                exists = 1;
            }
        }

        fprintf(tempFile, "%s", line);
    }

    if (exists)
    {
        printf("%s already exists in index.txt", path);
        fclose(file);
        fclose(tempFile);
    }
    else
    {
        snprintf(line, sizeof(line), "%s", path);
        fprintf(tempFile, "%s", line);

        fclose(file);
        fclose(tempFile);

        file = fopen("index.txt", "w");
        if (file == NULL)
        {
            perror("Cannot open file index.txt!");
            return;
        }

        rewind(tempFile);
        char c;
        while ((c = fgetc(tempFile)) != EOF)
        {
            fputc(c, file);
        }

        fclose(file);
    }
}

void deleteFile(char *path)
{
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("Cannot open file index.txt!");
        return;
    }

    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        perror("Cannot open temp file!");
        fclose(file);
        return;
    }

    char line[BUFF_SIZE];
    char pathInFile[BUFF_SIZE];
    int exists = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%[^\n]", pathInFile))
        {
            if (strcmp(path, pathInFile) == 0)
            {
                exists = 1;
            }
            else
            {
                fprintf(tempFile, "%s", line);
            }
        }
    }

    fclose(file);
    fclose(tempFile);

    if (exists)
    {
        file = fopen("index.txt", "w");
        if (file == NULL)
        {
            perror("Cannot open file index.txt!");
            return;
        }

        rewind(tempFile);
        char c;
        while ((c = fgetc(tempFile)) != EOF)
        {
            fputc(c, file);
        }

        fclose(file);
    }
    else
    {
        printf("%s is not exists in index.txt", path);
    }
}