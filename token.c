#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "instruction.h"
#include "token.h"

static void next(struct context *ctx)
{
    const char character = ctx->text++[0];
    if (character == '\n') {
        ++ctx->line;
        ctx->column = 0;
    } else {
        ++ctx->column;
    }
}

static int is_operator(int character) { return strchr("+-|[]@,;", character) != NULL; }

static int is_separator(int character)
{
    return isspace(character) || is_operator(character) || character == '.' || !character;
}

static int get_operator_type(int character)
{
    switch (character) {
    default:
        assert(0);
    case '+':
        return TOKEN_TYPE_OPERATOR_PLUS;
    case '-':
        return TOKEN_TYPE_OPERATOR_MINUS;
    case '|':
        return TOKEN_TYPE_OPERATOR_VBAR;
    case '[':
        return TOKEN_TYPE_OPERATOR_BRACKET_LEFT;
    case ']':
        return TOKEN_TYPE_OPERATOR_BRACKET_RIGHT;
    case '@':
        return TOKEN_TYPE_OPERATOR_AT;
    case ';':
        return TOKEN_TYPE_OPERATOR_SEMICOLON;
    case ',':
        return TOKEN_TYPE_OPERATOR_COMMA;
    }
}

struct token tokenize(struct context *ctx)
{
    while (isspace(*ctx->text) || *ctx->text == '\t') {
        next(ctx);
    }

    struct token token;
    token.filename = ctx->filename;
    token.line = ctx->line;
    token.column = ctx->column;

    if (!*ctx->text) {
        token.type = TOKEN_TYPE_NONE;
        return token;
    }

    const char *contents = ctx->text;

    if (strchr("!Pp", *ctx->text)) {
        token.type = TOKEN_TYPE_PREDICATE;

        if ((token.data.predicate.negated = *ctx->text == '!')) {
            next(ctx);
            if (!strchr("Pp", *ctx->text)) {
                fatal_error(&token, "fatal: invalid usage of '!'\n");
            }
        }
        next(ctx);

        if (strchr("Tt", *ctx->text)) {
            token.data.predicate.index = 7;
        } else if (*ctx->text < '0' || *ctx->text > '6') {
            fatal_error(&token, "out of range predicate");
        } else {
            token.data.predicate.index = *ctx->text - '0';
        }
        next(ctx);
        return token;
    }
    if (is_operator(*ctx->text) && (!strchr("+-", *ctx->text) || !isdigit(ctx->text[1]))) {
        token.type = get_operator_type(*ctx->text);
        next(ctx);
        return token;
    }
    if (isdigit(*ctx->text) || strchr("+-", *ctx->text)) {
        token.type = TOKEN_TYPE_IMMEDIATE;
        while (isdigit(*ctx->text) || strchr("+-AaBbCcDdEeFfXx", *ctx->text)) {
            next(ctx);
        }

        if (!is_separator(*ctx->text)) {
            fprintf(stderr, "no separator after immediate\n");
            exit(EXIT_FAILURE);
        }

        char *conversion_end = NULL;
        token.data.immediate = (int64_t)strtoll(contents, &conversion_end, 0);
        if (conversion_end != ctx->text) {
            fatal_error(&token, "failed to convert integer constant \33[1;31m%.*s\33[0m",
                        (int)(ctx->text - contents), contents);
        }
        return token;
    }
    if (*ctx->text == 'R') {
        token.type = TOKEN_TYPE_REGISTER;

        next(ctx);
        if (*ctx->text == 'Z') {
            next(ctx);

            if (!is_separator(*ctx->text)) {
                fatal_error(&token, "no separator after register");
            }

            token.data.regster = ZERO_REGISTER;
            return token;
        }

        while (isdigit(*ctx->text)) {
            next(ctx);
        }

        if (!is_separator(*ctx->text)) {
            fatal_error(&token, "no separator after register");
        }

        char *conversion_end = NULL;
        const long long value = (int64_t)strtoll(contents + 1, &conversion_end, 10);
        if (conversion_end != ctx->text) {
            fatal_error(&token, "invalid register index \33[1;31m%.*s\33[0m",
                        (int)(ctx->text - contents), contents);
        }
        if (value < 0 || value >= NUM_USER_REGISTERS) {
            fatal_error(&token,
                        "register index \33[1;31m%.*s\33[0m is out of range, expected to be "
                        "from 0 to 254 inclusively",
                        (int)(ctx->text - contents), contents);
        }

        token.data.regster = (uint8_t)value;
        return token;
    }

    token.type = TOKEN_TYPE_IDENTIFIER;
    token.data.string.text = contents;

    do {
        next(ctx);
    } while (!is_separator(*ctx->text));
    token.data.string.size = (size_t)(ctx->text - contents);
    return token;
}

const char *token_type_name(int type)
{
    switch (type) {
    default:
        return "unknown";
    case TOKEN_TYPE_NONE:
        return "nothing";
    case TOKEN_TYPE_IDENTIFIER:
        return "identifier";
    case TOKEN_TYPE_REGISTER:
        return "register";
    case TOKEN_TYPE_PREDICATE:
        return "predicate";
    case TOKEN_TYPE_IMMEDIATE:
        return "immediate";
    case TOKEN_TYPE_OPERATOR_PLUS:
        return "plus";
    case TOKEN_TYPE_OPERATOR_MINUS:
        return "minus";
    case TOKEN_TYPE_OPERATOR_VBAR:
        return "vertical bar";
    case TOKEN_TYPE_OPERATOR_BRACKET_LEFT:
        return "squared bracket left";
    case TOKEN_TYPE_OPERATOR_BRACKET_RIGHT:
        return "squared bracket right";
    case TOKEN_TYPE_OPERATOR_AT:
        return "at";
    case TOKEN_TYPE_OPERATOR_SEMICOLON:
        return "semicolon";
    case TOKEN_TYPE_OPERATOR_COMMA:
        return "comma";
    }
}

const char *token_extra_info(const struct token *token)
{
    if (token->type != TOKEN_TYPE_IDENTIFIER) {
        return "";
    }
    return token->data.string.text;
}

int token_extra_info_size(const struct token *token)
{
    if (token->type != TOKEN_TYPE_IDENTIFIER) {
        return 0;
    }
    return (int)token->data.string.size;
}
