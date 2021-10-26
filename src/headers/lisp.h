#ifndef _LISP_H
#define _LISP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <headers/mpc.h>

/*===================================forward declarations===================================*/
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/*===================================define===================================*/
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define	MAXLINE	4096			/* max line length */
#define VERSION "0.0.0.0.0.7"   /* my lisp version */

// for debug
#define MPC_AST_PRINT 0x1

#define DEBUG_VERBOSE_SET   0x0


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

/*===================================data type===================================*/
// {0, 1, 2, 3} lisp value type
enum { LVAL_ERR,  LVAL_NUM,   LVAL_SYM, 
       LVAL_FUN,  LVAL_SEXPR, LVAL_QEXPR };

typedef lval* (*lbuiltin)(lenv*, lval*);

struct lval {
    int type;

    /* Basic */
    double num;
    char* err;
    char* sym;

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
// lisp environment
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

// operation/op.c
lval*   builtin_op(lenv*, lval*, char*);
lval*   builtin_var(lenv*, lval*, char*);
lval*   builtin_head(lenv*, lval*);
lval*   builtin_tail(lenv*, lval*);
lval*   builtin_list(lenv*, lval*);
lval*   builtin_eval(lenv*, lval*);
lval*   builtin_join(lenv*, lval*);
lval*   builtin_init(lenv*, lval*);
lval*   builtin_len(lenv*, lval*);
lval*   builtin_cons(lenv*, lval*);
lval*   builtin_add(lenv*, lval*);
lval*   builtin_sub(lenv*, lval*);
lval*   builtin_mul(lenv*, lval*);
lval*   builtin_div(lenv*, lval*);
lval*   builtin_mod(lenv*, lval*);
lval*   builtin_max(lenv*, lval*);
lval*   builtin_min(lenv*, lval*);
lval*   builtin_def(lenv*, lval*);
lval*   builtin_put(lenv*, lval*);
lval*   builtin_symbol(lenv*, lval*);
lval*   builtin_exit(lenv*, lval*);
lval*   builtin_lambda(lenv*, lval*);

// expression/expr.c
lval*   lval_join(lval*, lval*);
lval*   lval_num(double);
lval*   lval_err(char* fmt, ...);
lval*   lval_sym(char*);
lval*   lval_sexpr(void);
lval*   lval_qexpr(void);
lval*   lval_func(lbuiltin);
lval*   lval_lambda(lval*, lval*);
lval*   lval_read_num(mpc_ast_t*);
lval*   lval_read(mpc_ast_t*);
lval*   lval_add(lval*, lval*);
lval*   lval_eval_sexpr(lenv*, lval*);
lval*   lval_eval(lenv*, lval*);
lval*   lval_take(lval*, int);
lval*   lval_pop(lval*, int);
lval*   lval_copy(lval*);
char*   ltype_name(int t);
void    lval_del(lval* v);
void    lval_print(lval*);
void    lval_println(lval*);
void    lval_expr_print(lval*, char, char);
lval*   lval_call(lenv*, lval*, lval*);

// environment/env.c
lenv*   lenv_new(void);
lenv*   lenv_copy(lenv*);
void    lenv_del(lenv*);
lval*   lenv_get(lenv*, lval*);
void    lenv_put(lenv*, lval*, lval*);
void    lenv_def(lenv*, lval*, lval*);
void    lenv_add_builtin(lenv*, char*, lbuiltin);
void    lenv_add_builtins(lenv*);
void    print_env(lenv*);
void    print_env_symbol(lenv*);
void    print_env_value(lenv*);

// lib/debug.c
uint64_t debug_printf(uint64_t open_set, const char *format, ...);
uint64_t debug_mpc_ast_print(uint64_t open_set, mpc_ast_t* a);

#endif