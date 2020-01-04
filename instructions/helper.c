#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "instructions/helper.h"
#include "token.h"

int find_in_table(
	const struct token* token, const char* const* table, const char* prefix, uint64_t* value)
{
	check(token, TOKEN_TYPE_IDENTIFIER);

	const char* text = token->data.string.text;
	size_t length = token->data.string.size;

	if (prefix) {
		const size_t prefix_length = strlen(prefix);
		if (length < prefix_length) {
			return 0;
		}
		if (memcmp(text, prefix, prefix_length) != 0) {
			return 0;
		}
		text += prefix_length;
		length -= prefix_length;
	}

	for (size_t i = 0; table[i]; ++i) {
		const size_t item_length = strlen(table[i]);
		if (item_length != length) {
			continue;
		}
		if (memcmp(text, table[i], item_length) != 0) {
			continue;
		}
		*value = (uint64_t)i;
		return true;
	}
	return false;
}

void dest_gpr0(struct context* ctx, struct instruction* instr)
{
	const struct token token = tokenize(ctx);
	check(&token, TOKEN_TYPE_REGISTER);

	const uint8_t dest = token.data.regster;
	write_register(instr, dest);
	add_bits(instr, dest);
}
