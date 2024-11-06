#include <unistd.h>

#include "ui.h"

int main() {
    init();
    while(1) {
        refreshScreen();
        handleKeyPress();
    }
    disableRawMode();
    return 0;
}
