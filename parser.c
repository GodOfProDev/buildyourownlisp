#include "parser.h"


static mpc_parser_t *number;
static mpc_parser_t *operator;
static mpc_parser_t *expr;
static mpc_parser_t *lispy;

static bool *isRunning;

void parser_init(bool *is_running_p) {
    isRunning = is_running_p;

    number = mpc_new("number");
    operator = mpc_new("operator");
    expr = mpc_new("expr");
    lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
              "number   : /-?[0-9]+/;"
              "operator : \"min\" | \"max\" | '+' | '-' | '*' | '/' | '%' | '^';"
              "expr     : <number> | '(' <operator> <expr>+ ')';"
              "lispy    : /^/ <operator> <expr>+ /$/;",
              number, operator, expr, lispy
    );
}

parser_result parser_parse(char *input) {
    mpc_result_t ast;
    parser_result r;

    r.ast = NULL;
    r.err = NULL;
    r.hasErrored = false;

    if (mpc_parse("<stdin>", input, lispy, &ast)) {
        r.ast = ast.output;
    } else {
        r.err = ast.error;
        r.hasErrored = true;
    }

    return r;
}

void parser_cleanup(int unused) {
    mpc_cleanup(4, number, operator, expr, lispy);
    *isRunning = false;
}
