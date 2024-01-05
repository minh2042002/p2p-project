#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "helper.h"

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

    // Đọc dòng từ file
    if (fgets(line, sizeof(line), file) != NULL)
    {
        // In ra dòng đã đọc được
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

    // Đóng file sau khi đọc xong
    fclose(file);
    return id;
}

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
