#ifndef INSTRUCTIONS_HELPER_H_INCLUDED
#define INSTRUCTIONS_HELPER_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#include "instruction.h"
#include "token.h"

#define DEFINE_INSTRUCTION(name)                                             \
	void parse_##name(struct context* ctx, struct instruction* instr)

int find_in_table(
	const struct token* token, const char* const* table, const char* prefix, uint64_t* value);

void dest_gpr0(struct context* ctx, struct instruction* instr);

#endif // INSTRUCTIONS_HELPER_H_INCLUDED
