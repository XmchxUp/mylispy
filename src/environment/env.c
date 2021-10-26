#include <string.h>
#include <headers/lisp.h>
#include <headers/expr.h>
#include <headers/op.h>
#include <headers/env.h>

lenv* lenv_new(void) {
    lenv* e = Malloc(sizeof(lenv));
    e->par = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

lenv* lenv_copy(lenv* e) {
    lenv* res = Malloc(sizeof(lenv));
    res->count = e->count;
    res->par = e->par;
    res->syms = Malloc(sizeof(char*) * res->count);
    res->vals = Malloc(sizeof(lval*) * res->count);

    for (int i = 0; i < res->count; i++) {
        res->syms[i] = Malloc(sizeof(char) * (strlen(e->syms[i]) + 1));
        strcpy(res->syms[i], e->syms[i]);
        res->vals[i] = lval_copy(e->vals[i]);
    }
    return res;
}

void lenv_del(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        Free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    Free(e->syms);
    Free(e->vals);
    Free(e);
}

// 返回符号k的值
lval* lenv_get(lenv* e, lval* k) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            return lval_copy(e->vals[i]);
        }
    }

    if (e->par) {
        return lenv_get(e->par, k);
    }
    return lval_err("unbound symbol! '%s'", k->sym);
}

// 向环境e添加值
void lenv_put(lenv* e, lval* k, lval* v) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    e->count += 1;
    e->vals = Realloc(e->vals, sizeof(lval*) * e->count);
    e->syms = Realloc(e->syms, sizeof(lval*) * e->count);

    int lastIdx = e->count - 1;
    e->vals[lastIdx] = lval_copy(v);
    e->syms[lastIdx] = Malloc(strlen(k->sym) + 1);
    strcpy(e->syms[lastIdx], k->sym);
}

// 向全局环境加入值
void lenv_def(lenv* e, lval* k, lval* v) {
    while (e->par) {
        e = e->par;
    }
    lenv_put(e, k, v);
}

void print_env(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        printf("%s: ", e->syms[i]);
        lval_print(e->vals[i]);
        putchar('\n');
    }
}

void print_env_symbol(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        printf("%s ", e->syms[i]);
    }
    putchar('\n');
}

void print_env_value(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        lval_print(e->vals[i]);
        putchar(' ');
    }
    putchar('\n');
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
    lval* k = lval_sym(name);
    lval* v = lval_func(func);
    lenv_put(e, k, v);

    lval_del(v);
    lval_del(k);
}

void lenv_add_builtins(lenv* e) {
    
    lenv_add_builtin(e, "symbol", builtin_symbol);
    lenv_add_builtin(e, "exit", builtin_exit);
    lenv_add_builtin(e, "lambda", builtin_lambda);
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "=", builtin_put);

    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);
    lenv_add_builtin(e, "len",  builtin_len);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "init", builtin_init);

    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "add", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "sub", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "mul", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "div", builtin_div);
    lenv_add_builtin(e, "max", builtin_max);
    lenv_add_builtin(e, "min", builtin_min);
    lenv_add_builtin(e, "%", builtin_mod);
    lenv_add_builtin(e, "mod", builtin_mod);
}