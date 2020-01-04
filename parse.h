#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

struct context;
struct instruction;

int parse_instruction(struct context *ctx, struct instruction *instr);

#endif // PARSE_H_INCLUDED
