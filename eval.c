#include "eval.h"
#include <string.h>

lval lval_num(const long x)
{
    lval r;
    r.num = x;
    r.type = LVAL_NUM;

    return r;
}

lval lval_err(const LValErr x)
{
    lval r;
    r.err = x;
    r.type = LVAL_ERR;

    return r;
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
