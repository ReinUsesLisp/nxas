#pragma once

#include <memory>

#define CHECK(result)                                                                              \
    do {                                                                                           \
        if (error tmpres_##__LINE__ = (result)) {                                                  \
            return tmpres_##__LINE__;                                                              \
        }                                                                                          \
    } while (0)

struct token;

class error
{
    friend error fail(const token& token, const char* fmt, ...);

  public:
    error() = default;
    operator bool() const
    {
        return static_cast<bool>(message);
    }

    [[noreturn]] void raise();

  private:
    std::unique_ptr<char[]> message;
};

error fail(const token& token, const char* fmt, ...);

[[noreturn]] void fatal_error(const token& token, const char* fmt, ...);

[[noreturn]] void fatal_error(const char* fmt, ...);
