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

void init_mpc();
void parse_input(char *input);
void signal_handler(int unused);

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
              "operator : '+' | '-' | '*' | '/' | '%' | '^';"
              "expr     : <number> | '(' <operator> <expr>+ ')';"
              "lispy    : /^/ <operator> <expr>+ /$/;",
              number, operator, expr, lispy);
}

void parse_input(char *input) {
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, lispy, &r)) {
        mpc_ast_print(r.output);
        mpc_ast_delete(r.output);
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