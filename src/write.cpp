#include <cstdint>
#include <fstream>
#include <span>

#include "token.h"

void context::write(std::span<const uint64_t> code, std::ofstream& outfp) const
{
    const bool has_gfx_header = type && *type != program_type::compute;
    const std::size_t code_size = code.size() + (has_gfx_header ? 8 : 0);

    if (is_dksh) {
        write_dksh(code_size, outfp);
    }
    if (has_gfx_header) {
        write_gfx_header(outfp);
    }
    outfp.write(reinterpret_cast<const char*>(code.data()), code.size() * sizeof(uint64_t));
}

void context::write_gfx_header(std::ofstream&) const
{
    fatal_error("write_gfx_header is not implemented");
}
