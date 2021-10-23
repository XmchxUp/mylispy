#include <headers/lisp.h>

// list 接收一个或者多个参数的S-表达式，返回一个包含所有参数的Q-表达式
lval* builtin_list(lval* v) {
    v->type = LVAL_SEXPR;
    return v;
}

// join 接受一个或者多个Q-表达式，返回一个将其连在一起的Q-表达式
lval* builtin_join(lval* v) {
    for (int i = 0; i < v->count; i++) {
        LASSERT(v, v->cell[i]->type == LVAL_QEXPR,
            "Function 'head' passed incorrect types!");
    }

    lval* a = lval_pop(v, 0);
    while (v->count) {
        a = lval_join(a, lval_pop(v, 0));
    }

    lval_del(v);
    return a;
}


// eval 接受一个Q-表达式，将其看做一个S-表达式，并运行
lval* builtin_eval(lval* v) {
    LASSERT(v, v->count == 1, 
        "Function 'head' passed too many arguments!");
    LASSERT(v, v->cell[0]->type == LVAL_QEXPR,
        "Function 'head' passed incorrect types!");
    lval* a = lval_take(v, 0);
    a->type = LVAL_SEXPR;
    return lval_eval(a);
}

// head 接受一个Q-表达式，返回一个包含其第一个元素的Q-表达式
lval* builtin_head(lval* v) {
    LASSERT(v, v->count == 1, 
        "Function 'head' passed too many arguments!");
    LASSERT(v, v->cell[0]->type == LVAL_QEXPR,
        "Function 'head' passed incorrect types!");
    LASSERT(v, v->cell[0]->count != 0, 
        "Function 'head' passed {}!");
    
    lval* a = lval_take(v, 0);
    while (a->count > 1) {
        lval_del(lval_pop(a, 1));
    }
    return a;
}

// tail 接受一个Q-表达式，返回一个除首元素外的Q-表达式
lval* builtin_tail(lval* v) {
    LASSERT(v, v->count == 1, 
        "Function 'tail' passed too many arguments!");
    LASSERT(v, v->cell[0]->type == LVAL_QEXPR,
        "Function 'tail' passed incorrect types!");
    LASSERT(v, v->cell[0]->count != 0, 
        "Function 'tail' passed {}!");

    lval* a = lval_take(v, 0);
    
    lval_del(lval_pop(a, 0));
    return a;
}

// 将num添加到v(Q-表达式)首位
// (cons 1 {2 3}) -> {1 2 3}
lval* builtin_cons(lval* v) {
    LASSERT(v, v->count == 2, 
        "Function 'cons' must be two arguments!");
    LASSERT(v, v->cell[1]->type == LVAL_QEXPR,
        "Function 'cons' passed incorrect types!");
    lval* x = lval_qexpr();
    x = lval_add(x, lval_pop(v, 0));
    x = lval_join(x, lval_pop(v, 0));
    lval_del(v);
    return x;
}

// 返回Q-表达式中的元素个数
lval* builtin_len(lval* v) {
    LASSERT(v, v->count == 1, 
        "Function 'len' passed too many arguments!");
    LASSERT(v, v->cell[0]->type == LVAL_QEXPR,
        "Function 'len' passed incorrect types!");

    lval* res = lval_num(v->cell[0]->count);
    lval_del(v);
    return res;
}

// 返回Q-表达式中除最后一个元素外的其他元素
lval* builtin_init(lval* v) {
    return NULL;
}

lval* builtin(lval* v, char* func) {
    if (strcmp("list", func) == 0) {
        return builtin_list(v);
    }
    if (strcmp("eval", func) == 0) {
        return builtin_eval(v);
    }
    if (strcmp("join", func) == 0) {
        return builtin_join(v);
    }
    if (strcmp("head", func) == 0) {
        return builtin_head(v);
    }
    if (strcmp("tail", func) == 0) {
        return builtin_tail(v);
    }
    if (strcmp("len", func) == 0) {
        return builtin_len(v);
    }
    if (strcmp("cons", func) == 0) {
        return builtin_cons(v);
    }
    if (strstr("add+sub-div/mul*mod%pow^maxmin", func)) {
        return builtin_op(v, func);
    }
    lval_del(v);
    return lval_err("Unknown Function!");
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
        } else if (strcmp(op, "%") == 0 ||
                   strcmp(op, "mod") == 0) {
            x->num = fmod(x->num, y->num);
        } else if (strcmp(op, "^") == 0 ||
                   strcmp(op, "pow") == 0) {
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