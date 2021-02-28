#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>

#include "context.h"
#include "fp16.h"
#include "helper.h"
#include "operand.h"
#include "token.h"

static error assemble_gpr(context& ctx, token& token, opcode& op, int address)
{
    CHECK(confirm_type(token, token_type::regster));

    op.add_bits(static_cast<uint64_t>(token.data.regster) << address);

    token = ctx.tokenize();
    return try_reuse(ctx, token, op, address);
}

static error confirm_type_next(context& ctx, token& token, token_type type)
{
    token = ctx.tokenize();
    return confirm_type(token, type);
}

error confirm_type(const token& token, token_type type)
{
    if (token.type == type) {
        return {};
    }
    if (token.type == token_type::none) {
        return fail(token, "expected \33[1m%s\33[0m", name(type));
    }
    const std::string_view tkninfo = info(token);
    return fail(token, "expected \33[1m%s\33[0m and got \33[1m%s %.*s\33[0m", name(type),
                name(token.type), static_cast<int>(std::size(tkninfo)), std::data(tkninfo));
}

int equal(const token& token, const char* string)
{
    if (token.type != token_type::identifier) {
        return 0;
    }

    return token.data.string.compare(string) == 0;
}

error convert_integer(const token& token, int64_t min, int64_t max, uint64_t* result)
{
    CHECK(confirm_type(token, token_type::immediate));

    int64_t value = token.data.immediate;
    if (value < min || value > max) {
        return fail(token,
                    "integer \33[1m%d\33[0m is out of range, expected to be "
                    "from %" PRId64 " to %" PRId64 " inclusively",
                    value, min, max);
    }
    *result = static_cast<uint64_t>(value);
    return {};
}

std::optional<uint64_t> find_in_table(const token& token, const char* const* table,
                                      std::string_view prefix)
{
    if (token.type != token_type::identifier) {
        return {};
    }

    std::string_view text = token.data.string;

    if (!prefix.empty()) {
        if (prefix.compare(text.substr(0, std::size(prefix))) != 0) {
            return {};
        }
        text = text.substr(std::size(prefix));
    }

    for (size_t i = 0; table[i]; ++i) {
        if (text.compare(table[i]) != 0) {
            continue;
        }
        return static_cast<uint64_t>(i);
    }
    return {};
}

error try_reuse(context& ctx, token& token, opcode& op, int address)
{
    if (!equal(token, ".reuse")) {
        return {};
    }
    switch (address) {
    case 8:
        op.add_reuse(reuse_flag::gpr8);
        break;
    case 20:
        op.add_reuse(reuse_flag::gpr20);
        break;
    case 39:
        op.add_reuse(reuse_flag::gpr39);
        break;
    default:
        return fail(token, "register cannot be reused");
    }

    token = ctx.tokenize();
    return {};
}

error assemble_dest_gpr(context& ctx, token& token, opcode& op, int address)
{
    return assemble_gpr(ctx, token, op, address);
}

error assemble_source_gpr(context& ctx, token& token, opcode& op, int address)
{
    return assemble_gpr(ctx, token, op, address);
}

error assemble_signed_20bit_immediate(context& ctx, token& token, opcode& op)
{
    int64_t value = 0;
    CHECK(convert_integer(token, -(1 << 19), max_bits(19), reinterpret_cast<uint64_t*>(&value)));

    uint64_t raw = 0;
    uint64_t negative = 0;
    if (value < 0) {
        raw = (value - (1 << 19)) & max_bits(19);
        negative = 1;
    } else {
        raw = static_cast<uint64_t>(value);
        negative = 0;
    }

    op.add_bits(raw << 20);
    op.add_bits(negative << 56);

    token = ctx.tokenize();
    return {};
}

error assemble_float_immediate(context& ctx, token& token, opcode& op, int bits)
{
    static constexpr char MESSAGE[] = "expected floating-point literal, QNAN, or INF";

    float value;
    switch (token.type) {
    case token_type::float_immediate:
        value = token.data.float_immediate;
        break;
    case token_type::immediate:
        value = static_cast<float>(token.data.immediate);
        if (static_cast<int64_t>(token.data.immediate) != value) {
            return fail(token, "integer immediate " PRId64 "cannot be represented as float",
                        token.data.immediate);
        }
        break;
    case token_type::plus:
    case token_type::minus:
    case token_type::identifier: {
        const bool negate = token.type == token_type::minus;
        if (token.type != token_type::identifier) {
            token = ctx.tokenize();
        }
        if (equal(token, "QNAN")) {
            value = std::numeric_limits<float>::quiet_NaN();
        } else if (equal(token, "INF")) {
            value = std::numeric_limits<float>::infinity();
        } else {
            return fail(token, MESSAGE);
        }
        if (negate) {
            value = -value;
        }
        break;
    }
    default:
        return fail(token, MESSAGE);
    }
    uint32_t raw;
    std::memcpy(&raw, &value, sizeof(raw));
    if (bits == 20) {
        // TODO: add a setting to warn/error precision losses
        // if ((raw >> 12) << 12 != raw) ...
        op.add_bits(static_cast<uint64_t>((raw << 1) >> 13) << 20);
        if (raw >> 31 != 0) {
            op.add_bits(1ULL << 56);
        }
    } else if (bits == 32) {
        op.add_bits(static_cast<uint64_t>(raw) << 20);
    } else {
        assert(false && "invalid bits");
    }
    token = ctx.tokenize();
    return {};
}

error assemble_double_immediate(context& ctx, token& token, opcode& op)
{
    static constexpr char MESSAGE[] = "expected floating-point literal, QNAN, or INF";

    double value;
    switch (token.type) {
    case token_type::float_immediate:
        value = token.data.float_immediate;
        if (value < 0) {
            op.add_bits(1ULL << 56);
        }
        break;
    case token_type::immediate:
        value = static_cast<double>(token.data.immediate);
        if (value < 0) {
            op.add_bits(1ULL << 56);
        }
        break;
    case token_type::plus:
    case token_type::minus:
    case token_type::identifier: {
        if (token.type == token_type::minus) {
            op.add_bits(1ULL << 56);
        }
        if (token.type != token_type::identifier) {
            token = ctx.tokenize();
        }
        if (equal(token, "QNAN")) {
            value = std::numeric_limits<double>::quiet_NaN();
        } else if (equal(token, "INF")) {
            value = std::numeric_limits<double>::infinity();
        } else {
            return fail(token, MESSAGE);
        }
        break;
    }
    default:
        return fail(token, MESSAGE);
    }
    uint64_t raw;
    std::memcpy(&raw, &value, sizeof(raw));
    op.add_bits((static_cast<uint64_t>(raw << 1) >> 45) << 20);

    token = ctx.tokenize();
    return {};
}

error assemble_half_float_immediate(context& ctx, token& token, opcode& op, int bits, int offset,
                                    int neg_bit)
{
    static constexpr char MESSAGE[] = "expected floating-point literal, QNAN, or INF";

    uint16_t value;
    switch (token.type) {
    case token_type::float_immediate:
        value = fp32_to_fp16(token.data.float_immediate);
        if (token.data.float_immediate < 0 && neg_bit >= 0) {
            op.add_bits(1ULL << neg_bit);
        }
        break;
    case token_type::immediate:
        value = fp32_to_fp16(static_cast<float>(token.data.immediate));
        if (token.data.immediate < 0 && neg_bit >= 0) {
            op.add_bits(1ULL << neg_bit);
        }
        break;
    case token_type::plus:
    case token_type::minus:
    case token_type::identifier: {
        const bool negate = token.type == token_type::minus;
        if (token.type != token_type::identifier) {
            token = ctx.tokenize();
        }
        if (equal(token, "QNAN")) {
            value = 0x7fff;
        } else if (equal(token, "INF")) {
            value = 0x7c00;
        } else {
            return fail(token, MESSAGE);
        }
        if (negate) {
            if (neg_bit >= 0) {
                op.add_bits(1ULL << neg_bit);
            } else {
                value |= 0x8000;
            }
        }
        break;
    }
    default:
        return fail(token, MESSAGE);
    }
    if (bits == 9) {
        // TODO: add a setting to warn/error precision losses
        op.add_bits(static_cast<uint64_t>(static_cast<uint16_t>(value << 1) >> 7) << offset);
    } else if (bits == 16) {
        op.add_bits(static_cast<uint64_t>(value) << offset);
    } else {
        assert(false && "invalid bits");
    }
    token = ctx.tokenize();
    return {};
}

error assemble_constant_buffer(context& ctx, token& token, opcode& op)
{
    const struct token first_token = token;

    if (!equal(token, "c")) {
        return fail(token, "expected constant buffer");
    }

    CHECK(confirm_type_next(ctx, token, token_type::bracket_left));

    token = ctx.tokenize();
    uint64_t value;
    CHECK(convert_integer(token, 0, max_bits(5), &value));
    op.add_bits(static_cast<uint64_t>(value) << 34);

    CHECK(confirm_type_next(ctx, token, token_type::bracket_right));
    CHECK(confirm_type_next(ctx, token, token_type::bracket_left));

    token = ctx.tokenize();
    CHECK(convert_integer(token, INT16_MIN, INT16_MAX, &value));
    const int offset = static_cast<int16_t>(value);
    if (offset % 4) {
        return fail(first_token, "immediate constant buffer access has to be aligned to 4 bytes");
    }
    op.add_bits(static_cast<uint64_t>(static_cast<uint16_t>(offset) >> 2) << 20);

    CHECK(confirm_type_next(ctx, token, token_type::bracket_right));

    token = ctx.tokenize();
    return {};
}
