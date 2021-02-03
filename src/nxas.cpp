#include <algorithm>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "context.h"
#include "error.h"
#include "opcode.h"
#include "parse.h"
#include "token.h"

namespace nxas {

static uint64_t generate_sched(std::span<const opcode> opcodes, size_t index,
                               size_t num_instructions, size_t address)
{
    if (index + address >= num_instructions) {
        return 0x7E0ULL << (address * 21);
    }
    const opcode& op = opcodes[index + address];
    return (op.sched.raw | (static_cast<uint64_t>(op.reuse) << 17)) << (address * 21);
}

std::vector<uint64_t> assemble(const std::string& code, const char* filename)
{
    // Prepare our context
    context ctx(filename, code.c_str());

    // To determine the number of instructions to allocate we count the number of semicolons this
    // is possible because every instruction ends with a semicolon on the number of decoding
    // instructions we allocate one extra because one instruction might be bugged and it won't have
    // a semicolon (which will trigger a fatal)
    const size_t max_decode_instructions = std::ranges::count(code, ';') + 1;
    std::vector<opcode> opcodes(max_decode_instructions);

    size_t index = 0;
    while (parse_instruction(ctx, opcodes[index++])) {
    }
    assert(index == max_decode_instructions);
    const size_t num_instructions = index - 1;

    std::vector<uint64_t> blob;
    blob.reserve(max_decode_instructions * 4 / 3 + 4);
    for (index = 0; index < num_instructions; ++index) {
        if (index % 3 == 0) {
            uint64_t sched = 0;
            for (size_t address = 0; address < 3; ++address) {
                sched |= generate_sched(opcodes, index, num_instructions, address);
            }
            blob.push_back(sched);
        }
        blob.push_back(opcodes[index].value);
    }
    while (blob.size() % 4) {
        blob.push_back(0x50B0000000070F00ULL);
    }
    return ctx.binary(blob);
}

} // namespace nxas
