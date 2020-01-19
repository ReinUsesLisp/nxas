#pragma once

#include "error.h"
#include "opcode.h"
#include "token.h"

#define DECLARE_OPERAND(name) error name(context& ctx, token& token, opcode& op)
#define DEFINE_OPERAND(name) DECLARE_OPERAND(name)

typedef error (*operand)(context&, token&, opcode&);

DECLARE_OPERAND(comma);

DECLARE_OPERAND(dgpr0);
DECLARE_OPERAND(sgpr0);
DECLARE_OPERAND(sgpr8);
DECLARE_OPERAND(sgpr20);
DECLARE_OPERAND(cbuf);
DECLARE_OPERAND(imm);
DECLARE_OPERAND(imm32);
DECLARE_OPERAND(cc);

DECLARE_OPERAND(mask4_12);
DECLARE_OPERAND(mask4_39);

DECLARE_OPERAND(nop_trig);
DECLARE_OPERAND(nop_tests);
DECLARE_OPERAND(nop_mask);

DECLARE_OPERAND(stg_e);
DECLARE_OPERAND(stg_cache);
DECLARE_OPERAND(stg_size);
DECLARE_OPERAND(stg_address);

DECLARE_OPERAND(shr_format);
DECLARE_OPERAND(shr_mode);
DECLARE_OPERAND(shr_xmode);
DECLARE_OPERAND(shr_brev);

DECLARE_OPERAND(exit_keeprefcount);
DECLARE_OPERAND(exit_tests);