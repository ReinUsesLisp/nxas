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
    INSN(0xF0C8000000000000ULL, 0, "S2R", dgpr<0>, comma, s2r),
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
    INSN(0x7080000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<57>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<56>, cbuf, comma, neg<51>, sgpr<39>, fp16::swizzle<53>),
    // TODO: HFMA2 immediate
    INSN(0x6080000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<57>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<56>, sgpr<39>, fp16::swizzle<53>, comma, neg<51>, cbuf),
    INSN(0x5D00000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<37>, sat<32>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<31>, sgpr<20>, fp16::swizzle<28>, comma, neg<35>, sgpr<39>, fp16::swizzle<35>),
    INSN(0x5D08000000000000ULL, 0, "HMUL2", fp16::merge<49>, fp16::control<39>, sat<32>, dgpr<0>, comma, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>),
    INSN(0x7880000000000000ULL, 0, "HMUL2", fp16::merge<49>, fp16::control<39>, sat<52>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, abs<54, cbuf>),
    // TODO: HMUL2 immediate
    INSN(0x5D10000000000000ULL, 0, "HADD2", fp16::merge<49>, ftz<39>, sat<32>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>),
    INSN(0x7A80000000000000ULL, 0, "HADD2", fp16::merge<49>, ftz<39>, sat<32>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, abs<54, cbuf>),
    // TODO: HADD2 immediate
    INSN(0x5D18000000000000ULL, 0, "HSET2", bf<49>, hset2::compare<35>, ftz<50>, pred_combine<45>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>, comma, pred<39, true>),
    INSN(0x7C80000000000000ULL, 0, "HSET2", bf<53>, hset2::compare<49>, ftz<54>, pred_combine<45>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, cbuf, comma, pred<39, true>),
    // TODO: HSET2 immediate
    INSN(0x5D20000000000000ULL, 0, "HSETP2", hset2::compare<35>, h_and<49>, ftz<6>, pred_combine<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>, comma, pred<39, true>),
    INSN(0x7E80000000000000ULL, 0, "HSETP2", hset2::compare<49>, h_and<53>, ftz<6>, pred_combine<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, abs<54, cbuf>, comma, pred<39, true>),
    // TODO: HSETP2 immediate
    INSN(0x5C00000000000000ULL, 0, "BFE", int_sign<48>, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C00000000000000ULL, 0, "BFE", int_sign<48>, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3800000000000000ULL, 0, "BFE", int_sign<48>, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0x5CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, cbuf),
    INSN(0x38E8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, imm),
    INSN(0x5C10000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, sgpr<20>),
    INSN(0x4C10000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, cbuf),
    INSN(0x3810000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, imm, post_neg<48>),
    INSN(0x5C18000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, sgpr<20>, comma, uint<5, 39>),
    INSN(0x4C18000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, cbuf, comma, uint<5, 39>),
    INSN(0x3818000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, imm, comma, uint<5, 39>),
    INSN(0x5C30000000000000ULL, 0, "FLO", flo::sign, sh<41>, dgpr<0>, cc, comma, tilde<40>, sgpr<20>),
    INSN(0x4C30000000000000ULL, 0, "FLO", flo::sign, sh<41>, dgpr<0>, cc, comma, tilde<40>, cbuf),
    INSN(0x3830000000000000ULL, 0, "FLO", flo::sign, sh<41>, dgpr<0>, cc, comma, imm, inv<40>),
    INSN(0xEF10000000000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>),
    INSN(0xEF10000010000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, uint<5, 20>, comma, sgpr<39>),
    INSN(0xEF10000020000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uint<13, 34>),
    INSN(0xEF10000030000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, uint<5, 20>, comma, uint<13, 34>),
    INSN(0xE350000000000000ULL, 0, "CONT", flow_tests),
    INSN(0xE340000000000000ULL, 0, "BRK", flow_tests),
    INSN(0xE330000000000000ULL, 0, "KIL", flow_tests),
    INSN(0xE320000000000000ULL, 0, "RET", flow_tests),
    INSN(0xE310000000000000ULL, 0, "LONGJMP", flow_tests),
    INSN(0xE300000000000000ULL, 0, "EXIT", keeprefcount, flow_tests),
    INSN(0xEB20000000000000ULL, 0, "SUST", image::p, image::type, store_cache<24>, image::rgba, image::clamp, memory::address<false>, comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xEB28000000000000ULL, 0, "SUST", image::p, image::type, store_cache<24>, image::rgba, image::clamp, memory::address<false>, comma, sgpr<0>, comma, uint<13, 36>),
    INSN(0xEB20000000000000ULL, 0, "SUST", image::d, image::ba<52>, image::type, store_cache<24>, image::size, image::clamp, memory::address<false>, comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xEB28000000000000ULL, 0, "SUST", image::d, image::ba<52>, image::type, store_cache<24>, image::size, image::clamp, memory::address<false>, comma, sgpr<0>, comma, uint<13, 36>),
    INSN(0xEB00000000000000ULL, 0, "SULD", image::p, image::type, load_cache<24>, image::rgba, image::clamp, dgpr<0>, comma, memory::address<false>, comma, sgpr<39>),
    INSN(0xEB08000000000000ULL, 0, "SULD", image::p, image::type, load_cache<24>, image::rgba, image::clamp, dgpr<0>, comma, memory::address<false>, comma, uint<13, 36>),
    INSN(0xEB00000000000000ULL, 0, "SULD", image::d, image::ba<23>, image::type, load_cache<24>, image::size, image::clamp, dgpr<0>, comma, memory::address<false>, comma, sgpr<39>),
    INSN(0xEB08000000000000ULL, 0, "SULD", image::d, image::ba<23>, image::type, load_cache<24>, image::size, image::clamp, dgpr<0>, comma, memory::address<false>, comma, uint<13, 36>),
    INSN(0xEED8000000000000ULL, RD, "STG", stg::e, store_cache<46>, memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEF58000000000000ULL, RD, "STS", memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEF50000000000000ULL, RD, "STL", stl::cache, memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEF48000000000000ULL, RD|WR, "LDS", lds::u, memory::size, dgpr<0>, comma, memory::address),
    INSN(0xE2A0000000000000ULL, NO_PRED, "PBK", label),
    // TODO: PBK constant buffer
};
// clang-format on

#endif // TABLE_H_INCLUDED
