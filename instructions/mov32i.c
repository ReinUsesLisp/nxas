#include "instructions/helper.h"

DEFINE_INSTRUCTION(mov32i)
{
	add_bits(instr, 0x0100000000000000ULL);
	instr->delay = 6;

	dest_gpr0(ctx, instr);
	check_comma(ctx, advance(ctx));

	add_bits(instr, parse_integer(ctx, advance(ctx), 0, UINT32_MAX) << 20);

	struct view token = advance(ctx);
	int64_t mask_value = 0xf;
	if (equal(&token, ",")) {
		mask_value = parse_integer(ctx, advance(ctx), 0, 0xf);
		token = advance(ctx);
	}
	add_bits(instr, (uint64_t)mask_value << 12);

	check_eol(ctx, token);
}
