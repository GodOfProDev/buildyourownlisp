#include "parser.h"


static mpc_parser_t *number;
static mpc_parser_t *symbol;
static mpc_parser_t *sexpr;
static mpc_parser_t *expr;
static mpc_parser_t *lispy;

static bool *isRunning;

void parser_init(bool *is_running_p) {
    isRunning = is_running_p;

    number = mpc_new("number");
    symbol = mpc_new("symbol");
    sexpr = mpc_new("sexpr");
    expr = mpc_new("expr");
    lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
              "number   : /-?[0-9]+/ ;"
              "symbol   : \"min\" | \"max\" | '+' | '-' | '*' | '/' | '%' | '^' ;"
              "sexpr    : '(' <expr>* ')' ;"
              "expr     : <number> | <symbol> | <sexpr> ;"
              "lispy    : /^/ <expr>* /$/ ;",
              number, symbol, sexpr, expr, lispy
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
    mpc_cleanup(5, number, symbol, sexpr, expr, lispy);
    *isRunning = false;
}
