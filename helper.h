#ifndef INSTRUCTIONS_HELPER_H_INCLUDED
#define INSTRUCTIONS_HELPER_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#include "instruction.h"
#include "token.h"

#define DEFINE_INSTRUCTION(name) void parse_##name(struct context *ctx, struct instruction *instr)

#define MAX_BITS(size) ((1ULL << (size)) - 1)

void check(const struct token *token, int type);

int checking_equal(const struct token *token, const char *string);

int equal(const struct token *token, const char *string);

uint64_t get_integer(const struct token *token, int64_t min, int64_t max);

int find_in_table(const struct token *token, const char *const *table, const char *prefix,
                  uint64_t *value);

void try_reuse(struct context *ctx, struct token *token, struct instruction *instr, int address);

void assemble_dest_gpr(struct context *ctx, struct token *token, struct instruction *instr,
                       int address);

void assemble_source_gpr(struct context *ctx, struct token *token, struct instruction *instr,
                         int address);

void assemble_signed_20bit_immediate(struct context *ctx, struct token *token,
                                     struct instruction *instr);

void assemble_constant_buffer(struct context *ctx, struct token *token, struct instruction *instr);

void assemble_gpr20_cbuf_imm(struct context *ctx, struct token *token, struct instruction *instr,
                             uint64_t register_opcode, uint64_t cbuf_opcode,
                             uint64_t immediate_opcode);

#endif // INSTRUCTIONS_HELPER_H_INCLUDED
