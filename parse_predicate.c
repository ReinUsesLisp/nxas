#include <assert.h>
#include <stdbool.h>

#include "error.h"
#include "instruction.h"
#include "source.h"

void parse_predicate(
	struct context* ctx, struct instruction* instr, struct view input,
	size_t shift, bool is_instruction, bool is_negable)
{
	static const char msg[] = "expected P0 to P6 or PT but got ‘%.*s‘";
	const char* text = input.text;

	if (is_instruction) {
		assert(*text == '@');
		++text;
	}

	bool negated = false;
	if (is_negable && *text == '!') {
		negated = true;
		++text;
	}

	if (text[0] != 'P') {
		fatal_error(ctx, &input, msg, input.length, input.text);
	}

	int index;
	if (text[1] == 'T') {
		index = 7;
	} if ((text[1] >= '0' && text[1] <= '6')) {
		index = text[1] - '0';
	} else {
		fatal_error(ctx, &input, msg, input.length, input.text);
	}

	if (!is_delimiter(text[2])) {
		fatal_error(ctx, &input, msg, input.length, input.text);
	}

	const uint64_t opcode = ((uint64_t)index | (negated ? 8 : 0)) << shift;

	read_predicate(instr, index);
	add_bits(instr, opcode);
}
