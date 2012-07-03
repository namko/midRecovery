/*
 *  Terminal.h:
 *      - Class for abstraction of terminal control.
 */
#ifndef __TERMINAL_H_
#define __TERMINAL_H_

#include <iostream>

class Terminal {
public:
    Terminal() { }

    void clear() const {
        std::cout << "\033c";
    }

    void text_reset() const {
        std::cout << "\033[0m";
    }

    void text_bold() const {
        std::cout << "\033[1m";
    }

    void text_underline() const {
        std::cout << "\033[4m";
    }

    void text_inverse() const {
        std::cout << "\033[7m";
    }
};

extern const Terminal gTerminal;

#endif //   __TERMINAL_H_

