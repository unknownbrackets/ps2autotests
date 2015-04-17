#include <stdio.h>
#include "block.h"
#include "ops.h"
#include "types.h"

namespace VU {

// TODO: <assert.h> doesn't link?
#define assert(cond) \
	if (!(cond)) { \
		printf("Assert failed: %s", #cond); \
		asm ("break 100"); \
	}

Block::Block(LIW *addr)
	: start_(addr), addr_(addr) {
}

void Block::Wr(LowerOp l) {
	Wr(NOP(), l);
}

void Block::Wr(UpperOp u) {
	Wr(u, IADD(VI00, VI00, VI00));
}

void Block::Wr(UpperOp u, LowerOp l) {
	*addr_++ = (((u64)u.v) << 32) | (u64)l.v;
}

void Block::WrImm(float imm) {
	WrImm(NOP(), imm);
}

void Block::WrImm(u32 imm) {
	WrImm(NOP(), imm);
}

void Block::WrImm(s32 imm) {
	WrImm(NOP(), imm);
}

void Block::WrImm(UpperOp u, float imm) {
	// Force upper flag.
	Wr(UpperOp(u.v | UPPER_I), LowerImm(imm));
}

void Block::WrImm(UpperOp u, u32 imm) {
	// Force upper flag.
	Wr(UpperOp(u.v | UPPER_I), LowerImm(imm));
}

void Block::WrImm(UpperOp u, s32 imm) {
	// Force upper flag.
	Wr(UpperOp(u.v | UPPER_I), LowerImm(imm));
}

Label Block::L() {
	Label l;
	l.dest_ = addr_;
	return l;
}

void Block::L(Label &l) {
	l.dest_ = addr_;
	FixupLabel(l);
}

Label Block::B(UpperOp u, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_B, u, VF00, VF00, delayu, delayl);
}

void Block::B(UpperOp u, Label l, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_B, u, l, VF00, VF00, delayu, delayl);
}

Label Block::BAL(UpperOp u, Reg t, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_BAL, u, VF00, t, delayu, delayl);
}

void Block::BAL(UpperOp u, Label l, Reg t, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_BAL, u, l, VF00, t, delayu, delayl);
}

Label Block::IBEQ(UpperOp u, Reg t, Reg s, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_IBEQ, u, s, t, delayu, delayl);
}

void Block::IBEQ(UpperOp u, Label l, Reg t, Reg s, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_IBEQ, u, l, s, t, delayu, delayl);
}

Label Block::IBGEZ(UpperOp u, Reg s, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_IBGEZ, u, s, VF00, delayu, delayl);
}

void Block::IBGEZ(UpperOp u, Label l, Reg s, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_IBGEZ, u, l, s, VF00, delayu, delayl);
}

Label Block::IBGTZ(UpperOp u, Reg s, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_IBGTZ, u, s, VF00, delayu, delayl);
}

void Block::IBGTZ(UpperOp u, Label l, Reg s, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_IBGTZ, u, l, s, VF00, delayu, delayl);
}

Label Block::IBLEZ(UpperOp u, Reg s, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_IBLEZ, u, s, VF00, delayu, delayl);
}

void Block::IBLEZ(UpperOp u, Label l, Reg s, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_IBLEZ, u, l, s, VF00, delayu, delayl);
}

Label Block::IBLTZ(UpperOp u, Reg s, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_IBLTZ, u, s, VF00, delayu, delayl);
}

void Block::IBLTZ(UpperOp u, Label l, Reg s, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_IBLTZ, u, l, s, VF00, delayu, delayl);
}

Label Block::IBNE(UpperOp u, Reg t, Reg s, UpperOp delayu, LowerOp delayl) {
	return DeferBranch(BRANCH_IBNE, u, s, t, delayu, delayl);
}

void Block::IBNE(UpperOp u, Label l, Reg t, Reg s, UpperOp delayu, LowerOp delayl) {
	return DoBranch(BRANCH_IBNE, u, l, s, t, delayu, delayl);
}

void Block::SafeExit() {
	Wr(NOP(UPPER_E));
	Wr(NOP());
}

void Block::FixupLabel(Label l) {
	assert(l.src_ != NULL);
	assert(l.dest_ != NULL);

	// The 1 accounts for the delay slot.  Note that 1 is 64 bits.
	s32 distance = l.dest_ - l.src_ - 1;
	assert(distance <= 0x03FF && distance >= -0x0400);

	LowerOp lower = EncodeFixupLower(l, (s16)distance);

	// Write the new instruction.
	*l.src_ = (((u64)l.upper_.v) << 32) | (u64)lower.v;
}

LowerOp Block::EncodeFixupLower(Label l, s16 dist) {
	switch (l.type_) {
	case BRANCH_B:
		return VU::B(dist);
	case BRANCH_BAL:
		return VU::BAL(l.t_, dist);
	case BRANCH_IBEQ:
		return VU::IBEQ(l.t_, l.s_, dist);
	case BRANCH_IBGEZ:
		return VU::IBGEZ(l.s_, dist);
	case BRANCH_IBGTZ:
		return VU::IBGTZ(l.s_, dist);
	case BRANCH_IBLEZ:
		return VU::IBLEZ(l.s_, dist);
	case BRANCH_IBLTZ:
		return VU::IBLTZ(l.s_, dist);
	case BRANCH_IBNE:
		return VU::IBNE(l.t_, l.s_, dist);
	default:
		assert(false);
		return IADD(VI00, VI00, VI00);
	}
}

Label Block::DeferBranch(BranchType type, UpperOp u, Reg s, Reg t, UpperOp delayu, LowerOp delayl) {
	Label l;
	l.Setup(addr_, type, u, s, t);
	// This will get fixed up later.
	Wr(NOP(UPPER_E));
	Wr(delayu, delayl);
	return l;
}

void Block::DoBranch(BranchType type, UpperOp u, Label l, Reg s, Reg t, UpperOp delayu, LowerOp delayl) {
	l.Setup(addr_, type, u, s, t);
	// We fix this up right away.
	Wr(NOP(UPPER_E));
	FixupLabel(l);
	Wr(delayu, delayl);
}

}
