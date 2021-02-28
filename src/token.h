#pragma once

#include <cstdint>
#include <string_view>

enum class token_type
{
    none,
    identifier,
    regster,
    predicate,
    immediate,
    float_immediate,
    plus,
    minus,
    tilde,
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
    int64_t immediate;
    double float_immediate;
    uint8_t regster;
};

struct token
{
    const char* filename;
    token_data data;
    token_type type;
    int line;
    int column;
};

const char* name(token_type type);

std::string_view info(const token& token);
