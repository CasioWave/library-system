#include <stdio.h>

#include "ui.h"
#include "library.h"
#include "login.h"


int main() {
    printf("main.c\n");
    helloFromUi();
    helloFromLibrary();
    helloFromLogin();
    return 0;
}
