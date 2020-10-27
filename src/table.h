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
    INSN(0x5C58000000000000ULL, 0, "FADD", ftz<44>, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, sgpr<20>>),
    INSN(0x4C58000000000000ULL, 0, "FADD", ftz<44>, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, cbuf>),
    // TODO: FADD immediate
    INSN(0x5BB0000000000000ULL, 0, "FSETP", float_compare<48>, ftz<47>, pred_combine<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, neg<6>, abs<44, sgpr<20>>, comma, pred<39, true>),
    INSN(0x4BB0000000000000ULL, 0, "FSETP", float_compare<48>, ftz<47>, pred_combine<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, neg<6>, abs<44, cbuf>, comma, pred<39, true>),
    // TODO: FSETP immediate
    INSN(0x5B00000000000000ULL, 0, "XMAD", xmad::signs, psl<36>, xmad::mode_a, mrg<37>, x<38>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, sgpr<20>, half<35>, comma, sgpr<39>),
    INSN(0x5100000000000000ULL, 0, "XMAD", xmad::signs, xmad::mode_b, x<54>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, sgpr<39>, half<52>, comma, cbuf),
    INSN(0x4E00000000000000ULL, 0, "XMAD", xmad::signs, psl<55>, xmad::mode_b, mrg<56>, x<54>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, cbuf, half<52>, comma, sgpr<39>),
    INSN(0x3600000000000000ULL, 0, "XMAD", xmad::signs, psl<36>, xmad::mode_a, mrg<37>, x<38>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, imm16, comma, sgpr<39>),
    INSN(0x5C28000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C28000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3828000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0x5CF8000000000000ULL, 0, "SHF", shf::r, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma,        sgpr<20>, comma, sgpr<39>),
    INSN(0x5BF8000000000000ULL, 0, "SHF", shf::l, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma,        sgpr<20>, comma, sgpr<39>),
    INSN(0x38F8000000000000ULL, 0, "SHF", shf::r, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma, uinteger<6, 20>, comma, sgpr<39>),
    INSN(0x36F8000000000000ULL, 0, "SHF", shf::l, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma, uinteger<6, 20>, comma, sgpr<39>),
    INSN(0x5CA8000000000000ULL, 0, "F2F", ftz<44>, float_format<8>, float_format<10>, f2f::rounding, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, half<41>),
    INSN(0x4CA8000000000000ULL, 0, "F2F", ftz<44>, float_format<8>, float_format<10>, f2f::rounding, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, half<41>),
    // TODO: F2F immediate
    INSN(0x5CB0000000000000ULL, 0, "F2I", ftz<44>, f2i::int_format, float_format<10>, f2i::rounding, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, half<41>),
    INSN(0x4CB0000000000000ULL, 0, "F2I", ftz<44>, f2i::int_format, float_format<10>, f2i::rounding, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, half<41>),
    // TODO: F2I immediate
    INSN(0x5CB8000000000000ULL, 0, "I2F", float_format<8>, i2f::int_format, fp_rounding<39>, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, i2f::selector),
    INSN(0x4CB8000000000000ULL, 0, "I2F", float_format<8>, i2f::int_format, fp_rounding<39>, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, i2f::selector),
    INSN(0x38B8000000000000ULL, 0, "I2F", float_format<8>, i2f::int_format, fp_rounding<39>, dgpr<0>, cc, comma, imm, post_neg<45>, post_abs<49>, i2f::selector),
    INSN(0x5CE0000000000000ULL, 0, "I2I", i2i::format<8, 12>, i2i::format<10, 13>, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, i2i::selector),
    INSN(0x4CE0000000000000ULL, 0, "I2I", i2i::format<8, 12>, i2i::format<10, 13>, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, i2i::selector),
    INSN(0x38E0000000000000ULL, 0, "I2I", i2i::format<8, 12>, i2i::format<10, 13>, sat<50>, dgpr<0>, cc, comma, imm, post_neg<45>, post_abs<49>),
    INSN(0x7080000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<57>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<56>, cbuf, comma, neg<51>, sgpr<39>, fp16::swizzle<53>),
    // TODO: HFMA2 immediate
    INSN(0x6080000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<57>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<56>, sgpr<39>, fp16::swizzle<53>, comma, neg<51>, cbuf),
    INSN(0x5D00000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<37>, sat<32>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<31>, sgpr<20>, fp16::swizzle<28>, comma, neg<30>, sgpr<39>, fp16::swizzle<35>),
    INSN(0x5D08000000000000ULL, 0, "HMUL2", fp16::merge<49>, fp16::control<39>, sat<32>, dgpr<0>, comma, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>),
    INSN(0x7880000000000000ULL, 0, "HMUL2", fp16::merge<49>, fp16::control<39>, sat<52>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, abs<54, cbuf>),
    // TODO: HMUL2 immediate
    INSN(0x5D10000000000000ULL, 0, "HADD2", fp16::merge<49>, ftz<39>, sat<32>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>),
    INSN(0x7A80000000000000ULL, 0, "HADD2", fp16::merge<49>, ftz<39>, sat<32>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, abs<54, cbuf>),
    // TODO: HADD2 immediate
    INSN(0x5D18000000000000ULL, 0, "HSET2", bf<49>, float_compare<35>, ftz<50>, pred_combine<45>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>, comma, pred<39, true>),
    INSN(0x7C80000000000000ULL, 0, "HSET2", bf<53>, float_compare<49>, ftz<54>, pred_combine<45>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, cbuf, comma, pred<39, true>),
    // TODO: HSET2 immediate
    INSN(0x5D20000000000000ULL, 0, "HSETP2", float_compare<35>, h_and<49>, ftz<6>, pred_combine<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>, comma, pred<39, true>),
    INSN(0x7E80000000000000ULL, 0, "HSETP2", float_compare<49>, h_and<53>, ftz<6>, pred_combine<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, abs<54, cbuf>, comma, pred<39, true>),
    // TODO: HSETP2 immediate
    INSN(0x3A04000000000000ULL, 0, "VMNMX", video::dest_sign, video::src_format<37, 48>, video::src_format<29, 49>, video::mx<56>, sat<55>, video::vmnmx_op<51>, dgpr<0>, cc, comma, sgpr<8>, video::selector<36, 37>, comma, sgpr<20>, video::selector<28, 29>, comma, sgpr<39>),
    INSN(0x5C00000000000000ULL, 0, "BFE", bfe::format, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C00000000000000ULL, 0, "BFE", bfe::format, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3800000000000000ULL, 0, "BFE", bfe::format, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0x5090000000000000ULL, 0, "PSETP", pred_combine<24>, pred_combine<45>, pred<3>, comma, pred<0>, comma, pred<12, true>, comma, pred<29, true>, comma, pred<39, true>),
    INSN(0x5CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, cbuf),
    INSN(0x38E8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, imm),
    INSN(0x5CF0000000000000ULL, 0, "R2P", p2r::mode<40>, comma, sgpr<8>, byte_selector<41>, comma, sgpr<20>),
    INSN(0x4CF0000000000000ULL, 0, "R2P", p2r::mode<40>, comma, sgpr<8>, byte_selector<41>, comma, cbuf),
    INSN(0x38F0000000000000ULL, 0, "R2P", p2r::mode<40>, comma, sgpr<8>, byte_selector<41>, comma, imm),
    INSN(0x5C10000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, sgpr<20>),
    INSN(0x4C10000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, cbuf),
    INSN(0x3810000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, imm, post_neg<48>),
    INSN(0x5C18000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, sgpr<20>, comma, uinteger<5, 39>),
    INSN(0x4C18000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, cbuf, comma, uinteger<5, 39>),
    INSN(0x3818000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, imm, comma, uinteger<5, 39>),
    INSN(0x5C30000000000000ULL, 0, "FLO", flo::sign, sh<41>, dgpr<0>, cc, comma, tilde<40>, sgpr<20>),
    INSN(0x4C30000000000000ULL, 0, "FLO", flo::sign, sh<41>, dgpr<0>, cc, comma, tilde<40>, cbuf),
    INSN(0x3830000000000000ULL, 0, "FLO", flo::sign, sh<41>, dgpr<0>, cc, comma, imm, inv<40>),
    INSN(0x5C08000000000000ULL, 0, "POPC", dgpr<0>, comma, tilde<40>, sgpr<20>),
    INSN(0x4C08000000000000ULL, 0, "POPC", dgpr<0>, comma, tilde<40>, cbuf),
    INSN(0x3808000000000000ULL, 0, "POPC", dgpr<0>, comma, imm, inv<40>),
    INSN(0x5BDF000000000000ULL, 0, "LEA", lea::hi, x<38>,                  dgpr<0>, cc, comma, neg<37>, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>),
    INSN(0x5BDF000000000000ULL, 0, "LEA", lea::hi, x<38>,                  dgpr<0>, cc, comma, neg<37>, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>, comma, uinteger<5, 28>),
    INSN(0x5BD8000000000000ULL, 0, "LEA", lea::hi, x<38>, pred<48>, comma, dgpr<0>, cc, comma, neg<37>, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>),
    INSN(0x5BD8000000000000ULL, 0, "LEA", lea::hi, x<38>, pred<48>, comma, dgpr<0>, cc, comma, neg<37>, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>, comma, uinteger<5, 28>),
    INSN(0x1807000000000000ULL, 0, "LEA", lea::hi, x<57>,                  dgpr<0>, cc, comma, neg<56>, sgpr<8>, comma, cbuf, comma, sgpr<39>),
    INSN(0x1807000000000000ULL, 0, "LEA", lea::hi, x<57>,                  dgpr<0>, cc, comma, neg<56>, sgpr<8>, comma, cbuf, comma, sgpr<39>, comma, uinteger<5, 51>),
    INSN(0x1800000000000000ULL, 0, "LEA", lea::hi, x<57>, pred<48>, comma, dgpr<0>, cc, comma, neg<56>, sgpr<8>, comma, cbuf, comma, sgpr<39>),
    INSN(0x1800000000000000ULL, 0, "LEA", lea::hi, x<57>, pred<48>, comma, dgpr<0>, cc, comma, neg<56>, sgpr<8>, comma, cbuf, comma, sgpr<39>, comma, uinteger<5, 51>),
    INSN(0x5BD7000000000000ULL, 0, "LEA", lea::lo, x<46>,                  dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, sgpr<20>),
    INSN(0x5BD7000000000000ULL, 0, "LEA", lea::lo, x<46>,                  dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, sgpr<20>, comma, uinteger<5, 39>),
    INSN(0x5BD0000000000000ULL, 0, "LEA", lea::lo, x<46>, pred<48>, comma, dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, sgpr<20>),
    INSN(0x5BD0000000000000ULL, 0, "LEA", lea::lo, x<46>, pred<48>, comma, dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, sgpr<20>, comma, uinteger<5, 39>),
    INSN(0x4BD7000000000000ULL, 0, "LEA", lea::lo, x<46>,                  dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, cbuf),
    INSN(0x4BD7000000000000ULL, 0, "LEA", lea::lo, x<46>,                  dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, cbuf, comma, uinteger<5, 39>),
    INSN(0x4BD0000000000000ULL, 0, "LEA", lea::lo, x<46>, pred<48>, comma, dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, cbuf),
    INSN(0x4BD0000000000000ULL, 0, "LEA", lea::lo, x<46>, pred<48>, comma, dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, cbuf, comma, uinteger<5, 39>),
    INSN(0x36D7000000000000ULL, 0, "LEA", lea::lo, x<46>,                  dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, imm),
    INSN(0x36D7000000000000ULL, 0, "LEA", lea::lo, x<46>,                  dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, imm, comma, uinteger<5, 39>),
    INSN(0x36D0000000000000ULL, 0, "LEA", lea::lo, x<46>, pred<48>, comma, dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, imm),
    INSN(0x36D0000000000000ULL, 0, "LEA", lea::lo, x<46>, pred<48>, comma, dgpr<0>, cc, comma, neg<45>, sgpr<8>, comma, imm, comma, uinteger<5, 39>),
    INSN(0x5BA0000000000000ULL, 0, "FCMP", float_compare<48>, ftz<47>, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>),
    INSN(0x53A0000000000000ULL, 0, "FCMP", float_compare<48>, ftz<47>, dgpr<0>, comma, sgpr<8>, comma, sgpr<39>, comma,     cbuf),
    INSN(0x4BA0000000000000ULL, 0, "FCMP", float_compare<48>, ftz<47>, dgpr<0>, comma, sgpr<8>, comma,     cbuf, comma, sgpr<39>),
    // TODO: FCMP immediate
    INSN(0xEF10000000000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma,        sgpr<20>, comma, sgpr<39>),
    INSN(0xEF10000010000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, uinteger<5, 20>, comma, sgpr<39>),
    INSN(0xEF10000020000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma,        sgpr<20>, comma, uinteger<13, 34>),
    INSN(0xEF10000030000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, uinteger<5, 20>, comma, uinteger<13, 34>),
    INSN(0xF0F8000000000000ULL, 0, "SYNC", flow_tests),
    INSN(0xE350000000000000ULL, 0, "CONT", flow_tests),
    INSN(0xE340000000000000ULL, 0, "BRK", flow_tests),
    INSN(0xE330000000000000ULL, 0, "KIL", flow_tests),
    INSN(0xE320000000000000ULL, 0, "RET", flow_tests),
    INSN(0xE310000000000000ULL, 0, "LONGJMP", flow_tests),
    INSN(0xE300000000000000ULL, 0, "EXIT", keeprefcount, flow_tests),
    INSN(0xEB20000000000000ULL, 0, "SUST", image::p, image::type, store_cache<24>, image::rgba, image::clamp, memory::address<false>, comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xEB28000000000000ULL, 0, "SUST", image::p, image::type, store_cache<24>, image::rgba, image::clamp, memory::address<false>, comma, sgpr<0>, comma, uinteger<13, 36>),
    INSN(0xEB20000000000000ULL, 0, "SUST", image::d, image::ba<52>, image::type, store_cache<24>, image::size, image::clamp, memory::address<false>, comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xEB28000000000000ULL, 0, "SUST", image::d, image::ba<52>, image::type, store_cache<24>, image::size, image::clamp, memory::address<false>, comma, sgpr<0>, comma, uinteger<13, 36>),
    INSN(0xEB00000000000000ULL, 0, "SULD", image::p, image::type, load_cache<24>, image::rgba, image::clamp, dgpr<0>, comma, memory::address<false>, comma, sgpr<39>),
    INSN(0xEB08000000000000ULL, 0, "SULD", image::p, image::type, load_cache<24>, image::rgba, image::clamp, dgpr<0>, comma, memory::address<false>, comma, uinteger<13, 36>),
    INSN(0xEB00000000000000ULL, 0, "SULD", image::d, image::ba<23>, image::type, load_cache<24>, image::size, image::clamp, dgpr<0>, comma, memory::address<false>, comma, sgpr<39>),
    INSN(0xEB08000000000000ULL, 0, "SULD", image::d, image::ba<23>, image::type, load_cache<24>, image::size, image::clamp, dgpr<0>, comma, memory::address<false>, comma, uinteger<13, 36>),
    INSN(0xEED8000000000000ULL, RD, "STG", stg::e, store_cache<46>, memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEF58000000000000ULL, RD, "STS", memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEF50000000000000ULL, RD, "STL", stl::cache, memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEED0000000000000ULL, RD|WR, "LDG", stg::e, ldg::cache, ldg::size, dgpr<0>, comma, memory::address),
    INSN(0xEF48000000000000ULL, RD|WR, "LDS", lds::u, memory::size, dgpr<0>, comma, memory::address),
    INSN(0xEF40000000000000ULL, RD|WR, "LDL", ldl::cache, memory::size, dgpr<0>, comma, memory::address),
    INSN(0xED00000000000000ULL, RD|WR, "ATOM", atom::e, atom::operation, atomic_size<49>, dgpr<0>, comma, memory::address<true, 28, 20, 0>, comma, sgpr<20>),
    INSN(0xEC00000000000000ULL, RD|WR, "ATOMS", atoms::operation, atoms::size, dgpr<0>, comma, memory::address<true, 30, 22, 2>, comma, sgpr<20>),
    INSN(0xEBF8000000000000ULL, RD|WR, "RED", red::e, red::operation, atomic_size<20>, memory::address<true, 28, 20, 0>, comma, sgpr<0>),
    INSN(0xE240000000000000ULL, 0, "BRA", u<7>, lmt<6>, flow_tests, comma, label),
    INSN(0xE24000000000000FULL, 0, "BRA", u<7>, lmt<6>, label),
    INSN(0xE2A0000000000000ULL, NO_PRED, "PBK", label),
    // TODO: PBK constant buffer
    INSN(0xE290000000000000ULL, NO_PRED, "SSY", label),
    // TODO: SSY constant buffer
    INSN(0xE260000000000000ULL, NO_PRED, "CAL", noinc, label),
    // TODO: CAL constant buffer
};
// clang-format on

#endif // TABLE_H_INCLUDED
