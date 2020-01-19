#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "error.h"
#include "token.h"

void error::raise()
{
    std::fputs(message.get(), stderr);
    std::fputc('\n', stderr);
    std::exit(EXIT_FAILURE);
}

error fail(const token& token, const char* fmt, ...)
{
    static const char errfmt[] = "\33[1m%s:\33[1m%d:%d: \33[1;31merror:\33[0m ";

    const int line = token.line + 1;
    const int column = token.column + 1;

    const int first_length = std::snprintf(NULL, 0, errfmt, token.filename, line, column);
    std::va_list ap;
    va_start(ap, fmt);
    const int second_length = std::vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    const int length = first_length + second_length + 1;
    error error;
    error.message = std::make_unique<char[]>(length);

    std::snprintf(error.message.get(), length, errfmt, token.filename, line, column);
    va_start(ap, fmt);
    std::vsnprintf(error.message.get() + first_length, length - first_length, fmt, ap);
    va_end(ap);

    return error;
}

// TODO: deduplicate this code

void fatal_error(const token& token, const char* fmt, ...)
{
    std::fprintf(stderr, "\33[1m%s:\33[1m%d:%d: ", token.filename, token.line + 1,
                 token.column + 1);
    std::fprintf(stderr, "\33[1;31merror:\33[0m ");

    std::va_list ap;
    va_start(ap, fmt);
    std::vfprintf(stderr, fmt, ap);
    va_end(ap);
    std::fputc('\n', stderr);

    std::exit(EXIT_FAILURE);
}

void fatal_error(const char* fmt, ...)
{
    std::fprintf(stderr, "\33[1;31merror:\33[0m ");

    std::va_list ap;
    va_start(ap, fmt);
    std::vfprintf(stderr, fmt, ap);
    va_end(ap);
    std::fputc('\n', stderr);

    std::exit(EXIT_FAILURE);
}
