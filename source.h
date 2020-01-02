#ifndef SOURCE_H_INCLUDED
#define SOURCE_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

struct context;

struct view
{
	const char* text;
	size_t length;
	int line;
	int column;
};

static inline bool is_delimiter(char character)
{
	return character == ' ' || character == '\t' || character == '\n'
		|| character == '\r' || character == 0;
}

static inline bool is_empty(const struct view* token)
{
	return token->text == NULL;
}

void show(struct view token);

bool equal(const struct view* token, const char* string);

struct view advance(struct context* ctx);

#endif // SOURCE_H_INCLUDED
