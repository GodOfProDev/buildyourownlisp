#include "eval.h"
#include <string.h>

lval *lval_num(long x) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;

    return v;
}

lval *lval_err(char *m) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);

    return v;
}

lval *lval_sym(char *s) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);

    return v;
}

lval *lval_sexpr() {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;

    return v;
}

void lval_del(lval *v) {
    switch (v->type) {
        case LVAL_NUM: break;

        case LVAL_ERR: free(v->err);
            break;
        case LVAL_SYM: free(v->sym);
            break;

        case LVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
        // Free the the memory allocated to contain the pointers
            free(v->cell);
            break;
    }

    // Free the memory allocated for the "lval" struct itself
    free(v);
}

lval *lval_add(lval *v, lval *x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval *) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval *lval_read_num(mpc_ast_t *t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval *lval_read(mpc_ast_t *t) {
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    // If root (> tag) or sexpr then create empty list

    lval *x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strcmp(t->tag, "sexpr") == 0) { x = lval_sexpr(); }

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }

        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

void lval_expr_print(lval *v, char open, char close) {
    putchar(open);

    for (int i = 0; i < v->count; i++) {
        lval_print(v->cell[i]);

        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }

    putchar(close);
}

void lval_print(lval *v) {
    switch (v->type) {
        case LVAL_NUM: printf("%li", v->num);
            break;
        case LVAL_ERR: printf("Error: %s", v->err);
            break;
        case LVAL_SYM: printf("%s", v->sym);
            break;
        case LVAL_SEXPR: lval_expr_print(v, '(', ')');
            break;
    }
}

void lval_println(lval *v) {
    lval_print(v);
    putchar('\n');
}

lval *eval_op(const lval x, const char *op, const lval y) {
    if (x.type == LVAL_ERR) { return &x; }
    if (y.type == LVAL_ERR) { return &y; }

    if (strstr(op, "+")) { return lval_num(x.num + y.num); }
    if (strstr(op, "-")) { return lval_num(x.num - y.num); }
    if (strstr(op, "*")) { return lval_num(x.num * y.num); }
    /*if (strstr(op, "/")) { return (y.num != 0) ? lval_num(x.num / y.num) : lval_err(LERR_DIV_ZERO); }
    if (strstr(op, "%")) { return (y.num != 0) ? lval_num(x.num % y.num) : lval_err(LERR_DIV_ZERO); }
    if (strstr(op, "^")) { return lval_num(x.num ^ y.num); }
    if (strstr(op, "min")) { return (x.num > y.num) ? lval_num(y.num) : lval_num(x.num); }
    if (strstr(op, "max")) { return (x.num > y.num) ? lval_num(x.num) : lval_num(y.num); }

    return lval_err(LERR_BAD_OP);*/
}

lval eval(const mpc_ast_t *ast) {
    /*if (strstr(ast->tag, "number")) {
        errno = 0;
        long x = strtol(ast->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }*/

    char *op = ast->children[1]->contents;
    lval x = eval(ast->children[2]);

    int i = 3;
    while (strstr(ast->children[i]->tag, "expr")) {
        /*
        x = eval_op(x, op, eval(ast->children[i]));
        */
        i++;
    }

    return x;
}
