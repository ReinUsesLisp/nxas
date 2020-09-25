#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "helper.h"
#include "opcode.h"
#include "operand.h"
#include "parse.h"
#include "table.h"
#include "token.h"

static error assemble_predicate(const token& token, opcode& op, std::size_t shift, int is_negable)
{
    CHECK(confirm_type(token, token_type::predicate));

    if (!is_negable && token.data.predicate.negated) {
        return fail(token, "predicate can't be negated");
    }
    std::uint64_t bits = token.data.predicate.index;
    bits |= (token.data.predicate.negated ? 1 : 0) << 3;
    bits <<= shift;
    op.add_bits(bits);
    return {};
}

static error parse_insn(context& ctx, opcode& op, const insn& insn)
{
    op.add_bits(insn.opcode);

    token token = ctx.tokenize();
    if ((insn.flags & NO_PRED) && (~op.value & (7ULL << 16))) {
        return fail(token, "%s does not support predicated execution", insn.mnemonic);
    }

    for (std::size_t i = 0; insn.operands[i]; ++i) {
        CHECK(insn.operands[i](ctx, token, op));
    }
    return confirm_type(token, token_type::semicolon);
}

bool parse_instruction(context& ctx, opcode& op)
{
    token token = ctx.tokenize();
    if (token.type == token_type::none) {
        return false;
    }

    while (token.type == token_type::identifier && token.data.string[0] == '.') {
        ctx.parse_option(token);
    }

    if (ctx.pc % 0x20 == 0) {
        ctx.pc += 8;
    }

    if (token.type == token_type::at) {
        token = ctx.tokenize();
        if (error message = assemble_predicate(token, op, 16, 1); message) {
            message.raise();
        }
        token = ctx.tokenize();
    } else {
        // write always execute by default
        op.add_bits(7ULL << 16);
    }

    if (token.type != token_type::identifier) {
        fatal_error(token, "expected mnemonic");
    }

    const context saved_context = ctx;
    const opcode saved_op = op;
    error message;

    const std::size_t num_insns = sizeof(table) / sizeof(table[0]);
    for (std::size_t i = 0; i < num_insns; ++i) {
        const insn& insn = table[i];
        if (!equal(token, insn.mnemonic)) {
            continue;
        }

        message = parse_insn(ctx, op, insn);
        if (!message) {
            // successfully decoded instruction
            ctx.pc += 8;
            return true;
        }
        // failure, restore context
        ctx = saved_context;
        op = saved_op;
    }

    if (message) {
        message.raise();
    } else {
        fatal_error(token, "unknown mnemonic \33[1m%.*s\33[0m", std::size(token.data.string),
                    std::data(token.data.string));
    }
}
