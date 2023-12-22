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

    receive(client_socket, buffer, &bytes_received);

    char file_path[128];

    while (1)
    {
        printf("Nhập đường dẫn file hoặc nhấn Enter để kết thúc: ");
        fgets(file_path, sizeof(file_path), stdin);
        file_path[strcspn(file_path, "\n")] = '\0';

        if (strlen(file_path) == 0)
        {
            break;
        }

        int res = readAndSendFile(client_socket, file_path);
        if (!res) {
            continue;
        }

        receive(client_socket, buffer, &bytes_received);
    }

    close(client_socket);

    return 0;
}