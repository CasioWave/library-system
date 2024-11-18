#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "login.h"
#include "utils.h"

int NUSERS = 0;

User* fetchUsers(char *filename) {
    FILE* fp;
    fp = fopen(filename, "r");
    CSV userData;
    userData = readCSV(fp);
    fclose(fp);
    if (userData.nrows == 0) return NULL;
    User* users;
    users = calloc(userData.nrows, sizeof(User));

    for (int i = 0; i < userData.nrows; ++i) {
        users[i].username = strdup(userData.data[i][0]);
        users[i].password = strdup(userData.data[i][1]);
        users[i].priv = atoi(userData.data[i][2]);
    }

    NUSERS = userData.nrows;
    for (int i = 0; i < userData.nrows; ++i) {
        free(userData.data[i]);
    }
    free(userData.data);

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


    int ret = LOGIN_FAILURE;
    User* users;
    users = fetchUsers("users.csv");
    int usrMatch = 0;
    for (int i = 0; i < NUSERS; ++i) {
       if (strcmp(users[i].username, username) == 0) {
           usrMatch++;
           if (strcmp(users[i].password, password) == 0) {
               *uname = strdup(username);
               *priv = users[i].priv;
               ret =  LOGIN_SUCCESS;
               break;
           }
       }
    }
    if (usrMatch == 0) {
        char choice[LOGIN_MAXLIM];
        printf("You seem to be a new user. Would you like to create an account ? (y/n): ");
        fgets(choice, LOGIN_MAXLIM, stdin);
        if (choice[0] == 'y' || choice[1] == 'Y') {
            registerUser(username, password);
            printf("You have been signed up as a student. To change your priviledges, contact the admin. You'll now be logged In automatically please wait.\n");
            *uname = strdup(username);
            *priv = STUDENT;
            ret =  SIGNUP;
            sleep(3);
        } 
    }
    for (int i = 0; i < NUSERS; ++i) {
        free(users[i].password);
        free(users[i].username);
    }
    free(users);
    return ret;
}

void registerUser(char* username, char* password) {
    FILE* fp = NULL;
    fp = fopen("users.csv", "a");
    fprintf(fp, "\n%s,%s,%d", username, password, STUDENT);
    fclose(fp);
}
