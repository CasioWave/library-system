#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "login.h"
#include "utils.h"

int NUSERS = 0;

User* fetchUsers(char *filename) {
    FILE* fp;
    fp = fopen(filename, "r");
    CSV userData;
    userData = readCSV(fp);
    fclose(fp);

    User* users;
    users = calloc(userData.nrows, sizeof(User));

    for (int i = 0; i < userData.nrows; ++i) {
        users[i].username = strdup(userData.data[i][0]);
        users[i].password = strdup(userData.data[i][1]);
        users[i].priv = atoi(userData.data[i][2]);
    }

    NUSERS = userData.nrows;

    return users;
}

int login(int *priv, char ** uname) {
    char username[LOGIN_MAXLIM];
    char password[LOGIN_MAXLIM];
    printf("Username: ");
    fgets(username, LOGIN_MAXLIM, stdin);
    username[strlen(username) - 1] = '\0';
    printf("Password: ");
    fgets(password, LOGIN_MAXLIM, stdin);
    password[strlen(password) - 1] = '\0';


    User* users;
    users = fetchUsers("users.csv");

    for (int i = 0; i < NUSERS; ++i) {
       if (strcmp(users[i].username, username) == 0) {
           if (strcmp(users[i].password, password) == 0) {
               *uname = strdup(username);
               *priv = users[i].priv;
               return LOGIN_SUCCESS;
           }
       }
    }
    return LOGIN_FAILURE;
}
