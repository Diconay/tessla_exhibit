#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>

class Keyboard {
public:
    struct State {
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

    Keyboard();
    ~Keyboard();

    bool init();
    void deinit();
    bool poll();

    State state;

private:
    int fd;
    State counts;
    bool read(State &cnt, State &st, bool init = false);
};

#endif // KEYBOARD_H