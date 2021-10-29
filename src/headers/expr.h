#ifndef _EXPR_H
#define _EXPR_H

lval*   lval_join(lval*, lval*);
lval*   lval_num(double);
lval*   lval_err(char* fmt, ...);
lval*   lval_sym(char*);
lval*   lval_bool(double);
lval*   lval_str(char* str);
lval*   lval_sexpr(void);
lval*   lval_qexpr(void);
lval*   lval_func(lbuiltin);
lval*   lval_lambda(lval*, lval*);
lval*   lval_read_num(mpc_ast_t*);
lval*   lval_read_str(mpc_ast_t*);
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
void    lval_print_str(lval*);
void    lval_expr_print(lval*, char, char);
lval*   lval_call(lenv*, lval*, lval*);
int     lval_eq(lval*, lval*);

#endif