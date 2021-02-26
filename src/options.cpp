#include "context.h"
#include "error.h"
#include "helper.h"
#include "token.h"

void context::parse_option(token& token)
{
    const int option_line = token.line;
    const auto check_option_line = [option_line, &token] {
        if (token.line != option_line) {
            fatal_error(token, "unexpected new line in option");
        }
    };
    const auto parse_type = [this, &check_option_line, &token] {
        if (type) {
            fatal_error(token, "program type already specified");
        }
        token = tokenize();
        check_option_line();

        if (equal(token, "vertex")) {
            type = program_type::vertex;
        } else if (equal(token, "tess_control") || equal(token, "hull")) {
            type = program_type::tess_control;
        } else if (equal(token, "tess_eval") || equal(token, "domain")) {
            type = program_type::tess_eval;
        } else if (equal(token, "geometry")) {
            type = program_type::geometry;
        } else if (equal(token, "fragment") || equal(token, "pixel")) {
            type = program_type::fragment;
        } else if (equal(token, "compute")) {
            type = program_type::compute;
        } else {
            fatal_error(token, "invalid program type \33[1m%.*s\33[0m",
                        static_cast<int>(token.data.string.size()), token.data.string.data());
        }
    };
    if (equal(token, ".dksh")) {
        is_dksh = true;
        parse_type();
    } else if (equal(token, ".type")) {
        parse_type();
    } else if (equal(token, ".num_gprs")) {
        token = tokenize();
        if (token.type == token_type::immediate) {
            fatal_error(token, "expected number of general purpose registers");
        }
        num_gprs = static_cast<int>(token.data.immediate);
        check_option_line();
    } else if (equal(token, ".workgroup_size")) {
        for (int i = 0; i < 3; ++i) {
            token = tokenize();
            if (token.type != token_type::immediate) {
                fatal_error(token, "expected workgroup sizes");
            }
            block_dimensions[i] = static_cast<int>(token.data.immediate);
        }
        check_option_line();
    } else if (equal(token, ".shared_memory")) {
        token = tokenize();
        if (token.type != token_type::immediate) {
            fatal_error(token, "expected shared memory size");
        }
        shared_mem_size = static_cast<int>(token.data.immediate);
    } else if (equal(token, ".local_memory")) {
        token = tokenize();
        if (token.type != token_type::immediate) {
            fatal_error(token, "expected local memory size");
        }
        local_mem_size = static_cast<int>(token.data.immediate);
    }
    token = tokenize();
    if (option_line == token.line) {
        fatal_error(token, "expected new line after option");
    }
}
