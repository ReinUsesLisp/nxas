#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#define NO_PRED 1U
#define RD 2U
#define WR 4U

#define INSN(opcode, flags, mnemonic, ...)                                                         \
    {                                                                                              \
        opcode, mnemonic, flags, (operand[]) { __VA_ARGS__, NULL }                                 \
    }

struct insn
{
    uint64_t opcode;
    char mnemonic[12];
    unsigned flags;
    operand *operands;
};

static const struct insn table[] = {
    INSN(0x50B0000000000000ULL, 0, "NOP", nop_trig, nop_tests),
    INSN(0x50B0000000000000ULL, 0, "NOP", nop_trig, nop_mask),
    INSN(0x50B0000000000000ULL, 0, "NOP", nop_trig, nop_tests, comma, nop_mask),
    INSN(0x5C98078000000000ULL, 0, "MOV", dgpr0, comma, sgpr20),
    INSN(0x5C98000000000000ULL, 0, "MOV", dgpr0, comma, sgpr20, comma, mask4_39),
    INSN(0x4C98078000000000ULL, 0, "MOV", dgpr0, comma, cbuf),
    INSN(0x4C98000000000000ULL, 0, "MOV", dgpr0, comma, cbuf, comma, mask4_39),
    INSN(0x3898078000000000ULL, 0, "MOV", dgpr0, comma, imm),
    INSN(0x3898000000000000ULL, 0, "MOV", dgpr0, comma, imm, comma, mask4_39),
    INSN(0x010000000000F000ULL, 0, "MOV32I", dgpr0, comma, imm32),
    INSN(0x0100000000000000ULL, 0, "MOV32I", dgpr0, comma, imm32, comma, mask4_12),
    INSN(0x5C28000000000000ULL, 0, "SHR", shr_format, shr_mode, shr_xmode, shr_brev, dgpr0, cc,
         comma, sgpr8, comma, sgpr20),
    INSN(0x4C28000000000000ULL, 0, "SHR", shr_format, shr_mode, shr_xmode, shr_brev, dgpr0, cc,
         comma, sgpr8, comma, cbuf),
    INSN(0x3828000000000000ULL, 0, "SHR", shr_format, shr_mode, shr_xmode, shr_brev, dgpr0, cc,
         comma, sgpr8, comma, imm),
    INSN(0xE300000000000000ULL, 0, "EXIT", exit_keeprefcount, exit_tests),
    INSN(0xEED8000000000000ULL, RD, "STG", stg_e, stg_cache, stg_size, stg_address, comma, sgpr0),
};

#endif // TABLE_H_INCLUDED
