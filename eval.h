#ifndef EVAL_H
#define EVAL_H
#include "mpc.h"

typedef enum {
    LVAL_NUM,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_SEXPR
} LValType;

typedef struct {
    LValType type;
    long num;

    char *err;
    char *sym;

    int count;
    struct lval **cell;
} lval;

lval *lval_num(const long x);

lval *lval_err(char *m);

lval *lval_sym(char *s);

lval *lval_sexpr();

void lval_del(lval *v);

void lval_print(const lval x);

void lval_println(const lval x);

lval eval_op(const lval x, const char *op, const lval y);

lval eval(const mpc_ast_t *ast);

#endif //EVAL_H
