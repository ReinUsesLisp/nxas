#include <cstdint>
#include <span>
#include <vector>

#include "context.h"
#include "error.h"

std::vector<uint64_t> context::binary(std::span<const uint64_t> code) const
{
    constexpr size_t potential_extra_size = 64;

    const bool has_gfx_header = type && *type != program_type::compute;
    const size_t code_size = code.size() + (has_gfx_header ? 8 : 0);

    std::vector<uint64_t> result;
    result.reserve(code.size() + potential_extra_size);
    if (is_dksh) {
        write_dksh(code_size, result);
    }
    if (has_gfx_header) {
        write_gfx_header(result);
    }
    result.insert(result.end(), code.begin(), code.end());
    if (is_dksh) {
        result.resize(result.size() + 32 - code.size() % 32);
    }
    return result;
}

void context::write_gfx_header(std::vector<uint64_t>& output) const
{
    fatal_error("write_gfx_header is not implemented");
}
