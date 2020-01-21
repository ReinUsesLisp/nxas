#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#define NO_PRED 1U
#define RD 2U
#define WR 4U

#define INSN(opcode, flags, mnemonic, ...)                                                         \
    {                                                                                              \
        opcode, mnemonic, flags, (operand[])                                                       \
        {                                                                                          \
            __VA_ARGS__, nullptr                                                                   \
        }                                                                                          \
    }

struct insn
{
    uint64_t opcode;
    char mnemonic[12];
    unsigned flags;
    operand* operands;
};

// clang-format off
constexpr insn table[] = {
    INSN(0x50B0000000000000ULL, 0, "NOP", nop::trig, nop::tests),
    INSN(0x50B0000000000000ULL, 0, "NOP", nop::trig, nop::mask),
    INSN(0x50B0000000000000ULL, 0, "NOP", nop::trig, nop::tests, comma, nop::mask),
    INSN(0x5C98078000000000ULL, 0, "MOV", dgpr<0>, comma, sgpr<20>),
    INSN(0x5C98000000000000ULL, 0, "MOV", dgpr<0>, comma, sgpr<20>, comma, mask4<39>),
    INSN(0x4C98078000000000ULL, 0, "MOV", dgpr<0>, comma, cbuf),
    INSN(0x4C98000000000000ULL, 0, "MOV", dgpr<0>, comma, cbuf, comma, mask4<39>),
    INSN(0x3898078000000000ULL, 0, "MOV", dgpr<0>, comma, imm),
    INSN(0x3898000000000000ULL, 0, "MOV", dgpr<0>, comma, imm, comma, mask4<39>),
    INSN(0x010000000000F000ULL, 0, "MOV32I", dgpr<0>, comma, imm32),
    INSN(0x0100000000000000ULL, 0, "MOV32I", dgpr<0>, comma, imm32, comma, mask4<12>),
    INSN(0x5B00000000000000ULL, 0, "XMAD", xmad::signs, psl<36>, xmad::mode_a, mrg<37>, x<38>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, sgpr<20>, half<35>, comma, sgpr<39>),
    INSN(0x5100000000000000ULL, 0, "XMAD", xmad::signs, xmad::mode_b, x<54>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, sgpr<39>, half<52>, comma, cbuf),
    INSN(0x4E00000000000000ULL, 0, "XMAD", xmad::signs, psl<55>, xmad::mode_b, mrg<56>, x<54>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, cbuf, half<52>, comma, sgpr<39>),
    INSN(0x3600000000000000ULL, 0, "XMAD", xmad::signs, psl<36>, xmad::mode_a, mrg<37>, x<38>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, imm16, comma, sgpr<39>),
    INSN(0x5C28000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C28000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3828000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0x5CA8000000000000ULL, 0, "F2F", ftz<44>, float_format<8>, float_format<10>, f2f::rounding, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, half<41>),
    INSN(0x4CA8000000000000ULL, 0, "F2F", ftz<44>, float_format<8>, float_format<10>, f2f::rounding, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, half<41>),
    // TODO: F2F immediate
    INSN(0x5CB0000000000000ULL, 0, "F2I", ftz<44>, f2i::int_format, float_format<10>, f2i::rounding, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, half<41>),
    INSN(0x4CB0000000000000ULL, 0, "F2I", ftz<44>, f2i::int_format, float_format<10>, f2i::rounding, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, half<41>),
    // TODO: F2I immediate
    INSN(0x5C00000000000000ULL, 0, "BFE", int_sign<48>, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C00000000000000ULL, 0, "BFE", int_sign<48>, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3800000000000000ULL, 0, "BFE", int_sign<48>, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0x5CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, cbuf),
    INSN(0x38E8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, imm),
    INSN(0xE300000000000000ULL, 0, "EXIT", exit_insn::keeprefcount, exit_insn::tests),
    INSN(0xEED8000000000000ULL, RD, "STG", stg::e, stg::cache, stg::size, stg::address, comma, sgpr<0>),
    INSN(0xE2A0000000000000ULL, NO_PRED, "PBK", label),
    // TODO: PBK constant buffer
};
// clang-format on

#endif // TABLE_H_INCLUDED
