#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "token.h"

void fatal_error(const struct token *token, const char *fmt, ...)
{
    if (token) {
        if (token->filename) {
            fprintf(stderr, "\33[1m%s:", token->filename);
        }
        fprintf(stderr, "\33[1m%d:%d: ", token->line + 1, token->column + 1);
    }
    fprintf(stderr, "\33[1;31merror:\33[0m ");

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);

    exit(EXIT_FAILURE);
}
