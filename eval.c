#include "eval.h"
#include <string.h>

lval *lval_num(const long x)
{
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;

    return v;
}

lval *lval_err(char *m)
{
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
        case LVAL_SYM: free(v->sym);

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

void lval_print(const lval x)
{
    switch (x.type)
    {
    case LVAL_NUM:
        printf("%li", x.num);
        break;
    case LVAL_ERR:
        switch (x.err)
        {
        case LERR_DIV_ZERO:
            printf("Error: Division by Zero!");
            break;
        case LERR_BAD_OP:
            printf("Error: Invalid Operator!");
            break;
        case LERR_BAD_NUM:
            printf("Error: Invalid Number!");
            break;
        }
        break;
    }
}

void lval_println(const lval x)
{
    lval_print(x);
    putchar('\n');
}

lval eval_op(const lval x, const char* op, const lval y)
{
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    if (strstr(op, "+")) { return lval_num(x.num + y.num); }
    if (strstr(op, "-")) { return lval_num(x.num - y.num); }
    if (strstr(op, "*")) { return lval_num(x.num * y.num); }
    if (strstr(op, "/")) { return (y.num != 0) ? lval_num(x.num / y.num) : lval_err(LERR_DIV_ZERO); }
    if (strstr(op, "%")) { return (y.num != 0) ? lval_num(x.num % y.num) : lval_err(LERR_DIV_ZERO); }
    if (strstr(op, "^")) { return lval_num(x.num ^ y.num); }
    if (strstr(op, "min")) { return (x.num > y.num) ? lval_num(y.num) : lval_num(x.num); }
    if (strstr(op, "max")) { return (x.num > y.num) ? lval_num(x.num) : lval_num(y.num); }

    return lval_err(LERR_BAD_OP);
}

lval eval(const mpc_ast_t* ast)
{
    if (strstr(ast->tag, "number"))
    {
        errno = 0;
        long x = strtol(ast->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    char* op = ast->children[1]->contents;
    lval x = eval(ast->children[2]);

    int i = 3;
    while (strstr(ast->children[i]->tag, "expr"))
    {
        x = eval_op(x, op, eval(ast->children[i]));
        i++;
    }

    return x;
}
