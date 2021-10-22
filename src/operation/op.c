#include <headers/lisp.h>

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