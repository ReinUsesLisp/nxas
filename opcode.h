#pragma once

#include <cassert>
#include <cstdint>

#define ZERO_REGISTER 255
#define TRUE_PREDICATE 7

#define NUM_REGISTERS 256
#define NUM_PREDICATES 8
#define NUM_CONDITION_CODES 4

#define NUM_USER_REGISTERS 255
#define NUM_USER_PREDICATES 7

namespace reuse_flag {
constexpr unsigned gpr8 = 1U;
constexpr unsigned gpr20 = 2U;
constexpr unsigned gpr39 = 4U;
} // namespace reuse_flag

struct opcode
{
    std::uint64_t value = 0;
    std::uint8_t reuse = 0;

    void add_bits(std::uint64_t bits);

    void add_reuse(unsigned flag);
};

inline void opcode::add_bits(std::uint64_t bits)
{
    assert((value & bits) == 0);
    value |= bits;
}

inline void opcode::add_reuse(unsigned flag)
{
    assert((reuse & flag) == 0);
    reuse |= static_cast<std::uint8_t>(flag);
}
