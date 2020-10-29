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

static bool is_contained(std::string_view string, int character)
{
    return std::find(std::begin(string), std::end(string), (char)character) != std::end(string);
}

static bool is_operator(int character)
{
    return is_contained("+-~|[]@,;", character);
}

static bool is_separator(int character)
{
    return is_contained(" \t\r\n.", character) || is_operator(character) || !character;
}

static bool is_alpha(int character)
{
    return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z');
}

static bool is_decimal(int character)
{
    return character >= '0' && character <= '9';
}

static bool is_alnum(int character)
{
    return is_alpha(character) || is_decimal(character);
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
    case '~':
        return token_type::tilde;
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

context::context(const char* filename_, const char* text_)
    : filename{filename_}, text_begin{text_}, text{text_}
{
    generate_labels();
}

context::~context() = default;

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

        bool is_true = false;
        std::optional<int> index;
        if (is_contained("Tt", *text)) {
            is_true = true;
            index = 7;
        } else if (std::isdigit(*text)) {
            index = *text - '0';
        }
        if (index) {
            // check the next character if the next character is a separator
            next();
            if (is_separator(*text)) {
                if (!is_true && (*index > 6 || *index < 0)) {
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
        bool has_hex = false;
        bool is_floating_point = false;
        bool is_hex = false;
        while (std::isdigit(*text) || is_contained("+-", *text) ||
               (is_hex = is_contained("AaBbCcDdEeFfXx", *text))) {
            has_hex |= is_hex;
            next();
        }
        if (*text == '.') {
            if (has_hex) {
                fatal_error(token, "floating point literal cannot have hex characters");
            }
            is_floating_point = true;
            do {
                next();
            } while (std::isdigit(*text));
        }
        if (!is_separator(*text)) {
            fatal_error(token, "no separator after immediate");
        }
        // TODO: use charconv here once gcc 11 ships
        char* conversion_end = nullptr;
        if (is_floating_point) {
            token.type = token_type::float_immediate;
            token.data.float_immediate = std::strtof(contents, &conversion_end);
        } else {
            token.type = token_type::immediate;
            token.data.immediate =
                static_cast<std::int64_t>(std::strtoll(contents, &conversion_end, 0));
        }
        if (conversion_end != text) {
            fatal_error(token, "failed to parse literal \33[1;31m%.*s\33[0m",
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

        bool invalid_register = false;
        while (!is_separator(*text)) {
            if (!std::isdigit(*text)) {
                invalid_register = true;
                break;
            }
            next();
        }

        if (!invalid_register) {
            char* conversion_end = NULL;
            const long long value =
                static_cast<std::int64_t>(std::strtoll(contents + 1, &conversion_end, 10));
            if (conversion_end != text) {
                fatal_error(token, "invalid register index \33[1;31m%.*s\33[0m",
                            (int)(text - contents), contents);
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
    }

    do {
        next();
    } while (!is_separator(*text));

    if (text[-1] == ':') {
        // ignore labels
        return tokenize();
    }

    token.type = token_type::identifier;
    token.data.string = std::string_view(contents, static_cast<std::size_t>(text - contents));
    return token;
}

std::optional<std::int64_t> context::find_label(std::string_view label) const
{
    const auto it = labels.find(std::string{label});
    if (it == std::end(labels)) {
        return {};
    }
    return it->second;
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

void context::reset()
{
    text = text_begin;
    line = 0;
    column = 0;
    pc = 0;
}

void context::generate_labels()
{
    const auto advance = [this] {
        if (*text == ';') {
            pc += 8;
            if (pc % 0x20 == 0) {
                pc += 8;
            }
        }
        next();
    };
    const auto next_line = [this, advance] {
        do {
            advance();
        } while (*text && column > 0);
    };

    assert(line == 0 && column == 0);

    pc = 8;

    while (*text) {
        if (!is_alpha(*text)) {
            next_line();
            continue;
        }
        const char* const potential_label = text;
        while (is_alnum(*text) || *text == '_') {
            advance();
        }
        if (*text == ':') {
            std::string label;
            label.append(potential_label, static_cast<std::size_t>(text - potential_label));
            labels.insert({std::move(label), pc});
        }
        next_line();
    }

    reset();
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
    case token_type::tilde:
        return "tilde";
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
