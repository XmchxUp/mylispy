#include "headers/lisp.h"



static char input[MAXLINE];

int main(int argc, char** argv) {
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        fputs("lisp> ", stdout);
        if (fgets(input, MAXLINE, stdin) == NULL) {
            err_msg("input error\n");
        }
        printf("No you're a %s", input);
    }
    return 0;
}