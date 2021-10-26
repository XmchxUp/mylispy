#ifndef _ENV_H
#define _ENV_H

lenv*   lenv_new(void);
lenv*   lenv_copy(lenv*);
lval*   lenv_get(lenv*, lval*);
void    lenv_del(lenv*);
void    lenv_put(lenv*, lval*, lval*);
void    lenv_def(lenv*, lval*, lval*);
void    lenv_add_builtin(lenv*, char*, lbuiltin);
void    lenv_add_builtins(lenv*);
void    print_env(lenv*);
void    print_env_symbol(lenv*);
void    print_env_value(lenv*);


#endif