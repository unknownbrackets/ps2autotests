#ifndef PS2AUTOTESTS_VU_ASSEMBLE_TYPES_H
#define PS2AUTOTESTS_VU_ASSEMBLE_TYPES_H

#include <tamtypes.h>

namespace VU {

struct LowerOp {
	explicit LowerOp() {
		// This is an IADD vi00, vi00, vi00 (a NOP.)
		v = 0x80000030;
	}
	explicit LowerOp(u32 imm) {
		v = imm;
	}

	bool operator == (const LowerOp &rhs) {
		return v == rhs.v;
	}
	bool operator != (const LowerOp &rhs) {
		return v != rhs.v;
	}

	u32 v;
};

struct UpperOp {
	explicit UpperOp() {
		// This is a NOP.
		v = 0x000002FF;
	}
	explicit UpperOp(u32 imm) {
		v = imm;
	}

	bool operator == (const UpperOp &rhs) {
		return v == rhs.v;
	}
	bool operator != (const UpperOp &rhs) {
		return v != rhs.v;
	}

	u32 v;
};

typedef u64 LIW;

enum Flags {
	UPPER_NONE = 0,

	// Integer in lower.
	UPPER_I = 1 << 31,
	// End.
	UPPER_E = 1 << 30,
	// Interlock?
	UPPER_M = 1 << 29,
	// Debug.
	UPPER_D = 1 << 28,
	// Trap.
	UPPER_T = 1 << 27,
};

static inline Flags operator | (const Flags &lhs, const Flags &rhs) {
	return Flags((u32)lhs | (u32)rhs);
}

enum Dest {
	DEST_X = 1 << 3,
	DEST_Y = 1 << 2,
	DEST_Z = 1 << 1,
	DEST_W = 1 << 0,

	DEST_NONE = 0,
	DEST_XY = DEST_X | DEST_Y,
	DEST_XYZ = DEST_XY | DEST_Z,
	DEST_XYZW = DEST_XYZ | DEST_W,
};

static inline Dest operator | (const Dest &lhs, const Dest &rhs) {
	return Dest((u32)lhs | (u32)rhs);
}

enum Field {
	FIELD_X = 0,
	FIELD_Y = 1,
	FIELD_Z = 2,
	FIELD_W = 3,
};

enum Reg {
	VF00, VF01, VF02, VF03, VF04, VF05, VF06, VF07,
	VF08, VF09, VF10, VF11, VF12, VF13, VF14, VF15,
	VF16, VF17, VF18, VF19, VF20, VF21, VF22, VF23,
	VF24, VF25, VF26, VF27, VF28, VF29, VF30, VF31,

	// Note that these are offset by 32.
	VI00, VI01, VI02, VI03, VI04, VI05, VI06, VI07,
	VI08, VI09, VI10, VI11, VI12, VI13, VI14, VI15,

	VF_CONST = VF00,

	VI_ZERO = VI00,
	VI_SP = VI14,
	VI_LR = VI15,
};

enum BranchType {
	BRANCH_B,
	BRANCH_BAL,
	BRANCH_IBEQ,
	BRANCH_IBGEZ,
	BRANCH_IBGTZ,
	BRANCH_IBLEZ,
	BRANCH_IBLTZ,
	BRANCH_IBNE,
};

class Block;

struct Label {
protected:
	Label() : dest_(NULL), src_(NULL), upper_() {
	}

	void Setup(LIW *src, BranchType type, UpperOp u, Reg s = VF00, Reg t = VF00) {
		src_ = src;
		upper_ = u;
		type_ = type;
		s_ = s;
		t_ = t;
	}

	LIW *dest_;

	// For fixup.
	LIW *src_;
	UpperOp upper_;
	BranchType type_;
	Reg s_;
	Reg t_;

	friend class Block;
};

}

#endif
