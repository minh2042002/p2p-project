#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

struct Client
{
    uint32_t id;
    struct sockaddr_in _addr;
    int isLogin; // 1 - login, 0 - logout
    struct Client *next;
    struct Client *prev;
};

#endif