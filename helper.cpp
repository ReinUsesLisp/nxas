#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"
#include "token.h"

static error assemble_gpr(context& ctx, token& token, opcode& op, int address)
{
    CHECK(confirm_type(token, token_type::regster));

    op.add_bits(static_cast<std::uint64_t>(token.data.regster) << address);

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

error convert_integer(const token& token, std::int64_t min, std::int64_t max, std::uint64_t* result)
{
    CHECK(confirm_type(token, token_type::immediate));

    std::int64_t value = token.data.immediate;
    if (value < min || value > max) {
        return fail(token,
                    "integer \33[1m%d\33[0m is out of range, expected to be "
                    "from %" PRId64 " to %" PRId64 " inclusively",
                    value, min, max);
    }
    *result = static_cast<std::uint64_t>(value);
    return {};
}

std::optional<std::uint64_t> find_in_table(const token& token, const char* const* table,
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

    for (std::size_t i = 0; table[i]; ++i) {
        if (text.compare(table[i]) != 0) {
            continue;
        }
        return static_cast<std::uint64_t>(i);
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
    std::int64_t value;
    CHECK(
        convert_integer(token, -(1 << 19), MAX_BITS(19), reinterpret_cast<std::uint64_t*>(&value)));

    std::uint64_t raw, negative;
    if (value < 0) {
        raw = (value - (1 << 19)) & MAX_BITS(19);
        negative = 1;
    } else {
        raw = static_cast<std::uint64_t>(value);
        negative = 0;
    }

    op.add_bits(raw << 20);
    op.add_bits(negative << 56);

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
    std::uint64_t value;
    CHECK(convert_integer(token, 0, MAX_BITS(5), &value));
    op.add_bits(static_cast<std::uint64_t>(value) << 34);

    CHECK(confirm_type_next(ctx, token, token_type::bracket_right));
    CHECK(confirm_type_next(ctx, token, token_type::bracket_left));

    token = ctx.tokenize();
    CHECK(convert_integer(token, INT16_MIN, INT16_MAX, &value));
    const int offset = static_cast<std::int16_t>(value);
    if (offset % 4) {
        return fail(first_token, "immediate constant buffer access has to be aligned to 4 bytes");
    }
    op.add_bits(static_cast<std::uint64_t>(static_cast<std::uint16_t>(offset) >> 2) << 20);

    CHECK(confirm_type_next(ctx, token, token_type::bracket_right));

    token = ctx.tokenize();
    return {};
}

error assemble_gpr20_cbuf_imm(context& ctx, token& token, opcode& op, std::uint64_t register_opcode,
                              std::uint64_t cbuf_opcode, std::uint64_t immediate_opcode)
{
    token = ctx.tokenize();
    switch (token.type) {
    case token_type::regster:
        op.add_bits(register_opcode);
        return assemble_source_gpr(ctx, token, op, 20);
    case token_type::identifier:
        op.add_bits(cbuf_opcode);
        return assemble_constant_buffer(ctx, token, op);
    case token_type::immediate:
        op.add_bits(immediate_opcode);
        return assemble_signed_20bit_immediate(ctx, token, op);
    default:
        return fail(token, "expected immediate, constant buffer or register");
    }
}
