#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mpc.h"
#include "parser.h"
#include "eval.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, sizeof(buffer), stdin);

    // the plus one is for the null terminator because the strlen doesn't account for that
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);

    // remove the \n
    cpy[strlen(cpy) - 1] = '\0';

    return cpy;
}

void add_history(char* unused)
{
}
#else
#include <edithistory/readline.h>
#include <edithistory/history.h>
#endif

static bool isRunning = true;

int main(void)
{
    puts("Lispy 0.1");
    puts("Press Crtl+C to Exit!");

    parser_init(&isRunning);

    signal(SIGINT, parser_cleanup);

    while (isRunning)
    {
        char* input = readline("lispy> ");
        add_history(input);

        const parser_result r = parser_parse(input);
        if (!r.hasErrored)
        {
            lval val = eval(r.ast);
            lval_println(val);

            mpc_ast_delete(r.ast);
        }
        else
        {
            mpc_err_print(r.err);
            mpc_err_delete(r.err);
        }

        free(input);
    }
}
