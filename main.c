#include <unistd.h>

#include "ui.h"

int main() {
    init();
    while(1) {
        refreshScreen();
        handleKeyPress();
    }
    return 0;
}
