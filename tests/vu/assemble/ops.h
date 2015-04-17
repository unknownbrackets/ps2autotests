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
UpperOp NOP(Flags f = UPPER_NONE);

// Lower instructions.

LowerOp LowerImm(float imm);
LowerOp LowerImm(u32 imm);
LowerOp LowerImm(s32 imm);

LowerOp IADD(Reg d, Reg s, Reg t);
LowerOp IADDI(Reg t, Reg s, u8 imm5);
LowerOp IADDIU(Reg t, Reg s, u16 imm15);
LowerOp IAND(Reg d, Reg s, Reg t);
LowerOp IOR(Reg d, Reg s, Reg t);
LowerOp ISUB(Reg d, Reg s, Reg t);
LowerOp ISUBIU(Reg t, Reg s, u16 imm15);

}

#endif
