#ifndef USER_H
#define USER_H

#define MAX_USERS 100

typedef struct {
    char id[20];
    char pw[20];
} USER;

extern USER users[];
extern int userCount;

void loadUsersFromFile(const char *filename);
int login();

#endif