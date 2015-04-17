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

LowerOp IADD(Reg d, Reg s, Reg t);
LowerOp IADDI(Reg t, Reg s, s8 imm5);
LowerOp IADDIU(Reg t, Reg s, u16 imm15);
LowerOp IAND(Reg d, Reg s, Reg t);
LowerOp IOR(Reg d, Reg s, Reg t);
LowerOp ISUB(Reg d, Reg s, Reg t);
LowerOp ISUBIU(Reg t, Reg s, u16 imm15);

}

#endif
