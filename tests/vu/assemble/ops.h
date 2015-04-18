#ifndef PS2AUTOTESTS_VU_ASSEMBLE_OPS_H
#define PS2AUTOTESTS_VU_ASSEMBLE_OPS_H

#include "types.h"

namespace VU {

// Upper instructions.

UpperOp ABS(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp ADD(Dest dest, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp ADDbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp ADDi(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp ADDq(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp ADDA(Dest dest, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp ADDAbc(Dest dest, Field bc, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp ADDAi(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp ADDAq(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp CLIP(Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp FTOI0(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp FTOI4(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp FTOI12(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp FTOI15(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp ITOF0(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp ITOF4(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp ITOF12(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp ITOF15(Dest dest, Reg t, Reg s, Flags f = UPPER_NONE);
UpperOp MADD(Dest dest, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MADDbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MADDi(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MADDq(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MADDA(Dest dest, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MADDAbc(Dest dest, Field bc, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MADDAi(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp MADDAq(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp MAX(Dest dest, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MAXbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MINIi(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MINI(Dest dest, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MINIbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MAXi(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MSUB(Dest dest, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MSUBbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MSUBi(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MSUBq(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MSUBA(Dest dest, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MSUBAbc(Dest dest, Field bc, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MSUBAi(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp MSUBAq(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp MUL(Dest dest, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MULbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MULi(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MULq(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp MULA(Dest dest, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MULAbc(Dest dest, Field bc, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp MULAi(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp MULAq(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp NOP(Flags f = UPPER_NONE);
UpperOp OPMULA(Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp OPMSUB(Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp SUB(Dest dest, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp SUBbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp SUBi(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp SUBq(Dest dest, Reg d, Reg s, Flags f = UPPER_NONE);
UpperOp SUBA(Dest dest, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp SUBAbc(Dest dest, Field bc, Reg s, Reg t, Flags f = UPPER_NONE);
UpperOp SUBAi(Dest dest, Reg s, Flags f = UPPER_NONE);
UpperOp SUBAq(Dest dest, Reg s, Flags f = UPPER_NONE);

// Lower instructions.

LowerOp LowerImm(float imm);
LowerOp LowerImm(u32 imm);
LowerOp LowerImm(s32 imm);

LowerOp B(s16 imm11);
LowerOp BAL(Reg t, s16 imm11);
LowerOp DIV(Field fsf, Reg s, Field ftf, Reg t);
// Note that E* and MFP/WAITP are EFU and not available on vu0.
LowerOp EATAN(Field fsf, Reg s);
LowerOp EATANxy(Reg s);
LowerOp EATANxz(Reg s);
LowerOp EEXP(Field fsf, Reg s);
LowerOp ELENG(Reg s);
LowerOp ERCPR(Field fsf, Reg s);
LowerOp ERLENG(Reg s);
LowerOp ERSADD(Reg s);
LowerOp ERSQRT(Field fsf, Reg s);
LowerOp ESADD(Reg s);
LowerOp ESIN(Field fsf, Reg s);
LowerOp ESQRT(Field fsf, Reg s);
LowerOp ESUM(Reg s);
// Warning: fixed result to VI01 for FCAND, FCEQ, and FCOR.  Param is there only for clarity.
LowerOp FCAND(Reg d_VI01, u32 imm24);
LowerOp FCEQ(Reg d_VI01, u32 imm24);
// This one does support arbitrary parameters.
LowerOp FCGET(Reg t);
// Warning: fixed result to VI01 for FCAND, FCEQ, and FCOR.  Param is there only for clarity.
LowerOp FCOR(Reg d_VI01, u32 imm24);
LowerOp FCSET(u32 imm24);
LowerOp FMAND(Reg t, Reg s);
LowerOp FMEQ(Reg t, Reg s);
LowerOp FMOR(Reg t, Reg s);
LowerOp FSAND(Reg t, u16 imm12);
LowerOp FSEQ(Reg t, u16 imm12);
LowerOp FSOR(Reg t, u16 imm12);
LowerOp FSSET(u16 imm12);
LowerOp IADD(Reg d, Reg s, Reg t);
LowerOp IADDI(Reg t, Reg s, s8 imm5);
LowerOp IADDIU(Reg t, Reg s, u16 imm15);
LowerOp IAND(Reg d, Reg s, Reg t);
LowerOp IBEQ(Reg t, Reg s, s16 imm11);
LowerOp IBGEZ(Reg s, s16 imm11);
LowerOp IBGTZ(Reg s, s16 imm11);
LowerOp IBLEZ(Reg s, s16 imm11);
LowerOp IBLTZ(Reg s, s16 imm11);
LowerOp IBNE(Reg t, Reg s, s16 imm11);
LowerOp ILW(Dest dest, Reg t, Reg s, s16 imm11);
LowerOp ILWR(Dest dest, Reg t, Reg s);
LowerOp IOR(Reg d, Reg s, Reg t);
LowerOp ISUB(Reg d, Reg s, Reg t);
LowerOp ISUBIU(Reg t, Reg s, u16 imm15);
LowerOp ISW(Dest dest, Reg t, Reg s, s16 imm11);
LowerOp ISWR(Dest dest, Reg t, Reg s);
LowerOp JALR(Reg t, Reg s);
LowerOp JR(Reg s);
LowerOp LQ(Dest dest, Reg t, Reg si, s16 imm11);
LowerOp LQD(Dest dest, Reg t, Reg si);
LowerOp LQI(Dest dest, Reg t, Reg si);
LowerOp MFIR(Dest dest, Reg t, Reg si);
LowerOp MFP(Dest dest, Reg t);
LowerOp MOVE(Dest dest, Reg t, Reg s);
LowerOp MR32(Dest dest, Reg t, Reg s);
LowerOp MTIR(Dest dest, Reg ti, Field fsf, Reg s);
LowerOp RGET(Dest dest, Reg t);
LowerOp RINIT(Field fsf, Reg s);
LowerOp RNEXT(Dest dest, Reg t);
LowerOp RSQRT(Field fsf, Reg s, Field ftf, Reg t);
LowerOp RXOR(Field fsf, Reg s);
LowerOp SQ(Dest dest, Reg t, Reg si, s16 imm11);
LowerOp SQD(Dest dest, Reg t, Reg si);
LowerOp SQI(Dest dest, Reg t, Reg si);
LowerOp SQRT(Field ftf, Reg t);
LowerOp WAITP();
LowerOp WAITQ();
LowerOp XGKICK(Reg s);
LowerOp XITOP(Reg t);
LowerOp XTOP(Reg t);

}

#endif
