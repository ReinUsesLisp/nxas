#include "helper.h"

static const char *formats[] = {"U32", "S32", NULL};
static const char *modes[] = {"C", "W", NULL};
static const char *xmodes[] = {"", "INVALIDSHRXMODE1", "X", "XHI", NULL};

DEFINE_INSTRUCTION(shr)
{
    struct token token = tokenize(ctx);

    uint64_t format = 1;
    if (find_in_table(&token, formats, ".", &format)) {
        token = tokenize(ctx);
    }
    add_bits(instr, format << 48);

    uint64_t mode = 0;
    if (find_in_table(&token, modes, ".", &mode)) {
        token = tokenize(ctx);
    }
    add_bits(instr, mode << 39);

    uint64_t xmode = 0;
    if (find_in_table(&token, xmodes, ".", &xmode)) {
        token = tokenize(ctx);
    }
    add_bits(instr, xmode << 43);

    if (equal(&token, ".BREV")) {
        add_bits(instr, 1ULL << 40);
        token = tokenize(ctx);
    }

    assemble_source_gpr(ctx, &token, instr, 0);
    if (equal(&token, ".CC")) {
        add_bits(instr, 1ULL << 47);
        token = tokenize(ctx);
    }
    check(&token, TOKEN_TYPE_OPERATOR_COMMA);

    token = tokenize(ctx);
    assemble_dest_gpr(ctx, &token, instr, 8);
    check(&token, TOKEN_TYPE_OPERATOR_COMMA);

    assemble_gpr20_cbuf_imm(ctx, &token, instr, 0x5C28000000000000ULL, 0x4C28000000000000ULL,
                            0x3828000000000000ULL);

    check(&token, TOKEN_TYPE_OPERATOR_SEMICOLON);
}
