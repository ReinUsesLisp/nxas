#include "helper.h"

static const char *tests[] = {"F",       "LT",      "EQ",  "LE",     "GT",     "NE",     "GE",
                              "NUM",     "NAN",     "LTU", "EQU",    "LEU",    "GTU",    "NEU",
                              "GEU",     "T",       "OFF", "LO",     "SFF",    "LS",     "HI",
                              "SFT",     "HS",      "OFT", "CSM_TA", "CSM_TR", "CSM_MX", "FCSM_TA",
                              "FCSM_TR", "FCSM_MX", "RLE", "RGT",    NULL};

DEFINE_INSTRUCTION(nop)
{
    add_bits(instr, 0x50B0000000000000ULL);

    struct token token = tokenize(ctx);
    if (equal(&token, ".TRIG")) {
        add_bits(instr, 1ULL << 13);

        token = tokenize(ctx);
    }

    uint64_t test_index = 15;
    if (equal(&token, "CC")) {
        token = tokenize(ctx);
        if (!find_in_table(&token, tests, ".", &test_index)) {
            return fail(&token, "unexpected test for NOP");
        }
        token = tokenize(ctx);
    }
    add_bits(instr, test_index << 8);

    if (token.type == TOKEN_TYPE_IMMEDIATE) {
        uint64_t value;
        CHECK(convert_integer(&token, 0, UINT16_MAX, &value));
        add_bits(instr, value << 20);
        token = tokenize(ctx);
    }

    return confirm_type(&token, TOKEN_TYPE_OPERATOR_SEMICOLON);
}
