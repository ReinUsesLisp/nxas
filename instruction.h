#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

#include <assert.h>
#include <stdint.h>

#define ZERO_REGISTER 255
#define TRUE_PREDICATE 7

#define NUM_REGISTERS 256
#define NUM_PREDICATES 8
#define NUM_CONDITION_CODES 4

#define NUM_USER_REGISTERS 255
#define NUM_USER_PREDICATES 7

#define REUSE_FLAG_GPR8 1U
#define REUSE_FLAG_GPR20 2U
#define REUSE_FLAG_GPR39 4U

struct instruction
{
    uint64_t value;
    uint8_t reuse;
};

static inline void add_bits(struct instruction *instr, uint64_t opcode)
{
    assert((instr->value & opcode) == 0);
    instr->value |= opcode;
}

static inline void add_reuse(struct instruction *instr, unsigned flag)
{
    assert((instr->reuse & flag) == 0);
    instr->reuse |= (uint8_t)flag;
}

#endif // INSTRUCTION_H_INCLUDED
