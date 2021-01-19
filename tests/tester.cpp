#include <cstdint>
#include <cstdio>
#include <cstring>
#include <charconv>
#include <cinttypes>
#include <string>

#include "nxas.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "%s usage: <expression> (<expected opcode>)\n", argv[0]);
        return 1;
    }
    const std::string expression = std::string(argv[1]) + ';';
    const char* const expected = argc > 2 ? argv[2] : nullptr;

    std::vector<uint64_t> result = nxas::assemble(expression);
    if (!expected) {
        return 0;
    }
    const size_t expected_len = std::strlen(expected);
    if (expected_len < 3) {
        std::fprintf(stderr, "Expected opcode is too short\n");
        return 1;
    }
    if (expected[0] != '0' || (expected[1] != 'x' && expected[1] != 'X')) {
        std::fprintf(stderr, "Expected opcode has to start with '0x'\n");
        return 1;
    }
    uint64_t expected_opcode = 0;
    const auto res = std::from_chars(expected + 2, expected + expected_len, expected_opcode, 16);
    if (res.ec != std::errc{}) {
        std::fprintf(stderr, "Expected opcode is not valid hex\n");
        return 1;
    }
    if (result[1] != expected_opcode) {
        std::fprintf(stderr, "Incorrect opcode: 0x%" PRIX64 " vs 0x%" PRIX64 "\n",
                     result[1], expected_opcode);
        return 1;
    }
    return 0;
}
