#include "headers/lisp.h"
#include <editline/history.h>
#include <editline/readline.h>

int main(int argc, char** argv) {
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("lisp> ");

        add_history(input);

        printf("No you're a %s\n", input);

        free(input);
    }
    return 0;
}