#ifndef INSTRUCTIONS_HELPER_H_INCLUDED
#define INSTRUCTIONS_HELPER_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "error.h"
#include "instruction.h"
#include "source.h"

#define DEFINE_INSTRUCTION(name)                                             \
	void parse_##name(struct context* ctx, struct instruction* instr)

struct context;

bool find_in_table(
	const char* const* table, const char* prefix, struct view token,
	uint64_t* value);

int64_t parse_integer(
	struct context* ctx, struct view token, int64_t min, int64_t max);

uint8_t parse_register(struct context* ctx, struct view token);

void check_comma(struct context* ctx, struct view token);

void check_eol(struct context* ctx, struct view token);

void dest_gpr0(struct context* ctx, struct instruction* instr);

#endif // INSTRUCTIONS_HELPER_H_INCLUDED
