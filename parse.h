#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

#include <stdbool.h>

#include "source.h"

struct context;
struct instruction;

bool parse_instruction(struct context* ctx, struct instruction* instr);

void parse_predicate(
	struct context* ctx, struct instruction* instr, struct view input,
	size_t shift, bool is_instruction, bool is_negable);

#endif // PARSE_H_INCLUDED
