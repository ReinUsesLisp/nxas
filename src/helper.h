#pragma once

#include <cstddef>
#include <cstdint>

#include <optional>
#include <string_view>

#include "error.h"
#include "token.h"

class context;
struct opcode;

#define DEFINE_INSTRUCTION(name) error parse_##name(context& ctx, opcode& op)

#define MAX_BITS(size) ((1ULL << (size)) - 1)

error confirm_type(const token& token, token_type type);

int equal(const token& token, const char* string);

error convert_integer(const token& token, int64_t min, int64_t max, uint64_t* result);

std::optional<uint64_t> find_in_table(const token& token, const char* const* table,
                                      std::string_view prefix);

error try_reuse(context& ctx, token& token, opcode& op, int address);

error assemble_dest_gpr(context& ctx, token& token, opcode& op, int address);

error assemble_source_gpr(context& ctx, token& token, opcode& op, int address);

error assemble_signed_20bit_immediate(context& ctx, token& token, opcode& op);

error assemble_float_20bit_immediate(context& ctx, token& token, opcode& op);

error assemble_constant_buffer(context& ctx, token& token, opcode& op);
