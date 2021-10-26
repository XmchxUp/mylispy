#ifndef _OP_H
#define _OP_H

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

#endif