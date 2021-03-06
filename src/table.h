#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "operand.h"

#define INSN(opcode, flags, mnemonic, ...)                                                         \
    insn                                                                                           \
    {                                                                                              \
        opcode, mnemonic, flags,                                                                   \
        {                                                                                          \
            __VA_ARGS__                                                                            \
        }                                                                                          \
    }

constexpr unsigned NO_PRED = 1;
constexpr unsigned RD = 2;
constexpr unsigned WR = 4;

struct insn
{
    uint64_t opcode;
    char mnemonic[12];
    unsigned flags;
    std::vector<operand> operands;
};

// clang-format off
const insn table[]{
    INSN(0xEFA0000000000000ULL, 0, "AL2P", al2p::o, amem::size, pred<44>, comma, dgpr<0>, comma, sgpr<8>, comma, sinteger<11, 20>),
    INSN(0xEFA0700000000000ULL, 0, "AL2P", al2p::o, amem::size,                  dgpr<0>, comma, sgpr<8>, comma, sinteger<11, 20>),
    INSN(0xEFA000000000FF00ULL, 0, "AL2P", al2p::o, amem::size, pred<44>, comma, dgpr<0>,                 comma, sinteger<11, 20>),
    INSN(0xEFA070000000FF00ULL, 0, "AL2P", al2p::o, amem::size,                  dgpr<0>,                 comma, sinteger<11, 20>),
    INSN(0xEFA0000000000000ULL, 0, "AL2P", al2p::o, amem::size, pred<44>, comma, dgpr<0>, comma, sgpr<8>),
    INSN(0xEFA0700000000000ULL, 0, "AL2P", al2p::o, amem::size,                  dgpr<0>, comma, sgpr<8>),
    INSN(0xEFA000000000FF00ULL, 0, "AL2P", al2p::o, amem::size, pred<44>, comma, dgpr<0>),
    INSN(0xEFA070000000FF00ULL, 0, "AL2P", al2p::o, amem::size,                  dgpr<0>),
    INSN(0xEFD8000000000000ULL, 0, "ALD", ald::o,            ald::size, dgpr<0>, comma, ald::imm_attr,   default_rz<39>),
    INSN(0xEFD8000000000000ULL, 0, "ALD", ald::o,            ald::size, dgpr<0>, comma, ald::imm_attr,   comma, sgpr<39>),
    INSN(0xEFD8000000000000ULL, 0, "ALD", ald::o, ald::p,    ald::size, dgpr<0>, comma, ald::patch_attr, default_rz<39>),
    INSN(0xEFD8000000000000ULL, 0, "ALD", ald::o, ald::p,    ald::size, dgpr<0>, comma, ald::patch_attr, comma, sgpr<39>),
    INSN(0xEFD8000000000000ULL, 0, "ALD", ald::o, ald::phys, ald::size, dgpr<0>, comma, ald::phys_attr,  default_rz<39>),
    INSN(0xEFD8000000000000ULL, 0, "ALD", ald::o, ald::phys, ald::size, dgpr<0>, comma, ald::phys_attr,  comma, sgpr<39>),
    INSN(0xEFF0000000000000ULL, 0, "AST",            ald::size, ald::imm_attr,   comma, sgpr<0>, default_rz<39>),
    INSN(0xEFF0000000000000ULL, 0, "AST",            ald::size, ald::imm_attr,   comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xEFF0000000000000ULL, 0, "AST", ald::p,    ald::size, ald::patch_attr, comma, sgpr<0>, default_rz<39>),
    INSN(0xEFF0000000000000ULL, 0, "AST", ald::phys, ald::size, ald::phys_attr,  comma, sgpr<0>, default_rz<39>),
    INSN(0xEFF0000000000000ULL, 0, "AST", ald::phys, ald::size, ald::phys_attr,  comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xED00000000000000ULL, RD|WR, "ATOM", atom::e, atom::operation, atomic_size<49>, dgpr<0>, comma, memory::address<true, 28, 20, 0>, comma, sgpr<20>),
    INSN(0xEC00000000000000ULL, RD|WR, "ATOMS", atoms::operation, atoms::size, dgpr<0>, comma, memory::address<true, 30, 22, 2>, comma, sgpr<20>),
    INSN(0xF0B8000000000000ULL, 0, "B2R",              dgpr<0>, comma, uinteger<8, 8>),
    INSN(0xF0B8000000000000ULL, 0, "B2R", b2r::warp,   dgpr<0>),
    INSN(0xF0B8000000000000ULL, 0, "B2R", b2r::result, dgpr<0>, comma, pred<45, true>),
    // TODO: BAR
    INSN(0x5C00000000000000ULL, 0, "BFE", bfe::format, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C00000000000000ULL, 0, "BFE", bfe::format, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3800000000000000ULL, 0, "BFE", bfe::format, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0x5BF0000000000000ULL, 0, "BFI", dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>),
    INSN(0x53F0000000000000ULL, 0, "BFI", dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<39>, comma, cbuf),
    INSN(0x4BF0000000000000ULL, 0, "BFI", dgpr<0>, cc, comma, sgpr<8>, comma, cbuf,     comma, sgpr<39>),
    INSN(0x36F0000000000000ULL, 0, "BFI", dgpr<0>, cc, comma, sgpr<8>, comma, imm,      comma, sgpr<39>),
    INSN(0xE3A0000000000000ULL, 0, "BPT", bpt::mode, uinteger<20, 20>),
    INSN(0xE3A0000000000000ULL, 0, "BPT", bpt::mode),
    INSN(0x5098000000000000ULL, 0, "CSET", bf<44>, cc_tests, bop<45>, dgpr<0>, cc, comma, cc_text, comma, pred<39, true>),
    INSN(0x50A0000000000000ULL, 0, "CSETP", cc_tests, bop<45>, pred<3>, comma, pred<0>, comma, cc_text, comma, pred<39, true>),
    INSN(0x5C70000000000000ULL, 0, "DADD", fp_rounding<39>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, sgpr<20>>),
    INSN(0x4C70000000000000ULL, 0, "DADD", fp_rounding<39>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, cbuf>),
    INSN(0x3870000000000000ULL, 0, "DADD", fp_rounding<39>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, dimm20, post_neg<45>, post_abs<49>),
    INSN(0x5B70000000000000ULL, 0, "DFMA", fp_rounding<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, sgpr<20>, comma, neg<49>, sgpr<39>),
    INSN(0x5370000000000000ULL, 0, "DFMA", fp_rounding<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, sgpr<39>, comma, neg<49>, cbuf),
    INSN(0x4B70000000000000ULL, 0, "DFMA", fp_rounding<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, cbuf,     comma, neg<49>, sgpr<39>),
    INSN(0x3670000000000000ULL, 0, "DFMA", fp_rounding<50>, dgpr<0>, cc, comma, sgpr<8>, comma, dimm20, post_neg<48>, comma, neg<49>, sgpr<39>),
    INSN(0x5C50000000000000ULL, 0, "DMNMX", dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, sgpr<20>>, comma, pred<39, true>),
    INSN(0x4C50000000000000ULL, 0, "DMNMX", dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, cbuf>, comma, pred<39, true>),
    INSN(0x3850000000000000ULL, 0, "DMNMX", dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, dimm20, post_neg<45>, post_abs<49>, comma, pred<39, true>),
    INSN(0x5C80000000000000ULL, 0, "DMUL", fp_rounding<39>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, sgpr<20>),
    INSN(0x4C80000000000000ULL, 0, "DMUL", fp_rounding<39>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, cbuf),
    INSN(0x3880000000000000ULL, 0, "DMUL", fp_rounding<39>, dgpr<0>, cc, comma, sgpr<8>, comma, dimm20, post_neg<48>),
    INSN(0x5900000000000000ULL, 0, "DSET", bf<52>, float_compare<48>, bop<45>, dgpr<0>, cc, comma, neg<43>, abs<54, sgpr<8>>, comma, neg<53>, abs<44, sgpr<20>>, comma, pred<39, true>),
    INSN(0x4900000000000000ULL, 0, "DSET", bf<52>, float_compare<48>, bop<45>, dgpr<0>, cc, comma, neg<43>, abs<54, sgpr<8>>, comma, neg<53>, abs<44, cbuf>,     comma, pred<39, true>),
    INSN(0x3200000000000000ULL, 0, "DSET", bf<52>, float_compare<48>, bop<45>, dgpr<0>, cc, comma, neg<43>, abs<54, sgpr<8>>, comma, dimm20, post_neg<53>, post_abs<44>, comma, pred<39, true>),
    INSN(0x5B80000000000000ULL, 0, "DSETP", float_compare<48>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, neg<6>, abs<44, sgpr<20>>, comma, pred<39, true>),
    INSN(0x4B80000000000000ULL, 0, "DSETP", float_compare<48>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, neg<6>, abs<44, cbuf>,     comma, pred<39, true>),
    INSN(0x3680000000000000ULL, 0, "DSETP", float_compare<48>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, dimm20, post_neg<6>, post_abs<44>, comma, pred<39, true>),
    INSN(0x50B0000000000F00ULL, 0, "NOP", nop::trig),
    INSN(0x50B0000000000000ULL, 0, "NOP", nop::trig, cc_text, cc_tests),
    INSN(0x50B0000000000F00ULL, 0, "NOP", nop::trig, nop::mask),
    INSN(0x50B0000000000000ULL, 0, "NOP", nop::trig, cc_text, cc_tests, comma, nop::mask),
    INSN(0x5C98078000000000ULL, 0, "MOV", dgpr<0>, comma, sgpr<20>),
    INSN(0x5C98000000000000ULL, 0, "MOV", dgpr<0>, comma, sgpr<20>, comma, mask4<39>),
    INSN(0x4C98078000000000ULL, 0, "MOV", dgpr<0>, comma, cbuf),
    INSN(0x4C98000000000000ULL, 0, "MOV", dgpr<0>, comma, cbuf, comma, mask4<39>),
    INSN(0x3898078000000000ULL, 0, "MOV", dgpr<0>, comma, imm),
    INSN(0x3898000000000000ULL, 0, "MOV", dgpr<0>, comma, imm, comma, mask4<39>),
    INSN(0x010000000000F000ULL, 0, "MOV32I", dgpr<0>, comma, uimm32),
    INSN(0x0100000000000000ULL, 0, "MOV32I", dgpr<0>, comma, uimm32, comma, mask4<12>),
    INSN(0x5080000000000000ULL, 0, "MUFU", mufu::operation, sat<50>, dgpr<0>, comma, neg<48>, abs<46, sgpr<8>>),
    INSN(0xF0C8000000000000ULL, 0, "S2R", dgpr<0>, comma, s2r),
    INSN(0x5C58000000000000ULL, 0, "FADD", ftz<44>, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, sgpr<20>>),
    INSN(0x4C58000000000000ULL, 0, "FADD", ftz<44>, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, cbuf>),
    INSN(0x3858000000000000ULL, 0, "FADD", ftz<44>, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, fimm, post_neg<45>, post_abs<49>),
    INSN(0x0800000000000000ULL, 0, "FADD32I", ftz<55>, dgpr<0>, cc<52>, comma, neg<56>, abs<54, sgpr<8>>, comma, fimm32, post_neg<53>, post_abs<57>),
    INSN(0x5980000000000000ULL, 0, "FFMA", fmul::fmz<53>, fp_rounding<51>, sat<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, sgpr<20>,           comma, neg<49>, sgpr<39>),
    INSN(0x5180000000000000ULL, 0, "FFMA", fmul::fmz<53>, fp_rounding<51>, sat<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, sgpr<39>,           comma, neg<49>, cbuf),
    INSN(0x4980000000000000ULL, 0, "FFMA", fmul::fmz<53>, fp_rounding<51>, sat<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, cbuf,               comma, neg<49>, sgpr<39>),
    INSN(0x3280000000000000ULL, 0, "FFMA", fmul::fmz<53>, fp_rounding<51>, sat<50>, dgpr<0>, cc, comma, sgpr<8>, comma,          fimm, post_neg<48>, comma, neg<49>, sgpr<39>),
    INSN(0x0c00000000000000ull, 0, "FFMA32I", fmul::fmz<53>, sat<55>, dgpr<0>, cc<52>, comma, neg<56>, sgpr<8>, comma, fimm32, comma, neg<57>, mirror_dgpr<0>),
    INSN(0x5C60000000000000ULL, 0, "FMNMX", ftz<44>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, sgpr<20>>, comma, pred<39, true>),
    INSN(0x4C60000000000000ULL, 0, "FMNMX", ftz<44>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, neg<45>, abs<49, cbuf>,     comma, pred<39, true>),
    INSN(0x3860000000000000ULL, 0, "FMNMX", ftz<44>, dgpr<0>, cc, comma, neg<48>, abs<46, sgpr<8>>, comma, fimm, post_neg<45>, post_abs<49>, comma, pred<39, true>),
    INSN(0x5C68000000000000ULL, 0, "FMUL", fmul::fmz<44>, fmul::scale, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, sgpr<20>),
    INSN(0x4C68000000000000ULL, 0, "FMUL", fmul::fmz<44>, fmul::scale, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, sgpr<8>, comma, neg<48>, cbuf),
    INSN(0x3868000000000000ULL, 0, "FMUL", fmul::fmz<44>, fmul::scale, fp_rounding<39>, sat<50>, dgpr<0>, cc, comma, sgpr<8>, comma, fimm, post_neg<48>),
    INSN(0x1E00000000000000ULL, 0, "FMUL32I", fmul::fmz<53>, sat<55>, dgpr<0>, cc<52>, comma, sgpr<8>, comma, fimm32),
    INSN(0x5800000000000000ULL, 0, "FSET", bf<52>, float_compare<48>, ftz<55>, bop<45>, dgpr<0>, cc, comma, neg<43>, abs<54, sgpr<8>>, comma, neg<53>, abs<44, sgpr<20>>, comma, pred<39, true>),
    INSN(0x4800000000000000ULL, 0, "FSET", bf<52>, float_compare<48>, ftz<55>, bop<45>, dgpr<0>, cc, comma, neg<43>, abs<54, sgpr<8>>, comma, neg<53>, abs<44, cbuf>,     comma, pred<39, true>),
    INSN(0x3000000000000000ULL, 0, "FSET", bf<52>, float_compare<48>, ftz<55>, bop<45>, dgpr<0>, cc, comma, neg<43>, abs<54, sgpr<8>>, comma, fimm, post_neg<53>, post_abs<44>, comma, pred<39, true>),
    INSN(0x5BB0000000000000ULL, 0, "FSETP", float_compare<48>, ftz<47>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, neg<6>, abs<44, sgpr<20>>, comma, pred<39, true>),
    INSN(0x4BB0000000000000ULL, 0, "FSETP", float_compare<48>, ftz<47>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, neg<6>, abs<44, cbuf>, comma, pred<39, true>),
    INSN(0x36B0000000000000ULL, 0, "FSETP", float_compare<48>, ftz<47>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<7, sgpr<8>>, comma, fimm, post_neg<6>, post_abs<44>, comma, pred<39, true>),
    INSN(0x5B00000000000000ULL, 0, "XMAD", xmad::signs, psl<36>, xmad::mode_a, mrg<37>, x<38>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, sgpr<20>, half<35>, comma, sgpr<39>),
    INSN(0x5100000000000000ULL, 0, "XMAD", xmad::signs,          xmad::mode_b,          x<54>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma, sgpr<39>, half<52>, comma,     cbuf),
    INSN(0x4E00000000000000ULL, 0, "XMAD", xmad::signs, psl<55>, xmad::mode_b, mrg<56>, x<54>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma,     cbuf, half<52>, comma, sgpr<39>),
    INSN(0x3600000000000000ULL, 0, "XMAD", xmad::signs, psl<36>, xmad::mode_a, mrg<37>, x<38>, dgpr<0>, cc, comma, sgpr<8>, half<53>, comma,   uimm16,           comma, sgpr<39>),
    INSN(0x5C28000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C28000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3828000000000000ULL, 0, "SHR", shr::format, shr::mode, shr::xmode, brev<40>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0x5CF8000000000000ULL, 0, "SHF", shf::r, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma,        sgpr<20>, comma, sgpr<39>),
    INSN(0x5BF8000000000000ULL, 0, "SHF", shf::l, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma,        sgpr<20>, comma, sgpr<39>),
    INSN(0x38F8000000000000ULL, 0, "SHF", shf::r, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma, uinteger<6, 20>, comma, sgpr<39>),
    INSN(0x36F8000000000000ULL, 0, "SHF", shf::l, w<50>, shf::max_shift, shf::xmode, dgpr<0>, cc, comma, sgpr<8>, comma, uinteger<6, 20>, comma, sgpr<39>),
    INSN(0x5CA8000000000000ULL, 0, "F2F", ftz<44>, float_format<8>, float_format<10>, f2f::rounding, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, half<41>),
    INSN(0x4CA8000000000000ULL, 0, "F2F", ftz<44>, float_format<8>, float_format<10>, f2f::rounding, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, half<41>),
    INSN(0x38A8000000000000ULL, 0, "F2F", ftz<44>, float_format<8>, float_format<10>, f2f::rounding, sat<50>, dgpr<0>, cc, comma, fimm, post_neg<45>, post_abs<49>, half<41>),
    INSN(0x5CB0000000000000ULL, 0, "F2I", ftz<44>, f2i::int_format, float_format<10>, f2i::rounding, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, half<41>),
    INSN(0x4CB0000000000000ULL, 0, "F2I", ftz<44>, f2i::int_format, float_format<10>, f2i::rounding, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, half<41>),
    // TODO: F2I immediate
    INSN(0x5CB8000000000000ULL, 0, "I2F", float_format<8>, i2f::int_format, fp_rounding<39>, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, i2f::selector),
    INSN(0x4CB8000000000000ULL, 0, "I2F", float_format<8>, i2f::int_format, fp_rounding<39>, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, i2f::selector),
    INSN(0x38B8000000000000ULL, 0, "I2F", float_format<8>, i2f::int_format, fp_rounding<39>, dgpr<0>, cc, comma, imm, post_neg<45>, post_abs<49>, i2f::selector),
    INSN(0x5CE0000000000000ULL, 0, "I2I", i2i::format<8, 12>, i2i::format<10, 13>, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, sgpr<20>>, i2i::selector),
    INSN(0x4CE0000000000000ULL, 0, "I2I", i2i::format<8, 12>, i2i::format<10, 13>, sat<50>, dgpr<0>, cc, comma, neg<45>, abs<49, cbuf>, i2i::selector),
    INSN(0x38E0000000000000ULL, 0, "I2I", i2i::format<8, 12>, i2i::format<10, 13>, sat<50>, dgpr<0>, cc, comma, imm, post_neg<45>, post_abs<49>),
    INSN(0x5D10000000000000ULL, 0, "HADD2", fp16::merge<49>, ftz<39>, sat<32>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>),
    INSN(0x7A80000000000000ULL, 0, "HADD2", fp16::merge<49>, ftz<39>, sat<52>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, abs<54, cbuf>),
    INSN(0x7A00000000000000ULL, 0, "HADD2", fp16::merge<49>, ftz<39>, sat<52>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, fimm9_high<56>, comma, fimm9_low<29>),
    INSN(0x2C00000000000000ULL, 0, "HADD2_32I", ftz<55>, sat<52>, dgpr<0>, comma, neg<56>, sgpr<8>, fp16::swizzle<53>, comma, fimm16_high, comma, fimm16_low),
    INSN(0x7080000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<57>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<56>, cbuf, comma, neg<51>, sgpr<39>, fp16::swizzle<53>),
    INSN(0x7000000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<57>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, fimm9_high<56>, comma, fimm9_low<29>, comma, neg<51>, sgpr<39>, fp16::swizzle<53>),
    INSN(0x6080000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<57>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<56>, sgpr<39>, fp16::swizzle<53>, comma, neg<51>, cbuf),
    INSN(0x5D00000000000000ULL, 0, "HFMA2", fp16::merge<49>, fp16::control<37>, sat<32>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<47>, comma, neg<31>, sgpr<20>, fp16::swizzle<28>, comma, neg<30>, sgpr<39>, fp16::swizzle<35>),
    INSN(0x2800000000000000ULL, 0, "HFMA2_32I", fp16::control<55>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<53>, comma, fimm16_high, comma, fimm16_low, comma, neg<52>, mirror_dgpr<0>),
    INSN(0x5D08000000000000ULL, 0, "HMUL2", fp16::merge<49>, fp16::control<39>, sat<32>, dgpr<0>, comma,          abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>),
    INSN(0x7880000000000000ULL, 0, "HMUL2", fp16::merge<49>, fp16::control<39>, sat<52>, dgpr<0>, comma,          abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<43>, abs<54, cbuf>),
    INSN(0x7800000000000000ULL, 0, "HMUL2", fp16::merge<49>, fp16::control<39>, sat<52>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, fimm9_high<56>, comma, fimm9_low<29>),
    INSN(0x2A00000000000000ULL, 0, "HMUL2_32I", fp16::control<55>, sat<52>, dgpr<0>, comma, sgpr<8>, fp16::swizzle<53>, comma, fimm16_high, comma, fimm16_low),
    INSN(0x5D18000000000000ULL, 0, "HSET2", bf<49>, float_compare<35>, ftz<50>, bop<45>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>, comma, pred<39, true>),
    INSN(0x7C80000000000000ULL, 0, "HSET2", bf<53>, float_compare<49>, ftz<54>, bop<45>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, cbuf, comma, pred<39, true>),
    INSN(0x7C00000000000000ULL, 0, "HSET2", bf<53>, float_compare<49>, ftz<54>, bop<45>, dgpr<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, fimm9_high<56>, comma, fimm9_low<29>, comma, pred<39, true>),
    INSN(0x5D20000000000000ULL, 0, "HSETP2", float_compare<35>, h_and<49>, ftz<6>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<31>, abs<30, sgpr<20>>, fp16::swizzle<28>, comma, pred<39, true>),
    INSN(0x7E80000000000000ULL, 0, "HSETP2", float_compare<49>, h_and<53>, ftz<6>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, neg<56>, abs<54, cbuf>, comma, pred<39, true>),
    INSN(0x7E00000000000000ULL, 0, "HSETP2", float_compare<49>, h_and<53>, ftz<6>, bop<45>, pred<3>, comma, pred<0>, comma, neg<43>, abs<44, sgpr<8>>, fp16::swizzle<47>, comma, fimm9_high<56>, comma, fimm9_low<29>, comma, pred<39, true>),
    INSN(0x5090000000000000ULL, 0, "PSETP", bop<24>, bop<45>, pred<3>, comma, pred<0>, comma, pred<12, true>, comma, pred<29, true>, comma, pred<39, true>),
    INSN(0x5088000000000000ULL, 0, "PSET", bf<44>, bop<24>, bop<45>, dgpr<0>, cc, comma, pred<12, true>, comma, pred<29, true>, comma, pred<39, true>),
    INSN(0x5CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4CE8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, cbuf),
    INSN(0x38E8000000000000ULL, 0, "P2R", byte_selector<41>, dgpr<0>, comma, p2r::mode<40>, comma, sgpr<8>, comma, imm),
    INSN(0x5CF0000000000000ULL, 0, "R2P", p2r::mode<40>, comma, sgpr<8>, byte_selector<41>, comma, sgpr<20>),
    INSN(0x4CF0000000000000ULL, 0, "R2P", p2r::mode<40>, comma, sgpr<8>, byte_selector<41>, comma, cbuf),
    INSN(0x38F0000000000000ULL, 0, "R2P", p2r::mode<40>, comma, sgpr<8>, byte_selector<41>, comma, imm),
    INSN(0x5C10000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, sgpr<20>),
    INSN(0x4C10000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, cbuf),
    INSN(0x3810000000000000ULL, 0, "IADD", po<48, 49>, sat<50>, x<43>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, imm, post_neg<48>),
    INSN(0x1C00000000000000ULL, 0, "IADD32I", po<55, 56>, sat<54>, x<53>, dgpr<0>, cc<52>, comma, neg<56>, sgpr<8>, comma, simm32),
    INSN(0x5CC0000000000000ULL, 0, "IADD3", iadd3::shift, x<48>, dgpr<0>, cc, comma, neg<51>, sgpr<8>, iadd3::half<35>, comma, neg<50>, sgpr<20>, iadd3::half<33>,   comma, neg<49>, sgpr<39>, iadd3::half<31>),
    INSN(0x4CC0000000000000ULL, 0, "IADD3",               x<48>, dgpr<0>, cc, comma, neg<51>, sgpr<8>,                  comma, neg<50>, cbuf,                        comma, neg<49>, sgpr<39>),
    INSN(0x38C0000000000000ULL, 0, "IADD3",               x<48>, dgpr<0>, cc, comma, neg<51>, sgpr<8>,                  comma,          uimm_extended, post_neg<50>, comma, neg<49>, sgpr<39>),
    INSN(0x5B40000000000000ULL, 0, "ICMP", integer_compare<49>, int_sign, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>),
    INSN(0x5340000000000000ULL, 0, "ICMP", integer_compare<49>, int_sign, dgpr<0>, comma, sgpr<8>, comma, sgpr<39>, comma, cbuf),
    INSN(0x5340000000000000ULL, 0, "ICMP", integer_compare<49>, int_sign, dgpr<0>, comma, sgpr<8>, comma, cbuf,     comma, sgpr<39>),
    INSN(0x3640000000000000ULL, 0, "ICMP", integer_compare<49>, int_sign, dgpr<0>, comma, sgpr<8>, comma, imm,      comma, sgpr<39>),
    INSN(0x5C20000000000000ULL, 0, "IMNMX", int_sign, imnmx::mode, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>, comma, pred<39, true>),
    INSN(0x4C20000000000000ULL, 0, "IMNMX", int_sign, imnmx::mode, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf,     comma, pred<39, true>),
    INSN(0x3820000000000000ULL, 0, "IMNMX", int_sign, imnmx::mode, dgpr<0>, cc, comma, sgpr<8>, comma, imm,      comma, pred<39, true>),
    INSN(0x5C18000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, sgpr<20>, comma, uinteger<5, 39>),
    INSN(0x4C18000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, cbuf,     comma, uinteger<5, 39>),
    INSN(0x3818000000000000ULL, 0, "ISCADD", po<48, 49>, dgpr<0>, cc, comma, neg<49>, sgpr<8>, comma, neg<48>, imm,      comma, uinteger<5, 39>),
    INSN(0x5B50000000000000ULL, 0, "ISET", bf<44>, integer_compare<49>, int_sign, x<43>, bop<45>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>, comma, pred<39, true>),
    INSN(0x4B50000000000000ULL, 0, "ISET", bf<44>, integer_compare<49>, int_sign, x<43>, bop<45>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf,     comma, pred<39, true>),
    INSN(0x3650000000000000ULL, 0, "ISET", bf<44>, integer_compare<49>, int_sign, x<43>, bop<45>, dgpr<0>, cc, comma, sgpr<8>, comma, imm,      comma, pred<39, true>),
    INSN(0x5B60000000000000ULL, 0, "ISETP", integer_compare<49>, int_sign, x<43>, bop<45>, pred<3>, comma, pred<0>, comma, sgpr<8>, comma, sgpr<20>, comma, pred<39, true>),
    INSN(0x4B60000000000000ULL, 0, "ISETP", integer_compare<49>, int_sign, x<43>, bop<45>, pred<3>, comma, pred<0>, comma, sgpr<8>, comma, cbuf, comma, pred<39, true>),
    INSN(0x3660000000000000ULL, 0, "ISETP", integer_compare<49>, int_sign, x<43>, bop<45>, pred<3>, comma, pred<0>, comma, sgpr<8>, comma, imm, comma, pred<39, true>),
    INSN(0x5C30000000000000ULL, 0, "FLO", int_sign, sh<41>, dgpr<0>, cc, comma, tilde<40>, sgpr<20>),
    INSN(0x4C30000000000000ULL, 0, "FLO", int_sign, sh<41>, dgpr<0>, cc, comma, tilde<40>, cbuf),
    INSN(0x3830000000000000ULL, 0, "FLO", int_sign, sh<41>, dgpr<0>, cc, comma, imm, inv<40>),
    INSN(0x5C08000000000000ULL, 0, "POPC", dgpr<0>, comma, tilde<40>, sgpr<20>),
    INSN(0x4C08000000000000ULL, 0, "POPC", dgpr<0>, comma, tilde<40>, cbuf),
    INSN(0x3808000000000000ULL, 0, "POPC", dgpr<0>, comma, imm, inv<40>),
    INSN(0xEF90000000000000ULL, 0, "LDC", ldc::size, ldc::mode, dgpr<0>, comma, ldc::address),
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
    INSN(0x5C40000000000000ULL, 0, "LOP", lop::bit_op<41>, x<43>, lop::pred_op, pred<48>, comma, dgpr<0>, cc, comma, tilde<39>, sgpr<8>, comma, tilde<40>, sgpr<20>),
    INSN(0x4C40000000000000ULL, 0, "LOP", lop::bit_op<41>, x<43>, lop::pred_op, pred<48>, comma, dgpr<0>, cc, comma, tilde<39>, sgpr<8>, comma, tilde<40>, cbuf),
    INSN(0x3840000000000000ULL, 0, "LOP", lop::bit_op<41>, x<43>, lop::pred_op, pred<48>, comma, dgpr<0>, cc, comma, tilde<39>, sgpr<8>, comma, tilde<40>, imm),
    INSN(0x0400000000000000ULL, 0, "LOP32I", lop::bit_op<53>, x<57>, dgpr<0>, cc<52>, comma, tilde<55>, sgpr<8>, comma, uimm32, inv<56>),
    INSN(0x5BE0000000000000ULL, 0, "LOP3", lop3::lut, x<38>, lop3::operation, pred<48>, comma, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>, comma, uinteger<8, 28>),
    INSN(0x5BE7000000000000ULL, 0, "LOP3", lop3::lut, x<38>, lop3::operation,                  dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>, comma, uinteger<8, 28>),
    INSN(0x3C00000000000000ULL, 0, "LOP3", lop3::lut,                                          dgpr<0>, cc, comma, sgpr<8>, comma, imm, comma, sgpr<39>, comma, uinteger<8, 48>),
    INSN(0x0200000000000000ULL, 0, "LOP3", lop3::lut,                                          dgpr<0>, cc, comma, sgpr<8>, comma, cbuf, comma, sgpr<39>, comma, uinteger<8, 48>),
    INSN(0x5BA0000000000000ULL, 0, "FCMP", float_compare<48>, ftz<47>, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, sgpr<39>),
    INSN(0x53A0000000000000ULL, 0, "FCMP", float_compare<48>, ftz<47>, dgpr<0>, comma, sgpr<8>, comma, sgpr<39>, comma,     cbuf),
    INSN(0x4BA0000000000000ULL, 0, "FCMP", float_compare<48>, ftz<47>, dgpr<0>, comma, sgpr<8>, comma,     cbuf, comma, sgpr<39>),
    INSN(0x36A0000000000000ULL, 0, "FCMP", float_compare<48>, ftz<47>, dgpr<0>, comma, sgpr<8>, comma,     fimm, comma, sgpr<39>),
    INSN(0x5CA0000000000000ULL, 0, "SEL", dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, pred<39, true>),
    INSN(0x4CA0000000000000ULL, 0, "SEL", dgpr<0>, comma, sgpr<8>, comma, cbuf,     comma, pred<39, true>),
    INSN(0x38A0000000000000ULL, 0, "SEL", dgpr<0>, comma, sgpr<8>, comma, imm ,     comma, pred<39, true>),
    INSN(0xEF10000000000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma,        sgpr<20>, comma, sgpr<39>),
    INSN(0xEF10000010000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, uinteger<5, 20>, comma, sgpr<39>),
    INSN(0xEF10000020000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma,        sgpr<20>, comma, uinteger<13, 34>),
    INSN(0xEF10000030000000ULL, 0, "SHFL", shfl::mode, pred<48>, comma, dgpr<0>, comma, sgpr<8>, comma, uinteger<5, 20>, comma, uinteger<13, 34>),
    INSN(0x5C48000000000000ULL, 0, "SHL", w<39>, x<43>, dgpr<0>, cc, comma, sgpr<8>, comma, sgpr<20>),
    INSN(0x4C48000000000000ULL, 0, "SHL", w<39>, x<43>, dgpr<0>, cc, comma, sgpr<8>, comma, cbuf),
    INSN(0x3848000000000000ULL, 0, "SHL", w<39>, x<43>, dgpr<0>, cc, comma, sgpr<8>, comma, imm),
    INSN(0xF0F8000000000000ULL, 0, "SYNC", flow_tests),
    INSN(0xE350000000000000ULL, 0, "CONT", flow_tests),
    INSN(0xE340000000000000ULL, 0, "BRK", flow_tests),
    INSN(0xE330000000000000ULL, 0, "KIL", flow_tests),
    INSN(0xE320000000000000ULL, 0, "RET", flow_tests),
    INSN(0xE310000000000000ULL, 0, "LONGJMP", flow_tests),
    INSN(0xE300000000000000ULL, 0, "EXIT", keeprefcount, flow_tests),
    INSN(0x5C90000000000000ULL, 0, "RRO", rro::mode, dgpr<0>, comma, neg<45>, abs<49, sgpr<20>>),
    INSN(0x4C90000000000000ULL, 0, "RRO", rro::mode, dgpr<0>, comma, neg<45>, abs<49, cbuf>),
    INSN(0x3890000000000000ULL, 0, "RRO", rro::mode, dgpr<0>, comma, fimm, post_neg<45>, post_abs<49>),
    INSN(0xEB20000000000000ULL, 0, "SUST", image::p, image::type, store_cache<24>, image::rgba, image::clamp, memory::address<false>, comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xEB28000000000000ULL, 0, "SUST", image::p, image::type, store_cache<24>, image::rgba, image::clamp, memory::address<false>, comma, sgpr<0>, comma, uinteger<13, 36>),
    INSN(0xEB20000000000000ULL, 0, "SUST", image::d, image::ba<52>, image::type, store_cache<24>, image::size, image::clamp, memory::address<false>, comma, sgpr<0>, comma, sgpr<39>),
    INSN(0xEB28000000000000ULL, 0, "SUST", image::d, image::ba<52>, image::type, store_cache<24>, image::size, image::clamp, memory::address<false>, comma, sgpr<0>, comma, uinteger<13, 36>),
    INSN(0xEB00000000000000ULL, 0, "SULD", image::p, image::type, load_cache<24>, image::rgba, image::clamp, dgpr<0>, comma, memory::address<false>, comma, sgpr<39>),
    INSN(0xEB08000000000000ULL, 0, "SULD", image::p, image::type, load_cache<24>, image::rgba, image::clamp, dgpr<0>, comma, memory::address<false>, comma, uinteger<13, 36>),
    INSN(0xEB00000000000000ULL, 0, "SULD", image::d, image::ba<23>, image::type, load_cache<24>, image::size, image::clamp, dgpr<0>, comma, memory::address<false>, comma, sgpr<39>),
    INSN(0xEB08000000000000ULL, 0, "SULD", image::d, image::ba<23>, image::type, load_cache<24>, image::size, image::clamp, dgpr<0>, comma, memory::address<false>, comma, uinteger<13, 36>),
    INSN(0x50D8000000000000ULL, 0, "VOTE", vote::operation, dgpr<0>, comma, pred<45, false>, comma, pred<39, true>),
    INSN(0x5F04000000000000ULL, 0, "VMAD", video::src_format<37, 48>, video::src_format<29, 49>, po<53, 54>, vmad::scale, sat<55>, dgpr<0>, cc, comma, neg<54>, sgpr<8>, video::selector<36, 37>, comma, sgpr<20>, video::selector<28, 29>, comma, neg<53>, sgpr<39>),
    INSN(0x5F00000000000000ULL, 0, "VMAD", video::src_format<37, 48>, video::imm_format<29, 49>, po<53, 54>, vmad::scale, sat<55>, dgpr<0>, cc, comma, neg<54>, sgpr<8>, video::selector<36, 37>, comma, uinteger<16, 20>,                  comma, neg<53>, sgpr<39>),
    INSN(0x3A04000000000000ULL, 0, "VMNMX", video::dest_sign, video::src_format<37, 48>, video::src_format<29, 49>, video::mx<56>, sat<55>, video::vmnmx_op<51>, dgpr<0>, cc, comma, sgpr<8>, video::selector<36, 37>, comma, sgpr<20>, video::selector<28, 29>, comma, sgpr<39>),
    INSN(0x3A00000000000000ULL, 0, "VMNMX", video::dest_sign, video::src_format<37, 48>, video::imm_format<29, 49>, video::mx<56>, sat<55>, video::vmnmx_op<51>, dgpr<0>, cc, comma, sgpr<8>, video::selector<36, 37>, comma, uinteger<16, 20>,                  comma, sgpr<39>),
    INSN(0x50F4000000000000ULL, 0, "VSETP", vsetp::integer_compare<43>, video::src_format<37, 48>, video::src_format<29, 49>, bop<45>, pred<3>, comma, pred<0>, comma, sgpr<8>, video::selector<36, 37>, comma, sgpr<20>, video::selector<28, 29>, comma, pred<39, true>),
    INSN(0x50F0000000000000ULL, 0, "VSETP", vsetp::integer_compare<43>, video::src_format<37, 48>, video::imm_format<29, 49>, bop<45>, pred<3>, comma, pred<0>, comma, sgpr<8>, video::selector<36, 37>, comma, uinteger<16, 20>,                  comma, pred<39, true>),
    INSN(0xEED8000000000000ULL, RD, "STG", stg::e, store_cache<46>, memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEF58000000000000ULL, RD, "STS", memory::size, memory::address, comma, sgpr<0>),
    INSN(0xEF50000000000000ULL, RD, "STL", stl::cache, memory::size, memory::address, comma, sgpr<0>),
    INSN(0xC000000000000000ULL, 0, "TEX",         ndv<35>, nodep<49>, aoffi<54>, blod<55>, dc<50>, lc<58>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uinteger<13, 36>, comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDE80000000000000ULL, 0, "TEX", b_text, ndv<35>, nodep<49>, aoffi<36>, blod<37>, dc<50>, lc<40>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, zero,             comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xD000000000000000ULL, 0, "TEXS", sample_size<59>, texs_mode<53>, nodep<49>, dgpr<28>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uinteger<13, 36>, comma, texs_type<53>, comma, texs_swizzle),
    INSN(0xC800000000000000ULL, 0, "TLD4", tld4::component<56>,         tld4::offset<54>, dc<50>, ndv<35>, nodep<49>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uinteger<13, 36>, comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDEC0000000000000ULL, 0, "TLD4", tld4::component<38>, b_text, tld4::offset<36>, dc<50>, ndv<35>, nodep<49>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, zero,             comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDF00000000000000ULL, 0, "TLD4S", sample_size_inv<55>, tld4::component<52>, aoffi<51>, dc<50>, nodep<49>, dgpr<28>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uinteger<13, 36>),
    INSN(0xDC00000000000000ULL, 0, "TLD",         tld::lod<55>, aoffi<35>, ms<50>, cl<54>, nodep<49>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uinteger<13, 36>, comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDD00000000000000ULL, 0, "TLD", b_text, tld::lod<55>, aoffi<35>, ms<50>, cl<54>, nodep<49>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, zero,             comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xD200000000000000ULL, 0, "TLDS", sample_size<59>, tld::lod<53>, aoffi<54>, ms<55>, nodep<49>, dgpr<28>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uinteger<13, 36>, comma, tlds_type<53>, comma, texs_swizzle),
    INSN(0xDF58000000000000ULL, 0, "TMML",         lod_text, ndv<35>, nodep<49>, dgpr<0>, comma, sgpr<8>, comma,      uinteger<13, 36>, comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDF60000000000000ULL, 0, "TMML", b_text, lod_text, ndv<35>, nodep<49>, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, zero, comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDE00000000000000ULL, 0, "TXD",         lc<50>, aoffi<35>, nodep<49>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma, uinteger<13, 36>, comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDE40000000000000ULL, 0, "TXD", b_text, lc<50>, aoffi<35>, nodep<49>, pred<51>, comma, dgpr<0>, comma, sgpr<8>, comma, sgpr<20>, comma,             zero, comma, tex_type<28>, comma, uinteger<4, 31>),
    INSN(0xDF48000000000000ULL, 0, "TXQ",         nodep<49>, dgpr<0>, comma, sgpr<8>, comma, txq::mode, comma, uinteger<13, 36>, comma, uinteger<4, 31>),
    INSN(0xDF50000000000000ULL, 0, "TXQ", b_text, nodep<49>, dgpr<0>, comma, sgpr<8>, comma, txq::mode, comma,             zero, comma, uinteger<4, 31>),
    INSN(0xEED0000000000000ULL, RD|WR, "LDG", stg::e, ldg::cache, ldg::size, dgpr<0>, comma, memory::address),
    INSN(0xEEC8000000000000ULL, RD|WR, "LDG", stg::e, ldg::cache, ldg::size, inverted_pred<41>, comma, dgpr<0>, comma, memory::address<true, 20, 20>),
    INSN(0xEF48000000000000ULL, RD|WR, "LDS", lds::u, memory::size, dgpr<0>, comma, memory::address),
    INSN(0xEF40000000000000ULL, RD|WR, "LDL", ldl::cache, memory::size, dgpr<0>, comma, memory::address),
    INSN(0xEBF8000000000000ULL, RD|WR, "RED", red::e, red::operation, atomic_size<20>, memory::address<true, 28, 20, 0>, comma, sgpr<0>),
    INSN(0xE240000000000000ULL, 0, "BRA", u<7>, lmt<6>, flow_tests, comma, label),
    INSN(0xE24000000000000FULL, 0, "BRA", u<7>, lmt<6>, label),
    INSN(0xE2A0000000000000ULL, NO_PRED, "PBK", label),
    // TODO: PBK constant buffer
    INSN(0xE290000000000000ULL, NO_PRED, "SSY", label),
    // TODO: SSY constant buffer
    INSN(0xE260000000000000ULL, NO_PRED, "CAL", noinc, label),
    // TODO: CAL constant buffer
    INSN(0xE2B0000000000000ULL, NO_PRED, "PCNT", label),
};
// clang-format on
