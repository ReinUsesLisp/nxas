#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ZERO_REGISTER  255
#define TRUE_PREDICATE   7

#define NUM_REGISTERS       256
#define NUM_PREDICATES        8
#define NUM_CONDITION_CODES   4

#define MAX_USER_REGISTER  255
#define MAX_USER_PREDICATE   6

#define CONDITION_CODE_ZERO_FLAG     0
#define CONDITION_CODE_SIGN_FLAG     1
#define CONDITION_CODE_CARRY_FLAG    2
#define CONDITION_CODE_OVERFLOW_FLAG 3

#define INSTRUCTION_FLAG_YIELD          1U
#define INSTRUCTION_FLAG_VARIABLE_DELAY 2U

struct instruction
{
	uint64_t value;
	uint64_t register_dependencies[4];
	uint64_t register_stores[4];
	uint8_t predicate_dependencies;
	uint8_t predicate_stores;
	uint8_t condition_code_dependencies_stores;
	uint8_t read_dependency_barrier;
	uint8_t write_dependency_barrier;
	uint8_t wait_dependency_barrier;
	uint8_t delay;
	uint8_t flags;
};

static inline void add_bits(struct instruction* instr, uint64_t opcode)
{
	assert((instr->value & opcode) == 0);

	instr->value |= opcode;
}

static inline void read_register(struct instruction* instr, size_t index)
{
	assert(index < NUM_REGISTERS);
	instr->register_dependencies[index / 64] |= 1ULL << (index % 64);
}

static inline bool is_register_read(const struct instruction* instr, size_t index)
{
	assert(index < NUM_REGISTERS);
	return (instr->register_dependencies[index / 64] >> (index % 64)) & 1;
}

static inline void write_register(struct instruction* instr, size_t index)
{
	assert(index < NUM_REGISTERS);
	instr->register_stores[index / 64] |= 1ULL << (index % 64);
}

static inline bool is_register_written(const struct instruction* instr, size_t index)
{
	assert(index < NUM_REGISTERS);
	return (instr->register_stores[index / 64] >> (index % 64)) & 1;
}

static inline void read_predicate(struct instruction* instr, size_t index)
{
	assert(index < NUM_PREDICATES);
	instr->predicate_dependencies |= 1U << index;
}

static inline bool is_predicate_read(const struct instruction* instr, size_t index)
{
	assert(index < NUM_PREDICATES);
	return (instr->predicate_dependencies >> index) & 1;
}

static inline void write_predicate(struct instruction* instr, size_t index)
{
	assert(index < NUM_PREDICATES);
	instr->predicate_stores |= 1U << index;
}

static inline bool is_predicate_written(const struct instruction* instr, size_t index)
{
	assert(index < NUM_PREDICATES);
	return (instr->predicate_stores >> index) & 1;
}

static inline void read_condition_code(struct instruction* instr, size_t index)
{
	assert(index < NUM_CONDITION_CODES);
	instr->condition_code_dependencies_stores |= 1U << index;
}

static inline bool is_condition_code_read(const struct instruction* instr, size_t index)
{
	assert(index < NUM_CONDITION_CODES);
	return (instr->condition_code_dependencies_stores >> index) & 1;
}

static inline void write_condition_code(struct instruction* instr, size_t index)
{
	assert(index < NUM_CONDITION_CODES);
	instr->condition_code_dependencies_stores |= 1U << (index + NUM_CONDITION_CODES);
}

static inline bool is_condition_code_written(const struct instruction* instr, size_t index)
{
	assert(index < NUM_CONDITION_CODES);
	return (instr->condition_code_dependencies_stores >> (index + NUM_CONDITION_CODES)) & 1;
}

#endif // INSTRUCTION_H_INCLUDED
