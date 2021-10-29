#ifndef _OP_H
#define _OP_H

lval*   builtin_op(lenv*, lval*, char*);
lval*   builtin_var(lenv*, lval*, char*);
lval*   builtin_head(lenv*, lval*);
lval*   builtin_second(lenv*, lval*);
lval*   builtin_car(lenv*, lval*);
lval*   builtin_cdr(lenv*, lval*);
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
lval*   builtin_gt(lenv*, lval*);
lval*   builtin_lt(lenv*, lval*);
lval*   builtin_ge(lenv*, lval*);
lval*   builtin_le(lenv*, lval*);
lval*   builtin_ord(lenv*, lval*, char*);
lval*   builtin_eq(lenv*, lval*);
lval*   builtin_ne(lenv*, lval*);
lval*   builtin_cmp(lenv*, lval*, char*);
lval*   builtin_if(lenv*, lval*);
lval*   builtin_and(lenv*, lval*);
lval*   builtin_or(lenv*, lval*);
lval*   builtin_not(lenv*, lval*);
lval*   builtin_logic(lenv*, lval*, char*);
lval*   builtin_load(lenv*, lval*);
lval*   builtin_print(lenv*, lval*);
lval*   builtin_error(lenv*, lval*);


#endif