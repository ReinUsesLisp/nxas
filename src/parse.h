#pragma once

class context;
struct opcode;

bool parse_instruction(context& ctx, opcode& op);
