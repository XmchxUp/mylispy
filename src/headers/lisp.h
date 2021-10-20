#ifndef _LISP_H
#define _LISP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/*===================================define===================================*/
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define	MAXLINE	4096			/* max line length */
#define VERSION "0.0.0.0.0.1"   /* my lisp version */

#define MPC_PRINT 0x1

#define DEBUG_VERBOSE_SET   0x1

/*===================================function===================================*/
// lib/error.c
void	err_msg(const char *, ...);
void	err_dump(const char *, ...);
void	err_quit(const char *, ...);
void	err_cont(int, const char *, ...);
void	err_exit(int, const char *, ...);
void	err_ret(const char *, ...);
void	err_sys(const char *, ...);

// lib/debug.c
uint64_t debug_printf(uint64_t open_set, const char *format, ...);

/*===================================data type===================================*/
// {0, 1} lisp value type
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR };

// lisp value
typedef struct {
    int type;

    char* err;
    char* sym;
    
    int count;
    lval** cell;
} lval;


#endif