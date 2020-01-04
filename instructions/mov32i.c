#include "helper.h"

DEFINE_INSTRUCTION(mov32i)
{
    add_bits(instr, 0x0100000000000000ULL);

    struct token token = tokenize(ctx);
    assemble_dest_gpr(ctx, &token, instr, 0);
    check(&token, TOKEN_TYPE_OPERATOR_COMMA);

    token = tokenize(ctx);
    add_bits(instr, get_integer(&token, 0, UINT32_MAX) << 20);

    token = tokenize(ctx);
    uint64_t mask = 0xf;
    if (token.type == TOKEN_TYPE_OPERATOR_COMMA) {
        token = tokenize(ctx);
        mask = get_integer(&token, 0, 0xf);

        token = tokenize(ctx);
    }
    add_bits(instr, mask << 12);

    check(&token, TOKEN_TYPE_OPERATOR_SEMICOLON);
}
