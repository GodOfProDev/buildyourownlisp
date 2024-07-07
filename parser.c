#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char *readline(char *prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);

    // input: hello -> hello\n\0
    // hello\n\0 -> strlen = 6 , buff size = 7

    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';

    return cpy;
}

void add_history(char *unused) {
}

#else
#include <editline/readline.h>
#include <editline/history.h>

#endif

static bool isRunning = true;

// Enum for possile lval types
enum {
    LVAL_NUM,
    LVAL_ERR
};

// Enum for possible error types
enum {
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
};

typedef struct {
    int type;
    long num;
    int err;
} lval;

void init_mpc();
void parse_input(const char *input);
void signal_handler(int unused);
lval eval(mpc_ast_t *tree);
lval eval_op(lval x, char* op, lval y);
lval lval_num(long x);
lval lval_err(int x);
void lval_print(lval v);
void lval_println(lval v);

int main(int argc, char **argv) {
    init_mpc();
    signal(SIGINT, signal_handler);

    puts("Lispy 0.1");
    puts("Press Crtl+C To Exit");

    while (isRunning) {
        char *input = readline("lispy> ");
        add_history(input);

        parse_input(input);

        free(input);
    }

    return 0;
}

lval eval_op(lval x, char* op, lval y) {

    /* If either value is an error return it */
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }

    /* Otherwise do maths on the number values */
    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
    if (strcmp(op, "/") == 0) {
        /* If second operand is zero return error */
        return y.num == 0
          ? lval_err(LERR_DIV_ZERO)
          : lval_num(x.num / y.num);
    }

    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *tree) {
    if (strstr(tree->tag, "number")) {
        // Check if there is an error in conversion
        errno = 0;
        long x = strtol(tree->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    char *op = tree->children[1]->contents;
    lval x = eval(tree->children[2]);

    int i = 3;
    while (strstr(tree->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(tree->children[i]));
        i++;
    }

    return x;
}

static mpc_parser_t *number;
static mpc_parser_t *operator;
static mpc_parser_t *expr;
static mpc_parser_t *lispy;

void init_mpc() {
    number = mpc_new("number");
    operator = mpc_new("operator");
    expr = mpc_new("expr");
    lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
              "number   : /-?[0-9]+/;"
              "operator : \"min\" | \"max\" | '+' | '-' | '*' | '/' | '%' | '^';"
              "expr     : <number> | '(' <operator> <expr>+ ')';"
              "lispy    : /^/ <operator> <expr>+ /$/;",
              number, operator, expr, lispy);
}

void parse_input(const char *input) {
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, lispy, &r)) {
        mpc_ast_t *ast = r.output;
        lval result = eval(ast);
        lval_println(result);
        mpc_ast_delete(ast);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }
}

void signal_handler(int unused) {
    mpc_cleanup(4, number, operator, expr, lispy);
    printf("Shutting down");
    isRunning = false;
}

lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

void lval_print(lval v) {
    switch (v.type) {
        case LVAL_NUM:
            printf("%li", v.num);
            break;
        case LVAL_ERR:
            switch (v.err) {
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

void lval_println(lval v) {
    lval_print(v);
    putchar('\n');
}