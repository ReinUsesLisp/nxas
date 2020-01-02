#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "source.h"
#include "error.h"

void fatal_error(
	const struct context* ctx, const struct view* view, const char* fmt, ...)
{
	if (ctx && ctx->filename) {
		fprintf(stderr, "\33[1m%s:", ctx->filename);
	}
	if (view || ctx) {
		const int line = view ? view->line : ctx->line;
		const int column = view ? view->column : ctx->column;
		fprintf(stderr, "\33[1m%d:%d: ", line + 1, column + 1);
	}
	fprintf(stderr, "\33[1;31merror:\33[0m ");

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}
