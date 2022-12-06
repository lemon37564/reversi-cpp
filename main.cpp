#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ai.hpp"
#include "node.hpp"

int main() {
    char input[64];
    char color[2];
    char strength[2];

    while (scanf("%s %s", input, color) != EOF) {
        Color cl = BLACK;
        if (strcmp(color, "2") == 0) {
            cl = WHITE;
        }

        AI ai = AI(cl, Strength::STRONG);

        char* res = ai.move(input);
        printf("%s\n", res);
        free(res);
    }
}