#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#include "error.h"

enum
{
	TOKEN_TYPE_NONE,
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_REGISTER,
	TOKEN_TYPE_PREDICATE,
	TOKEN_TYPE_IMMEDIATE,
	TOKEN_TYPE_OPERATOR_PLUS,
	TOKEN_TYPE_OPERATOR_MINUS,
	TOKEN_TYPE_OPERATOR_VBAR,
	TOKEN_TYPE_OPERATOR_BRACKET_LEFT,
	TOKEN_TYPE_OPERATOR_BRACKET_RIGHT,
	TOKEN_TYPE_OPERATOR_AT,
	TOKEN_TYPE_OPERATOR_SEMICOLON,
	TOKEN_TYPE_OPERATOR_COMMA,
};

struct token_string
{
	const char* text;
	size_t size;
};

struct token_predicate
{
	int index;
	int negated;
};

union token_data
{
	struct token_predicate predicate;
	struct token_string string;
	int64_t immediate;
	uint8_t regster;
};

struct token
{
	const char* filename;
	union token_data data;
	int type;
	int line;
	int column;
};

struct context
{
	const char* filename;
	const char* text;
	int line;
	int column;	
};

struct token tokenize(struct context* ctx);

int equal(const struct token* token, const char* string);

uint64_t get_integer(const struct token* token, int64_t min, int64_t max);

const char* token_type_name(int type);

const char* token_extra_info(const struct token* token);

int token_extra_info_size(const struct token* token);

static inline void check(const struct token* token, int type)
{
	if (token->type == type) {
		return;
	}
	if (token->type == TOKEN_TYPE_NONE) {
		fatal_error(token, "expected \33[1m%s\33[0m", token_type_name(type));
	}
	fatal_error(
		token, "expected \33[1m%s\33[0m and got \33[1m%s %.*s\33[0m",
		token_type_name(type), token_type_name(token->type),
		token_extra_info_size(token), token_extra_info(token));
}

static inline int checking_equal(const struct token* token, const char* string)
{
	check(token, TOKEN_TYPE_IDENTIFIER);
	return equal(token, string);
}

#endif // TOKENIZER_H_INCLUDED
