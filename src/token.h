#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "error.h"
#include "span.h"

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
    std::int64_t immediate;
    float float_immediate;
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

    void parse_option(token& token);

    std::optional<std::int64_t> find_label(std::string_view label) const;

    void write(span<std::uint64_t> code, std::ofstream& outfp) const;

    std::int64_t pc = 0;

  private:
    enum class program_type
    {
        vertex = 0,
        tess_control = 1,
        tess_eval = 2,
        geometry = 3,
        fragment = 4,
        compute = 5,
    };

    void next() noexcept;

    void reset();

    void generate_labels();

    void write_gfx_header(std::ofstream& outfp) const;

    void write_dksh(std::size_t code_size, std::ofstream& outfp) const;

    const char* filename;
    const char* text_begin;
    const char* text;
    int line = 0;
    int column = 0;
    std::unordered_map<std::string, std::int64_t> labels;

    std::optional<program_type> type;
    bool is_dksh = false;

    // generic dksh
    std::optional<std::string> entrypoint;
    int num_gprs = 0;
    int local_mem_size = 0;

    // vertex shader
    std::optional<std::string> second_entrypoint;
    int second_num_gprs = 0;

    // fragment shader
    bool early_fragment_tests = false;
    bool post_depth_coverage = false;
    bool persample_invocation = false;

    // compute shader
    std::array<unsigned, 3> block_dimensions = {1, 1, 1};
    int shared_mem_size = 0;
    int num_barriers = 0;
};

const char* name(token_type type);

std::string_view info(const token& token);
