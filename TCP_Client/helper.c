#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#include "helper.h"

int getIndex() {
    char line[100];
    int index;
    
    FILE* file = fopen("config.txt", "rb");
    if (file == NULL) {
        perror("Canot open file to get index!");
        return -1;
    }

    // Đọc dòng từ file
    if (fgets(line, sizeof(line), file) != NULL) {
        // In ra dòng đã đọc được
        index = atoi(line);
    } else {
        index = -1;
    }

    // Đóng file sau khi đọc xong
    fclose(file);
    return index;
}