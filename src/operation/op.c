#include <headers/lisp.h>

// list 接收一个或者多个参数的S-表达式，返回一个包含所有参数的Q-表达式
lval* builtin_list(lenv* e, lval* v) {
    v->type = LVAL_SEXPR;
    return v;
}

// join 接受一个或者多个Q-表达式，返回一个将其连在一起的Q-表达式
lval* builtin_join(lenv* e, lval* v) {
    for (int i = 0; i < v->count; i++) {
        LASSERT_TYPE("join", v, i, LVAL_QEXPR);
    }

    lval* a = lval_pop(v, 0);
    while (v->count) {
        a = lval_join(a, lval_pop(v, 0));
    }

    lval_del(v);
    return a;
}


// eval 接受一个Q-表达式，将其看做一个S-表达式，并运行
lval* builtin_eval(lenv* e, lval* v) {
    LASSERT_NUM("eval", v, 1);
    LASSERT_TYPE("eval", v, 0, LVAL_QEXPR);
    lval* a = lval_take(v, 0);
    a->type = LVAL_SEXPR;
    return lval_eval(e, a);
}

// head 接受一个Q-表达式，返回一个包含其第一个元素的Q-表达式
lval* builtin_head(lenv* e, lval* v) {
    LASSERT_NUM("head", v, 1);
    LASSERT_TYPE("head", v, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("head", v, 0);
    
    lval* a = lval_take(v, 0);
    while (a->count > 1) {
        lval_del(lval_pop(a, 1));
    }
    return a;
}

// tail 接受一个Q-表达式，返回一个除首元素外的Q-表达式
lval* builtin_tail(lenv* e, lval* v) {
    LASSERT_NUM("tail", v, 1);
    LASSERT_TYPE("tail", v, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("tail", v, 0);

    lval* a = lval_take(v, 0);
    
    lval_del(lval_pop(a, 0));
    return a;
}

// 将元素arg1添加到arg2(Q-表达式)首位
// (cons 1 {a 2 3}) -> {1 a 2 3}
lval* builtin_cons(lenv* e, lval* v) {
    LASSERT_NUM("cons", v, 2);
    LASSERT_TYPE("cons", v, 1, LVAL_QEXPR);
    lval* x = lval_qexpr();
    x = lval_add(x, lval_pop(v, 0));
    x = lval_join(x, lval_pop(v, 0));
    lval_del(v);
    return x;
}

// 返回Q-表达式中的元素个数
lval* builtin_len(lenv* e, lval* v) {
    LASSERT_NUM("len", v, 1);
    LASSERT_TYPE("len", v, 0, LVAL_QEXPR);

    lval* res = lval_num(v->cell[0]->count);
    lval_del(v);
    return res;
}

// 返回Q-表达式中除最后一个元素外的其他元素
lval* builtin_init(lenv* e, lval* v) {
    LASSERT_NUM("init", v, 1);
    LASSERT_TYPE("init", v, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("init", v, 0);

    lval* a = lval_take(v, 0);
    lval_del(lval_pop(a, a->count - 1));
    return a;
}

lval* builtin_add(lenv* e, lval* v) {
    return builtin_op(e, v, "+");
}

lval* builtin_sub(lenv* e, lval* v) {
    return builtin_op(e, v, "-");
}

lval* builtin_mul(lenv* e, lval* v) {
    return builtin_op(e, v, "*");
}

lval* builtin_div(lenv* e, lval* v) {
    return builtin_op(e, v, "/");
}

lval* builtin_mod(lenv* e, lval* v) {
    return builtin_op(e, v, "%");
}

lval* builtin_max(lenv* e, lval* v) {
    return builtin_op(e, v, "max");
}

lval* builtin_min(lenv* e, lval* v) {
    return builtin_op(e, v, "max");
}

lval* builtin_def(lenv* e, lval* v) {
    LASSERT_TYPE("def", v, 0, LVAL_QEXPR);

    lval* syms = v->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(v, (syms->cell[i]->type == LVAL_SYM),
            "Function 'def' cannot define non-symbol. "
            "Got %s, Expected %s.",
            ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
    }

    LASSERT(v, (syms->count == v->count - 1),
        "Function 'def' passed too many arguments for symbols. "
        "Got %i, Expected %i.",
        syms->count, v->count - 1);
    
    for (int i = 0; i < syms->count; i++) {
        lenv_put(e, syms->cell[i], v->cell[i + 1]);
    }

    lval_del(v);
    return lval_sexpr();
}

lval* builtin_symbol(lenv* e, lval* v) {
    LASSERT_TYPE("symbol", v, 0, LVAL_NUM);
    LASSERT_NUM("symbol", v, 1);
    lval* x = v->cell[0];
    if (x->num == 0) {
        print_env_symbol(e);
    } else if (x->num == 1) {
        print_env_value(e);
    } else {
        print_env(e);
    }
    lval_del(v);
    return lval_sexpr();
}

lval* builtin_op(lenv* e, lval* v, char* op) {
    // ensure all arguments are numbers
    for (int i = 0; i < v->count; i++) {
        LASSERT_TYPE(op, v, i, LVAL_NUM);
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