#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#include "serverHandler.h"
#include "Client.h"

#define BUFF_SIZE 256

uint32_t generateClientID()
{
    uint32_t random_id = (uint32_t)rand();
    return random_id;
}

uint32_t findClient(char *ip_address, uint16_t port)
{
    FILE *file;
    char line[BUFF_SIZE];
    char search_string;
}

int deleteIndex(uint32_t client_id, const char *file_name)
{
    FILE *file = fopen("index.txt", "r+"); // Mở file để đọc và ghi
    if (file == NULL)
    {
        perror("Cannot open file index.txt!");
        return 2; // Trả về 2 để biểu thị không thể mở file
    }

    FILE *tempFile = tmpfile(); // Mở một file tạm thời
    if (tempFile == NULL)
    {
        perror("Không thể tạo file tạm thời");
        fclose(file);
        return 2; // Trả về 2 để biểu thị không thể tạo file tạm thời
    }

    // Kiểm tra từng dòng trong file
    char line[BUFF_SIZE];
    uint32_t id;
    char ip[16];
    uint16_t port;
    char filename[BUFF_SIZE];

    int existFile = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Sử dụng sscanf để đọc từng trường từ dòng
        if (sscanf(line, "%u %15s %hu %[^\n]", &id, ip, &port, filename) == 4)
        {
            // So sánh id và filename với giá trị mong muốn
            if (id == client_id && strcmp(filename, file_name) == 0)
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
    fclose(tempFile);

    if (existFile)
    {
        // Mở file gốc để ghi lại nội dung từ tempFile
        file = fopen("index.txt", "w");
        if (file == NULL)
        {
            perror("Không thể mở file");
            return 2; // Trả về 0 để biểu thị không thể mở file
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
        return 1; // Trả về 1 để biểu thị đã xóa thành công
    }
    else
    {
        return 0;
    }
}

int updateIndex(uint32_t client_id, char *client_ip, uint16_t client_port, char *file_name)
{
    FILE *file = fopen("index.txt", "r+"); // Mở file để đọc
    if (file == NULL)
    {
        perror("Không thể mở file");
        return 0; // Trả về 0 để biểu thị không tìm thấy
    }

    FILE *tempFile = tmpfile(); // Mở một file tạm thời
    if (tempFile == NULL)
    {
        perror("Không thể tạo file tạm thời");
        fclose(file);
        return 0; // Trả về 0 để biểu thị không thể tạo file tạm thời
    }

    // Kiểm tra từng dòng trong file
    char line[BUFF_SIZE];
    uint32_t id;
    char ip[BUFF_SIZE];
    uint16_t port;
    char filename[BUFF_SIZE];

    int recordUpdated = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Sử dụng sscanf để đọc từng trường từ dòng
        if (sscanf(line, "%u %15s %hu %[^\n]", &id, ip, &port, filename) == 4)
        {
            // So sánh id và filename với giá trị mong muốn
            if (id == client_id && strcmp(filename, file_name) == 0)
            {
                snprintf(line, sizeof(line), "%u %s %u %s", client_id, client_ip, client_port, file_name);
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
    fclose(tempFile);

    // Mở file gốc để ghi lại nội dung từ tempFile
    file = fopen("index.txt", "w");
    if (file == NULL)
    {
        perror("Không thể mở file");
        return 0;
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
    return 1;
}

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

    inet_ntop(AF_INET, &(peer_addr.sin_addr), client_ip, sizeof(client_ip));
    *client_port = ntohs(peer_addr.sin_port);

    return 1;
}

/// @brief write log to file
/// @param client_addr contain ip address and port of client
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