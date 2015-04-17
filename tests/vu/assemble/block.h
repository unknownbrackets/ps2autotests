#ifndef PS2AUTOTESTS_VU_ASSEMBLE_BLOCK_H
#define PS2AUTOTESTS_VU_ASSEMBLE_BLOCK_H

#include "types.h"

namespace VU {

class Block {
public:
	Block(LIW *addr);

	void Wr(LowerOp l);
	void Wr(UpperOp u);
	void Wr(UpperOp u, LowerOp l);
	void WrImm(float imm);
	void WrImm(u32 imm);
	void WrImm(s32 imm);
	void WrImm(UpperOp u, float imm);
	void WrImm(UpperOp u, u32 imm);
	void WrImm(UpperOp u, s32 imm);

	void SafeExit();

protected:
	LIW *start_;
	LIW *addr_;
};

}

#endif
