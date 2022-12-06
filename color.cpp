#include "color.hpp"
#include <cstdio>
#include <cstdlib>

const char* color_name(Color c) {
    switch (c) {
        case BLACK:
            return "Black";
        case WHITE:
            return "White";
        case NONE:
            return "None";
        case BORDER:
            return "Border";
        default:
            printf("FATAL: Unknown color");
            exit(0);
    }
}

Color color_reverse(Color c) {
    switch (c) {
        case BLACK:
            return WHITE;
        case WHITE:
            return BLACK;
        default:
            printf("FATAL: wrong reverse of %s", color_name(c));
            exit(0);
    }
}
