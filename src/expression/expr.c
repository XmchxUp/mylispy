#include <headers/lisp.h>
#include <float.h>

/* Create a new number type lval */
lval* lval_num(double x) {
    lval* res = Malloc(sizeof(lval));
    res->type = LVAL_NUM;
    res->num = x;
    return res;
}

/* Create a new error type lval */
lval* lval_err(char* m) {
    lval* res = Malloc(sizeof(lval));
    res->type = LVAL_ERR;
    res->err = Malloc(strlen(m) + 1);
    strcpy(res->err, m);
    return res;
}

/* Construct a pointer to a new Symbol lval */
lval* lval_sym(char* s) {
    lval* res = Malloc(sizeof(lval));
    res->type = LVAL_SYM;
    res->sym = Malloc(strlen(s) + 1);
    strcpy(res->sym, s);
    return res;
}

/* A pointer to a new empty Sexpr lval */
lval* lval_sexpr(void) {
  lval* v = Malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

void lval_del(lval* lv) {
    switch (lv->type)
    {
    case LVAL_NUM: 
        break;
    case LVAL_ERR:
        Free(lv->err);
        break;
    case LVAL_SYM:
        Free(lv->sym);
        break;
    case LVAL_SEXPR:
        for (int i = 0; i < lv->count; i++) {
            lval_del(lv->cell[i]);
        }
        free(lv->cell);
        break;
    }
    free(lv);
}

lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE ? 
        lval_num(x) : 
        lval_err("invalid number");
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

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0 ||
            strcmp(t->children[i]->contents, ")") == 0 ||
            strcmp(t->children[i]->tag, "regex") == 0) {
            continue;
        }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

/* Symbol Expression v add one element x */
lval* lval_add(lval* v, lval* x) {
    v->count += 1;
    v->cell = Realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval* lval_eval_sexpr(lval* v) {
    // evaluate children
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // error checking
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }

    // empty expression
    if (v->count == 0) {
        return v;
    }

    // signle expression
    if (v->count == 1) {
        return lval_take(v, 0);
    }

    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYM) {
        lval_del(f);
        lval_del(v);
        return lval_err("S-expression Does not start with symbol!");
    }

    lval* result = builtin_op(v, f->sym);
    lval_del(f);
    return result;
}

lval* lval_eval(lval* v) {
    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(v);
    }
    return v;
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
        free(v->cell);
        v->cell = NULL;
    }
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
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
    case LVAL_SEXPR:
        lval_expr_print(lv, '(', ')');
        break;
    }
}

lval* builtin_op(lval* v, char* op) {
    // ensure all arguments are numbers
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type != LVAL_NUM) {
            lval_del(v);
            return lval_err("Cannot operate on non-number!");
        }
    }

    lval* x = lval_pop(v, 0);
    
    if ((strcmp(op, "-") == 0) && v->count == 0) {
        x->num = 0 - x->num;
    }

    while (v->count > 0) {
        lval* y = lval_pop(v, 0);

        if (strcmp(op, "+") == 0 ||
            strcmp(op, "add") == 0) {
            x->num += y->num;
        } else if (strcmp(op, "-") == 0 ||
                   strcmp(op, "sub") == 0) {
            x->num -= y->num;
        } else if (strcmp(op, "*") == 0 ||
                   strcmp(op, "mul") == 0) {
            x->num *= y->num;
        } else if (strcmp(op, "/") == 0 ||
                   strcmp(op, "div") == 0) {
            if (y->num == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division By Zero!");
                break;
            }
            x->num /= y->num;
        } else if (strcmp(op, "%") == 0) {
            x->num = fmod(x->num, y->num);
        } else if (strcmp(op, "^") == 0) {
            x->num = pow(x->num, y->num);
        } else if (strcmp(op, "max") == 0) {
            x->num = MAX(x->num, y->num);
        } else if (strcmp(op, "min") == 0) {
            x->num = MIN(x->num, y->num);
        }
        lval_del(y);
    }

    lval_del(v); 
    return x;
}