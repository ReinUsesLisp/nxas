#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "token.h"

error fail(const struct token *token, const char *fmt, ...)
{
    static const char errfmt[] = "\33[1m%s:\33[1m%d:%d: \33[1;31merror:\33[0m ";

    assert(token);

    const int line = token->line + 1;
    const int column = token->column + 1;

    const int first_length = snprintf(NULL, 0, errfmt, token->filename, line, column);
    va_list ap;
    va_start(ap, fmt);
    const int second_length = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    const int length = first_length + second_length + 1;
    char *text = (char *)malloc(length);
    if (!text) {
        fatal_error(NULL, "out of memory");
    }

    snprintf(text, length, errfmt, token->filename, line, column);
    va_start(ap, fmt);
    vsnprintf(text + first_length, length - first_length, fmt, ap);
    va_end(ap);

    return text;
}

void fatal_error(const struct token *token, const char *fmt, ...)
{
    if (token) {
        fprintf(stderr, "\33[1m%s:\33[1m%d:%d: ", token->filename, token->line + 1,
                token->column + 1);
    }
    fprintf(stderr, "\33[1;31merror:\33[0m ");

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);

    exit(EXIT_FAILURE);
}

void report_error(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    free(message);
    exit(EXIT_FAILURE);
}
