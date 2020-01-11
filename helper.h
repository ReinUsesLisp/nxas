#ifndef INSTRUCTIONS_HELPER_H_INCLUDED
#define INSTRUCTIONS_HELPER_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

#include "error.h"
#include "instruction.h"
#include "token.h"

#define DEFINE_INSTRUCTION(name) error parse_##name(struct context *ctx, struct instruction *instr)

#define MAX_BITS(size) ((1ULL << (size)) - 1)

error confirm_type(const struct token *token, int type);

int equal(const struct token *token, const char *string);

error convert_integer(const struct token *token, int64_t min, int64_t max, uint64_t *result);

int find_in_table(const struct token *token, const char *const *table, const char *prefix,
                  uint64_t *value);

error try_reuse(struct context *ctx, struct token *token, struct instruction *instr, int address);

error assemble_dest_gpr(struct context *ctx, struct token *token, struct instruction *instr,
                        int address);

error assemble_source_gpr(struct context *ctx, struct token *token, struct instruction *instr,
                          int address);

error assemble_signed_20bit_immediate(struct context *ctx, struct token *token,
                                      struct instruction *instr);

error assemble_constant_buffer(struct context *ctx, struct token *token, struct instruction *instr);

error assemble_gpr20_cbuf_imm(struct context *ctx, struct token *token, struct instruction *instr,
                              uint64_t register_opcode, uint64_t cbuf_opcode,
                              uint64_t immediate_opcode);

#endif // INSTRUCTIONS_HELPER_H_INCLUDED
