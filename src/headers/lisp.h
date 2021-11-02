#ifndef _LISP_H
#define _LISP_H

#include <stdlib.h>
#include <stdint.h>
#include <headers/mpc.h>

struct lenv;
struct lval;
typedef struct lenv lenv;
typedef struct lval lval;

/*===================================define===================================*/
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define	MAXLINE	4096			/* max line length */
#define VERSION "0.0.0.0.1.2"   /* my lisp version */

#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

#define LASSERT_TYPE(func, args, index, expect) \
    LASSERT(args, args->cell[index]->type == expect, \
        "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
        func, index, ltype_name(args->cell[index]->type), ltype_name(expect));

#define LASSERT_NUM(func, args, num) \
    LASSERT(args, args->count == num, \
        "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
        func, args->count, num);

#define LASSERT_NOT_EMPTY(func, args, index) \
    LASSERT(args, args->cell[index]->count != 0, \
        "Function '%s' passed {} for argument %i.", func, index);

// for debug
#define MPC_AST_PRINT 0x1

#define DEBUG_VERBOSE_SET   0x0

/*===================================gloabl data===================================*/
mpc_parser_t* Number;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Symbol;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;  
mpc_parser_t* Lispy;

/*===================================data type===================================*/
#define TRUE    1
#define FALSE   0

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_BOOL, LVAL_STR,
       LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval* (*lbuiltin)(lenv*, lval*);

struct lval {
    int type;

    /* Basic */
    double num;
    char* err;
    char* sym;
    char* str;

    /* Function */
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;
    
    /* Expression */
    int count;
    lval** cell;
};

// Each entry in one list has a correspoiding entry in the other list at the same position
struct lenv {
    lenv* par;
    int count;
    char** syms;
    lval** vals;
};

/*===================================function===================================*/
// lib/error.c
void	err_msg(const char *, ...);
void	err_dump(const char *, ...);
void	err_quit(const char *, ...);
void	err_cont(int, const char *, ...);
void	err_exit(int, const char *, ...);
void	err_ret(const char *, ...);
void	err_sys(const char *, ...);

// lib/util.c
/* Dynamic storage allocation wrappers */
void    *Malloc(size_t size);
void    *Realloc(void *ptr, size_t size);
void    *Calloc(size_t nmemb, size_t size);
void    Free(void *ptr);

// lib/debug.c
uint64_t debug_printf(uint64_t open_set, const char *format, ...);
uint64_t debug_mpc_ast_print(uint64_t open_set, mpc_ast_t* a);

#endif