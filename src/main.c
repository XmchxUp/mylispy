#include <stdio.h>
#include <stdlib.h>

/* comipling on windows */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

/* fake add_history function*/
void add_history(char* unused) {}
/* otherwise */
#else
#ifdef __linux__
#include <editline/readline.h>
#include <editline/history.h>
#endif

#ifdef __MACH__
#include <editline/readline.h>
#endif
#endif

int main(int argc, char** argv) {
    puts("XM's Lispy Version 0.0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("doge>>> ");

        add_history(input);

        printf("No you're a %s\n", input);

        free(input);
    }
    return 0;
}