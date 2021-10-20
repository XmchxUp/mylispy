#include <headers/lisp.h>

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

void lval_println(lval* x) {
    lval_print(x);
    putchar('\n');
}

void lval_print(lval* lv) {
    // TODO: fix
    switch (lv->type)
    {
    case LVAL_NUM:
        // 去掉尾随零位g, 并调整精度 
        // https://stackoverflow.com/questions/66039239/is-there-a-way-to-automatically-printf-a-float-to-the-number-of-decimal-places-i
        printf("%.*g", DECIMAL_DIG, lv->num);
        break;
    case LVAL_ERR:
        if (lv->err == LERR_DIV_ZERO) {
            printf("Error: Division By Zero!");
        } else if (lv->err == LERR_BAD_OP) {
            printf("Error: Invalid Operator!");
        } else if (lv->err == LERR_BAD_NUM) {
            printf("Error: Invalid Number!");
        }
        break;
    }
}