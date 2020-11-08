#pragma once

#include <stddef.h>
#include <stdint.h>

#include <optional>
#include <string_view>

#include "error.h"
#include "opcode.h"
#include "token.h"

#define DEFINE_INSTRUCTION(name) error parse_##name(context& ctx, opcode& op)

#define MAX_BITS(size) ((1ULL << (size)) - 1)

error confirm_type(const token& token, token_type type);

int equal(const token& token, const char* string);

error convert_integer(const token& token, std::int64_t min, std::int64_t max,
                      std::uint64_t* result);

std::optional<std::uint64_t> find_in_table(const token& token, const char* const* table,
                                           std::string_view prefix);

error try_reuse(context& ctx, token& token, opcode& op, int address);

error assemble_dest_gpr(context& ctx, token& token, opcode& op, int address);

error assemble_source_gpr(context& ctx, token& token, opcode& op, int address);

error assemble_signed_20bit_immediate(context& ctx, token& token, opcode& op);

error assemble_float_20bit_immediate(context& ctx, token& token, opcode& op);

error assemble_constant_buffer(context& ctx, token& token, opcode& op);
