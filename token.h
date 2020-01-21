#pragma once

#include <string_view>

#include <cstddef>
#include <cstdint>

#include "error.h"

enum class token_type
{
    none,
    identifier,
    regster,
    predicate,
    immediate,
    plus,
    minus,
    vbar,
    bracket_left,
    bracket_right,
    at,
    semicolon,
    comma,
};

struct token_predicate
{
    int index;
    int negated;
};

union token_data
{
    token_predicate predicate;
    std::string_view string{};
    std::int64_t immediate;
    std::uint8_t regster;
};

struct token
{
    const char* filename;
    union token_data data;
    token_type type;
    int line;
    int column;
};

class context
{
  public:
    context(const char* filename_, const char* text_) : filename{filename_}, text{text_} {}

    token tokenize();

    std::int64_t pc = 0;

  private:
    void next();

    const char* filename;
    const char* text;
    int line = 0;
    int column = 0;
};

const char* name(token_type type);

std::string_view info(const token& token);
