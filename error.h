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

using error = char *;

error fail(const struct token *token, const char *fmt, ...);

[[noreturn]] void fatal_error(const struct token *token, const char *fmt, ...);

[[noreturn]] void report_error(char *message);

#endif // ERROR_H_INCLUDED
