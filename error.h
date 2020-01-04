#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

struct context;
struct token;

_Noreturn void fatal_error(const struct token *token, const char *fmt, ...);

#endif // ERROR_H_INCLUDED
