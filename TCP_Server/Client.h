#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

struct Client
{
    uint32_t index;
    struct sockaddr_in _addr;
    int isLogin; // 1 - login, 0 - logout
};

#endif