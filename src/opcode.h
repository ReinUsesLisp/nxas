#pragma once

#include <cassert>
#include <cstdint>

constexpr int ZERO_REGISTER = 255;
constexpr int TRUE_PREDICATE = 7;

constexpr int NUM_REGISTERS = 256;
constexpr int NUM_PREDICATES = 8;
constexpr int NUM_CONDITION_CODES = 4;

constexpr int NUM_USER_REGISTERS = 255;
constexpr int NUM_USER_PREDICATES = 7;

namespace reuse_flag
{
    constexpr unsigned gpr8 = 1U;
    constexpr unsigned gpr20 = 2U;
    constexpr unsigned gpr39 = 4U;
}

struct opcode
{
    uint64_t value = 0;
    uint32_t reuse = 0;

    union
    {
        struct
        {
            uint32_t stall : 4;
            uint32_t yield : 1;
            uint32_t write_barrier : 3;
            uint32_t read_barrier : 3;
            uint32_t wait_barrier : 6;
            uint32_t padding : 15;
        } values;
        uint32_t raw = 0;
    } sched;

    void add_bits(uint64_t bits);

    void add_reuse(unsigned flag);
};

inline void opcode::add_bits(uint64_t bits)
{
    assert((value & bits) == 0);
    value |= bits;
}

inline void opcode::add_reuse(unsigned flag)
{
    assert((reuse & flag) == 0);
    reuse |= static_cast<uint8_t>(flag);
}
