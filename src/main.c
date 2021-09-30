#include <stdio.h>
#include <stdlib.h>
#include <headers/mpc.h>

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

static long eval(mpc_ast_t* t);
static long eval_op(long x, char* op, long y);

int main(int argc, char** argv) {
    /*Create Some Parsers */
    mpc_parser_t* Number        = mpc_new("number");
    mpc_parser_t* Operator      = mpc_new("operator");
    mpc_parser_t* Expr          = mpc_new("expr");
    mpc_parser_t* Lispy         = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                   \
            number   : /-?\\d+(.\\d+)?/ ;                                   \
            operator : '+' | '-' | '*' | '/' | '%' |                        \
                        \"add\" | \"sub\" | \"mul\" | \"div\" ;             \
            expr     : <number> | '(' <operator> <expr>+ ')' ;              \
            lispy    : /^/ <expr> | <operator> <expr>+ /$/ ;                \
        ",
        Number, Operator, Expr, Lispy);

    puts("XM's Lispy Version 0.0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    while (1) {
        char* input = readline("doge>>> ");
        add_history(input);

        // parse input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // long result = eval(r.output);
            mpc_ast_print(r.output);
            // printf("%li\n", result);
            // mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
}

static long eval(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }

    char* op = t->children[1]->contents;

    long x = eval(t->children[2]); 

    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}

static long eval_op(long x, char* op, long y) {
    if (strcmp(op, "+") == 0) return x + y;
    if (strcmp(op, "-") == 0) return x - y;
    if (strcmp(op, "*") == 0) return x * y;
    if (strcmp(op, "/") == 0) return x / y;
    return 0;
}