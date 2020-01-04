#include "helper.h"

DEFINE_INSTRUCTION(mov)
{
    struct token token = tokenize(ctx);
    assemble_dest_gpr(ctx, &token, instr, 8);
    check(&token, TOKEN_TYPE_OPERATOR_COMMA);

    token = tokenize(ctx);
    switch (token.type) {
    case TOKEN_TYPE_REGISTER:
        add_bits(instr, 0x5c98000000000000ULL);
        assemble_source_gpr(ctx, &token, instr, 20);
        break;
    case TOKEN_TYPE_IMMEDIATE:
        add_bits(instr, 0x3898000000000000ULL);
        assemble_signed_20bit_immediate(ctx, &token, instr);
        break;
    case TOKEN_TYPE_IDENTIFIER:
        add_bits(instr, 0x4c98000000000000ULL);
        assemble_constant_buffer(ctx, &token, instr);
        break;
    default:
        fatal_error(&token, "expected immediate, constant buffer or register");
    }

    uint64_t mask = 0xf;
    if (token.type == TOKEN_TYPE_OPERATOR_COMMA) {
        token = tokenize(ctx);
        mask = get_integer(&token, 0, 0xf);

        token = tokenize(ctx);
    }
    add_bits(instr, mask << 39);

    check(&token, TOKEN_TYPE_OPERATOR_SEMICOLON);
}
