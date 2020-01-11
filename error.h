#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#define CHECK(result)                                                                              \
    do {                                                                                           \
        char *tmpres_##__LINE__ = (result);                                                        \
        if (tmpres_##__LINE__) {                                                                   \
            return tmpres_##__LINE__;                                                              \
        }                                                                                          \
    } while (0)

struct token;

typedef char *error;

error fail(const struct token *token, const char *fmt, ...);

_Noreturn void fatal_error(const struct token *token, const char *fmt, ...);

_Noreturn void report_error(char* message);

#endif // ERROR_H_INCLUDED
