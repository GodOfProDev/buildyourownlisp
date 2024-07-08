#ifndef PARSER_H
#define PARSER_H
#include "mpc.h"

typedef struct {
    mpc_ast_t *ast;
    mpc_err_t *err;
    bool hasErrored;
} parser_result;

void parser_init(bool *isRunning);

parser_result parser_parse(char *input);

void parser_cleanup(int unused);

#endif //PARSER_H
