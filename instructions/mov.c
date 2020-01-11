#include "helper.h"

DEFINE_INSTRUCTION(mov)
{
    struct token token = tokenize(ctx);
    CHECK(assemble_dest_gpr(ctx, &token, instr, 8));
    CHECK(confirm_type(&token, TOKEN_TYPE_OPERATOR_COMMA));

    assemble_gpr20_cbuf_imm(ctx, &token, instr, 0x5C98000000000000ULL, 0x4C98000000000000ULL,
                            0x3898000000000000ULL);

    uint64_t mask = 0xf;
    if (token.type == TOKEN_TYPE_OPERATOR_COMMA) {
        token = tokenize(ctx);
        CHECK(convert_integer(&token, 0, 0xf, &mask));

        token = tokenize(ctx);
    }
    add_bits(instr, mask << 39);

    return confirm_type(&token, TOKEN_TYPE_OPERATOR_SEMICOLON);
}
