#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "helper.h"
#include "instruction.h"
#include "parse.h"
#include "token.h"

#define INSTR(name, func) error parse_##func(struct context *ctx, struct instruction *instr);
#include "instructions/list.inc"
#undef INSTR

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

int parse_instruction(struct context *ctx, struct instruction *instr)
{
    memset(instr, 0, sizeof *instr);

    struct token token = tokenize(ctx);
    if (token.type == TOKEN_TYPE_NONE) {
        return 0;
    }

    if (token.type == TOKEN_TYPE_OPERATOR_AT) {
        token = tokenize(ctx);
        char* message = assemble_predicate(&token, instr, 16, 1);
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
    error message = NULL;

#define INSTR(name, func)                                                                          \
    if (equal(&token, name)) {                                                                     \
        if (message) {                                                                             \
            free(message);                                                                         \
        }                                                                                          \
        message = parse_##func(ctx, instr);                                                        \
        if (!message) {                                                                            \
            return 1;                                                                              \
        }                                                                                          \
        *ctx = saved_context;                                                                      \
    }
#include "instructions/list.inc"
#undef INSTR

    if (message) {
        report_error(message);
    } else {
        fatal_error(&token, "unknown mnemonic %.*s", token.data.string.size,
                    token.data.string.text);
    }
}
