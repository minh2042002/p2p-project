#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "clientHandler.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Sử dụng: %s <IP_Addr> <Port_Number>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port_number = atoi(argv[2]);

    int client_socket = createSocket();
    if (client_socket == -1)
    {
        return 1;
    }
    
    int connect = connectServer(server_ip, port_number, client_socket);
    if (connect == 0)
    {
        return 1;
    }


    /// Client handler

    char buffer[256];
    int bytes_received;

    received(client_socket, buffer, &bytes_received);

    char file_path[128];
    int file_size;

    while (1)
    {
        printf("Nhập đường dẫn file hoặc nhấn Enter để kết thúc: ");
        fgets(file_path, sizeof(file_path), stdin);
        file_path[strcspn(file_path, "\n")] = '\0';

        if (strlen(file_path) == 0)
        {
            break;
        }

        FILE *file = fopen(file_path, "rb");
        if (file == NULL)
        {
            perror("Không thể mở file");
            continue;
        }

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *token = strtok(file_path, "/");

        // Loop to find the last file name in the path
        char *file_name = NULL;
        while (token != NULL) {
            file_name = token; // Lưu giá trị token vào fileName
            token = strtok(NULL, "/");
        }   

        // request send file with format " UPLD <name_file> <file_size> "
        sprintf(buffer, "UPLD %s %d", file_name, file_size);
        send(client_socket, buffer, 256, 0);

        received(client_socket, buffer, &bytes_received);

        // read file and send file
        while (!feof(file))
        {
            bytes_received = fread(buffer, 1, sizeof(buffer), file);
            send(client_socket, buffer, bytes_received, 0);
        }

        fclose(file);

        received(client_socket, buffer, &bytes_received);
    }

    close(client_socket);

    return 0;
}
