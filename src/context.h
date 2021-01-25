#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "token.h"

class context
{
  public:
    context(const char* filename_, const char* text_);
    ~context();

    token tokenize();

    void parse_option(token& token);

    std::optional<int64_t> find_label(std::string_view label) const;

    std::vector<uint64_t> binary(std::span<const uint64_t> code) const;

    int64_t pc = 0;

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

    void write_gfx_header(std::vector<uint64_t>& output) const;

    void write_dksh(size_t code_size, std::vector<uint64_t>& output) const;

    const char* filename;
    const char* text_begin;
    const char* text;
    int line = 0;
    int column = 0;
    std::unordered_map<std::string, int64_t> labels;

    std::optional<program_type> type;
    bool is_dksh = false;

    // generic dksh
    std::optional<std::string> entrypoint;
    int num_gprs = 8;
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
