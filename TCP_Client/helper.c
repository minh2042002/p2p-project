#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "helper.h"
#define BUFF_SIZE 256
/**
 * @brief get client's id from config file
 */
int getID()
{
    char line[100];
    uint32_t id;
    char *endptr;

    FILE *file = fopen("config.txt", "rb");
    if (file == NULL)
    {
        perror("Canot open file to get id!");
        return -1;
    }

    if (fgets(line, sizeof(line), file) != NULL)
    {
        id = strtoul(line, &endptr, 10);
        if (*endptr != '\0' && *endptr != '\n')
        {
            perror("Cannot convert to uint32_t!");
            id = -1;
        }
    }
    else
    {
        id = -1;
    }
    fclose(file);
    return id;
}

/**
 * @brief get file name from file path
 * @param path file's path
 * @param filename buffer to store file name
 *
 */
void getFileName(const char *path, char *filename)
{
    // Find end location of '/'
    const char *lastSlash = strrchr(path, '/');
    char directory[256];

    if (lastSlash != NULL)
    {
        // '/' is exists, copy directory before '/'
        strncpy(directory, path, lastSlash - path + 1);
        directory[lastSlash - path + 1] = '\0';
        strcpy(filename, lastSlash + 1);
    }
    else
    {
        directory[0] = '\0';
        strcpy(filename, path);
    }
}

/**
 * @brief create file if file is not exist
 * @param fileName file name
 */
void createFileIfNotExist(char *fileName)
{
    if (access(fileName, F_OK) == -1)
    {
        int fileDescriptor = open(fileName, O_CREAT, 0644);

        if (fileDescriptor == -1)
        {
            perror("Error: can not create file! \n");
            exit(EXIT_FAILURE);
        }
        else
        {
            close(fileDescriptor);
        }
    }
};

/**
 * @brief save share file path in to index file
 * @param path file's path
 * @param fileName file name
 */
void saveFile(char *path, char *fileName)
{
    createFileIfNotExist("index.txt");
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("ERROR: Can not open file!\n");
        exit(EXIT_FAILURE);
    }

    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        perror("ERROR: Can not create temp file!\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char line[BUFF_SIZE];
    char fileNameTmp[BUFF_SIZE];
    char filePath[BUFF_SIZE];
    int recordUpdated = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%s : %[^\n]", fileNameTmp, filePath) == 2)
        {
            if (strcmp(fileName, fileNameTmp) == 0)
            {
                snprintf(line, sizeof(line), "%s : %s\n", fileName, path);
                recordUpdated = 1;
            }
        }

        fprintf(tempFile, "%s", line);
    }

    if (!recordUpdated)
    {
        fprintf(tempFile, "%s : %s\n", fileName, path);
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
};

/**
 * @brief delete file path from index file
 * @param path file path to delete
 */
void deleteFile(char *path)
{
    FILE *file = fopen("index.txt", "r+");
    if (file == NULL)
    {
        perror("ERROR: Can not open file index.txt!");
        exit(EXIT_FAILURE);
    }

    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        perror("ERROR: Can not create temp file!\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    char line[BUFF_SIZE];
    char fileName[BUFF_SIZE];
    char filePath[BUFF_SIZE];

    int existFile = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%s : %[^\n]", fileName, filePath) == 2)
        {
            if (strcmp(fileName, path) == 0 || strcmp(filePath, path) == 0)
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
    }
};

/**
 * @brief get size of file
 * @param filePath file's path
 * @return file's size
 */
long long getFileSize(char *filePath)
{
    struct stat file_info;
    if (stat(filePath, &file_info) == 0)
    {
        return (long long)file_info.st_size;
    }
    else
    {
        printf("%s\n", filePath);
        printf("-Error Can not get file info\n");
        return -1;
    }
}

/**
 * @brief check file exist or not
 * @param filePath path of file
 * @return 1 if file is exist, 0 if file is not exist
 */
int checkFileExistOrNot(char *filePath)
{
    if (access(filePath, F_OK) != -1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
};
