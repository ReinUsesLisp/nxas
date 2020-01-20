#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

#include "error.h"
#include "opcode.h"
#include "parse.h"
#include "span.h"
#include "token.h"

namespace {

std::string read_file(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        fatal_error(nullptr, "%s: failed to open", filename);
    }
    file.seekg(0, std::ios::end);
    std::string text(file.tellg(), ' ');

    file.seekg(0, std::ios::beg);
    file.read(std::data(text), std::size(text));
    return text;
}

std::uint64_t generate_sched(span<opcode> opcodes, std::size_t index, std::size_t num_instructions,
                             std::size_t address)
{
    if (index + address >= num_instructions) {
        return 0x7E0ULL << (address * 21);
    }

    const opcode& op = opcodes[index + address];
    return (0x7E0ULL | (static_cast<std::uint64_t>(op.reuse) << 17)) << (address * 21);
}

} // anonymous namespace

int main(int argc, char** argv)
{
    const char* input_file = nullptr;
    const char* output_file = nullptr;

    for (int i = 1; i < argc; ++i) {
        // parse output file
        if (std::strcmp(argv[i], "-o") == 0) {
            if (output_file) {
                fatal_error("command line output file already provided");
            }
            if (++i == argc) {
                fatal_error("expected command line syntax: \"-o\" <output file>");
            }
            output_file = argv[i];
            continue;
        }
        // there's no modifier, it's an input file
        if (input_file) {
            fatal_error("command line input file already provided");
        }
        input_file = argv[i];
    }

    if (!input_file) {
        fatal_error("no input file");
    }

    // prepare our context
    const std::string input_text = read_file(input_file);
    context ctx(input_file, std::data(input_text));

    // to determine the number of instructions to allocate we count the number of semicolons this
    // is possible because every instruction ends with a semicolon on the number of decoding
    // instructions we allocate one extra because one instruction might be bugged and it won't have
    // a semicolon (which will trigger a fatal)
    const std::size_t num_scolons = std::count(std::begin(input_text), std::end(input_text), ';');
    const std::size_t max_decode_instructions = num_scolons + 1;
    const std::size_t max_output_instructions = num_scolons * 4 / 3 + 3;

    std::vector<std::uint64_t> output(max_output_instructions);
    std::vector<opcode> opcodes(max_decode_instructions);

    std::size_t index = 0;
    while (parse_instruction(ctx, opcodes[index++])) {
    }
    assert(index == max_decode_instructions);
    const std::size_t num_instructions = index - 1;

    std::size_t output_index = 0;
    for (index = 0; index < num_instructions; ++index) {
        if (index % 3 == 0) {
            std::uint64_t sched = 0;
            for (std::size_t address = 0; address < 3; ++address) {
                sched |= generate_sched(opcodes, index, num_instructions, address);
            }
            output[output_index++] = sched;
        }

        output[output_index++] = opcodes[index].value;
    }
    while (output_index % 4) {
        output[output_index++] = 0x50B0000000070F00ULL;
    }
    const std::size_t num_output = output_index;

    std::FILE* outfp = stdout;
    if (output_file) {
        outfp = std::fopen(output_file, "wb");
        if (!outfp) {
            fatal_error("%s: failed to open", output_file);
        }
    }
    const bool success =
        std::fwrite(std::data(output), sizeof(std::uint64_t), num_output, outfp) == num_output;

    if (output_file && std::fclose(outfp) != 0) {
        fatal_error("failed to close output file");
    }

    if (!success) {
        fatal_error("failed to write output binary");
    }
}
