#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string_view>

#include "error.h"
#include "opcode.h"
#include "token.h"

[[gnu::always_inline]] static bool is_contained(std::string_view string, int character)
{
    return std::find(std::begin(string), std::end(string), (char)character) != std::end(string);
}

[[gnu::always_inline]] static bool is_operator(int character)
{
    return is_contained("+-|[]@,;", character);
}

[[gnu::always_inline]] static bool is_separator(int character)
{
    return is_contained(" \t\r\n.", character) || is_operator(character) || !character;
}

static token_type get_operator_type(int character)
{
    switch (character) {
    default:
        assert(0);
    case '+':
        return token_type::plus;
    case '-':
        return token_type::minus;
    case '|':
        return token_type::vbar;
    case '[':
        return token_type::bracket_left;
    case ']':
        return token_type::bracket_right;
    case '@':
        return token_type::at;
    case ';':
        return token_type::semicolon;
    case ',':
        return token_type::comma;
    }
}

token context::tokenize()
{
    while (std::isspace(*text) || *text == '\t') {
        next();
    }

    token token;
    token.filename = filename;
    token.line = line;
    token.column = column;

    if (!*text) {
        token.type = token_type::none;
        return token;
    }

    const char* contents = text;
    const char character = *text;

    if (is_operator(character) && (!is_contained("+-", character) || !std::isdigit(text[1]))) {
        token.type = get_operator_type(*text);
        next();
        return token;
    }
    if (is_contained("!Pp", character)) {
        token.type = token_type::predicate;

        if ((token.data.predicate.negated = *text == '!')) {
            next();
            if (!is_contained("Pp", *text)) {
                fatal_error(token, "fatal: invalid usage of '!'\n");
            }
        }
        next();

        std::optional<int> index;
        if (is_contained("Tt", *text)) {
            index = 7;
        } else if (std::isdigit(*text)) {
            index = *text - '0';
        }
        if (index) {
            // check the next character if the next character is a separator
            next();
            if (is_separator(*text)) {
                if (*index > 6 || *index < 0) {
                    fatal_error(token, "out of range predicate");
                }
                token.data.predicate.index = *index;
                return token;
            }
            // if it's not a separator, we might have an identifier (e.g. P2R)
        }
    }
    if (std::isdigit(character) || is_contained("+-", character)) {
        token.type = token_type::immediate;
        while (std::isdigit(*text) || is_contained("+-AaBbCcDdEeFfXx", *text)) {
            next();
        }

        if (!is_separator(*text)) {
            fprintf(stderr, "no separator after immediate\n");
            exit(EXIT_FAILURE);
        }

        char* conversion_end = NULL;
        token.data.immediate =
            static_cast<std::int64_t>(std::strtoll(contents, &conversion_end, 0));
        if (conversion_end != text) {
            fatal_error(token, "failed to convert integer constant \33[1;31m%.*s\33[0m",
                        (int)(text - contents), contents);
        }
        return token;
    }
    if (character == 'R') {
        token.type = token_type::regster;

        next();
        if (*text == 'Z') {
            next();

            if (!is_separator(*text)) {
                fatal_error(token, "no separator after register");
            }

            token.data.regster = ZERO_REGISTER;
            return token;
        }

        while (std::isdigit(*text)) {
            next();
        }

        if (!is_separator(*text)) {
            fatal_error(token, "no separator after register");
        }

        char* conversion_end = NULL;
        const long long value =
            static_cast<std::int64_t>(std::strtoll(contents + 1, &conversion_end, 10));
        if (conversion_end != text) {
            fatal_error(token, "invalid register index \33[1;31m%.*s\33[0m", (int)(text - contents),
                        contents);
        }
        if (value < 0 || value >= NUM_USER_REGISTERS) {
            fatal_error(token,
                        "register index \33[1;31m%.*s\33[0m is out of range, expected to be "
                        "from 0 to 254 inclusively",
                        (int)(text - contents), contents);
        }

        token.data.regster = static_cast<uint8_t>(value);
        return token;
    }

    do {
        next();
    } while (!is_separator(*text));

    token.type = token_type::identifier;
    token.data.string = std::string_view(contents, static_cast<std::size_t>(text - contents));
    return token;
}

void context::next()
{
    const char character = text++[0];
    switch (character) {
    default:
        ++column;
        break;
    case '\t':
        column += 4;
        break;
    case '\n':
        ++line;
        column = 0;
        break;
    }
}

const char* name(token_type type)
{
    switch (type) {
    default:
        return "unknown";
    case token_type::none:
        return "nothing";
    case token_type::identifier:
        return "identifier";
    case token_type::regster:
        return "register";
    case token_type::predicate:
        return "predicate";
    case token_type::immediate:
        return "immediate";
    case token_type::plus:
        return "plus";
    case token_type::minus:
        return "minus";
    case token_type::vbar:
        return "vertical bar";
    case token_type::bracket_left:
        return "squared bracket left";
    case token_type::bracket_right:
        return "squared bracket right";
    case token_type::at:
        return "at";
    case token_type::semicolon:
        return "semicolon";
    case token_type::comma:
        return "comma";
    }
}

std::string_view info(const token& token)
{
    if (token.type != token_type::identifier) {
        return {};
    }
    return token.data.string;
}
