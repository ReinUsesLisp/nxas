#include <string_view>

#include "token.h"

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
    case token_type::float_immediate:
        return "floating-point immediate";
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
