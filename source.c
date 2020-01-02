#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "context.h"
#include "error.h"
#include "source.h"

static void handle_character(struct context* ctx, char character)
{
	if (character == '\n') {
		++ctx->line;
		ctx->column = 1;
	}
	++ctx->column;
	++ctx->text;
}

static bool is_finalizer(char character)
{
	return character == '.' || character == ';' || character == ',';
}

struct view advance(struct context* ctx)
{
	char previous = 0;
	if (ctx->line > 0 || ctx->column > 0) {
		previous = ctx->text[-1];
	}

	char character;
	while ((character = *ctx->text)) {
		if (!is_delimiter(character)) {
			break;
		}
		previous = character;
		handle_character(ctx, character);
	}
	if (!character) {
		fatal_error(
			ctx, NULL, "expected token but got nothing, missing ';'?");
	}

	struct view token = {
		.text = ctx->text,
		.line = ctx->line,
		.column = ctx->column
	};

	bool first = true;
	bool dot_failure = false;

	while ((character = *ctx->text)) {
		if (is_delimiter(character)) {
			// once we find a delimiter, there's nothing more in this token
			break;
		}
		if (!first && is_finalizer(character)) {
			// these characters also specify the end of a token,
			// unless we find them as the first character (since we want to
			// include them when they appear as first)
			break;
		}
		if (character == '.' && is_delimiter(previous)) {
			// having a dot that's not concatenated to a token is an error
			// e.g., "NOP.TRIG" is valid but "NOP .TRIG" isn't
			dot_failure = true;
		}

		handle_character(ctx, character);

		if (first && (character == ';' || character == ',')) {
			// when we have these characters, only include that one
			break;
		}
		first = false;
		previous = character;
	}

	token.length = (size_t)(ctx->text - token.text);

	if (dot_failure) {
		fatal_error(
			ctx, &token, "%.*s must be concatenated to another token",
			token.length, token.text);
	}

	return token;
}

void show(struct view token)
{
	fprintf(stderr, "%.*s\n", (int)token.length, token.text);
}

bool equal(const struct view* token, const char* string)
{
	return !is_empty(token)
		&& strlen(string) == token->length
		&& memcmp(token->text, string, token->length) == 0;
}
