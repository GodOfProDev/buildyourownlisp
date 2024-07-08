#ifndef EVAL_H
#define EVAL_H
#include "mpc.h"

typedef enum {
    LVAL_NUM,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_SEXPR
} LValType;

typedef struct lval {
    LValType type;
    long num;

    char *err;
    char *sym;

    int count;
    struct lval **cell;
} lval;

lval *lval_num(long x);

lval *lval_err(char *m);

lval *lval_sym(char *s);

lval *lval_sexpr();

void lval_del(lval *v);

lval *lval_add(lval *v, lval *x);

lval *lval_read_num(mpc_ast_t *t);

lval *lval_read(mpc_ast_t *t);

void lval_expr_print(lval *v, char open, char close);

void lval_print(lval *v);

void lval_println(lval *v);

lval *eval_op(const lval x, const char *op, const lval y);

lval eval(const mpc_ast_t *ast);

#endif //EVAL_H
