#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

struct keyboard_state {
    uint8_t start;
    uint8_t mode;
    uint8_t cancel;
    uint8_t left;
    uint8_t stop;
    uint8_t menu;
    uint8_t up;
    uint8_t down;
    uint8_t reset;
    uint8_t data;
    uint8_t enter;
    uint8_t right;
};

struct keyboard {
    int fd;
    struct keyboard_state counts;
    struct keyboard_state state;
};

int keyboard_init(struct keyboard *kb);
void keyboard_deinit(struct keyboard *kb);
bool keyboard_poll(struct keyboard *kb);

#endif // KEYBOARD_H