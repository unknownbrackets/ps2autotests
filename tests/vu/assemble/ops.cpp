#include <stdio.h>
#include "encoding.h"
#include "ops.h"
#include "types.h"

namespace VU {

enum UpperType0Opcode {
	OP_ADDbc =   0x0, // 0b0000
	OP_SUBbc =   0x1, // 0b0001
	OP_MADDbc =  0x2, // 0b0010
	OP_MSUBbc =  0x3, // 0b0011
	OP_MAXbc =   0x4, // 0b0100
	OP_MINIbc =  0x5, // 0b0101
	OP_MULbc =   0x6, // 0b0110
};

enum UpperType1Opcode {
	OP_MULq =    0x1C, // 0b011100
	OP_MAXi =    0x1D, // 0b011101
	OP_MULi =    0x1E, // 0b011110
	OP_MINIi =   0x1F, // 0b011111
	OP_ADDq =    0x20, // 0b100000
	OP_MADDq =   0x21, // 0b100001
	OP_ADDi =    0x22, // 0b100010
	OP_MADDi =   0x23, // 0b100011
	OP_SUBq =    0x24, // 0b100100
	OP_MSUBq =   0x25, // 0b100101
	OP_SUBi =    0x26, // 0b100110
	OP_MSUBi =   0x27, // 0b100111
	OP_ADD =     0x28, // 0b101000
	OP_MADD =    0x29, // 0b101001
	OP_MUL =     0x2A, // 0b101010
	OP_MAX =     0x2B, // 0b101011
	OP_SUB =     0x2C, // 0b101100
	OP_MSUB =    0x2D, // 0b101101
	OP_OPMSUB =  0x2E, // 0b101110
	OP_MINI =    0x2F, // 0b101111
};

enum UpperType2Opcode {
	OP_ADDAbc =  0x0F, // 0b000001111
	OP_SUBAbc =  0x1F, // 0b000011111
	OP_MADDAbc = 0x2F, // 0b000101111
	OP_MSUBAbc = 0x3F, // 0b000111111
	OP_MULAbc =  0x6F, // 0b001101111
};

enum UpperType3Opcode {
	OP_ITOF0 =   0x13C, // 0b00100111100
	OP_ITOF4 =   0x13D, // 0b00100111101
	OP_ITOF12 =  0x13E, // 0b00100111110
	OP_ITOF15 =  0x13F, // 0b00100111111
	OP_FTOI0 =   0x17C, // 0b00101111100
	OP_FTOI4 =   0x17D, // 0b00101111101
	OP_FTOI12 =  0x17E, // 0b00101111110
	OP_FTOI15 =  0x17F, // 0b00101111111
	OP_MULAq =   0x1FC, // 0b00111111100
	OP_ABS =     0x1FD, // 0b00111111101
	OP_MULAi =   0x1FE, // 0b00111111110
	OP_CLIP =    0x1FF, // 0b00111111111
	OP_ADDAq =   0x23C, // 0b01000111100
	OP_MADDAq =  0x23D, // 0b01000111101
	OP_SUBAq =   0x27C, // 0b01001111100
	OP_MSUBAq =  0x27D, // 0b01001111101
	OP_ADDAi =   0x23E, // 0b01000111110
	OP_MADDAi =  0x23F, // 0b01000111111
	OP_SUBAi =   0x27E, // 0b01001111110
	OP_MSUBAi =  0x27F, // 0b01001111111
	OP_ADDA =    0x2BC, // 0b01010111100
	OP_MADDA =   0x2BD, // 0b01010111101
	OP_MULA =    0x2BE, // 0b01010111110
	OP_SUBA =    0x2FC, // 0b01011111100
	OP_MSUBA =   0x2FD, // 0b01011111101
	OP_OPMULA =  0x2FE, // 0b01011111110
	OP_NOP =     0x2FF, // 0b01011111111
};

enum LowerType1Opcode {
	OP_IADD =    0x30, // 0b110000
	OP_ISUB =    0x31, // 0b110001
	OP_IAND =    0x34, // 0b110100
	OP_IOR =     0x35, // 0b110101
};

enum LowerType3Opcode {
};

enum LowerType4Opcode {
};

enum LowerType5Opcode {
	OP_IADDI =   0x32, // 0b110010
};

enum LowerType7Opcode {
};

enum LowerType8Opcode {
	OP_IADDIU =  0x08, // 0b0001000
	OP_ISUBIU =  0x09, // 0b0001001
};

enum LowerType9Opcode {
};

// Helpers for each of the encoding types.

UpperOp UpperType0(UpperType0Opcode op, Dest dest, Field bc, Reg fd, Reg fs, Reg ft, Flags f) {
	return UpperOp(f | DEST(dest) | VT(ft) | VS(fs) | VD(fd) | (op << 2) | BC(bc));
}

UpperOp UpperType1(UpperType1Opcode op, Dest dest, Reg fd, Reg fs, Reg ft, Flags f) {
	return UpperOp(f | DEST(dest) | VT(ft) | VS(fs) | VD(fd) | op);
}

UpperOp UpperType2(UpperType2Opcode op, Dest dest, Field bc, Reg fs, Reg ft, Flags f) {
	return UpperOp(f | DEST(dest) | VT(ft) | VS(fs) | (op << 2) | BC(bc));
}

UpperOp UpperType3(UpperType3Opcode op, Dest dest, Reg fs, Reg ft, Flags f) {
	return UpperOp(f | DEST(dest) | VT(ft) | VS(fs) | op);
}

static const u32 LOWER_TYPE1345 = 0x80000000;

LowerOp LowerType1(LowerType1Opcode op, Dest dest, Reg fd, Reg fs, Reg ft) {
	return LowerOp(LOWER_TYPE1345 | DEST(dest) | VT(ft) | VS(fs) | VD(fd) | op);
}

LowerOp LowerType3(LowerType3Opcode op, Dest dest, Reg fs, Reg ft) {
	return LowerOp(LOWER_TYPE1345 | DEST(dest) | VT(ft) | VS(fs) | op);
}

LowerOp LowerType4(LowerType4Opcode op, Field fsf, Reg fs, Field ftf, Reg ft) {
	return LowerOp(LOWER_TYPE1345 | FTF(ftf) | FSF(fsf) | VT(ft) | VS(fs) | op);
}

LowerOp LowerType5(LowerType5Opcode op, Reg fs, Reg ft, u8 imm5) {
	return LowerOp(LOWER_TYPE1345 | DEST(DEST_NONE) | VT(ft) | VS(fs) | IMM5(imm5) | op);
}

LowerOp LowerType7(LowerType7Opcode op, Dest dest, Reg fs, Reg ft, u16 imm11) {
	return LowerOp((op << 25) | DEST(dest) | VT(ft) | VS(fs) | IMM11(imm11));
}

LowerOp LowerType8(LowerType8Opcode op, Reg fs, Reg ft, u16 imm15) {
	return LowerOp((op << 25) | VT(ft) | VS(fs) | IMM15(imm15));
}

LowerOp LowerType9(LowerType9Opcode op, u32 imm24) {
	return LowerOp((op << 25) | IMM24(imm24));
}

// And now the upper instructions.

UpperOp ABS(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_ABS, dest, s, t, f);
}

UpperOp ADD(Dest dest, Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_ADD, dest, d, s, t, f);
}

UpperOp ADDbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f) {
	return UpperType0(OP_ADDbc, dest, bc, d, s, t, f);
}

UpperOp ADDi(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_ADDi, dest, d, s, VF00, f);
}

UpperOp ADDq(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_ADDq, dest, d, s, VF00, f);
}

UpperOp ADDA(Dest dest, Reg s, Reg t, Flags f) {
	return UpperType3(OP_ADDA, dest, s, t, f);
}

UpperOp ADDAbc(Dest dest, Field bc, Reg s, Reg t, Flags f) {
	return UpperType2(OP_ADDAbc, dest, bc, s, t, f);
}

UpperOp ADDAi(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_ADDAi, dest, s, VF00, f);
}

UpperOp ADDAq(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_ADDAq, dest, s, VF00, f);
}

UpperOp CLIP(Reg s, Reg t, Flags f) {
	return UpperType3(OP_CLIP, DEST_XYZ, s, t, f);
}

UpperOp FTOI0(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_FTOI0, dest, s, t, f);
}

UpperOp FTOI4(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_FTOI4, dest, s, t, f);
}

UpperOp FTOI12(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_FTOI12, dest, s, t, f);
}

UpperOp FTOI15(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_FTOI15, dest, s, t, f);
}

UpperOp ITOF0(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_ITOF0, dest, s, t, f);
}

UpperOp ITOF4(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_ITOF4, dest, s, t, f);
}

UpperOp ITOF12(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_ITOF12, dest, s, t, f);
}

UpperOp ITOF15(Dest dest, Reg t, Reg s, Flags f) {
	return UpperType3(OP_ITOF15, dest, s, t, f);
}

UpperOp MADD(Dest dest, Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_MADD, dest, d, s, t, f);
}

UpperOp MADDbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f) {
	return UpperType0(OP_MADDbc, dest, bc, d, s, t, f);
}

UpperOp MADDi(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MADDi, dest, d, s, VF00, f);
}

UpperOp MADDq(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MADDq, dest, d, s, VF00, f);
}

UpperOp MADDA(Dest dest, Reg s, Reg t, Flags f) {
	return UpperType3(OP_MADDA, dest, s, t, f);
}

UpperOp MADDAbc(Dest dest, Field bc, Reg s, Reg t, Flags f) {
	return UpperType2(OP_MADDAbc, dest, bc, s, t, f);
}

UpperOp MADDAi(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_MADDAi, dest, s, VF00, f);
}

UpperOp MADDAq(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_MADDAq, dest, s, VF00, f);
}

UpperOp MAX(Dest dest, Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_MAX, dest, d, s, t, f);
}

UpperOp MAXbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f) {
	return UpperType0(OP_MAXbc, dest, bc, d, s, t, f);
}

UpperOp MAXi(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MAXi, dest, d, s, VF00, f);
}

UpperOp MINI(Dest dest, Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_MINI, dest, d, s, t, f);
}

UpperOp MINIbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f) {
	return UpperType0(OP_MINIbc, dest, bc, d, s, t, f);
}

UpperOp MINIi(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MINIi, dest, d, s, VF00, f);
}

UpperOp MSUB(Dest dest, Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_MSUB, dest, d, s, t, f);
}

UpperOp MSUBbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f) {
	return UpperType0(OP_MSUBbc, dest, bc, d, s, t, f);
}

UpperOp MSUBi(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MSUBi, dest, d, s, VF00, f);
}

UpperOp MSUBq(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MSUBq, dest, d, s, VF00, f);
}

UpperOp MSUBA(Dest dest, Reg s, Reg t, Flags f) {
	return UpperType3(OP_MSUBA, dest, s, t, f);
}

UpperOp MSUBAbc(Dest dest, Field bc, Reg s, Reg t, Flags f) {
	return UpperType2(OP_MSUBAbc, dest, bc, s, t, f);
}

UpperOp MSUBAi(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_MSUBAi, dest, s, VF00, f);
}

UpperOp MSUBAq(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_MSUBAq, dest, s, VF00, f);
}

UpperOp MUL(Dest dest, Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_MUL, dest, d, s, t, f);
}

UpperOp MULbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f) {
	return UpperType0(OP_MULbc, dest, bc, d, s, t, f);
}

UpperOp MULi(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MULi, dest, d, s, VF00, f);
}

UpperOp MULq(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_MULq, dest, d, s, VF00, f);
}

UpperOp MULA(Dest dest, Reg s, Reg t, Flags f) {
	return UpperType3(OP_MULA, dest, s, t, f);
}

UpperOp MULAbc(Dest dest, Field bc, Reg s, Reg t, Flags f) {
	return UpperType2(OP_MULAbc, dest, bc, s, t, f);
}

UpperOp MULAi(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_MULAi, dest, s, VF00, f);
}

UpperOp MULAq(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_MULAq, dest, s, VF00, f);
}

UpperOp NOP(Flags f) {
	return UpperType3(OP_NOP, DEST_NONE, VF00, VF00, f);
}

UpperOp OPMULA(Reg s, Reg t, Flags f) {
	return UpperType3(OP_OPMULA, DEST_XYZ, s, t, f);
}

UpperOp OPMSUB(Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_OPMSUB, DEST_XYZ, d, s, t, f);
}

UpperOp SUB(Dest dest, Reg d, Reg s, Reg t, Flags f) {
	return UpperType1(OP_SUB, dest, d, s, t, f);
}

UpperOp SUBbc(Dest dest, Field bc, Reg d, Reg s, Reg t, Flags f) {
	return UpperType0(OP_SUBbc, dest, bc, d, s, t, f);
}

UpperOp SUBi(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_SUBi, dest, d, s, VF00, f);
}

UpperOp SUBq(Dest dest, Reg d, Reg s, Flags f) {
	return UpperType1(OP_SUBq, dest, d, s, VF00, f);
}

UpperOp SUBA(Dest dest, Reg s, Reg t, Flags f) {
	return UpperType3(OP_SUBA, dest, s, t, f);
}

UpperOp SUBAbc(Dest dest, Field bc, Reg s, Reg t, Flags f) {
	return UpperType2(OP_SUBAbc, dest, bc, s, t, f);
}

UpperOp SUBAi(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_SUBAi, dest, s, VF00, f);
}

UpperOp SUBAq(Dest dest, Reg s, Flags f) {
	return UpperType3(OP_SUBAq, dest, s, VF00, f);
}


// And now the lower instructions.

LowerOp LowerImm(float imm) {
	union {
		float f;
		u32 u;
	} v;
	v.f = imm;
	return LowerImm(v.u);
}

LowerOp LowerImm(u32 imm) {
	return LowerOp(imm);
}

LowerOp LowerImm(s32 imm) {
	return LowerImm((u32)imm);
}

LowerOp IADD(Reg d, Reg s, Reg t) {
	return LowerType1(OP_IADD, DEST_NONE, VI(d), VI(s), VI(t));
}

LowerOp IADDI(Reg t, Reg s, u8 imm5) {
	return LowerType5(OP_IADDI, VI(s), VI(t), imm5);
}

LowerOp IADDIU(Reg t, Reg s, u16 imm15) {
	return LowerType8(OP_IADDIU, VI(s), VI(t), imm15);
}

LowerOp IAND(Reg d, Reg s, Reg t) {
	return LowerType1(OP_IAND, DEST_NONE, VI(d), VI(s), VI(t));
}

LowerOp IOR(Reg d, Reg s, Reg t) {
	return LowerType1(OP_IOR, DEST_NONE, VI(d), VI(s), VI(t));
}

LowerOp ISUB(Reg d, Reg s, Reg t) {
	return LowerType1(OP_ISUB, DEST_NONE, VI(d), VI(s), VI(t));
}

LowerOp ISUBIU(Reg t, Reg s, u16 imm15) {
	return LowerType8(OP_ISUBIU, VI(s), VI(t), imm15);
}

}
