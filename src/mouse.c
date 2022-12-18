#include "mouse.h"
#include <SDL_mouse.h>

static int newLeft = 0, leftReset = 1, newRight = 0, rightReset = 1;

void mouseCheck() {
    if (!newLeft && leftReset && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LEFT) {
        newLeft = 1;
        leftReset = 0;
    }
    else if (newLeft) {
        newLeft = 0;
    }
    if (!newRight && rightReset && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RIGHT) {
        newRight = 1;
        rightReset = 0;
    }
    else if (newRight) {
        newRight = 0;
    }

    if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LEFT)) {
        leftReset = 1;
    }
    if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RIGHT)) {
        rightReset = 1;
    }
}

int mouseDownLeft() {
    return newLeft;
}
int mouseLeft() {
    return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LEFT;
}
int mouseUpLeft();
int mouseDownRight() {
    return newRight;
}
int mouseRight() {
    return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RIGHT;
}
int mouseUpRight();