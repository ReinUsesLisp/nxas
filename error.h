#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

struct context;
struct view;

_Noreturn void fatal_error(
	const struct context* ctx, const struct view* view, const char* fmt, ...);

#endif // ERROR_H_INCLUDED
