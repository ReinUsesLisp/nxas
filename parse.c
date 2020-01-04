#include <stddef.h>
#include <string.h>

#include "parse.h"
#include "instruction.h"
#include "token.h"
#include "error.h"

#define INSTR(name, func)                                                    \
	void parse_##func(struct context* ctx, struct instruction* instr);
#include "instructions/list.inc"
#undef INSTR

static void assemble_predicate(
	const struct token* token, struct instruction* instr, size_t shift, int is_negable)
{
	check(token, TOKEN_TYPE_PREDICATE);

	if (!is_negable && token->data.predicate.negated) {
		fatal_error(token, "predicate can't be negated");
	}
	uint64_t bits = token->data.predicate.index;
	bits |= (token->data.predicate.negated ? 1 : 0) << 3;
	bits <<= shift;
	add_bits(instr, bits);
}

int parse_instruction(struct context* ctx, struct instruction* instr)
{
	memset(instr, 0, sizeof *instr);

	struct token token = tokenize(ctx);
	if (token.type == TOKEN_TYPE_NONE) {
		return 0;
	}

	if (token.type == TOKEN_TYPE_OPERATOR_AT) {
		token = tokenize(ctx);
		assemble_predicate(&token, instr, 16, 1);

		token = tokenize(ctx);
	} else {
		// write always execute by default
		add_bits(instr, 7ULL << 16);
	}

	check(&token, TOKEN_TYPE_IDENTIFIER);
	
#define INSTR(name, func)                                                    \
	if (equal(&token, name)) {                                               \
		parse_##func(ctx, instr);                                            \
		return 1;                                                            \
	}
#include "instructions/list.inc"
#undef INSTR

	fatal_error(&token, "unknown mnemonic %.*s", token.data.string.size, token.data.string.text);
}
