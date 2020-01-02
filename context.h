#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

struct context
{
	const char* filename;
	const char* text;
	int line;
	int column;
};

#endif // CONTEXT_H_INCLUDED
