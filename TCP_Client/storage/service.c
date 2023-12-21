#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "service.h"


/// @brief Tìm kiếm địa chỉ IPv4 của một tên miền bất kỳ
/// @param domainName Tên miền của địa chỉ IPv4 cần tìm
void findIPAddressByDomainName(char *domainName) {
    struct addrinfo hints, *result, *ptr;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(domainName, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "Không thể phân giải tên miền.\n");
        return;
    }

    printf("Result:\n");
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        void *addr;
        char ip_address[INET6_ADDRSTRLEN];

        if (ptr->ai_family == AF_INET) { // Check là IPv4 thì in ra
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
            inet_ntop(ptr->ai_family, addr, ip_address, sizeof(ip_address));
            printf("%s\n", ip_address);
        } 

        
    }

    freeaddrinfo(result);
}


/// @brief Tìm kiếm tên miền từ một IP đã cho
/// @param ipAddress IP của tên miền cần tìm kiếm (IPv4)
void findDomainNameByIPAddress(char *ipAddress) {
    struct sockaddr_in sa;
    char host[NI_MAXHOST];

    memset(&sa, 0, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress, &(sa.sin_addr));

    int status = getnameinfo((struct sockaddr *)&sa, sizeof(struct sockaddr), host, NI_MAXHOST, NULL, 0, NI_NAMEREQD);

    if (status == 0) {
        printf("Result:\n");
        printf("%s\n", host);
    } else {
        printf("\nNot found information\n");
    }
}

/// @brief Kiểm tra chuỗi nhập vào có đúng định dạng IPv4
/// @param input Chuỗi nhập vào
/// @return 
int is_valid_ipv4(const char *input) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, input, &(sa.sin_addr)) != 0;
}

/// @brief kiểm tra chuỗi nhập vào có đúng là một domain
/// @param input Chuỗi nhập vào
/// @return 
int is_valid_domain(const char *input) {
    // Kiểm tra chuỗi ký tự chỉ chứa chữ cái, số và dấu chấm)
    for (int i = 0; input[i] != '\0'; ++i) {
        if (!isalnum(input[i]) && input[i] != '.') {
            return 0;
        }
    }

    // Kiểm tra định dạng của domain (ít nhất một dấu chấm và không bắt đầu hoặc kết thúc bằng dấu chấm)
    int dot_count = 0;
    int len = strlen(input);
    for (int i = 0; i < len; ++i) {
        if (input[i] == '.') {
            dot_count++;
            if (i == 0 || i == len - 1) {
                return 0;
            }
        }
    }
    return 1;
}
