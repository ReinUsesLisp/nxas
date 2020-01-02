#ifndef INSTRUCTIONS_HELPER_H_INCLUDED
#define INSTRUCTIONS_HELPER_H_INCLUDED

#include <stddef.h>

#include "error.h"
#include "instruction.h"
#include "source.h"

#define DEFINE_INSTRUCTION(name)                                             \
	void parse_##name(struct context* ctx, struct instruction* instr)

struct context;

bool find_in_table(
	const char* const* table, const char* prefix, struct view token,
	uint64_t* value);

int64_t convert_integer(
	struct context* ctx, const struct view* token, int64_t min, int64_t max);

void check_eol(struct context* ctx, const struct view* token);

#endif // INSTRUCTIONS_HELPER_H_INCLUDED
