#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"
#include "token.h"

static error assemble_gpr(struct context *ctx, struct token *token, struct instruction *instr,
                          int address)
{
    CHECK(confirm_type(token, TOKEN_TYPE_REGISTER));

    add_bits(instr, (uint64_t)token->data.regster << address);

    *token = tokenize(ctx);
    return try_reuse(ctx, token, instr, address);
}

static error confirm_type_next(struct context *ctx, struct token *token, int type)
{
    *token = tokenize(ctx);
    return confirm_type(token, type);
}

error confirm_type(const struct token *token, int type)
{
    if (token->type == type) {
        return NULL;
    }
    if (token->type == TOKEN_TYPE_NONE) {
        return fail(token, "expected \33[1m%s\33[0m", token_type_name(type));
    }
    return fail(token, "expected \33[1m%s\33[0m and got \33[1m%s %.*s\33[0m", token_type_name(type),
                token_type_name(token->type), token_extra_info_size(token),
                token_extra_info(token));
}

int equal(const struct token *token, const char *string)
{
    if (token->type != TOKEN_TYPE_IDENTIFIER) {
        return 0;
    }

    const size_t length = token->data.string.size;
    if (length != strlen(string)) {
        return 0;
    }
    return memcmp(token->data.string.text, string, length) == 0;
}

error convert_integer(const struct token *token, int64_t min, int64_t max, uint64_t *result)
{
    CHECK(confirm_type(token, TOKEN_TYPE_IMMEDIATE));

    int64_t value = token->data.immediate;
    if (value < min || value > max) {
        return fail(token,
                    "integer \33[1m%d\33[0m is out of range, expected to be "
                    "from %" PRId64 " to %" PRId64 " inclusively",
                    value, min, max);
    }
    *result = (uint64_t)value;
    return NULL;
}

int find_in_table(const struct token *token, const char *const *table, const char *prefix,
                  uint64_t *value)
{
    if (token->type != TOKEN_TYPE_IDENTIFIER) {
        return 0;
    }

    const char *text = token->data.string.text;
    size_t length = token->data.string.size;

    if (prefix) {
        const size_t prefix_length = strlen(prefix);
        if (length < prefix_length) {
            return 0;
        }
        if (memcmp(text, prefix, prefix_length) != 0) {
            return 0;
        }
        text += prefix_length;
        length -= prefix_length;
    }

    for (size_t i = 0; table[i]; ++i) {
        const size_t item_length = strlen(table[i]);
        if (item_length == 0 || item_length != length) {
            continue;
        }
        if (memcmp(text, table[i], item_length) != 0) {
            continue;
        }
        *value = (uint64_t)i;
        return true;
    }
    return false;
}

error try_reuse(struct context *ctx, struct token *token, struct instruction *instr, int address)
{
    if (!equal(token, ".reuse")) {
        return NULL;
    }
    switch (address) {
    case 8:
        add_reuse(instr, REUSE_FLAG_GPR8);
        return NULL;
    case 20:
        add_reuse(instr, REUSE_FLAG_GPR20);
        return NULL;
    case 39:
        add_reuse(instr, REUSE_FLAG_GPR39);
        return NULL;
    default:
        return fail(token, "register cannot be reused");
    }

    *token = tokenize(ctx);
}

error assemble_dest_gpr(struct context *ctx, struct token *token, struct instruction *instr,
                        int address)
{
    return assemble_gpr(ctx, token, instr, address);
}

error assemble_source_gpr(struct context *ctx, struct token *token, struct instruction *instr,
                          int address)
{
    return assemble_gpr(ctx, token, instr, address);
}

error assemble_signed_20bit_immediate(struct context *ctx, struct token *token,
                                      struct instruction *instr)
{
    int64_t value;
    CHECK(convert_integer(token, -(1 << 19), MAX_BITS(19), (uint64_t *)&value));

    uint64_t raw, negative;
    if (value < 0) {
        raw = (value - (1 << 19)) & MAX_BITS(19);
        negative = 1;
    } else {
        raw = (uint64_t)value;
        negative = 0;
    }

    add_bits(instr, raw << 20);
    add_bits(instr, negative << 56);

    *token = tokenize(ctx);
    return NULL;
}

error assemble_constant_buffer(struct context *ctx, struct token *token, struct instruction *instr)
{
    const struct token first_token = *token;

    if (!equal(token, "c")) {
        return fail(token, "expected constant buffer");
    }

    CHECK(confirm_type_next(ctx, token, TOKEN_TYPE_OPERATOR_BRACKET_LEFT));

    *token = tokenize(ctx);
    uint64_t value;
    CHECK(convert_integer(token, 0, MAX_BITS(5), &value));
    add_bits(instr, (uint64_t)value << 34);

    CHECK(confirm_type_next(ctx, token, TOKEN_TYPE_OPERATOR_BRACKET_RIGHT));
    CHECK(confirm_type_next(ctx, token, TOKEN_TYPE_OPERATOR_BRACKET_LEFT));

    *token = tokenize(ctx);
    CHECK(convert_integer(token, INT16_MIN, INT16_MAX, &value));
    const int offset = (int16_t)value;
    if (offset % 4) {
        return fail(&first_token, "immediate constant buffer access has to be aligned to 4 bytes");
    }
    add_bits(instr, (uint64_t)((uint16_t)offset >> 2) << 20);

    CHECK(confirm_type_next(ctx, token, TOKEN_TYPE_OPERATOR_BRACKET_RIGHT));

    *token = tokenize(ctx);
    return NULL;
}

error assemble_gpr20_cbuf_imm(struct context *ctx, struct token *token, struct instruction *instr,
                              uint64_t register_opcode, uint64_t cbuf_opcode,
                              uint64_t immediate_opcode)
{
    *token = tokenize(ctx);
    switch (token->type) {
    case TOKEN_TYPE_REGISTER:
        add_bits(instr, register_opcode);
        return assemble_source_gpr(ctx, token, instr, 20);
    case TOKEN_TYPE_IDENTIFIER:
        add_bits(instr, cbuf_opcode);
        return assemble_constant_buffer(ctx, token, instr);
    case TOKEN_TYPE_IMMEDIATE:
        add_bits(instr, immediate_opcode);
        return assemble_signed_20bit_immediate(ctx, token, instr);
    default:
        return fail(token, "expected immediate, constant buffer or register");
    }
}
