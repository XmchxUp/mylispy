#include <stdio.h>
#include <stdlib.h>
#include <headers/mpc.h>
#include <headers/lisp.h>
#include <headers/env.h>
#include <headers/expr.h>
#include <headers/op.h>

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

#define MPC_PARSER_COUNT 8

static lenv* e;

static void print_vertion_info();
static void create_parser();
static void clean_parser();
static void parse_input(char*);
static void load_builtin();

int main(int argc, char** argv) {

    create_parser();
    print_vertion_info();
    e = lenv_new();
    lenv_add_builtins(e);

    load_builtin();

    // interactive prompt
    if (argc == 1) {
        while (1) {
            char* input = readline("doge>>> ");
            add_history(input);
            parse_input(input);
            Free(input);
        }
    } else if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));
            lval* x = builtin_load(e, args);

            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }
    }

    lenv_del(e);
    clean_parser();
    return 0;
}

static void load_builtin() {
    // 根目录下运行加载
    lval* args = lval_add(lval_sexpr(), lval_str("./examples/builtin.doge"));
    lval* x = builtin_load(e, args);

    if (x->type == LVAL_ERR) {
        lval_println(x);
    }
    lval_del(x);
}

static void parse_input(char* input) {
    // parse input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
        debug_mpc_ast_print(MPC_AST_PRINT, r.output);
        lval* x = lval_eval(e, lval_read(r.output));
        lval_println(x);
        lval_del(x);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }
}

static void create_parser() {
    /*Create Some Parsers */
    String        = mpc_new("string");
    Comment       = mpc_new("comment");
    Number        = mpc_new("number");
    Symbol        = mpc_new("symbol");
    Sexpr         = mpc_new("sexpr");
    Qexpr         = mpc_new("qexpr");
    Expr          = mpc_new("expr");
    Lispy         = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                     \
            number   : /-?\\d+([.]\\d+)?/ ;                                    \
            string   : /\"(\\\\.|[^\"])*\"/ ;                                   \
            comment  : /;[^\\r\\n]*/ ;                                           \
            symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%|#]+/ ;                     \
            sexpr    : '(' <expr>* ')';                                           \
            qexpr    : '{' <expr>* '}';                                            \
            expr     : <number> | <symbol> | <string> | <comment> | <sexpr> | <qexpr> ;         \
            lispy    : /^/ <expr>* /$/ ;                                             \
        ",
        Number, String, Comment, Symbol, Sexpr, Qexpr, Expr, Lispy);
}

static void clean_parser() {
    mpc_cleanup(MPC_PARSER_COUNT, 
        Number, String, Comment, Symbol, 
        Sexpr, Qexpr, Expr, Lispy);
}

static void print_vertion_info() {
    printf("XM's Lispy Version %s\n", VERSION);
    printf("Type exit or Press Ctrl+c to Exit\n\n");
}
