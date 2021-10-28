#include <string.h>
#include <headers/lisp.h>
#include <headers/expr.h>
#include <headers/env.h>
#include <headers/op.h>

// list 接收一个或者多个S-表达式，返回一个包含所有参数的Q-表达式
lval* builtin_list(lenv* e, lval* v) {
    v->type = LVAL_QEXPR;
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

// second 接受一个Q-表达式，返回包含其第二个元素的Q-表达式
lval* builtin_second(lenv* e, lval* v) {
    LASSERT_NUM("second", v, 1);
    LASSERT_TYPE("second", v, 0, LVAL_QEXPR);
    
    lval* a = lval_take(v, 0);

    while (a->count > 2) {
        lval_del(lval_pop(a, 2));
    }

    lval_del(lval_pop(a, 0));
    return a;
}

// car 接受一个列表，返回列表中的第一个元素
lval* builtin_cdr(lenv* e, lval* v) {
    LASSERT_NUM("cdr", v, 1);
    LASSERT_TYPE("cdr", v, 0, LVAL_QEXPR);
    
    lval* a = lval_take(v, 0);
    lval_del(lval_pop(a, 0));
    return a;
}

// cdr 接受一个列表，返回列表中，除去第一个元素剩余的列表
lval* builtin_car(lenv* e, lval* v) {
    LASSERT_NUM("car", v, 1);
    LASSERT_TYPE("car", v, 0, LVAL_QEXPR);

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
    return builtin_var(e, v, "def");
}

lval* builtin_put(lenv* e, lval* v) {
    return builtin_var(e, v, "=");
}

lval* builtin_var(lenv* e, lval* v, char* func) {
    LASSERT_TYPE(func, v, 0, LVAL_QEXPR);

    lval* syms = v->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(v, (syms->cell[i]->type == LVAL_SYM),
            "Function 'def' cannot define non-symbol. "
            "Got %s, Expected %s.",
            ltype_name(syms->cell[i]->type), 
            ltype_name(LVAL_SYM));
    }

    LASSERT(v, (syms->count == v->count - 1),
        "Function 'def' passed too many arguments for symbols. "
        "Got %i, Expected %i.",
        syms->count, v->count - 1);
    
    for (int i = 0; i < syms->count; i++) {
        if (strcmp(func, "def") == 0) { // global
            lenv_def(e, syms->cell[i], v->cell[i + 1]);
        }

        if (strcmp(func, "=") == 0) { // local
            lenv_put(e, syms->cell[i], v->cell[i + 1]);
        }
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

lval* builtin_exit(lenv* e, lval* v) {
    printf("Bye~~\n");
    exit(EXIT_SUCCESS);
    return lval_sexpr();
}

lval* builtin_lambda(lenv* e, lval* v) {
    LASSERT_NUM("lambda", v, 2);
    LASSERT_TYPE("lambda", v, 0, LVAL_QEXPR);
    LASSERT_TYPE("lambda", v, 1, LVAL_QEXPR);

    // 检测形参为symbol
    for (int i = 0; i < v->cell[0]->count; i++) {
        LASSERT(v, (v->cell[0]->cell[i]->type == LVAL_SYM), 
            "Cannot define non-symbol. Got %s, Expected %s.",
            ltype_name(v->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }

    lval* formals = lval_pop(v, 0);
    lval* body = lval_pop(v, 0);
    
    lval_del(v);
    return lval_lambda(formals, body);
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

lval* builtin_gt(lenv* e, lval* v) {
    return builtin_ord(e, v, ">");
}

lval* builtin_lt(lenv* e, lval* v) {
    return builtin_ord(e, v, "<");
}

lval* builtin_ge(lenv* e, lval* v) {
    return builtin_ord(e, v, ">=");
}

lval* builtin_le(lenv* e, lval* v) {
    return builtin_ord(e, v, "<=");
}

lval* builtin_ord(lenv* e, lval* v, char* op) {
    LASSERT_NUM(op, v, 2);
    LASSERT_TYPE(op, v, 0, LVAL_NUM);
    LASSERT_TYPE(op, v, 1, LVAL_NUM);

    int r = 0;
    if (strcmp(op, ">") == 0) {
        r = (v->cell[0]->num > v->cell[1]->num);
    } else if (strcmp(op, ">=") == 0) {
        r = (v->cell[0]->num >= v->cell[1]->num);
    } else if (strcmp(op, "<") == 0) {
        r = (v->cell[0]->num < v->cell[1]->num);
    } else if (strcmp(op, "<=") == 0) {
        r = (v->cell[0]->num <= v->cell[1]->num);
    }

    lval_del(v);
    return lval_num((double)r);
}

lval* builtin_eq(lenv* e, lval* v) {
    return builtin_cmp(e, v, "==");
}

lval* builtin_ne(lenv* e, lval* v) {
    return builtin_cmp(e, v, "!=");
}

lval* builtin_cmp(lenv* e, lval* v, char* op) {
    LASSERT_NUM(op, v, 2);

    int r = 0;
    if (strcmp(op, "==") == 0) {
        r = lval_eq(v->cell[0], v->cell[1]);
    } else if (strcmp(op, "!=") == 0) {
        r = !lval_eq(v->cell[0], v->cell[1]);
    }
    lval_del(v);
    return lval_num((double) r);
}

lval* builtin_if(lenv* e, lval* v) {
    LASSERT_NUM("if", v, 3);
    LASSERT_TYPE("if", v, 0, LVAL_NUM);
    LASSERT_TYPE("if", v, 1, LVAL_QEXPR);
    LASSERT_TYPE("if", v, 2, LVAL_QEXPR);

    lval* x;
    v->cell[1]->type = LVAL_SEXPR;
    v->cell[2]->type = LVAL_SEXPR;

    if (v->cell[0]->num) {
        x = lval_eval(e, lval_pop(v, 1));
    } else {
        x = lval_eval(e, lval_pop(v, 2));
    }

    lval_del(v);
    return x;
}

lval* builtin_and(lenv* e, lval* v) {
    return builtin_logic(e, v, "&&");
}

lval* builtin_or(lenv* e, lval* v) {
    return builtin_logic(e, v, "||");
}

lval* builtin_not(lenv* e, lval* v) {
    LASSERT_NUM("not", v, 1);
    LASSERT_TYPE("not", v, 0, LVAL_NUM);

    int r = !(v->cell[0]->num);
    lval_del(v);
    return lval_num((double) r);
}

lval* builtin_logic(lenv* e, lval* v, char* op) {
    for (int i = 0; i < v->count; i++) {
        LASSERT_TYPE(op, v, i, LVAL_NUM);
    }

    int r = -1;
    if (strcmp(op, "&&") == 0) {
        r = 1;
        for (int i = 0; i < v->count; i++) {
            r = (r && (v->cell[i]->num));
        }
    } else if (strcmp(op, "||") == 0) {
        r = 0;
        for (int i = 0; i < v->count; i++) {
            r = (r || (v->cell[i]->num));
        }
    }

    lval_del(v);
    return lval_num((double) r);
}