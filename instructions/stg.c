#include "helper.h"

static const char *cache_table[] = {"CG", "CS", "WT"};

static const char *size_table[] = {
    "U8", "S8", "U16", "S16", "32", "64", "128",
};

DEFINE_INSTRUCTION(stg)
{
    add_bits(instr, 0xeed8000000000000ULL);

    struct token token = tokenize(ctx);
    if (equal(&token, ".E")) {
        add_bits(instr, 1ULL << 45);
        token = tokenize(ctx);
    }

    uint64_t cache = 0;
    if (find_in_table(&token, cache_table, ".", &cache)) {
        cache += 1;
        token = tokenize(ctx);
    }
    add_bits(instr, cache << 46);

    uint64_t size = 4;
    if (find_in_table(&token, size_table, ".", &size)) {
        token = tokenize(ctx);
    }
    add_bits(instr, size << 48);

    check(&token, TOKEN_TYPE_OPERATOR_BRACKET_LEFT);

    token = tokenize(ctx);
    uint8_t regster = ZERO_REGISTER;
    if (token.type == TOKEN_TYPE_REGISTER) {
        regster = token.data.regster;
        token = tokenize(ctx);

        try_reuse(ctx, &token, instr, 8);
    }
    add_bits(instr, (uint64_t)regster << 8);

    if (token.type == TOKEN_TYPE_IMMEDIATE) {
        const int is_zero_reg = regster == ZERO_REGISTER;
        const int64_t min = is_zero_reg ? 0 : -(1 << 23);
        const int64_t max = MAX_BITS(is_zero_reg ? 24 : 23);

        add_bits(instr, (get_integer(&token, min, max) & MAX_BITS(24)) << 20);
        token = tokenize(ctx);
    }
    check(&token, TOKEN_TYPE_OPERATOR_BRACKET_RIGHT);

    token = tokenize(ctx);
    check(&token, TOKEN_TYPE_OPERATOR_COMMA);

    token = tokenize(ctx);
    assemble_source_gpr(ctx, &token, instr, 0);

    check(&token, TOKEN_TYPE_OPERATOR_SEMICOLON);
}
