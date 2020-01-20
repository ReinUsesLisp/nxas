#include "operand.h"
#include "helper.h"
#include "token.h"

#define DEFINE_FLAG(name, string, address)                                                         \
    DEFINE_OPERAND(name) { return assemble_flag(ctx, op, token, string, address); }
#define DEFINE_UINT(name, max_size, address)                                                       \
    DEFINE_OPERAND(name) { return assemble_uint(ctx, op, token, max_size, address); }

#define DEFINE_DOT_TABLE(name, default_value, address, ...)                                        \
    DEFINE_OPERAND(name)                                                                           \
    {                                                                                              \
        static const char* table[] = {__VA_ARGS__, nullptr};                                       \
        const std::optional result = find_in_table(token, table, ".");                             \
        if (result) {                                                                              \
            token = ctx.tokenize();                                                                \
        }                                                                                          \
        op.add_bits(result.value_or(default_value) << address);                                    \
        return {};                                                                                 \
    }

static error assemble_cc_tests(context& ctx, opcode& op, token& token, const char* const tests[],
                               int address)
{
    std::optional<std::uint64_t> test_index;
    if (equal(token, "CC")) {
        token = ctx.tokenize();
        CHECK(confirm_type(token, token_type::identifier));

        test_index = find_in_table(token, tests, ".");
        if (!test_index) {
            return fail(token, "unexpected test %.*s", std::size(token.data.string),
                        std::data(token.data.string));
        }
        token = ctx.tokenize();
    }
    op.add_bits(test_index.value_or(15) << address);
    return {};
}

static error assemble_flag(context& ctx, opcode& op, token& token, const char* flag, int address)
{
    if (equal(token, flag)) {
        op.add_bits(1ULL << address);
        token = ctx.tokenize();
    }
    return {};
}

static error assemble_uint(context& ctx, opcode& op, token& token, std::int64_t max_size,
                           int address)
{
    std::uint64_t mask;
    CHECK(convert_integer(token, 0, max_size, &mask));
    op.add_bits(mask << address);
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(comma)
{
    CHECK(confirm_type(token, token_type::comma));
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(dgpr0) { return assemble_dest_gpr(ctx, token, op, 0); }
DEFINE_OPERAND(sgpr0) { return assemble_source_gpr(ctx, token, op, 0); }
DEFINE_OPERAND(sgpr8) { return assemble_source_gpr(ctx, token, op, 8); }
DEFINE_OPERAND(sgpr20) { return assemble_source_gpr(ctx, token, op, 20); }
DEFINE_OPERAND(sgpr39) { return assemble_source_gpr(ctx, token, op, 39); }

DEFINE_OPERAND(cbuf) { return assemble_constant_buffer(ctx, token, op); }
DEFINE_OPERAND(imm) { return assemble_signed_20bit_immediate(ctx, token, op); }

DEFINE_UINT(imm32, UINT32_MAX, 20)
DEFINE_UINT(imm16, UINT16_MAX, 20)

DEFINE_FLAG(cc, ".CC", 47)

DEFINE_UINT(mask4_12, 0xf, 12)
DEFINE_UINT(mask4_39, 0xf, 39)

DEFINE_FLAG(nop_trig, ".TRIG", 13)

DEFINE_OPERAND(nop_tests)
{
    static const char* tests[] = {
        "F",       "LT",      "EQ",      "LE",  "GT",  "NE",   "GE",     "NUM",    "NAN",
        "LTU",     "EQU",     "LEU",     "GTU", "NEU", "GEU",  "T",      "OFF",    "LO",
        "SFF",     "LS",      "HI",      "SFT", "HS",  "OFT",  "CSM_TA", "CSM_TR", "CSM_MX",
        "FCSM_TA", "FCSM_TR", "FCSM_MX", "RLE", "RGT", nullptr};
    return assemble_cc_tests(ctx, op, token, tests, 8);
}

DEFINE_UINT(nop_mask, UINT16_MAX, 20)

DEFINE_OPERAND(xmad_signs)
{
    const bool is_s16 = equal(token, ".S16");
    if (!is_s16 && !equal(token, ".U16")) {
        return {};
    }

    token = ctx.tokenize();

    if (is_s16) {
        op.add_bits(1ULL << 48);
    }

    if (equal(token, ".S16")) {
        op.add_bits(1ULL << 49);
    } else if (!equal(token, ".U16")) {
        return fail(token, "expected S16 or U16");
    }

    token = ctx.tokenize();
    return {};
}

DEFINE_FLAG(xmad_psl36, ".PSL", 36)
DEFINE_FLAG(xmad_psl55, ".PSL", 55)
DEFINE_DOT_TABLE(xmad_mode50, 0, 50, "", "CLO", "CHI", "CSFU", "CBCC")
DEFINE_DOT_TABLE(xmad_mode50_c, 0, 50, "", "CLO", "CHI", "CSFU");
DEFINE_FLAG(xmad_mrg37, ".MRG", 37)
DEFINE_FLAG(xmad_mrg56, ".MRG", 56)
DEFINE_FLAG(xmad_x38, ".X", 38)
DEFINE_FLAG(xmad_x54, ".X", 54)
DEFINE_DOT_TABLE(xmad_h1_53, 0, 53, "H0", "H1")
DEFINE_DOT_TABLE(xmad_h1_52, 0, 52, "H0", "H1")
DEFINE_DOT_TABLE(xmad_h1_35, 0, 35, "H0", "H1")

DEFINE_DOT_TABLE(shr_format, 1, 48, "U32", "S32")
DEFINE_DOT_TABLE(shr_mode, 0, 39, "C", "W")
DEFINE_DOT_TABLE(shr_xmode, 0, 43, "", "INVALIDSHRXMODE1", "X", "XHI")
DEFINE_FLAG(shr_brev, ".BREV", 40)

DEFINE_FLAG(exit_keeprefcount, ".KEEPREFCOUNT", 5)

DEFINE_OPERAND(exit_tests)
{
    static const char* tests[] = {
        "F",       "LT",      "EQ",      "LE",  "GT",  "NE",   "GE",     "NUM",    "NAN",
        "LTU",     "EQU",     "LEU",     "GTU", "NEU", "GEU",  "T",      "OFF",    "LO",
        "SFF",     "LS",      "HI",      "SFT", "HS",  "OFT",  "CSM_TA", "CSM_TR", "CSM_MX",
        "FCSM_TA", "FCSM_TR", "FCSM_MX", "RLE", "RGT", nullptr};
    return assemble_cc_tests(ctx, op, token, tests, 0);
}

DEFINE_FLAG(stg_e, ".E", 45)

DEFINE_OPERAND(stg_cache)
{
    static const char* table[] = {"", "CG", "CS", "WT"};
    std::optional<std::uint64_t> cache = find_in_table(token, table, ".");
    if (cache) {
        token = ctx.tokenize();
    }
    op.add_bits(cache.value_or(0) << 46);
    return {};
}

DEFINE_OPERAND(stg_size)
{
    static const char* table[] = {
        "U8", "S8", "U16", "S16", "32", "64", "128",
    };
    std::optional<std::uint64_t> size = find_in_table(token, table, ".");
    if (size) {
        token = ctx.tokenize();
    }
    op.add_bits(size.value_or(4) << 48); // defaults ".32"
    return {};
}

DEFINE_OPERAND(stg_address)
{
    CHECK(confirm_type(token, token_type::bracket_left));

    token = ctx.tokenize();
    std::uint8_t regster = ZERO_REGISTER;
    if (token.type == token_type::regster) {
        regster = token.data.regster;
        token = ctx.tokenize();

        try_reuse(ctx, token, op, 8);
    }
    op.add_bits(static_cast<std::uint64_t>(regster) << 8);

    if (token.type == token_type::immediate) {
        const int is_zero_reg = regster == ZERO_REGISTER;
        const std::int64_t min = is_zero_reg ? 0 : -(1 << 23);
        const std::int64_t max = MAX_BITS(is_zero_reg ? 24 : 23);

        std::uint64_t value;
        CHECK(convert_integer(token, min, max, &value));
        op.add_bits((value & MAX_BITS(24)) << 20);
        token = ctx.tokenize();
    }

    CHECK(confirm_type(token, token_type::bracket_right));
    token = ctx.tokenize();
    return {};
}
