#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "context.h"
#include "error.h"
#include "helper.h"
#include "opcode.h"
#include "operand.h"
#include "parse.h"
#include "table.h"
#include "token.h"

static error assemble_predicate(const token& token, opcode& op, size_t shift, int is_negable)
{
    CHECK(confirm_type(token, token_type::predicate));

    if (!is_negable && token.data.predicate.negated) {
        return fail(token, "predicate can't be negated");
    }
    uint64_t bits = token.data.predicate.index;
    bits |= (token.data.predicate.negated ? 1 : 0) << 3;
    bits <<= shift;
    op.add_bits(bits);
    return {};
}

static std::pair<error, int> parse_insn(context& ctx, opcode& op, const insn& insn)
{
    op.add_bits(insn.opcode);

    token token = ctx.tokenize();
    if ((insn.flags & NO_PRED) && (~op.value & (7ULL << 16))) {
        return {fail(token, "%s does not support predicated execution", insn.mnemonic), 0};
    }
    int score = 1;
    for (const operand& operand : insn.operands) {
        if (error message = operand(ctx, token, op); message) {
            return {std::move(message), score};
        }
        ++score;
    }
    return {confirm_type(token, token_type::semicolon), score};
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
    error error_message;
    int error_score = -1;

    const size_t num_insns = sizeof(table) / sizeof(table[0]);
    for (size_t i = 0; i < num_insns; ++i) {
        const insn& insn = table[i];
        if (!equal(token, insn.mnemonic)) {
            continue;
        }
        auto [insn_error, score] = parse_insn(ctx, op, insn);
        if (!insn_error) {
            // successfully decoded instruction
            ctx.pc += 8;
            return true;
        }
        if (score > error_score) {
            error_message = std::move(insn_error);
            error_score = score;
        }
        // failure, restore context
        ctx = saved_context;
        op = saved_op;
    }
    if (error_message) {
        error_message.raise();
    } else {
        fatal_error(token, "unknown mnemonic \33[1m%.*s\33[0m", std::size(token.data.string),
                    std::data(token.data.string));
    }
}
