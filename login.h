#pragma once

#define LOGIN_SUCCESS 0 
#define SIGNUP 1
#define LOGIN_FAILURE -1
#define LOGIN_MAXLIM 255

enum PRIV {
    ADMIN=1,
    FACULTY=2,
    STUDENT=4
};

typedef struct {
    char* username;
    char* password;
    int priv;
} User;

User* fetchUsers(char*);
int login(int *, char**);
void registerUser(char*, char*);
void registerUser(char* username, char* password);
