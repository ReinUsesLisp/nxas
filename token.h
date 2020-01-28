#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

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
    context(const char* filename_, const char* text_);
    ~context();

    token tokenize();

    std::optional<std::int64_t> find_label(std::string_view label) const;

    std::int64_t pc = 0;

  private:
    void next();

    void reset();

    void generate_labels();

    const char* filename;
    const char* text_begin;
    const char* text;
    int line = 0;
    int column = 0;
    std::unordered_map<std::string, std::int64_t> labels;
};

const char* name(token_type type);

std::string_view info(const token& token);
