#include <stdio.h>
#include "block.h"
#include "ops.h"
#include "types.h"

namespace VU {

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

void Block::SafeExit() {
	Wr(NOP(UPPER_E));
	Wr(NOP());
}

}
