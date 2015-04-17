#include <stdio.h>
#include "encoding.h"
#include "ops.h"
#include "types.h"

namespace VU {

enum UpperType0Opcode {
	OP_ADDbc =   0x0, // 0b0000
};

enum UpperType1Opcode {
	OP_ADDq =    0x20, // 0b100000
	OP_ADDi =    0x22, // 0b100010
	OP_ADD =     0x28, // 0b101000
};

enum UpperType2Opcode {
	OP_ADDAbc =  0x0F, // 0b000001111
};

enum UpperType3Opcode {
	OP_ABS =     0x1FD, // 0b00111111101
	OP_ADDAq =   0x23C, // 0b01000111100
	OP_ADDAi =   0x23E, // 0b01000111110
	OP_ADDA =    0x2BC, // 0b01010111100
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

UpperOp NOP(Flags f) {
	return UpperType3(OP_NOP, DEST_NONE, VF00, VF00, f);
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
