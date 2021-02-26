#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>

#include "context.h"
#include "error.h"

constexpr uint32_t DKSH_MAGIC = 0x48534B44;

struct dksh_header
{
    uint32_t magic;
    uint32_t header_sz;
    uint32_t control_sz;
    uint32_t code_sz;
    uint32_t programs_off;
    uint32_t num_programs;
};

struct dksh_program_header
{
    uint32_t type;
    uint32_t entrypoint;
    uint32_t num_gprs;
    uint32_t constbuf1_off;
    uint32_t constbuf1_sz;
    uint32_t per_warp_scratch_sz;
    union
    {
        struct
        {
            uint32_t alt_entrypoint;
            uint32_t alt_num_gprs;
        } vert;
        struct
        {
            bool has_table_3d1;
            bool early_fragment_tests;
            bool post_depth_coverage;
            bool persample_invocation;
            uint32_t table_3d1[4];
            uint32_t param_d8;
            uint16_t param_65b;
            uint16_t param_489;
        } frag;
        struct
        {
            bool flag_47c;
            bool has_table_490;
            bool _padding[2];
            uint32_t table_490[8];
        } geom;
        struct
        {
            uint32_t param_c8;
        } tess_eval;
        struct
        {
            std::array<uint32_t, 3> block_dims;
            uint32_t shared_mem_sz;
            uint32_t local_pos_mem_sz;
            uint32_t local_neg_mem_sz;
            uint32_t crs_sz;
            uint32_t num_barriers;
        } comp;
    };
    uint32_t reserved;
};

static constexpr size_t align256(size_t value)
{
    return (value + 0xff) & ~0xff;
}

static constexpr size_t align8(size_t value)
{
    return (value + 0xf) & ~0xf;
}

void context::write_dksh(size_t code_size, std::vector<uint64_t>& output) const
{
    if (!type) {
        fatal_error(
            "no type provided, "
            "valid are types: vertex, tess_control, tess_eval, geometry, fragment, compute");
    }
    const std::optional entrypoint_code_offset = find_label(entrypoint.value_or("main"));
    if (!entrypoint_code_offset) {
        fatal_error("entrypoint \"%s\" not found", entrypoint.value_or("main").c_str());
    }
    if (*entrypoint_code_offset % 32 != 8) {
        fatal_error("entrypoint is not aligned");
    }

    const int local_pos_sz = align8(local_mem_size);
    const int local_neg_sz = 0;
    const int crs_sz = 0x800;

    dksh_program_header program_header;
    std::memset(&program_header, 0, sizeof(program_header));
    program_header.type = static_cast<uint32_t>(*type);
    program_header.entrypoint = *entrypoint_code_offset - 8;
    program_header.num_gprs = num_gprs;
    program_header.constbuf1_off = 0; // TODO
    program_header.constbuf1_sz = 0;
    program_header.per_warp_scratch_sz = (local_pos_sz + local_neg_sz) * 32 + crs_sz;
    switch (*type) {
    case program_type::vertex:
        if (second_entrypoint || second_num_gprs) {
            fatal_error("dual vertex shaders are not implemented");
        }
        break;
    case program_type::fragment:
        program_header.frag.early_fragment_tests = early_fragment_tests;
        program_header.frag.post_depth_coverage = post_depth_coverage;
        program_header.frag.persample_invocation = persample_invocation;
        break;
    case program_type::compute:
        program_header.comp.block_dims = block_dimensions;
        program_header.comp.shared_mem_sz = align256(shared_mem_size);
        program_header.comp.local_pos_mem_sz = local_pos_sz;
        program_header.comp.local_neg_mem_sz = local_neg_sz;
        program_header.comp.crs_sz = crs_sz;
        program_header.comp.num_barriers = num_barriers;
        break;
    default:
        break;
    }
    static constexpr size_t dksh_size = sizeof(dksh_header) + sizeof(dksh_program_header);
    dksh_header header;
    header.magic = DKSH_MAGIC;
    header.header_sz = sizeof(dksh_header);
    header.control_sz = align256(dksh_size);
    header.code_sz = align256(code_size * sizeof(uint64_t));
    header.programs_off = sizeof(dksh_header);
    header.num_programs = 1;

    std::array<uint64_t, align256(dksh_size) / sizeof(uint64_t)> result{};
    char* const result_bytes = reinterpret_cast<char*>(result.data());
    std::memcpy(result_bytes, &header, sizeof(header));
    std::memcpy(result_bytes + sizeof(header), &program_header, sizeof(program_header));
    output.insert(output.end(), result.begin(), result.end());
}
