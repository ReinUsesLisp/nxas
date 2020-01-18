#include "operand.h"
#include "helper.h"
#include "token.h"

#define DEFINE_FLAG(name, string, address)                                                         \
    DEFINE_OPERAND(name) { return assemble_flag(ctx, instr, token, string, address); }
#define DEFINE_UINT(name, max_size, address)                                                       \
    DEFINE_OPERAND(name) { return assemble_uint(ctx, instr, token, max_size, address); }

#define DEFINE_DOT_TABLE(name, default_value, address, ...)                                        \
    DEFINE_OPERAND(name)                                                                           \
    {                                                                                              \
        static const char *table[] = {__VA_ARGS__, NULL};                                          \
        uint64_t value = default_value;                                                            \
        if (find_in_table(token, table, ".", &value)) {                                            \
            *token = tokenize(ctx);                                                                \
        }                                                                                          \
        add_bits(instr, value << address);                                                         \
        return NULL;                                                                               \
    }

static error assemble_cc_tests(struct context *ctx, struct instruction *instr, struct token *token,
                               const char *const tests[], int address)
{
    uint64_t test_index = 15;
    if (equal(token, "CC")) {
        *token = tokenize(ctx);
        CHECK(confirm_type(token, TOKEN_TYPE_IDENTIFIER));

        if (!find_in_table(token, tests, ".", &test_index)) {
            return fail(token, "unexpected test %.*s", (int)token->data.string.size,
                        token->data.string.text);
        }
        *token = tokenize(ctx);
    }
    add_bits(instr, test_index << address);
    return NULL;
}

static error assemble_flag(struct context *ctx, struct instruction *instr, struct token *token,
                           const char *flag, int address)
{
    if (equal(token, flag)) {
        add_bits(instr, 1ULL << address);
        *token = tokenize(ctx);
    }
    return NULL;
}

static error assemble_uint(struct context *ctx, struct instruction *instr, struct token *token,
                           int64_t max_size, int address)
{
    uint64_t mask;
    CHECK(convert_integer(token, 0, max_size, &mask));
    add_bits(instr, mask << address);
    *token = tokenize(ctx);
    return NULL;
}

DEFINE_OPERAND(comma)
{
    CHECK(confirm_type(token, TOKEN_TYPE_OPERATOR_COMMA));
    *token = tokenize(ctx);
    return NULL;
}

DEFINE_OPERAND(dgpr0) { return assemble_dest_gpr(ctx, token, instr, 0); }

DEFINE_OPERAND(sgpr0) { return assemble_source_gpr(ctx, token, instr, 0); }

DEFINE_OPERAND(sgpr8) { return assemble_source_gpr(ctx, token, instr, 8); }

DEFINE_OPERAND(sgpr20) { return assemble_source_gpr(ctx, token, instr, 20); }

DEFINE_OPERAND(cbuf) { return assemble_constant_buffer(ctx, token, instr); }

DEFINE_OPERAND(imm) { return assemble_signed_20bit_immediate(ctx, token, instr); }

DEFINE_UINT(imm32, UINT32_MAX, 20)

DEFINE_FLAG(cc, ".CC", 47)

DEFINE_UINT(mask4_12, 0xf, 12)
DEFINE_UINT(mask4_39, 0xf, 39)

DEFINE_FLAG(nop_trig, ".TRIG", 13)

DEFINE_OPERAND(nop_tests)
{
    static const char *tests[] = {
        "F",       "LT",      "EQ",      "LE",  "GT",  "NE",  "GE",     "NUM",    "NAN",
        "LTU",     "EQU",     "LEU",     "GTU", "NEU", "GEU", "T",      "OFF",    "LO",
        "SFF",     "LS",      "HI",      "SFT", "HS",  "OFT", "CSM_TA", "CSM_TR", "CSM_MX",
        "FCSM_TA", "FCSM_TR", "FCSM_MX", "RLE", "RGT", NULL};
    return assemble_cc_tests(ctx, instr, token, tests, 8);
}

DEFINE_UINT(nop_mask, UINT16_MAX, 20)

DEFINE_DOT_TABLE(shr_format, 1, 48, "U32", "S32")
DEFINE_DOT_TABLE(shr_mode, 0, 39, "C", "W")
DEFINE_DOT_TABLE(shr_xmode, 0, 43, "", "INVALIDSHRXMODE1", "X", "XHI")
DEFINE_FLAG(shr_brev, ".BREV", 40)

DEFINE_FLAG(exit_keeprefcount, ".KEEPREFCOUNT", 5)

DEFINE_OPERAND(exit_tests)
{
    static const char *tests[] = {
        "F",       "LT",      "EQ",      "LE",  "GT",  "NE",  "GE",     "NUM",    "NAN",
        "LTU",     "EQU",     "LEU",     "GTU", "NEU", "GEU", "T",      "OFF",    "LO",
        "SFF",     "LS",      "HI",      "SFT", "HS",  "OFT", "CSM_TA", "CSM_TR", "CSM_MX",
        "FCSM_TA", "FCSM_TR", "FCSM_MX", "RLE", "RGT", NULL};
    return assemble_cc_tests(ctx, instr, token, tests, 0);
}

DEFINE_FLAG(stg_e, ".E", 45)

DEFINE_OPERAND(stg_cache)
{
    static const char *table[] = {"", "CG", "CS", "WT"};
    uint64_t cache = 0;
    if (find_in_table(token, table, ".", &cache)) {
        *token = tokenize(ctx);
    }
    add_bits(instr, cache << 46);
    return NULL;
}

DEFINE_OPERAND(stg_size)
{
    static const char *table[] = {
        "U8", "S8", "U16", "S16", "32", "64", "128",
    };
    uint64_t size = 4; // defaults .32
    if (find_in_table(token, table, ".", &size)) {
        *token = tokenize(ctx);
    }
    add_bits(instr, size << 48);
    return NULL;
}

DEFINE_OPERAND(stg_address)
{
    CHECK(confirm_type(token, TOKEN_TYPE_OPERATOR_BRACKET_LEFT));

    *token = tokenize(ctx);
    uint8_t regster = ZERO_REGISTER;
    if (token->type == TOKEN_TYPE_REGISTER) {
        regster = token->data.regster;
        *token = tokenize(ctx);

        try_reuse(ctx, token, instr, 8);
    }
    add_bits(instr, (uint64_t)regster << 8);

    if (token->type == TOKEN_TYPE_IMMEDIATE) {
        const int is_zero_reg = regster == ZERO_REGISTER;
        const int64_t min = is_zero_reg ? 0 : -(1 << 23);
        const int64_t max = MAX_BITS(is_zero_reg ? 24 : 23);

        uint64_t value;
        CHECK(convert_integer(token, min, max, &value));
        add_bits(instr, (value & MAX_BITS(24)) << 20);
        *token = tokenize(ctx);
    }

    CHECK(confirm_type(token, TOKEN_TYPE_OPERATOR_BRACKET_RIGHT));
    *token = tokenize(ctx);
    return NULL;
}
