#include <assert.h>
#include <common.h>
#include <string.h>
#include <timer.h>
#include "test_runner.h"

static const bool DEBUG_TEST_RUNNER = false;

static u32 vu_reg_pos = 0x100;
// 16 integer regs, 32 float regs, 3 status regs.
static u32 vu_reg_size = 16 * (16 + 32 + 3);

static u8 *const vu0_reg_mem = vu0_mem + vu_reg_pos;
static u8 *const vu1_reg_mem = vu1_mem + vu_reg_pos;

// Helper to "sleep" the ee for a few ticks (less than a second.)
// This is used to wait for the vu test to finish.
// We don't use interlock in case it stalls.
static void delay_ticks(int t) {
	u32 goal = cpu_ticks() + t;
	while (cpu_ticks() < goal) {
		continue;
	}
}

// Determine whether the specified vu is running or not.
static bool vu_running(int vu) {
	// Must be 0 or 1.  Change to 0 or (1 << 8).
	u32 mask = vu == 0 ? 1 : (1 << 8);
	u32 stat;
	asm volatile (
		"cfc2 %0, $29"
		: "=r"(stat)
	);
	return (stat & mask) != 0;
}

void TestRunner::Execute() {
	// Just to be sure.
	RunnerExit();

	InitRegisters();
	InitFlags();

	if (vu_ == 1) {
		if (DEBUG_TEST_RUNNER) {
			printf("Calling vu0 code.\n");
		}
		asm volatile (
			// This writes to cmsar1 ($31).  The program is simply at 0.
			"ctc2 $0, $31\n"
		);
	} else {
		if (DEBUG_TEST_RUNNER) {
			printf("Calling vu0 code.\n");
		}
		asm volatile (
			// Actually call the microcode.
			"vcallms 0\n"
		);
	}

	if (DEBUG_TEST_RUNNER) {
		printf("Waiting for vu to finish.\n");
	}

	// Spin while waiting for the vu unit to finish.
	u32 max_ticks = cpu_ticks() + 100000;
	while (vu_running(vu_)) {
		if (cpu_ticks() > max_ticks) {
			printf("ERROR: Timed out waiting for vu code to finish.\n");
			// TODO: Force stop?
			break;
		}
		delay_ticks(200);
	}

	if (DEBUG_TEST_RUNNER && !vu_running(vu_)) {
		printf("Finished with vu code.\n");
	}
}

void TestRunner::RunnerExit() {
	using namespace VU;

	// The goal here is to store all the registers from the VU side.
	// This way we're not testing register transfer, we're testing VU interaction.
	// Issues with spilling may be more obvious this way.

	// First, integers.
	u32 pos = vu_reg_pos / 16;
	for (int i = 0; i < 16; ++i) {
		Wr(ISW(DEST_XYZW, Reg(VI00 + i), VI00, pos++));
	}
	// Then floats.
	Wr(IADDIU(VI02, VI00, pos));
	for (int i = 0; i < 32; ++i) {
		Wr(SQ(DEST_XYZW, Reg(VF00 + i), VI00, pos++));
	}

	// Lastly, flags (now that we've saved integers.)
	Wr(FCGET(VI01));
	Wr(ISW(DEST_XYZW, VI01, VI00, pos++));
	Wr(FMOR(VI01, VI00));
	Wr(ISW(DEST_XYZW, VI01, VI00, pos++));
	Wr(FSOR(VI01, 0));
	Wr(ISW(DEST_XYZW, VI01, VI00, pos++));

	// And actually write an exit (just two NOPs, the first with an E bit.)
	SafeExit();

	if (DEBUG_TEST_RUNNER) {
		printf("Emitted exit code.\n");
	}
}

void TestRunner::InitRegisters() {
	// Clear the register state (which will be set by the code run in RunnerExit.)
	if (vu_ == 1) {
		memset(vu1_reg_mem, 0xCC, vu_reg_size);
	} else {
		memset(vu0_reg_mem, 0xCC, vu_reg_size);
	}
}

void TestRunner::InitFlags() {
	// Grab the previous values.
	asm volatile (
		// Try to clear them first, if possible.
		"vnop\n"
		"ctc2 $0, $16\n"
		"ctc2 $0, $17\n"
		"ctc2 $0, $18\n"
		// Then read.
		"vnop\n"
		"cfc2 %0, $16\n"
		"cfc2 %1, $17\n"
		"cfc2 %2, $18\n"
		: "=&r"(status_), "=&r"(mac_), "=&r"(clipping_)
	);

	if (clipping_ != 0) {
		printf("WARNING: Clipping flag was not cleared.\n");
	}
}

void TestRunner::WrSetIntegerRegister(VU::Reg r, u32 v) {
	using namespace VU;

	assert(r >= VI00 && r <= VI15);
	assert((v & 0xFFFF) == v);

	// Except for 0x8000, we can get to everything with one op.
	if (v == 0x8000) {
		Wr(IADDI(r, VI00, -1));
		Wr(ISUBIU(r, r, 0x7FFF));
	} else if (v & 0x8000) {
		Wr(ISUBIU(r, VI00, v & ~0x8000));
	} else {
		Wr(IADDIU(r, VI00, v));
	}
}

void TestRunner::WrLoadIntegerRegister(VU::Dest dest, VU::Reg r, const u32 *val) {
	using namespace VU;

	assert(r >= VI00 && r <= VI15);

	u16 ptr;
	if (vu_ == 0) {
		assert(val >= (u32 *)vu0_mem && val < (u32 *)(vu0_mem + vu0_mem_size));
		ptr = ((u8 *)val - vu0_mem) / 16;
	} else {
		assert(val >= (u32 *)vu1_mem && val < (u32 *)(vu1_mem + vu1_mem_size));
		ptr = ((u8 *)val - vu1_mem) / 16;
	}

	if ((ptr & 0x3FF) == ptr) {
		Wr(ILW(dest, r, VI00, ptr));
	} else {
		// This shouldn't happen?  Even with 16kb, all reads should be < 0x400.
		printf("Unhandled pointer load.");
		assert(false);
	}
}

void TestRunner::PrintRegister(VU::Reg r, bool newline) {
	using namespace VU;

	if (r >= VI00 && r <= VI15) {
		PrintIntegerRegister(r - VI00);
	} else {
		PrintVectorRegister(r);
	}
	if (newline) {
		printf("\n");
	}
}

void TestRunner::PrintIntegerRegister(int i) {
	const u8 *regs = vu_ == 0 ? vu0_reg_mem : vu1_reg_mem;
	// The integer registers are first, so it's just at the specified position.
	const u32 *p = (const u32 *)(regs + 16 * i);

	printf("%04x", *p);
}

union FloatBits {
	float f;
	u32 u;
};

static inline void printFloatString(const FloatBits &bits) {
	// We want -0.0 and -NAN to show as negative.
	// So, let's just print the sign manually with an absolute value.
	FloatBits positive = bits;
	positive.u &= ~0x80000000;

	char sign = '+';
	if (bits.u & 0x80000000) {
		sign = '-';
	}

	printf("%c%0.2f", sign, positive.f);
}

static inline void printFloatBits(const FloatBits &bits) {
	printf("%08x/", bits.u);
	printFloatString(bits);
}

void TestRunner::PrintVectorRegister(int i) {
	const u8 *regs = vu_ == 0 ? vu0_reg_mem : vu1_reg_mem;
	// Skip the 16 integer registers, and jump to the vector.
	const FloatBits *p = (const FloatBits *)(regs + 16 * (16 + i));

	// Let's just print each lane separated by a space.
	printFloatBits(p[0]);
	printf(" ");
	printFloatBits(p[1]);
	printf(" ");
	printFloatBits(p[2]);
	printf(" ");
	printFloatBits(p[3]);
}

void TestRunner::PrintStatus(bool newline) {
	const u8 *regs = vu_ == 0 ? vu0_reg_mem : vu1_reg_mem;
	// Skip 16 integer regs and 32 vector regs, to get a base for the status regs.
	const u32 *flags = (const u32 *)(regs + 16 * (16 + 32));

	// Note that they are spaced out by 16 bytes (4 words.)
	u32 clipping = flags[0];
	u32 mac = flags[4];
	u32 status = flags[8];

	if (status == status_ && mac == mac_ && clipping == clipping_) {
		printf(" (no flag changes)");
	} else {
		printf(" st=+%04x,-%04x, mac=+%04x,-%04x, clip=%08x", status & ~status_, ~status & status_, mac & ~mac_, ~mac & mac_, clipping);
	}
	if (newline) {
		printf("\n");
	}
}
