#ifndef PS2AUTOTESTS_VU_TEST_RUNNER_H
#define PS2AUTOTESTS_VU_TEST_RUNNER_H

#include <common.h>
#include "assemble.h"

class TestRunner : public VU::Block {
public:
	// Create a test runner.  For a single op test, just pass it in the constructor.
	// For the vu parameter, specify which vu unit to run the test on.
	TestRunner(int vu, VU::LowerOp l)
		: VU::Block(vu == 0 ? vu0_micro : vu1_micro), vu_(vu) {
		Wr(l);
	}
	TestRunner(int vu, VU::UpperOp u = VU::UpperOp())
		: VU::Block(vu == 0 ? vu0_micro : vu1_micro), vu_(vu) {
		if (u != VU::UpperOp()) {
			Wr(u);
		}
	}

	// Write an exit and run the test.
	void Execute();

	// Emit an exit.
	void RunnerExit();

	// Emit code to set a float vector value.
	void WrLoadFloatRegister(VU::Dest dest, VU::Reg r, const u32 *val);
	
	// Emit code to set an integer value.
	void WrSetIntegerRegister(VU::Reg r, u32 val);
	void WrLoadIntegerRegister(VU::Dest dest, VU::Reg r, const u32 *val);

	// Print out any integer or float/vector register.
	void PrintRegister(VU::Reg r, bool newline);
	// Print out a summary of status register changes.
	void PrintStatus(bool newline);

protected:
	void InitRegisters();
	void InitFlags();
	void PrintIntegerRegister(int r);
	void PrintVectorRegister(int r);

	u16 GetValueAddress(const u32 *val);
	
	int vu_;
	u32 status_;
	u32 mac_;
	u32 clipping_;
};

#endif
