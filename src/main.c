#include <stdio.h>
#include <stdlib.h>
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

#define MPC_PARSER_COUNT 5

// local data
static mpc_parser_t* Number;
static mpc_parser_t* Symbol;
static mpc_parser_t* Sexpr;
static mpc_parser_t* Expr;  
static mpc_parser_t* Lispy;

static int number_of_leaf_node(mpc_ast_t*);
static int number_of_nodes(mpc_ast_t*);
static void print_vertion_info();
static void create_parser();
static void clean_parser();
static void parse_input(char*);

int main(int argc, char** argv) {

    create_parser();
    print_vertion_info();

    while (1) {
        char* input = readline("doge>>> ");
        add_history(input);
        parse_input(input);
        free(input);
    }

    clean_parser();
    return 0;
}

static void parse_input(char* input) {
    // parse input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
        // mpc_ast_print(r.output);
        lval* x = lval_read(r.output);
        lval_println(x);
        lval_del(x);
        // mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }
}

static void create_parser() {
    /*Create Some Parsers */
    Number        = mpc_new("number");
    Symbol        = mpc_new("symbol");
    Sexpr         = mpc_new("sexpr");
    Expr          = mpc_new("expr");
    Lispy         = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                    \
            number   : /-?\\d+([.]\\d+)?/ ;                                   \
            symbol   : '+' | '-' | '*' | '/' | '%' | '^' |                     \
                        \"add\" | \"sub\" | \"mul\" | \"div\" |                 \
                        \"min\" | \"max\";                                       \
            sexpr    : '(' <expr>* ')';                                           \
            expr     : <number> | <symbol> | <sexpr> ;                     \
            lispy    : /^/ <expr>* /$/ ;                                            \
        ",
        Number, Symbol, Sexpr, Expr, Lispy);
}

static void clean_parser() {
    mpc_cleanup(MPC_PARSER_COUNT, Number, Symbol, Sexpr, Expr, Lispy);
}

static void print_vertion_info() {
    printf("XM's Lispy Version %s\n", VERSION);
    printf("Press Ctrl+c to Exit\n\n");
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
static int number_of_nodes(mpc_ast_t* t) {
    if (t->children_num == 0) return 1;
    int total = 1;
    for (int i = 0; i < t->children_num; i++) {
        total += number_of_nodes(t->children[i]);
    }
    return total;
}