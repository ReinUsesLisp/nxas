#include "instructions/helper.h"

static const char* tests[] = {
	"F", "LT", "EQ", "LE", "GT", "NE", "GE", "NUM", "NAN", "LTU", "EQU",
	"LEU", "GTU", "NEU", "GEU", "T", "OFF", "LO", "SFF", "LS", "HI", "SFT",
	"HS", "OFT", "CSM_TA", "CSM_TR", "CSM_MX", "FCSM_TA", "FCSM_TR",
	"FCSM_MX", "RLE", "RGT", NULL
};

DEFINE_INSTRUCTION(nop)
{
	add_bits(instr, 0x50B0000000000000ULL);
	instr->delay = 1;

	struct view token = advance(ctx);
	if (equal(&token, ".TRIG")) {
		add_bits(instr, 1ULL << 13);
		instr->delay = 6;

		token = advance(ctx);

		// we don't know exactly what condition codes and/or predcates are
		// read when TRIG is used, assume all of them are read
		for (size_t i = 0; i <= MAX_USER_PREDICATE; ++i) {
			read_predicate(instr, i);
		}
		for (size_t i = 0; i < NUM_CONDITION_CODES; ++i) {
			read_condition_code(instr, i);
		}
	}

	uint64_t test = 15;
	if (equal(&token, "CC")) {
		token = advance(ctx);
		if (!find_in_table(tests, ".", token, &test)) {
			fatal_error(ctx, &token, "unexpected test for NOP");
		}
		token = advance(ctx);
	}
	add_bits(instr, test << 8);

	if (!equal(&token, ";")) {
		add_bits(instr, parse_integer(ctx, token, 0, UINT16_MAX) << 20);
		token = advance(ctx);
	}

	check_eol(ctx, token);
}
