#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "helper.h"
#include "instruction.h"
#include "operand.h"
#include "parse.h"
#include "table.h"
#include "token.h"

static error assemble_predicate(const struct token *token, struct instruction *instr, size_t shift,
                                int is_negable)
{
    CHECK(confirm_type(token, TOKEN_TYPE_PREDICATE));

    if (!is_negable && token->data.predicate.negated) {
        return fail(token, "predicate can't be negated");
    }
    uint64_t bits = token->data.predicate.index;
    bits |= (token->data.predicate.negated ? 1 : 0) << 3;
    bits <<= shift;
    add_bits(instr, bits);
    return NULL;
}

static error parse_insn(struct context *ctx, struct instruction *instr, const struct insn *insn)
{
    add_bits(instr, insn->opcode);

    struct token token = tokenize(ctx);
    if ((insn->flags & NO_PRED) && (instr->value & (7ULL << 16))) {
        return fail(&token, "%s does not support predicated execution", insn->mnemonic);
    }

    for (size_t i = 0; insn->operands[i]; ++i) {
        CHECK(insn->operands[i](ctx, &token, instr));
    }
    return confirm_type(&token, TOKEN_TYPE_OPERATOR_SEMICOLON);
}

int parse_instruction(struct context *ctx, struct instruction *instr)
{
    memset(instr, 0, sizeof *instr);

    struct token token = tokenize(ctx);
    if (token.type == TOKEN_TYPE_NONE) {
        return 0;
    }

    if (token.type == TOKEN_TYPE_OPERATOR_AT) {
        token = tokenize(ctx);
        char *message = assemble_predicate(&token, instr, 16, 1);
        if (message) {
            report_error(message);
        }

        token = tokenize(ctx);
    } else {
        // write always execute by default
        add_bits(instr, 7ULL << 16);
    }

    if (token.type != TOKEN_TYPE_IDENTIFIER) {
        fatal_error(&token, "expected mnemonic");
    }

    const struct context saved_context = *ctx;
    const struct instruction saved_instr = *instr;
    error message = NULL;

    const size_t num_insns = sizeof(table) / sizeof(table[0]);
    for (size_t i = 0; i < num_insns; ++i) {
        const struct insn *insn = &table[i];
        if (!equal(&token, insn->mnemonic)) {
            continue;
        }
        if (message) {
            free(message);
        }

        message = parse_insn(ctx, instr, insn);
        if (!message) {
            // successfully decoded insn
            return 1;
        }
        // failure, restore context
        *ctx = saved_context;
        *instr = saved_instr;
    }

    if (message) {
        report_error(message);
    } else {
        fatal_error(&token, "unknown mnemonic %.*s", token.data.string.size,
                    token.data.string.text);
    }
}
