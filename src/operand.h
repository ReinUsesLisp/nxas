#pragma once

#include <climits>
#include <cstdint>
#include <optional>

#include "context.h"
#include "error.h"
#include "helper.h"
#include "opcode.h"
#include "token.h"

#define DEFINE_OPERAND(name) inline error name(context& ctx, token& token, opcode& op)

#define DEFINE_FLAG(name, string, address)                                                         \
    DEFINE_OPERAND(name)                                                                           \
    {                                                                                              \
        return assemble_flag(ctx, op, token, string, address);                                     \
    }

#define DEFINE_UINT(name, max_size, address)                                                       \
    DEFINE_OPERAND(name)                                                                           \
    {                                                                                              \
        return assemble_uint(ctx, op, token, max_size, address);                                   \
    }

#define DEFINE_INT(name, bits, address)                                                            \
    DEFINE_OPERAND(name)                                                                           \
    {                                                                                              \
        return assemble_int(ctx, op, token, bits, address);                                        \
    }

#define DEFINE_DOT_TABLE(name, default_value, address, ...)                                        \
    DEFINE_OPERAND(name)                                                                           \
    {                                                                                              \
        static const char* table[] = {__VA_ARGS__, nullptr};                                       \
        const std::optional<uint64_t> result = find_in_table(token, table, ".");                   \
        if constexpr (default_value < 0) {                                                         \
            if (!result) {                                                                         \
                return fail(token, "invalid identifier");                                          \
            }                                                                                      \
        }                                                                                          \
        if (result) {                                                                              \
            token = ctx.tokenize();                                                                \
        }                                                                                          \
        op.add_bits(result.value_or(default_value) << address);                                    \
        return {};                                                                                 \
    }

typedef error (*operand)(context&, token&, opcode&);

inline error assemble_cc_tests(context& ctx, opcode& op, token& token, const char* const tests[],
                               int address)
{
    std::optional<uint64_t> test_index;
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

inline error assemble_flag(context& ctx, opcode& op, token& token, const char* flag, int address)
{
    if (equal(token, flag)) {
        op.add_bits(1ULL << address);
        token = ctx.tokenize();
    }
    return {};
}

inline error assemble_uint(context& ctx, opcode& op, token& token, int64_t max_size, int address)
{
    uint64_t value;
    CHECK(convert_integer(token, 0, max_size, &value));
    op.add_bits(value << address);
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(uimm_extended)
{
    uint64_t value;
    CHECK(convert_integer(token, 0, 1ULL << 20, &value));
    if (value >= 0x80000) {
        value -= 0x80000;
        op.add_bits(1ULL << 56);
    }
    op.add_bits(value << 20);
    token = ctx.tokenize();
    return {};
}

inline error assemble_int(context& ctx, opcode& op, token& token, int bits, int address)
{
    const int raw_bits = bits - 1;
    const int64_t min = -(1LL << raw_bits);
    const int64_t max = (1LL << raw_bits) - 1;
    uint64_t value;
    CHECK(convert_integer(token, min, max, &value));
    const uint64_t raw = value & ((1ULL << bits) - 1);
    op.add_bits(raw << address);
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(comma)
{
    CHECK(confirm_type(token, token_type::comma));
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(label)
{
    int64_t absolute;

    switch (token.type) {
    case token_type::identifier: {
        const std::optional label = ctx.find_label(token.data.string);
        if (!label) {
            return fail(token, "label \33[1m%.*s\33[0m not defined", std::size(token.data.string),
                        std::data(token.data.string));
        }
        absolute = *label;
        break;
    }
    case token_type::immediate:
        absolute = token.data.immediate;
        break;
    default:
        return fail(token, "expected label");
    }

    static constexpr int64_t max = max_bits(23);
    static constexpr int64_t min = -static_cast<int64_t>(max_bits(23)) - 1;
    const int64_t value = absolute - ctx.pc - 8;
    if (value > max || value < min) {
        return fail(token, "label out of range");
    }
    op.add_bits((static_cast<uint64_t>(value) & 0x7FFFFF) << 20);
    op.add_bits((value < 0 ? 1ULL : 0ULL) << 43);

    token = ctx.tokenize();
    return {};
}

template <int address>
DEFINE_OPERAND(dgpr)
{
    return assemble_dest_gpr(ctx, token, op, address);
}

template <int address>
DEFINE_OPERAND(sgpr)
{
    return assemble_source_gpr(ctx, token, op, address);
}

template <int address>
DEFINE_OPERAND(mirror_dgpr)
{
    // Hack mirror detecting by restoring the bits and then comparing
    const uint64_t old_op_value = op.value;
    op.value &= ~(uint64_t{0xff} << address);
    if (auto err = dgpr<address>(ctx, token, op)) {
        return err;
    }
    if (op.value != old_op_value) {
        return fail(token, "source register is not mirrored to destination register");
    }
    return {};
}

DEFINE_OPERAND(cbuf)
{
    return assemble_constant_buffer(ctx, token, op);
}

DEFINE_OPERAND(imm)
{
    return assemble_signed_20bit_immediate(ctx, token, op);
}

DEFINE_OPERAND(fimm)
{
    return assemble_float_immediate(ctx, token, op, 20);
}

DEFINE_OPERAND(fimm32)
{
    return assemble_float_immediate(ctx, token, op, 32);
}

DEFINE_OPERAND(dimm20)
{
    return assemble_double_immediate(ctx, token, op);
}

template <int neg_bit>
DEFINE_OPERAND(fimm9_low)
{
    return assemble_half_float_immediate(ctx, token, op, 9, 20, neg_bit);
}

template <int neg_bit>
DEFINE_OPERAND(fimm9_high)
{
    return assemble_half_float_immediate(ctx, token, op, 9, 30, neg_bit);
}

DEFINE_OPERAND(fimm16_low)
{
    return assemble_half_float_immediate(ctx, token, op, 16, 20, -1);
}

DEFINE_OPERAND(fimm16_high)
{
    return assemble_half_float_immediate(ctx, token, op, 16, 36, -1);
}

DEFINE_UINT(uimm32, UINT32_MAX, 20);

DEFINE_INT(simm32, 32, 20);

DEFINE_UINT(uimm16, UINT16_MAX, 20);

template <int address = 47>
DEFINE_FLAG(cc, ".CC", address);

template <int bits, int address>
DEFINE_UINT(uinteger, max_bits(bits), address);

template <int bits, int address>
DEFINE_INT(sinteger, bits, address);

template <int address>
DEFINE_UINT(mask4, 0xf, address);

template <int address>
DEFINE_FLAG(psl, ".PSL", address);

template <int address>
DEFINE_FLAG(x, ".X", address);

template <int address>
DEFINE_FLAG(mrg, ".MRG", address);

template <int address>
DEFINE_DOT_TABLE(half, 0, address, "H0", "H1");

template <int address>
DEFINE_FLAG(brev, ".BREV", address);

template <int address>
DEFINE_FLAG(ftz, ".FTZ", address);

template <int address>
DEFINE_FLAG(sat, ".SAT", address);

template <int address>
DEFINE_FLAG(bf, ".BF", address);

template <int address>
DEFINE_FLAG(h_and, ".H_AND", address);

template <int address>
DEFINE_FLAG(u, ".U", address);

template <int address>
DEFINE_FLAG(lmt, ".LMT", address);

template <int address>
DEFINE_FLAG(w, ".W", address);

template <int address>
DEFINE_DOT_TABLE(int_sign, 1, address, "S32", "U32");

template <int address>
DEFINE_DOT_TABLE(float_format, 2, address, "", "F16", "F32", "F64");

template <int address>
DEFINE_DOT_TABLE(byte_selector, 0, address, "B0", "B1", "B2", "B3");

template <int address>
DEFINE_DOT_TABLE(fp_rounding, 0, address, "RN", "RM", "RP", "RZ");

template <int address>
DEFINE_OPERAND(bop)
{
    static const char* table[] = {"AND", "OR", "XOR", "INVALIDBOP3", nullptr};
    const std::optional<int64_t> value = find_in_table(token, table, ".");
    if (!value) {
        return fail(token, "expected .AND, .OR or .XOR");
    }
    op.add_bits(*value << address);
    token = ctx.tokenize();
    return {};
}

template <int address, bool negable = false>
DEFINE_OPERAND(pred)
{
    CHECK(confirm_type(token, token_type::predicate));
    op.add_bits(static_cast<uint64_t>(token.data.predicate.index) << address);
    if constexpr (negable) {
        op.add_bits(static_cast<uint64_t>(token.data.predicate.negated) << (address + 3));
    } else {
        if (token.data.predicate.negated) {
            return fail(token, "predicate can't be negated");
        }
    }
    token = ctx.tokenize();
    return {};
}

template <int address>
DEFINE_OPERAND(inverted_pred)
{
    CHECK(confirm_type(token, token_type::predicate));
    op.add_bits(static_cast<uint64_t>(7 - token.data.predicate.index) << address);
    if (token.data.predicate.negated) {
        return fail(token, "predicate can't be negated");
    }
    token = ctx.tokenize();
    return {};
}

template <int address>
DEFINE_OPERAND(neg)
{
    if (token.type == token_type::minus) {
        op.add_bits(1ULL << address);
        token = ctx.tokenize();
    }
    return {};
}

template <int address>
DEFINE_OPERAND(tilde)
{
    if (token.type == token_type::tilde) {
        op.add_bits(1ULL << address);
        token = ctx.tokenize();
    }
    return {};
}

template <int address>
DEFINE_FLAG(post_neg, ".NEG", address);

template <int address>
DEFINE_FLAG(post_abs, ".ABS", address);

template <int address, operand inner>
DEFINE_OPERAND(abs)
{
    const bool is_absolute = token.type == token_type::vbar;
    if (is_absolute) {
        op.add_bits(1ULL << address);
        token = ctx.tokenize();
    }

    CHECK(inner(ctx, token, op));

    if (is_absolute) {
        CHECK(confirm_type(token, token_type::vbar));
        token = ctx.tokenize();
    }
    return {};
}

template <int address1, int address2>
DEFINE_OPERAND(po)
{
    if (!equal(token, ".PO")) {
        return {};
    }
    op.add_bits((1ULL << address1) | (1ULL << address2));
    token = ctx.tokenize();
    return {};
}

template <int address>
DEFINE_FLAG(sh, ".SH", address);

template <int address>
DEFINE_FLAG(inv, ".INV", address);

template <int address>
DEFINE_OPERAND(default_rz)
{
    op.add_bits(0xFFULL << address);
    return {};
}

namespace memory
{
    DEFINE_DOT_TABLE(size, 4, 48, "U8", "S8", "U16", "S16", "32", "64", "128");

    template <bool imm_offset = true, int addr = 20, int size = 24, int shr = 0>
    DEFINE_OPERAND(address)
    {
        CHECK(confirm_type(token, token_type::bracket_left));

        token = ctx.tokenize();
        uint8_t regster = ZERO_REGISTER;
        if (token.type == token_type::regster) {
            regster = token.data.regster;
            token = ctx.tokenize();

            try_reuse(ctx, token, op, 8);
        }
        op.add_bits(static_cast<uint64_t>(regster) << 8);

        if constexpr (imm_offset) {
            if (token.type == token_type::immediate) {
                const int is_zero_reg = regster == ZERO_REGISTER;
                const int64_t min = is_zero_reg ? 0 : -(1 << (size - 1));
                const int64_t max = max_bits(is_zero_reg ? size : (size - 1));

                uint64_t value;
                CHECK(convert_integer(token, min, max, &value));
                value >>= shr;
                op.add_bits((value & max_bits(size)) << addr);
                token = ctx.tokenize();
            }
        }

        CHECK(confirm_type(token, token_type::bracket_right));
        token = ctx.tokenize();
        return {};
    }
}

DEFINE_OPERAND(flow_tests)
{
    static const char* tests[] = {
        "F",       "LT",      "EQ",      "LE",  "GT",  "NE",   "GE",     "NUM",    "NAN",
        "LTU",     "EQU",     "LEU",     "GTU", "NEU", "GEU",  "T",      "OFF",    "LO",
        "SFF",     "LS",      "HI",      "SFT", "HS",  "OFT",  "CSM_TA", "CSM_TR", "CSM_MX",
        "FCSM_TA", "FCSM_TR", "FCSM_MX", "RLE", "RGT", nullptr};
    return assemble_cc_tests(ctx, op, token, tests, 0);
}

DEFINE_FLAG(keeprefcount, ".KEEPREFCOUNT", 5);

template <int address>
DEFINE_OPERAND(store_cache)
{
    static const char* table[] = {"", "CG", "CS", "WT", nullptr};
    std::optional<uint64_t> cache = find_in_table(token, table, ".");
    if (cache) {
        token = ctx.tokenize();
    }
    op.add_bits(cache.value_or(0) << address);
    return {};
}

namespace rro
{
    DEFINE_DOT_TABLE(mode, -1, 39, "SINCOS", "EX2");
}

template <int address>
DEFINE_DOT_TABLE(load_cache, 0, address, "", "CG", "CI", "CV");

namespace lop
{
    template <int address>
    DEFINE_DOT_TABLE(bit_op, -1, address, "AND", "OR", "XOR", "PASS_B");

    DEFINE_DOT_TABLE(pred_op, 0, 44, "", "T", "Z", "NZ");
}

namespace amem
{
    DEFINE_DOT_TABLE(size, 0, 47, "32", "64", "96", "128");
}

namespace ald
{
    DEFINE_FLAG(o, ".O", 32);

    DEFINE_OPERAND(p)
    {
        if (!equal(token, ".P")) {
            return fail(token, "expected .P");
        }
        op.add_bits(1ULL << 31);
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(phys)
    {
        if (!equal(token, ".PHYS")) {
            return fail(token, "expected .PHYS");
        }
        token = ctx.tokenize();
        return {};
    }

    DEFINE_DOT_TABLE(size, 0, 47, "32", "64", "96", "128");

    DEFINE_OPERAND(imm_attr)
    {
        if (!equal(token, "a")) {
            return fail(token, "expected 'a'");
        }
        token = ctx.tokenize();

        CHECK(confirm_type(token, token_type::bracket_left));
        token = ctx.tokenize();

        CHECK((uinteger<10, 20>(ctx, token, op)));
        op.add_bits(0xFFULL << 8);

        CHECK(confirm_type(token, token_type::bracket_right));
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(patch_attr)
    {
        if (!equal(token, "a")) {
            return fail(token, "expected 'a'");
        }
        token = ctx.tokenize();

        CHECK(confirm_type(token, token_type::bracket_left));
        token = ctx.tokenize();

        CHECK((sgpr<8>(ctx, token, op)));

        if (token.type == token_type::plus) {
            token = ctx.tokenize();
        }
        CHECK((sinteger<11, 20>(ctx, token, op)));

        CHECK(confirm_type(token, token_type::bracket_right));
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(phys_attr)
    {
        if (!equal(token, "a")) {
            return fail(token, "expected 'a'");
        }
        token = ctx.tokenize();

        CHECK(confirm_type(token, token_type::bracket_left));
        token = ctx.tokenize();

        CHECK((sgpr<8>(ctx, token, op)));

        CHECK(confirm_type(token, token_type::bracket_right));
        token = ctx.tokenize();
        return {};
    }
}

namespace al2p
{
    DEFINE_FLAG(o, ".O", 32);
}

namespace b2r
{
    DEFINE_OPERAND(warp)
    {
        if (!equal(token, ".WARP")) {
            return fail(token, "expected .WARP");
        }
        token = ctx.tokenize();
        op.add_bits(0xEULL << 32);
        op.add_bits(0xFFULL << 8);
        return {};
    }

    DEFINE_OPERAND(result)
    {
        if (!equal(token, ".RESULT")) {
            return fail(token, "expected .RESULT");
        }
        token = ctx.tokenize();
        op.add_bits(0xDULL << 32);
        op.add_bits(0xFFULL << 8);
        return {};
    }
}

namespace nop
{
    DEFINE_FLAG(trig, ".TRIG", 13);
    DEFINE_UINT(mask, UINT16_MAX, 20);
}

DEFINE_OPERAND(cc_text)
{
    if (!equal(token, "CC")) {
        return fail(token, "expected CC");
    }
    token = ctx.tokenize();
    return {};
}

DEFINE_DOT_TABLE(cc_tests, 15, 8, "F", "LT", "EQ", "LE", "GT", "NE", "GE", "NUM", "NAN", "LTU",
                 "EQU", "LEU", "GTU", "NEU", "GEU", "T", "OFF", "LO", "SFF", "LS", "HI", "SFT",
                 "HS", "OFT", "CSM_TA", "CSM_TR", "CSM_MX", "FCSM_TA", "FCSM_TR", "FCSM_MX", "RLE",
                 "RGT");

namespace xmad
{
    DEFINE_OPERAND(signs)
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
            return fail(token, "expected .S16 or .U16");
        }

        token = ctx.tokenize();
        return {};
    }

    DEFINE_DOT_TABLE(mode_a, 0, 50, "", "CLO", "CHI", "CSFU", "CBCC");
    DEFINE_DOT_TABLE(mode_b, 0, 50, "", "CLO", "CHI", "CSFU");
}

namespace lop3
{
    DEFINE_DOT_TABLE(operation, 0, 36, "", "T", "Z", "NZ")

    DEFINE_OPERAND(lut)
    {
        if (!equal(token, ".LUT")) {
            return fail(token, "expected .LUT");
        }
        token = ctx.tokenize();
        return {};
    }
}

namespace shr
{
    DEFINE_DOT_TABLE(format, 1, 48, "U32", "S32");
    DEFINE_DOT_TABLE(mode, 0, 39, "C", "W");
    DEFINE_DOT_TABLE(xmode, 0, 43, "", "INVALIDSHRXMODE1", "X", "XHI");
}

namespace bfe
{
    DEFINE_DOT_TABLE(format, 1, 48, "U32", "S32");
}

namespace bpt
{
    DEFINE_DOT_TABLE(mode, -1, 6, "DRAIN_ILLEGAL", "CAL", "PAUSE", "TRAP", "INT", "DRAIN");
}

namespace stg
{
    DEFINE_FLAG(e, ".E", 45)

    DEFINE_OPERAND(size)
    {
        static const char* table[] = {
            "U8", "S8", "U16", "S16", "32", "64", "128",
        };
        std::optional<uint64_t> size = find_in_table(token, table, ".");
        if (size) {
            token = ctx.tokenize();
        }
        op.add_bits(size.value_or(4) << 48); // defaults ".32"
        return {};
    }
}

namespace f2f
{
    DEFINE_OPERAND(rounding)
    {
        const int dst_format = (op.value >> 8) & 0b11;
        const int src_format = (op.value >> 10) & 0b11;
        if (dst_format == src_format) {
            std::optional<uint64_t> result;
            if (equal(token, ".ROUND")) {
                result = 0b1000;
            } else if (equal(token, ".FLOOR")) {
                result = 0b1001;
            } else if (equal(token, ".CEIL")) {
                result = 0b1010;
            } else if (equal(token, ".TRUNC")) {
                result = 0b1011;
            } else if (equal(token, ".PASS")) {
                result = 0b0011;
            }
            if (result) {
                token = ctx.tokenize();
            }
            op.add_bits(result.value_or(0) << 39);
            return {};
        }

        static const char* table[] = {"RN", "RM", "RP", "RZ", nullptr};
        const std::optional result = find_in_table(token, table, ".");
        if (result) {
            token = ctx.tokenize();
        }
        op.add_bits(result.value_or(0) << 39);
        return {};
    }
}

namespace fmul
{
    template <int address>
    DEFINE_DOT_TABLE(fmz, 0, address, "", "FTZ", "FMZ", "INVALIDFMZ3");

    DEFINE_DOT_TABLE(scale, 0, 41, "", "D2", "D4", "D8", "M8", "M4", "M2", "INVALIDSCALE37");
}

namespace iadd3
{
    DEFINE_DOT_TABLE(shift, 0, 37, "", "RS", "LS");

    template <int address>
    DEFINE_DOT_TABLE(half, 0, address, "", "H0", "H1");
}

namespace f2i
{
    DEFINE_OPERAND(int_format)
    {
        static const char* table_unsigned[] = {"INVALID0", "U16", "U32", "U64", nullptr};
        static const char* table_signed[] = {"INVALID1", "S16", "S32", "S64", nullptr};

        std::optional result = find_in_table(token, table_unsigned, ".");
        if (!result) {
            result = find_in_table(token, table_signed, ".");
            if (!result) {
                return fail(token, "expected .U16, .U32, .U64, .S16, .S32 or .S64");
            }
            op.add_bits(1ULL << 12);
        }
        op.add_bits(*result << 8);
        token = ctx.tokenize();
        return {};
    }

    DEFINE_DOT_TABLE(rounding, 0, 39, "", "FLOOR", "CEIL", "TRUNC");
}

namespace p2r
{
    template <int address>
    DEFINE_OPERAND(mode)
    {
        static const char* table[] = {"PR", "CC", nullptr};
        const std::optional<uint64_t> result = find_in_table(token, table, "");
        if (!result) {
            return fail(token, "expected PR or CC");
        }
        op.add_bits(*result << address);
        token = ctx.tokenize();
        return {};
    }
}

namespace fp16
{
    template <int address>
    DEFINE_DOT_TABLE(merge, 0, address, "", "F32", "MRG_H0", "MRG_H1");

    template <int address>
    DEFINE_DOT_TABLE(control, 0, address, "", "FTZ", "FMZ");

    template <int address>
    DEFINE_DOT_TABLE(swizzle, 0, address, "H1_H0", "F32", "H0_H0", "H1_H1");
}

namespace stl
{
    DEFINE_DOT_TABLE(cache, 0, 44, "", "CG", "CS", "WT");
}

namespace lds
{
    DEFINE_FLAG(u, ".U", 44);
}

template <int address>
DEFINE_DOT_TABLE(float_compare, -1, address, "F", "LT", "EQ", "LE", "GT", "NE", "GE", "NUM", "NAN",
                 "LTU", "EQU", "LEU", "GTU", "NEU", "GEU", "T");

template <int address>
DEFINE_DOT_TABLE(integer_compare, -1, address, "F", "LT", "EQ", "LE", "GT", "NE", "GE", "T");

namespace vsetp
{
    template <int address>
    DEFINE_DOT_TABLE(integer_compare, -1, address, "F", "LT", "EQ", "LE", "", "", "", "", "", "",
                     "", "", "", "", "", "", "GT", "NE", "GE", "T");
}

namespace mufu
{
    DEFINE_DOT_TABLE(operation, -1, 20, "COS", "SIN", "EX2", "LG2", "RCP", "RSQ", "RCP64H",
                     "RSQ64H", "SQRT");
}

namespace imnmx
{
    DEFINE_DOT_TABLE(mode, 0, 43, "", "XLO", "XMED", "XHI");
}

namespace ldc
{
    DEFINE_DOT_TABLE(mode, 0, 44, "", "IL", "IS", "ISL");
    DEFINE_DOT_TABLE(size, -1, 48, "U8", "S8", "U16", "S16", "32", "64");

    DEFINE_OPERAND(address)
    {
        if (!equal(token, "c")) {
            return fail(token, "expected constant buffer");
        }
        token = ctx.tokenize();
        CHECK(confirm_type(token, token_type::bracket_left));

        token = ctx.tokenize();
        uint64_t value;
        CHECK(convert_integer(token, 0, max_bits(5), &value));
        op.add_bits(static_cast<uint64_t>(value) << 36);

        token = ctx.tokenize();
        CHECK(confirm_type(token, token_type::bracket_right));

        token = ctx.tokenize();
        return memory::address<true, 20, 16, 0>(ctx, token, op);
    }
}

namespace image
{
    DEFINE_OPERAND(p)
    {
        if (!equal(token, ".P")) {
            return fail(token, "expected .P or .B");
        }
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(d)
    {
        if (!equal(token, ".D")) {
            return fail(token, "expected .P or .B");
        }
        op.add_bits(1ULL << 52);
        token = ctx.tokenize();
        return {};
    }

    template <int address>
    DEFINE_FLAG(ba, ".BA", address);

    DEFINE_DOT_TABLE(type, -1, 33, "1D", "1D_BUFFER", "1D_ARRAY", "2D", "2D_ARRAY", "3D");

    DEFINE_DOT_TABLE(rgba, 15, 20, "", "R", "G", "RG", "B", "RB", "GB", "RGB", "A", "RA", "GA",
                     "RGA", "BA", "RBA", "GBA", "RGBA");

    DEFINE_DOT_TABLE(size, 4, 20, "U8", "S8", "U16", "S16", "32", "64", "128");

    DEFINE_DOT_TABLE(clamp, 1, 49, "IGN", "", "TRAP");
}

DEFINE_OPERAND(s2r)
{
    static const char* table[] = {
        "SR_LANEID",
        "SR_CLOCK",
        "SR_VIRTCFG",
        "SR_VIRTID",
        "SR_PM0",
        "SR_PM1",
        "SR_PM2",
        "SR_PM3",
        "SR_PM4",
        "SR_PM5",
        "SR_PM6",
        "SR_PM7",
        "SR12",
        "SR13",
        "SR14",
        "SR_ORDERING_TICKET",
        "SR_PRIM_TYPE",
        "SR_INVOCATION_ID",
        "SR_Y_DIRECTION",
        "SR_THREAD_KILL",
        "SM_SHADER_TYPE",
        "SR_DIRECTCBEWRITEADDRESSLOW",
        "SR_DIRECTCBEWRITEADDRESSHIGH",
        "SR_DIRECTCBEWRITEENABLED",
        "SR_MACHINE_ID_0",
        "SR_MACHINE_ID_1",
        "SR_MACHINE_ID_2",
        "SR_MACHINE_ID_3",
        "SR_AFFINITY",
        "SR_INVOCATION_INFO",
        "SR_WSCALEFACTOR_XY",
        "SR_WSCALEFACTOR_Z",
        "", // SR_TID
        "", // SR_TID.X
        "", // SR_TID.Y
        "", // SR_TID.Z
        "SR_CTA_PARAM",
        "", // SR_CTAID.X
        "", // SR_CTAID.Y
        "", // SR_CTAID.Z
        "SR_NTID",
        "SR_CirQueueIncrMinusOne",
        "SR_NLATC",
        "SR43",
        "SR_SM_SPA_VERSION",
        "SR_MULTIPASSSHADERINFO",
        "SR_LWINHI",
        "SR_SWINHI",
        "SR_SWINLO",
        "SR_SWINSZ",
        "SR_SMEMSZ",
        "SR_SMEMBANKS",
        "SR_LWINLO",
        "SR_LWINSZ",
        "SR_LMEMLOSZ",
        "SR_LMEMHIOFF",
        "SR_EQMASK",
        "SR_LTMASK",
        "SR_LEMASK",
        "SR_GTMASK",
        "SR_GEMASK",
        "SR_REGALLOC",
        "SR_BARRIERALLOC",
        "SR63",
        "SR_GLOBALERRORSTATUS",
        "SR65",
        "SR_WARPERRORSTATUS",
        "SR_WARPERRORSTATUSCLEAR",
        "SR68",
        "SR69",
        "SR70",
        "SR71",
        "SR_PM_HI0",
        "SR_PM_HI1",
        "SR_PM_HI2",
        "SR_PM_HI3",
        "SR_PM_HI4",
        "SR_PM_HI5",
        "SR_PM_HI6",
        "SR_PM_HI7",
        "SR_CLOCKLO",
        "SR_CLOCKHI",
        "SR_GLOBALTIMERLO",
        "SR_GLOBALTIMERHI",
        "SR84",
        "SR85",
        "SR86",
        "SR87",
        "SR88",
        "SR89",
        "SR90",
        "SR91",
        "SR92",
        "SR93",
        "SR94",
        "SR95",
        "SR_HWTASKID",
        "SR_CIRCULARQUEUEENTRYINDEX",
        "SR_CIRCULARQUEUEENTRYADDRESSLOW",
        "SR_CIRCULARQUEUEENTRYADDRESSHIGH",
        "SR100",
        "SR101",
        "SR102",
        "SR103",
        "SR104",
        "SR105",
        "SR106",
        "SR107",
        "SR108",
        "SR109",
        "SR110",
        "SR111",
        "SR112",
        "SR113",
        "SR114",
        "SR115",
        "SR116",
        "SR117",
        "SR118",
        "SR119",
        "SR120",
        "SR121",
        "SR122",
        "SR123",
        "SR124",
        "SR125",
        "SR126",
        "SR127",
        "SR128",
        "SR129",
        "SR130",
        "SR131",
        "SR132",
        "SR133",
        "SR134",
        "SR135",
        "SR136",
        "SR137",
        "SR138",
        "SR139",
        "SR140",
        "SR141",
        "SR142",
        "SR143",
        "SR144",
        "SR145",
        "SR146",
        "SR147",
        "SR148",
        "SR149",
        "SR150",
        "SR151",
        "SR152",
        "SR153",
        "SR154",
        "SR155",
        "SR156",
        "SR157",
        "SR158",
        "SR159",
        "SR160",
        "SR161",
        "SR162",
        "SR163",
        "SR164",
        "SR165",
        "SR166",
        "SR167",
        "SR168",
        "SR169",
        "SR170",
        "SR171",
        "SR172",
        "SR173",
        "SR174",
        "SR175",
        "SR176",
        "SR177",
        "SR178",
        "SR179",
        "SR180",
        "SR181",
        "SR182",
        "SR183",
        "SR184",
        "SR185",
        "SR186",
        "SR187",
        "SR188",
        "SR189",
        "SR190",
        "SR191",
        "SR192",
        "SR193",
        "SR194",
        "SR195",
        "SR196",
        "SR197",
        "SR198",
        "SR199",
        "SR200",
        "SR201",
        "SR202",
        "SR203",
        "SR204",
        "SR205",
        "SR206",
        "SR207",
        "SR208",
        "SR209",
        "SR210",
        "SR211",
        "SR212",
        "SR213",
        "SR214",
        "SR215",
        "SR216",
        "SR217",
        "SR218",
        "SR219",
        "SR220",
        "SR221",
        "SR222",
        "SR223",
        "SR224",
        "SR225",
        "SR226",
        "SR227",
        "SR228",
        "SR229",
        "SR230",
        "SR231",
        "SR232",
        "SR233",
        "SR234",
        "SR235",
        "SR236",
        "SR237",
        "SR238",
        "SR239",
        "SR240",
        "SR241",
        "SR242",
        "SR243",
        "SR244",
        "SR245",
        "SR246",
        "SR247",
        "SR248",
        "SR249",
        "SR250",
        "SR251",
        "SR252",
        "SR253",
        "SR254",
        "SR255",
        nullptr,
    };

    std::optional<uint64_t> value;
    if (equal(token, "SR_TID")) {
        token = ctx.tokenize();
        if (equal(token, ".X")) {
            value = 33;
        } else if (equal(token, ".Y")) {
            value = 34;
        } else if (equal(token, ".Z")) {
            value = 35;
        } else {
            value = 32;
        }
        if (value != 32) {
            token = ctx.tokenize();
        }
    } else if (equal(token, "SR_CTAID")) {
        token = ctx.tokenize();
        if (equal(token, ".X")) {
            value = 37;
        } else if (equal(token, ".Y")) {
            value = 38;
        } else if (equal(token, ".Z")) {
            value = 39;
        } else {
            return fail(token, "expected .X, .Y or .Z after SR_CTAID");
        }
        token = ctx.tokenize();
    } else {
        value = find_in_table(token, table, "");
        if (!value) {
            return fail(token, "invalid system register \33[1m%.*s\33[0m",
                        static_cast<int>(token.data.string.size()), token.data.string.data());
        }
        token = ctx.tokenize();
    }
    op.add_bits(*value << 20);
    return {};
}

namespace shfl
{
    DEFINE_DOT_TABLE(mode, -1, 30, "IDX", "UP", "DOWN", "BFLY");
}

DEFINE_DOT_TABLE(int_sign, 1, 48, "U32", "S32");

namespace ldg
{
    DEFINE_OPERAND(size)
    {
        static const char* table[] = {"U8", "S8", "U16", "S16", "32", "64", "128", "U"};
        static const char* msg = "expected .U8, .S8, .U16, .S16, .32, .64, .128 or .U.128";

        const std::optional result = find_in_table(token, table, ".");
        if (result) {
            token = ctx.tokenize();
            if (*result == 7) { // .U
                if (!equal(token, ".128")) {
                    return fail(token, msg);
                }
                token = ctx.tokenize();
            }
        }
        op.add_bits(result.value_or(4) << 48);
        return {};
    }

    DEFINE_DOT_TABLE(cache, 0, 46, "", "CG", "CI", "CV");
}

namespace i2f
{
    DEFINE_OPERAND(int_format)
    {
        static const char* table_unsigned[] = {"U8", "U16", "U32", "U64", nullptr};
        static const char* table_signed[] = {"S8", "S16", "S32", "S64", nullptr};

        std::optional result = find_in_table(token, table_unsigned, ".");
        if (!result) {
            result = find_in_table(token, table_signed, ".");
            if (!result) {
                return fail(token, "expected .U8, .U16, .U32, .U64, .S8, .S16, .S32 or .S64");
            }
            op.add_bits(1ULL << 13);
        }
        op.add_bits(*result << 10);
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(selector)
    {
        static const char* bytes[] = {"B0", "B1", "B2", "B3", nullptr};
        static const char* shorts[] = {"H0", "INVALIDSIZE1", "H1", "INVALIDSIZE3", nullptr};

        const uint64_t type = (op.value >> 10) & 0b111;
        const char* const* const table = type == 1 ? shorts : bytes;
        const std::optional result = find_in_table(token, table, ".");
        if (result) {
            token = ctx.tokenize();
        }
        op.add_bits(result.value_or(0) << 41);
        return {};
    }
}

namespace i2i
{
    template <int address, int sign_address>
    DEFINE_OPERAND(format)
    {
        static const char* table_unsigned[] = {"U8", "U16", "U32", nullptr};
        static const char* table_signed[] = {"S8", "S16", "S32", nullptr};

        std::optional<int64_t> result = find_in_table(token, table_unsigned, ".");
        if (!result) {
            result = find_in_table(token, table_signed, ".");
            if (!result) {
                return fail(token, "expected .U8, .U16, .U32, .S8, .S16 or .S32");
            }
            op.add_bits(1ULL << sign_address);
        }
        op.add_bits(*result << address);
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(selector)
    {
        static const char* bytes[] = {"B0", "B1", "B2", "B3", nullptr};
        static const char* shorts[] = {"H0", "INVALIDSIZE1", "H1", "INVALIDSIZE3", nullptr};

        const uint64_t type = (op.value >> 10) & 0b111;
        const char* const* const table = type == 1 ? shorts : bytes;
        const std::optional result = find_in_table(token, table, ".");
        if (result) {
            token = ctx.tokenize();
        }
        op.add_bits(result.value_or(0) << 41);
        return {};
    }
}

namespace atoms
{
    DEFINE_DOT_TABLE(operation, -1, 52, "ADD", "MIN", "MAX", "INC", "DEC", "AND", "OR", "XOR",
                     "EXCH");

    DEFINE_DOT_TABLE(size, -1, 28, "U32", "S32", "U64", "S64");
}

namespace ldl
{
    DEFINE_DOT_TABLE(cache, 0, 44, "", "LU", "CI", "CV");
}

template <int address>
DEFINE_OPERAND(atomic_size)
{
    static const char* msg = "expected .U32, .S32, .U64, .S64, .F32.FTZ.RN, .F16x2.RN or .S64";
    static const char* table[] = {"U32", "S32", "U64", "F32", "F16x2", "S64", nullptr};
    const std::optional<uint64_t> result = find_in_table(token, table, ".");
    if (result) {
        token = ctx.tokenize();
        switch (*result) {
        case 3: // .F32.FTZ.RN
            if (!equal(token, ".FTZ")) {
                return fail(token, msg);
            }
            token = ctx.tokenize();
            [[fallthrough]];
        case 4: // .F16x2.RN
            if (!equal(token, ".RN")) {
                return fail(token, msg);
            }
            token = ctx.tokenize();
        }
    }
    op.add_bits(result.value_or(0) << address);
    return {};
}

namespace vote
{
    DEFINE_DOT_TABLE(operation, -1, 48, "ALL", "ANY", "EQ");
}

namespace atom
{
    DEFINE_FLAG(e, ".E", 48);

    DEFINE_DOT_TABLE(operation, -1, 52, "ADD", "MIN", "MAX", "INC", "DEC", "AND", "OR", "XOR",
                     "EXCH", "SAFEADD");
}

namespace lea
{
    DEFINE_OPERAND(hi)
    {
        if (!equal(token, ".HI")) {
            return fail(token, "expected .HI");
        }
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(lo)
    {
        if (equal(token, ".LO")) {
            token = ctx.tokenize();
        }
        return {};
    }
}

namespace shf
{
    DEFINE_OPERAND(r)
    {
        if (!equal(token, ".R")) {
            return fail(token, "expected .R");
        }
        token = ctx.tokenize();
        return {};
    }

    DEFINE_OPERAND(l)
    {
        if (!equal(token, ".L")) {
            return fail(token, "expected .L");
        }
        token = ctx.tokenize();
        return {};
    }

    DEFINE_DOT_TABLE(max_shift, 0, 37, "32", "INVALIDMAXSHIFT3", "U64", "S64");

    DEFINE_DOT_TABLE(xmode, 0, 48, "", "HI", "X", "XHI");
}

DEFINE_DOT_TABLE(noinc, 1, 6, "NOINC", "INC");

namespace video
{
    DEFINE_DOT_TABLE(dest_sign, 1, 54, "UD", "SD");

    template <int address, int sign_address>
    DEFINE_OPERAND(src_format)
    {
        static const char* signed_table[] = {"S8", "", "S16", "S32", nullptr};
        static const char* unsigned_table[] = {"U8", "", "U16", "U32", nullptr};
        bool is_signed = true;
        std::optional<int64_t> result = find_in_table(token, signed_table, ".");
        if (!result) {
            is_signed = false;
            result = find_in_table(token, unsigned_table, ".");
            if (!result) {
                is_signed = true;
            }
        }
        if (result) {
            token = ctx.tokenize();
        }
        op.add_bits(result.value_or(3) << address); // .U32
        op.add_bits((is_signed ? 1ULL : 0ULL) << sign_address);
        return {};
    }

    template <int address>
    DEFINE_DOT_TABLE(mx, 0, address, "MN", "MX");

    template <int address>
    DEFINE_DOT_TABLE(vmnmx_op, 7, address, "MRG_16H", "MRG_16L", "MRG_8B0", "MRG_8B2", "ACC", "MIN",
                     "MAX");

    template <int address, int type_address>
    DEFINE_OPERAND(selector)
    {
        static const char* bytes[] = {"B0", "B1", "B2", "B3", nullptr};
        static const char* shorts[] = {"H0", "H1", nullptr};

        const uint64_t type = (op.value >> type_address) & 0b111;
        const char* const* const table = type == 2 ? shorts : bytes;
        const std::optional result = find_in_table(token, table, ".");
        if (result) {
            token = ctx.tokenize();
        }
        op.add_bits(result.value_or(0) << address);
        return {};
    }

    template <int address, int sign_address>
    DEFINE_OPERAND(imm_format)
    {
        static const char* table[] = {"U16", "S16", nullptr};
        std::optional<int64_t> result = find_in_table(token, table, ".");
        if (result) {
            token = ctx.tokenize();
        }
        op.add_bits(result.value_or(1) << sign_address);
        return {};
    }
}

namespace vmad
{
    DEFINE_DOT_TABLE(scale, 0, 51, "", "SHR_7", "SHR_15", "INVALIDVMADSCALE3");
}

namespace red
{
    DEFINE_FLAG(e, ".E", 48);

    DEFINE_DOT_TABLE(operation, 0, 23, "ADD", "MIN", "MAX", "INC", "DEC", "AND", "OR", "XOR")
}

template <int address>
DEFINE_FLAG(ndv, ".NDV", address);

template <int address>
DEFINE_FLAG(nodep, ".NODEP", address);

template <int address>
DEFINE_FLAG(dc, ".DC", address);

template <int address>
DEFINE_FLAG(aoffi, ".AOFFI", address);

template <int address>
DEFINE_FLAG(lc, ".LC", address);

template <int address>
DEFINE_DOT_TABLE(blod, 0, address, "", "LZ", "LB", "LL", "INVALIDBLOD4", "INVALIDBLOD5", "LBA",
                 "LLA");

template <int address>
DEFINE_OPERAND(tex_type)
{
    static const char* types[]{
        "1D", "ARRAY_1D", "2D", "ARRAY_2D", "3D", "ARRAY_3D", "CUBE", "ARRAY_CUBE",
    };
    const std::optional<uint64_t> result = find_in_table(token, types, "");
    if (!result) {
        return fail(token,
                    "expected 1D, ARRAY_1D, 2D, ARRAY_2D, 3D, ARRAY_3D, CUBE, or ARRAY_CUBE");
    }
    token = ctx.tokenize();
    op.add_bits(*result << address);
    return {};
}

DEFINE_OPERAND(b_text)
{
    if (!equal(token, ".B")) {
        return fail(token, "expected .B");
    }
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(lod_text)
{
    if (!equal(token, ".LOD")) {
        return fail(token, "expected .LOD");
    }
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(zero)
{
    if (token.type != token_type::immediate || token.data.immediate != 0) {
        return fail(token, "expected 0");
    }
    token = ctx.tokenize();
    return {};
}

template <int address>
DEFINE_DOT_TABLE(sample_size, 1, address, "F16", "");

template <int address>
DEFINE_DOT_TABLE(sample_size_inv, 0, address, "", "F16");

enum class Lod
{
    None,
    LL,
    LZ,
};

template <int address>
DEFINE_OPERAND(texs_mode)
{
    Lod lod = Lod::None;
    bool dc = false;

    if (equal(token, ".LZ")) {
        lod = Lod::LZ;
        token = ctx.tokenize();
    } else if (equal(token, ".LL")) {
        lod = Lod::LL;
        token = ctx.tokenize();
    }
    if (equal(token, ".DC")) {
        dc = true;
        token = ctx.tokenize();
    }
    // Write in the opcode as a payload, these values don't match the actual ISA
    op.add_bits(((dc ? 0b100ULL : 0ULL) | static_cast<uint64_t>(lod)) << address);
    return {};
}

template <int address>
DEFINE_OPERAND(tlds_type)
{
    static constexpr const char* table[]{
        "1D", "2D", "3D", "ARRAY_2D", nullptr,
    };
    const bool ll = ((op.value >> address) & 1) != 0;
    const bool aoffi = ((op.value >> (address + 1)) & 1) != 0;
    const bool ms = ((op.value >> (address + 2)) & 1) != 0;
    op.value &= ~(uint64_t(0b111) << address);

    const std::optional<uint64_t> type = find_in_table(token, table, "");
    if (!type) {
        return fail(token, "expected 1D, 2D, ARRAY_2D, or 3D");
    }
    uint64_t mode = 0;
    switch (*type) {
    case 0: // 1D
        if (aoffi) {
            return fail(token, "1D has no AOFFI");
        }
        if (ms) {
            return fail(token, "1D has no MS");
        }
        mode = ll ? 1 : 0;
        break;
    case 1: // 2D
        if (aoffi && ms) {
            return fail(token, "AOFFI and MS can't be used at the same time");
        }
        if (ll) {
            if (ms) {
                return fail(token, "LL can't be used with MS in 2D");
            }
            mode = aoffi ? 12 : 5;
        } else {
            if (aoffi) {
                mode = 4;
            } else if (ms) {
                mode = 6;
            } else {
                mode = 2;
            }
        }
        break;
    case 2: // 3D
        if (aoffi) {
            return fail(token, "AOFFI can't be used in 3D");
        }
        if (ms) {
            return fail(token, "MS can't be used in 3D");
        }
        mode = 7;
        break;
    case 3: // ARRAY_2D
        if (aoffi) {
            return fail(token, "AOFFI can't be used in ARRAY_2D");
        }
        if (ms) {
            return fail(token, "MS can't be used in ARRAY_2D");
        }
        mode = 8;
        break;
    }
    op.add_bits(mode << address);
    token = ctx.tokenize();
    return {};
}

template <int address>
DEFINE_OPERAND(texs_type)
{
    static constexpr const char* table[]{
        "1D", "2D", "", "", "", "", "", "ARRAY_2D", "", "", "3D", "", "CUBE", "", nullptr,
    };
    const std::optional<uint64_t> type = find_in_table(token, table, "");
    if (!type) {
        return fail(token, "expected 1D, 2D, ARRAY_2D, 3D, or CUBE");
    }
    const Lod lod = static_cast<Lod>((op.value >> address) & 0b011);
    const bool dc = ((op.value >> address) & 0b100) != 0;
    op.value &= ~(uint64_t{0b111} << address);

    switch (*type) {
    case 0: // 1D
        if (lod != Lod::LZ || dc) {
            return fail(token, "1D is only valid with LZ and no-DC");
        }
        op.add_bits(0x0000000000000000ULL);
        break;
    case 1: // 2D
        if (dc) {
            switch (lod) {
            case Lod::None:
                op.add_bits(0x0080000000000000ULL);
                break;
            case Lod::LZ:
                op.add_bits(0x00C0000000000000ULL);
                break;
            case Lod::LL:
                op.add_bits(0x00A0000000000000ULL);
                break;
            }
        } else {
            switch (lod) {
            case Lod::None:
                op.add_bits(0x0020000000000000ULL);
                break;
            case Lod::LZ:
                op.add_bits(0x0040000000000000ULL);
                break;
            case Lod::LL:
                op.add_bits(0x0060000000000000ULL);
                break;
            }
        }
        break;
    case 7: // ARRAY_2D
        if (dc) {
            if (lod != Lod::LZ) {
                return fail(token, "DC ARRAY_2D is only valid with LZ");
            }
            op.add_bits(0x0120000000000000ULL);
        } else {
            switch (lod) {
            case Lod::None:
                op.add_bits(0x00E0000000000000ULL);
                break;
            case Lod::LZ:
                op.add_bits(0x0100000000000000ULL);
                break;
            default:
                return fail(token, "LL ARRAY_2D is illegal");
            }
        }
        break;
    case 10: // 3D
        if (dc) {
            return fail(token, "3D cannot be DC");
        }
        switch (lod) {
        case Lod::None:
            op.add_bits(0x0140000000000000ULL);
            break;
        case Lod::LZ:
            op.add_bits(0x0160000000000000ULL);
            break;
        default:
            return fail(token, "3D can only have the default lod or LZ");
        }
        break;
    case 12: // CUBE
        if (dc) {
            return fail(token, "CUBE cannot be DC");
        }
        switch (lod) {
        case Lod::None:
            op.add_bits(0x0180000000000000ULL);
            break;
        case Lod::LL:
            op.add_bits(0x01A0000000000000ULL);
            break;
        default:
            return fail(token, "CUBE can only have the default lod or LL");
        }
        break;
    }
    token = ctx.tokenize();
    return {};
}

DEFINE_OPERAND(texs_swizzle)
{
    if (((op.value >> 28) & 0xFF) == 0xFF) {
        if (equal(token, "R")) {
            op.add_bits(0x0000000000000000ULL);
        } else if (equal(token, "G")) {
            op.add_bits(0x0004000000000000ULL);
        } else if (equal(token, "B")) {
            op.add_bits(0x0008000000000000ULL);
        } else if (equal(token, "A")) {
            op.add_bits(0x000C000000000000ULL);
        } else if (equal(token, "RG")) {
            op.add_bits(0x0010000000000000ULL);
        } else if (equal(token, "RA")) {
            op.add_bits(0x0014000000000000ULL);
        } else if (equal(token, "GA")) {
            op.add_bits(0x0018000000000000ULL);
        } else if (equal(token, "BA")) {
            op.add_bits(0x001C000000000000ULL);
        } else {
            return fail(token, "invalid RGBA swizzle");
        }
    } else {
        if (equal(token, "RGB")) {
            op.add_bits(0x0000000000000000ULL);
        } else if (equal(token, "RGA")) {
            op.add_bits(0x0004000000000000ULL);
        } else if (equal(token, "RBA")) {
            op.add_bits(0x0008000000000000ULL);
        } else if (equal(token, "GBA")) {
            op.add_bits(0x000C000000000000ULL);
        } else if (equal(token, "RGBA")) {
            op.add_bits(0x0010000000000000ULL);
        } else {
            return fail(token, "invalid RGBA swizzle");
        }
    }
    token = ctx.tokenize();
    return {};
}

namespace tld4
{
    template <int address>
    DEFINE_DOT_TABLE(component, -1, address, "R", "G", "B", "A");

    template <int address>
    DEFINE_DOT_TABLE(offset, 0, address, "", "AOFFI", "PTP");
}

namespace tld
{
    template <int address>
    DEFINE_DOT_TABLE(lod, -1, address, "LZ", "LL");
}

template <int address>
DEFINE_FLAG(ms, ".MS", address);

template <int address>
DEFINE_FLAG(cl, ".CL", address);

namespace txq
{
    DEFINE_OPERAND(mode)
    {
        static const char* modes[]{
            "",
            "TEX_HEADER_DIMENSION",
            "TEX_HEADER_TEXTURE_TYPE",
            "",
            "",
            "TEX_HEADER_SAMPLER_POS",
            nullptr,
        };
        const std::optional<uint64_t> result = find_in_table(token, modes, "");
        if (!result) {
            return fail(token, "invalid mode");
        }
        op.add_bits(*result << 22);

        token = ctx.tokenize();
        return {};
    }
}
