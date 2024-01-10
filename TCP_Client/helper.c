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
void createFileIfNotExist(char *fileName)
{
    if (access(fileName, F_OK) != -1)
    {
    }
    else
    {
        printf("File chua ton tai, tao file...\n");
        int fileDescriptor = open(fileName, O_CREAT, 0644);

        if (fileDescriptor == -1)
        {
            perror("Loi khi tao file");
            exit(EXIT_FAILURE);
        }
        else
        {
            close(fileDescriptor); // Đóng file sau khi tạo
        }
    }
};
void saveFile(char *path, char *fileName)
{
    createFileIfNotExist("index.txt");
    FILE *file = fopen("index.txt", "r+"); // Mở file để đọc
    if (file == NULL)
    {
        perror("Không thể mở file");
        exit(EXIT_FAILURE);
    }

    FILE *tempFile = tmpfile(); // Mở một file tạm thời
    if (tempFile == NULL)
    {
        perror("Không thể tạo file tạm thời");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Kiểm tra từng dòng trong file
    char line[BUFF_SIZE];
    char fileNameTmp[BUFF_SIZE];
    char filePath[BUFF_SIZE];
    int recordUpdated = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Sử dụng sscanf để đọc từng trường từ dòng
        if (sscanf(line, "%s : %[^\n]", fileNameTmp, filePath) == 2)
        {
            // So sánh id và filename với giá trị mong muốn
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

    // Mở file gốc để ghi lại nội dung từ tempFile
    file = fopen("index.txt", "w");
    if (file == NULL)
    {
        perror("Không thể mở file");
        exit(EXIT_FAILURE);
    }

    // Copy từ tempFile vào file gốc
    rewind(tempFile);
    char c;
    while ((c = fgetc(tempFile)) != EOF)
    {
        fputc(c, file);
    }

    // Đóng file gốc và xóa file tạm thời
    fclose(file);
    fclose(tempFile);
};
void deleteFile(char *path)
{
    FILE *file = fopen("index.txt", "r+"); // Mở file để đọc và ghi
    if (file == NULL)
    {
        perror("Cannot open file index.txt!");
        exit(EXIT_FAILURE);
    }

    FILE *tempFile = tmpfile(); // Mở một file tạm thời
    if (tempFile == NULL)
    {
        perror("Không thể tạo file tạm thời");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Kiểm tra từng dòng trong file
    char line[BUFF_SIZE];
    char fileName[BUFF_SIZE];
    char filePath[BUFF_SIZE];

    int existFile = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Sử dụng sscanf để đọc từng trường từ dòng
        if (sscanf(line, "%s : %[^\n]", fileName, filePath) == 2)
        {
            // So sánh id và filename với giá trị mong muốn
            if (strcmp(fileName, path) == 0 || strcmp(filePath, path) == 0)
            {
                existFile = 1;
            }
            else
            {
                // Không phải dòng cần xóa, ghi vào tempFile
                fprintf(tempFile, "%s", line);
            }
        }
    }

    // Đóng file gốc và file tạm thời
    fclose(file);

    if (existFile)
    {
        // Mở file gốc để ghi lại nội dung từ tempFile
        file = fopen("index.txt", "w");
        if (file == NULL)
        {
            perror("Không thể mở file");
            exit(EXIT_FAILURE);
        }

        // Copy từ tempFile vào file gốc
        rewind(tempFile);
        char c;
        while ((c = fgetc(tempFile)) != EOF)
        {
            fputc(c, file);
        }

        // Đóng file gốc và xóa file tạm thời
        fclose(file);
        fclose(tempFile);
    }
    else
    {
    }
};
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