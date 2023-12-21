#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#include "serverHandler.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Sử dụng: %s <Port_Number> <Directory_name>\n", argv[0]);
        return 1;
    }

    int port_number = atoi(argv[1]);
    char *directory_name = argv[2];

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int server_socket = createSocket();
    if (server_socket == -1)
    {
        return 1;
    }

    int bind = bindServer(port_number, server_socket);
    if (bind == 0)
    {
        return 1;
    }

    /// Server handler

    listen(server_socket, 5);

    FILE *file;
    char buffer[256];
    int bytes_received;
    char message[256];

    while (1)
    {
        // connect with a client
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            perror("Lỗi khi chấp nhận kết nối");
            continue;
        }

        // connect is success
        printf("[+] Đã kết nối với %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        uint16_t port = ntohs(client_addr.sin_port);
        char ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), ip_address, INET_ADDRSTRLEN);

        sscanf("+OK Welcome to file server\n", "%[^\n]", message);
        send(client_socket, message, 256, 0);
        sscanf(message, "%[^\n]", buffer);
        write_log(port, ip_address, buffer);

        // request from client
        while (1)
        {
            bytes_received = recv(client_socket, buffer, 256, 0);
            buffer[bytes_received] = '\0';

            write_log(port, ip_address, buffer);

            char file_name[128];
            int file_size;
            sscanf(buffer, "UPLD %s %d", file_name, &file_size);

            // Create new file and wait send file from client
            char file_path[256];

            sprintf(file_path, "%s/%s", directory_name, file_name);
            file = fopen(file_path, "wb");
            if (file == NULL)
            {
                perror("Lỗi khi mở file");
                close(client_socket);
                continue;
            }

            sscanf("+OK Please send file\n", "%[^\n]", message);
            send(client_socket, message, 256, 0);

            // read file and write into new file
            bytes_received = 0;
            while (bytes_received < file_size)
            {
                int bytes = recv(client_socket, buffer, 256, 0);
                if (bytes == 0)
                {
                    close(client_socket);
                }
                else
                {
                    fwrite(buffer, 1, bytes, file);
                    bytes_received += bytes;
                }
            }

            fclose(file);

            sscanf("+OK Successful upload\n", "%[^\n]", message);
            send(client_socket, message, 256, 0);
            sscanf(message, "%[^\n]", buffer);
            write_log(port, ip_address, buffer);
        }
    }

    close(server_socket);

    return 0;
}
