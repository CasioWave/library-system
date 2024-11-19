#include "ui.h"

int main() {
    // initialise the UI
    init();
    // mainloop
    while(1) {
        refreshScreen();
        handleKeyPress();
    }
    return 0;
}
