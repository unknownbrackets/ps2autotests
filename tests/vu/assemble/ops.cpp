#include <stdio.h>
#include "encoding.h"
#include "ops.h"
#include "types.h"

namespace VU {

enum UpperType0Opcode {
};

enum UpperType1Opcode {
};

enum UpperType2Opcode {
};

enum UpperType3Opcode {
	OP_NOP =     0x17F, // 0b0101111111
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

UpperOp UpperType0(UpperType0Opcode op, Field bc, Dest dest, Reg fd, Reg fs, Reg ft, Flags f) {
	return UpperOp(f | DEST(dest) | VT(ft) | VS(fs) | VD(fd) | (op << 2) | BC(bc));
}

UpperOp UpperType1(UpperType1Opcode op, Dest dest, Reg fd, Reg fs, Reg ft, Flags f) {
	return UpperOp(f | DEST(dest) | VT(ft) | VS(fs) | VD(fd) | op);
}

UpperOp UpperType2(UpperType2Opcode op, Field bc, Dest dest, Reg fs, Reg ft, Flags f) {
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
