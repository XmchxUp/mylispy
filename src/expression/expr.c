#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <headers/lisp.h>
#include <headers/env.h>
#include <headers/op.h>
#include <headers/expr.h>

/* Create a new number type lval */
lval* lval_num(double x) {
    lval* res = Malloc(sizeof(lval));
    res->type = LVAL_NUM;
    res->num = x;
    return res;
}

/* Create a new error type lval */
lval* lval_err(char* fmt, ...) {
    lval* v = Malloc(sizeof(lval));
    v->type = LVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->err = Malloc(512);
    vsnprintf(v->err, 511, fmt, va);
    v->err = Realloc(v->err, strlen(v->err) + 1);

    va_end(va);

    return v;
}

/* Construct a pointer to a new Symbol lval */
lval* lval_sym(char* s) {
    lval* res = Malloc(sizeof(lval));
    res->type = LVAL_SYM;
    res->sym = Malloc(strlen(s) + 1);
    strcpy(res->sym, s);
    return res;
}

lval* lval_bool(double x) {
    lval* res = Malloc(sizeof(lval));
    res->type = LVAL_BOOL;
    res->num = x;
    return res;
}

lval* lval_str(char* str) {
    lval* v = Malloc(sizeof(lval));
    v->str = Malloc(strlen(str) + 1);
    strcpy(v->str, str);
    v->type = LVAL_STR;
    return v;
}

/* A pointer to a new empty Sexpr lval */
lval* lval_sexpr(void) {
  lval* v = Malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

/* A pointer to a new empty Qexpr lval */
lval* lval_qexpr(void) {
    lval* v = Malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

lval* lval_func(lbuiltin func) {
    lval* v = Malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->builtin = func;
    return v;
}

lval* lval_lambda(lval* formals, lval* body) {
    lval* v = Malloc(sizeof(lval));
    v->type = LVAL_FUN;

    v->builtin = NULL;

    v->env = lenv_new();
    v->formals = formals;
    v->body = body;
    return v;
}

void lval_del(lval* lv) {
    switch (lv->type)
    {
    case LVAL_FUN:
        if (!lv->builtin) {
            lenv_del(lv->env);
            lval_del(lv->formals);
            lval_del(lv->body);
        }
        break;
    case LVAL_STR:
        Free(lv->str);
        break;
    case LVAL_BOOL:
    case LVAL_NUM:
        break;
    case LVAL_ERR:
        Free(lv->err);
        break;
    case LVAL_SYM:
        Free(lv->sym);
        break;
    case LVAL_QEXPR:
    case LVAL_SEXPR:
        for (int i = 0; i < lv->count; i++) {
            lval_del(lv->cell[i]);
        }
        Free(lv->cell);
        break;
    }
    Free(lv);
}

lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE ? 
        lval_num(x) : 
        lval_err("invalid number");
}

lval* lval_read_str(mpc_ast_t* t) {
    t->contents[strlen(t->contents) - 1] = '\0';
    char* unescaped = Malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);

    unescaped = mpcf_unescape(unescaped);
    lval* str = lval_str(unescaped);
    Free(unescaped);
    return str;
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
*/
lval* lval_read(mpc_ast_t* t) {
    if (strstr(t->tag, "string")) {
        return lval_read_str(t);
    }

    if (strstr(t->tag, "number")) {
        return lval_read_num(t);
    }

    if (strstr(t->tag, "symbol")) {
        return lval_sym(t->contents);
    }

    lval* x = NULL;
    if (strcmp(t->tag, ">") == 0) { // root
        x = lval_sexpr();
    }

    if (strstr(t->tag, "sexpr")) {
        x = lval_sexpr();
    }

    if (strstr(t->tag, "qexpr")) {
        x = lval_qexpr();
    }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0 ||
            strcmp(t->children[i]->contents, ")") == 0 ||
            strcmp(t->children[i]->contents, "{") == 0 ||
            strcmp(t->children[i]->contents, "}") == 0 ||
            strcmp(t->children[i]->tag, "regex") == 0 ||
            strstr(t->children[i]->tag, "comment")) {
            continue;
        }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

/* Expression v add one element x */
lval* lval_add(lval* v, lval* x) {
    v->count += 1;
    v->cell = Realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval* lval_eval_sexpr(lenv* e, lval* v) {
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }

    if (v->count == 0) {
        return v;
    }

    if (v->count == 1 && 
        v->cell[0]->builtin != builtin_exit) {
        return lval_eval(e, lval_take(v, 0));
    }

    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_FUN) {
        lval* err = lval_err(
            "S-Expression starts with incorrect type. "
            "Got %s, Expected %s.",
            ltype_name(f->type), ltype_name(LVAL_FUN));
        lval_del(f); 
        lval_del(v);
        return err;
    }

    lval* result = lval_call(e, f, v);
    lval_del(f);
    return result;
}

lval* lval_eval(lenv* e, lval* v) {
    if (v->type == LVAL_SYM) {
        lval* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }

    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(e, v);
    }
    
    return v;
}

lval* lval_call(lenv* e, lval* f, lval* a) {
    if (f->builtin) {
        return f->builtin(e, a);
    }

    int given = a->count;
    int total = f->formals->count;

    while (a->count) {
        if (f->formals->count == 0) {
            lval_del(a);
            return lval_err(
                "Function passed too many arguments"
                "Got %i, Expected %i.", given, total);
        }

        lval* sym = lval_pop(f->formals, 0);

        if (strcmp(sym->sym, "&") == 0) {
            // 确保 '&' 后跟随一个 symbol
            if (f->formals->count != 1) {
                lval_del(a);
                return lval_err("Function format invalid. "
                    "Symbol '&' not followed by single symbol.");
            }

            lval* nsym = lval_pop(f->formals, 0);
            lenv_put(f->env, nsym, builtin_list(e, a));
            
            lval_del(sym); 
            lval_del(nsym);
            break;
        }

        lval* val = lval_pop(a, 0);
        lenv_put(f->env, sym, val);

        lval_del(val);
        lval_del(sym);
    }

    lval_del(a);

    if (f->formals->count > 0 &&
        strcmp(f->formals->cell[0]->sym, "&") == 0) {
          if (f->formals->count != 2) {
            return lval_err("Function format invalid. "
                "Symbol '&' not followed by single symbol.");
        }
        // delete &
        lval_del(lval_pop(f->formals, 0));

        lval* sym = lval_pop(f->formals, 0);
        lval* val = lval_qexpr();
        
        lenv_put(f->env, sym, val);

        lval_del(val);
        lval_del(sym); 
    }

    if (f->formals->count == 0) {
        f->env->par = e;
        return builtin_eval(f->env, 
            lval_add(lval_sexpr(), lval_copy(f->body)));
    } else {
        return lval_copy(f);
    }
}

int lval_eq(lval* x, lval* y) {
    if (x->type != y->type) {
        return 0;
    }

    switch (x->type) {
        case LVAL_STR:
            return (strcmp(x->str, y->str) == 0);
        case LVAL_BOOL:
        case LVAL_NUM: 
            return (x->num == y->num);
        case LVAL_ERR:
            return (strcmp(x->err, y->err) == 0);
        case LVAL_SYM:
            return (strcmp(x->sym, y->sym) == 0);
        case LVAL_FUN:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return lval_eq(x->formals, y->formals) &&
                    lval_eq(x->body, y->body);
            }
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            if (x->count != y->count) {
                return 0;
            }
            for (int i = 0; i < x->count; i++) {
                if (!lval_eq(x->cell[i], y->cell[i])) {
                    return 0;
                }
            }
            return 1;
        default:
            break;
    }
    return 0;
}

lval* lval_pop(lval* v, int i) {
    lval* x = v->cell[i];

    memmove(&v->cell[i], &v->cell[i + 1], 
        sizeof(lval*) * (v->count - i - 1));

    v->count -= 1;

    // 使用安全函数需特殊处理 BUG用时1个小时，定位速度太慢，多看文档
    /*
    The  realloc() function changes the size of the memory block pointed to by ptr to size bytes.  The contents will be unchanged in the range from
    the start of the region up to the minimum of the old and new sizes.  If the new size is larger than the old size, the added memory will not  be
    initialized.   If  ptr  is  NULL, then the call is equivalent to malloc(size), for all values of size; if size is equal to zero, and ptr is not
    NULL, then the call is equivalent to free(ptr).  Unless ptr is NULL, it must have been returned by an earlier call to  malloc(),  calloc(),  or
    realloc().  If the area pointed to was moved, a free(ptr) is done.
    */
    if (v->count > 0)  {
        v->cell = Realloc(v->cell, sizeof(lval*) * v->count);
    } else {
        Free(v->cell);
        v->cell = NULL;
    }
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval* lval_copy(lval* v) {
    lval* x = Malloc(sizeof(lval));
    x->type = v->type;

    switch (x->type)
    {
    case LVAL_FUN:
        if (v->builtin) {
            x->builtin = v->builtin;
        } else {
            x->builtin = NULL;
            x->env = lenv_copy(v->env);
            x->formals = lval_copy(v->formals);
            x->body = lval_copy(v->body);
        }
        break;
    case LVAL_STR:
        x->str = Malloc(strlen(v->str) + 1);
        strcpy(x->str, v->str);
        break;
    case LVAL_BOOL:
    case LVAL_NUM:
        x->num = v->num;
        break;
    case LVAL_ERR:
        x->err = Malloc(strlen(v->err) + 1);
        strcpy(x->err, v->err);
        break;
    case LVAL_SYM:
        x->sym = Malloc(strlen(v->sym) + 1);
        strcpy(x->sym, v->sym);
        break;
    case LVAL_SEXPR:
    case LVAL_QEXPR:
        x->count = v->count;
        x->cell = malloc(sizeof(lval*) * x->count);
        for (int i = 0; i < x->count; i++) {
            x->cell[i] = lval_copy(v->cell[i]);
        }
        break;
    }

    return x;
}

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        lval_print(v->cell[i]);

        if (i == (v->count - 1)) break;
        putchar(' ');
    }
    putchar(close);
}

void lval_println(lval* x) {
    lval_print(x);
    putchar('\n');
}

void lval_print(lval* lv) {
    switch (lv->type)
    {
    case LVAL_STR:
        lval_print_str(lv);
        break;
    case LVAL_BOOL:
        if (lv->num) {
            printf("#true");
        } else {
            printf("#false");
        }
        break;
    case LVAL_NUM:
        // 去掉尾随零位g, 并调整精度 
        // https://stackoverflow.com/questions/66039239/is-there-a-way-to-automatically-printf-a-float-to-the-number-of-decimal-places-i
        printf("%.*g", DECIMAL_DIG, lv->num);
        break;
    case LVAL_ERR:
        printf("Error: %s", lv->err);
        break;
    case LVAL_SYM:
        printf("%s", lv->sym);
        break;
    case LVAL_FUN:
        if (lv->builtin) {
            printf("<builtin>");
        } else {
            printf("(lambda "); 
            lval_print(lv->formals);
            putchar(' ');
            lval_print(lv->body);
            putchar(')');
        }
        break;
    case LVAL_SEXPR:
        lval_expr_print(lv, '(', ')');
        break;
    case LVAL_QEXPR:
        lval_expr_print(lv, '{', '}');
        break;
    }
}

void lval_print_str(lval* v) {
    char* escaped = Malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);
    Free(escaped);
}

lval* lval_join(lval* x, lval* y) {
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    lval_del(y);
    return x;
}

char* ltype_name(int t) {
  switch(t) {
    case LVAL_STR: return "String";
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    case LVAL_BOOL: return "Boolean";
    default: return "Unknown";
  }
}