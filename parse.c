#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "context.h"
#include "parse.h"
#include "instruction.h"
#include "source.h"
#include "error.h"

#define INSTR(name, func)                                                    \
	void parse_##func(struct context* ctx, struct instruction* instr);
#include "instructions/list.inc"
#undef INSTR

bool parse_instruction(struct context* ctx, struct instruction* instr)
{
	memset(instr, 0, sizeof *instr);

	struct view pred_or_mnemonic = advance(ctx);
	if (is_empty(&pred_or_mnemonic)) {
		return false;
	}

	if (pred_or_mnemonic.text[0] == '@') {
		parse_predicate(ctx, instr, pred_or_mnemonic, 16, true, true);
		pred_or_mnemonic = advance(ctx);
	} else {
		// write always execute by default
		add_bits(instr, 7ULL << 16);
	}

	const struct view mnemonic = pred_or_mnemonic;
	
#define INSTR(name, func)                                                    \
	if (equal(&mnemonic, name)) {                                            \
		parse_##func(ctx, instr);                                            \
		return true;                                                         \
	}
#include "instructions/list.inc"
#undef INSTR

	fatal_error(
		ctx, NULL, "unknown mnemonic %.*s", mnemonic.length, mnemonic.text);
}
