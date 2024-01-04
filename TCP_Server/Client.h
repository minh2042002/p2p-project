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

struct Client *create(uint32_t id, char *ip, int port);
void add(struct Client **head, struct Client *newClient);
struct Client *find(struct Client *head, uint32_t id);
void deleteByID(struct Client **head, uint32_t id);
void update(struct Client *client, const char *newIP);
void login(struct Client *client);
void logout(struct Client *client);
void saveAll(struct Client *head);
void loadFromFile(struct Client **head);
int count(struct Client *head);


#endif