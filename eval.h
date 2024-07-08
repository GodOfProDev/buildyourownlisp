#ifndef EVAL_H
#define EVAL_H
#include "mpc.h"

typedef enum
{
    LVAL_NUM,
    LVAL_ERR
} LValType;

typedef enum
{
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
} LValErr;

typedef struct
{
    long num;
    LValType type;
    LValErr err;
} lval;

lval lval_num(const long x);

lval lval_err(const LValErr x);

void lval_print(const lval x);
void lval_println(const lval x);

lval eval_op(const lval x, const char* op, const lval y);

lval eval(const mpc_ast_t* ast);

#endif //EVAL_H
