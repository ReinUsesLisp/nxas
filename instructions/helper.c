#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "instructions/helper.h"
#include "source.h"

bool find_in_table(
	const char* const* table, const char* prefix, struct view token,
	uint64_t* value)
{
	if (prefix) {
		const size_t prefix_length = strlen(prefix);
		if (token.length < prefix_length) {
			return false;
		}
		if (memcmp(token.text, prefix, prefix_length) != 0) {
			return false;
		}
		token.text += prefix_length;
		token.length -= prefix_length;
	}

	for (size_t i = 0; table[i]; ++i) {
		const size_t item_length = strlen(table[i]);
		if (item_length != token.length) {
			continue;
		}
		if (memcmp(token.text, table[i], item_length) != 0) {
			continue;
		}
		*value = (uint64_t)i;
		return true;
	}
	return false;
}

int64_t parse_integer(
	struct context* ctx, struct view token, int64_t min, int64_t max)
{
	// assume long long is at least 64 bits wide

	char* end = NULL;
	const long long value = strtoll(token.text, &end, 0);
	if (!end || token.text + token.length != end) {
		fatal_error(
			ctx, &token, "failure parsing %.*s as integer",
			token.length, token.text);
	}
	if (value < min || value > max) {
		fatal_error(
			ctx, &token, "%.*s is not in range [0x%llx; 0x%llx]",
			token.length, token.text, min, max);
	}
	return (int64_t)value;
}

uint8_t parse_register(struct context* ctx, struct view token)
{
	if (token.text[0] != 'R' || token.length < 2) {
		fatal_error(
			ctx, &token, "expected a register but got %.*s",
			token.length, token.text);
	}

	const size_t length = token.length;
	if (length == 2 && token.text[1] == 'Z') {
		return ZERO_REGISTER;
	}

	char* end = NULL;
	const long register_index = strtol(token.text + 1, &end, 10);
	if (token.text + token.length != end) {
		fatal_error(
			ctx, &token, "failed to parse register index in %.*s",
			token.length, token.text);
	}

	if (register_index < 0 || register_index > MAX_USER_REGISTER) {
		fatal_error(
			ctx, &token, "register index %ld is out of range",
			register_index);
	}
	
	return (uint8_t)register_index;
}

void check_comma(struct context* ctx, struct view token)
{
	if (!equal(&token, ",")) {
		fatal_error(
			ctx, &token, "expected ',' but got %.*s",
			token.length, token.text);
	}
}

void check_eol(struct context* ctx, struct view token)
{
	if (!equal(&token, ";")) {
		fatal_error(
			ctx, &token, "expected ';' but got %.*s",
			token.length, token.text);
	}
}

void dest_gpr0(struct context* ctx, struct instruction* instr)
{
	const uint8_t dest = parse_register(ctx, advance(ctx));
	write_register(instr, dest);
	add_bits(instr, dest);
}
