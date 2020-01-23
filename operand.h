#pragma once

#include <climits>
#include <cstdint>

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

#define DEFINE_DOT_TABLE(name, default_value, address, ...)                                        \
    DEFINE_OPERAND(name)                                                                           \
    {                                                                                              \
        static const char* table[] = {__VA_ARGS__, nullptr};                                       \
        const std::optional result = find_in_table(token, table, ".");                             \
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

inline error assemble_flag(context& ctx, opcode& op, token& token, const char* flag, int address)
{
    if (equal(token, flag)) {
        op.add_bits(1ULL << address);
        token = ctx.tokenize();
    }
    return {};
}

inline error assemble_uint(context& ctx, opcode& op, token& token, std::int64_t max_size,
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

DEFINE_OPERAND(label)
{
    switch (token.type) {
    case token_type::identifier:
        return fail(token, "labels are not implemented");
    case token_type::immediate: {
        static constexpr std::int64_t max = MAX_BITS(23);
        static constexpr std::int64_t min = -MAX_BITS(23) - 1;
        const std::int64_t value = token.data.immediate - ctx.pc - 8;
        if (value > max || value < min) {
            return fail(token, "label out of range");
        }
        op.add_bits((static_cast<std::uint64_t>(value) & 0x7FFFFF) << 20);
        op.add_bits((value < 0 ? 1ULL : 0ULL) << 43);

        token = ctx.tokenize();
        return {};
    }
    default:
        return fail(token, "expected label");
    }
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

DEFINE_OPERAND(cbuf)
{
    return assemble_constant_buffer(ctx, token, op);
}

DEFINE_OPERAND(imm)
{
    return assemble_signed_20bit_immediate(ctx, token, op);
}

DEFINE_UINT(imm32, UINT32_MAX, 20);

DEFINE_UINT(imm16, UINT16_MAX, 20);

DEFINE_FLAG(cc, ".CC", 47);

template <int bits, int address>
DEFINE_UINT(uint, MAX_BITS(bits), address);

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
DEFINE_DOT_TABLE(int_sign, 1, address, "S32", "U32");

template <int address>
DEFINE_DOT_TABLE(float_format, 2, address, "", "F16", "F32", "F64");

template <int address>
DEFINE_DOT_TABLE(byte_selector, 0, address, "B0", "B1", "B2", "B3");

template <int address>
DEFINE_OPERAND(pred_combine)
{
    static const char* table[] = {"AND", "OR", "XOR", nullptr};
    const std::optional value = find_in_table(token, table, ".");
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
    op.add_bits(static_cast<std::uint64_t>(token.data.predicate.index) << address);
    if constexpr (negable) {
        op.add_bits(static_cast<std::uint64_t>(token.data.predicate.negated) << (address + 3));
    } else {
        if (token.data.predicate.negated) {
            return fail(token, "predicate can't be negated");
        }
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
DEFINE_FLAG(post_neg, ".NEG", address);

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

namespace memory
{
    DEFINE_DOT_TABLE(size, 4, 48, "U8", "S8", "U16", "S16", "32", "64", "128");

    template <bool imm_offset = true>
    DEFINE_OPERAND(address)
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

        if constexpr (imm_offset) {
            if (token.type == token_type::immediate) {
                const int is_zero_reg = regster == ZERO_REGISTER;
                const std::int64_t min = is_zero_reg ? 0 : -(1 << 23);
                const std::int64_t max = MAX_BITS(is_zero_reg ? 24 : 23);

                std::uint64_t value;
                CHECK(convert_integer(token, min, max, &value));
                op.add_bits((value & MAX_BITS(24)) << 20);
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
    static const char* table[] = {"", "CG", "CS", "WT"};
    std::optional<std::uint64_t> cache = find_in_table(token, table, ".");
    if (cache) {
        token = ctx.tokenize();
    }
    op.add_bits(cache.value_or(0) << address);
    return {};
}

namespace nop
{
    DEFINE_FLAG(trig, ".TRIG", 13);

    DEFINE_OPERAND(tests)
    {
        static const char* tests[] = {
            "F",       "LT",      "EQ",      "LE",  "GT",  "NE",   "GE",     "NUM",    "NAN",
            "LTU",     "EQU",     "LEU",     "GTU", "NEU", "GEU",  "T",      "OFF",    "LO",
            "SFF",     "LS",      "HI",      "SFT", "HS",  "OFT",  "CSM_TA", "CSM_TR", "CSM_MX",
            "FCSM_TA", "FCSM_TR", "FCSM_MX", "RLE", "RGT", nullptr};
        return assemble_cc_tests(ctx, op, token, tests, 8);
    }

    DEFINE_UINT(mask, UINT16_MAX, 20);
}

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

namespace shr
{
    DEFINE_DOT_TABLE(format, 1, 48, "U32", "S32");
    DEFINE_DOT_TABLE(mode, 0, 39, "C", "W");
    DEFINE_DOT_TABLE(xmode, 0, 43, "", "INVALIDSHRXMODE1", "X", "XHI");
}

namespace stg
{
    DEFINE_FLAG(e, ".E", 45)

    DEFINE_OPERAND(size)
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
}

namespace f2f
{
    DEFINE_OPERAND(rounding)
    {
        static const char* equal[] = {"",      "",     "",      "PASS", "ROUND",
                                      "FLOOR", "CEIL", "TRUNC", nullptr};
        static const char* not_equal[] = {"RN", "RM", "RP", "RZ", nullptr};

        const int dst_format = (op.value >> 8) & 0b11;
        const int src_format = (op.value >> 10) & 0b11;
        const char* const* const table = dst_format == src_format ? equal : not_equal;
        const int default_value = dst_format == src_format ? 3 : 0;

        const std::optional result = find_in_table(token, table, ".");
        if (result) {
            token = ctx.tokenize();
        }
        op.add_bits(result.value_or(default_value) << 39);
        return {};
    }
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
        const std::optional result = find_in_table(token, table, "");
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

namespace hset2
{
    template <int address>
    DEFINE_DOT_TABLE(compare, -1, address, "F", "LT", "EQ", "LE", "GT", "NE", "GE", "NUM", "NAN",
                     "LTU", "EQU", "LEU", "GTU", "NEU", "GEU", "T");
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

    DEFINE_FLAG(ba, ".BA", 52);

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
        "",
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
        "",
        "",
        "",
        "SR_ORDERING_TICKET",
        "SR_PRIM_TYPE",
        "SR_INVOCATION_ID",
        "SR_Y_DIRECTION",
        "SR_THREAD_KILL",
        "SM_SHADER_TYPE",
        "SR_DIRECTCBEWRITEADDRESSLOW",
        "SR_DIRECTCBEWRITEADDRESSHIGH",
        "SR_DIRECTCBEWRITEENABLE",
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
        "",
        "", // SR_CTAID.X
        "", // SR_CTAID.Y
        "", // SR_CTAID.Z
        "SR_NTID",
        "SR_CirQueueIncrMinusOne",
        "SR_NLATC",
        "",
        "",
        "",
        "",
        "",
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
        "",
        "",
        "SR_GLOBALERRORSTATUS",
        "",
        "SR_WARPERRORSTATUS",
        "",
        "",
        "",
        "",
        "",
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
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "SR_HWTASKID",
        "SR_CIRCULARQUEUEENTRYINDEX",
        "SR_CIRCULARQUEUEENTRYADDRESSLOW",
        "SR_CIRCULARQUEUEENTRYADDRESSHIGH",
    };

    std::optional<std::uint64_t> value;
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
