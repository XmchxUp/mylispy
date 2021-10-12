#include <stdio.h>
#include <stdlib.h>
#include <headers/mpc.h>
#include <headers/lisp.h>

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
static int number_of_leaf_node(mpc_ast_t* t);
int number_of_nodes(mpc_ast_t* t);

int main(int argc, char** argv) {
    /*Create Some Parsers */
    mpc_parser_t* Number        = mpc_new("number");
    mpc_parser_t* Operator      = mpc_new("operator");
    mpc_parser_t* Expr          = mpc_new("expr");
    mpc_parser_t* Lispy         = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                    \
            number   : /-?\\d+([.]\\d+)?/ ;                                   \
            operator : '+' | '-' | '*' | '/' | '%' | '^' |                     \
                        \"add\" | \"sub\" | \"mul\" | \"div\" |                 \
                        \"min\" | \"max\";                                       \
            expr     : <number> | '(' <operator> <expr>+ ')' ;                    \
            lispy    : /^/ <operator> <expr>+ /$/ | /^/ '(' <operator> <expr>+ ')' /$/;                \
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
            // mpc_ast_print(r.output);
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
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

    // 括号起始()
    /*
    doge>>> (+ 1 2 3)
    > 
        regex 
        char:1:1 '('
        operator|char:1:2 '+'
        expr|number|regex:1:4 '1'
        expr|number|regex:1:6 '2'
        expr|number|regex:1:8 '3'
        char:1:9 ')'
        regex
    */
    int op_start = 0;
    if (strcmp(t->children[1]->contents, "(") == 0) {
        op_start = 1;
    }

    char* op = t->children[1 + op_start]->contents;

    long x = eval(t->children[2 + op_start]); 

    // - 1
    if (strcmp(op, "-") == 0 && number_of_leaf_node(t) == 1) {
        return -x;
    }

    int i = 3 + op_start;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}

static long eval_op(long x, char* op, long y) {
    if (strcmp(op, "+") == 0 ||
        strcmp(op, "add") == 0)
        return x + y;
    if (strcmp(op, "-") == 0 ||
        strcmp(op, "sub") == 0) 
        return x - y;
    if (strcmp(op, "*") == 0 ||
        strcmp(op, "mul") == 0) 
        return x * y;
    if (strcmp(op, "/") == 0 ||
        strcmp(op, "div") == 0) 
        return x / y;
    if (strcmp(op, "%") == 0)
        return x % y;
    if (strcmp(op, "^") == 0)
        return pow(x, y);
    if (strcmp(op, "max") == 0)
        return MAX(x, y);
    if (strcmp(op, "min") == 0)
        return MIN(x, y);
    return 0;
}

/*
doge>>> (+ 1 2 3)
> 
  regex 
  char:1:1 '('
  operator|char:1:2 '+'
  expr|number|regex:1:4 '1'
  expr|number|regex:1:6 '2'
  expr|number|regex:1:8 '3'
  char:1:9 ')'
  regex 
3
*/
static int number_of_leaf_node(mpc_ast_t* t) {
    if (t->children_num == 0) {
        if (strstr(t->tag, "expr"))
            return 1;
        return 0;
    }

    int total = 0;
    for (int i = 0; i < t->children_num; i++) {
        total += number_of_leaf_node(t->children[i]);
    }
    return total;
}

/*
doge>>> (+ 1 2)
> 
  regex 
  char:1:1 '('
  operator|char:1:2 '+'
  expr|number|regex:1:4 '1'
  expr|number|regex:1:6 '2'
  char:1:7 ')'
  regex 
8
*/
int number_of_nodes(mpc_ast_t* t) {
    if (t->children_num == 0) return 1;
    int total = 1;
    for (int i = 0; i < t->children_num; i++) {
        total += number_of_nodes(t->children[i]);
    }
    return total;
}