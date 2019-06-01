#include <common-ee.h>
#include "../test_runner.h"

static const u32 __attribute__((aligned(16))) C_FULLPOS[4]  = {0x40000000, 0x40000000, 0x40000000, 0x40000000};
static const u32 __attribute__((aligned(16))) C_FULLNEG[4]  = {0xC0000000, 0xC0000000, 0xC0000000, 0xC0000000};
static const u32 __attribute__((aligned(16))) C_HALFPOS[4]  = {0x40000000, 0xC0000000, 0x40000000, 0xC0000000};
static const u32 __attribute__((aligned(16))) C_HALFNEG[4]  = {0xC0000000, 0x40000000, 0xC0000000, 0x40000000};
static const u32 __attribute__((aligned(16))) C_WZERO[4]    = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
static const u32 __attribute__((aligned(16))) C_WNEGZERO[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
static const u32 __attribute__((aligned(16))) C_WNEGONE[4]  = {0xBF800000, 0xBF800000, 0xBF800000, 0xBF800000};
static const u32 __attribute__((aligned(16))) C_MAX[4]      = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
static const u32 __attribute__((aligned(16))) C_MIN[4]      = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

static u32 *const CVF_FULLPOS  = (u32 *)(vu0_mem + 0x0070);
static u32 *const CVF_FULLNEG  = (u32 *)(vu0_mem + 0x0080);
static u32 *const CVF_HALFPOS  = (u32 *)(vu0_mem + 0x0090);
static u32 *const CVF_HALFNEG  = (u32 *)(vu0_mem + 0x00A0);
static u32 *const CVF_WZERO    = (u32 *)(vu0_mem + 0x00B0);
static u32 *const CVF_WNEGZERO = (u32 *)(vu0_mem + 0x00C0);
static u32 *const CVF_WNEGONE  = (u32 *)(vu0_mem + 0x00E0);
static u32 *const CVF_MAX      = (u32 *)(vu0_mem + 0x00F0);
static u32 *const CVF_MIN      = (u32 *)(vu0_mem + 0x0100);

static void setup_vf_constants() {
	*(vu128 *)CVF_FULLPOS  = *(vu128 *)C_FULLPOS;
	*(vu128 *)CVF_FULLNEG  = *(vu128 *)C_FULLNEG;
	*(vu128 *)CVF_HALFPOS  = *(vu128 *)C_HALFPOS;
	*(vu128 *)CVF_HALFNEG  = *(vu128 *)C_HALFNEG;
	*(vu128 *)CVF_WZERO    = *(vu128 *)C_WZERO;
	*(vu128 *)CVF_WNEGZERO = *(vu128 *)C_WNEGZERO;
	*(vu128 *)CVF_WNEGONE  = *(vu128 *)C_WNEGONE;
	*(vu128 *)CVF_MAX      = *(vu128 *)C_MAX;
	*(vu128 *)CVF_MIN      = *(vu128 *)C_MIN;
}

class ClipTestRunner : public TestRunner {
public:
	ClipTestRunner(int vu) : TestRunner(vu) {
	}

	void PerformBasic(const char* name, const u32 *s, const u32* t) {
		using namespace VU;

		printf("Basic test: %s -> ", name);

		Reset();

		WrLoadFloatRegister(DEST_XYZW, VF01, s);
		WrLoadFloatRegister(DEST_XYZW, VF02, t);
		
		Wr(CLIP(VF01, VF02));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(FCGET(VI01));
		
		Execute();

		PrintRegister(VI01, true);
	}
	
	void PerformMulti() {
		using namespace VU;

		printf("Multi test:\n");

		Reset();

		WrLoadFloatRegister(DEST_XYZW, VF01, CVF_FULLPOS);
		WrLoadFloatRegister(DEST_XYZW, VF02, CVF_FULLNEG);
		WrLoadFloatRegister(DEST_XYZW, VF03, CVF_HALFPOS);
		WrLoadFloatRegister(DEST_XYZW, VF04, CVF_HALFNEG);
		
		Wr(CLIP(VF01, VF00), FCGET(VI01));
		Wr(CLIP(VF02, VF00), FCGET(VI02));
		Wr(CLIP(VF03, VF00), FCGET(VI03));
		Wr(CLIP(VF04, VF00), FCGET(VI04));
		Wr(FCGET(VI05));
		Wr(FCGET(VI06));
		Wr(FCGET(VI07));
		Wr(FCGET(VI08));
		Wr(FCGET(VI09));
		Wr(FCGET(VI10));
		
		Execute();

		PrintRegister(VI01, true);
		PrintRegister(VI02, true);
		PrintRegister(VI03, true);
		PrintRegister(VI04, true);
		PrintRegister(VI05, true);
		PrintRegister(VI06, true);
		PrintRegister(VI07, true);
		PrintRegister(VI08, true);
		PrintRegister(VI09, true);
		PrintRegister(VI10, true);
	}
	
	void PerformMulti2() {
		using namespace VU;

		printf("Multi test 2:\n");

		Reset();

		WrLoadFloatRegister(DEST_XYZW, VF01, CVF_FULLPOS);
		WrLoadFloatRegister(DEST_XYZW, VF02, CVF_FULLNEG);
		WrLoadFloatRegister(DEST_XYZW, VF03, CVF_HALFPOS);
		WrLoadFloatRegister(DEST_XYZW, VF04, CVF_HALFNEG);
		WrLoadFloatRegister(DEST_XYZW, VF05, CVF_WNEGONE);
		
		Wr(CLIP(VF01, VF00));
		Wr(CLIP(VF01, VF05));
		Wr(CLIP(VF02, VF00));
		Wr(CLIP(VF02, VF05));
		Wr(CLIP(VF03, VF00));
		Wr(CLIP(VF03, VF05), FCGET(VI01));
		Wr(CLIP(VF04, VF00));
		Wr(CLIP(VF04, VF05), FCGET(VI02));
		Wr(NOP());
		Wr(FCGET(VI03));
		Wr(NOP());
		Wr(FCGET(VI04));
		
		Execute();

		PrintRegister(VI01, true);
		PrintRegister(VI02, true);
		PrintRegister(VI03, true);
		PrintRegister(VI04, true);
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	setup_vf_constants();
	
	ClipTestRunner runner(0);
	
	runner.PerformBasic("FULL POS x -1", CVF_FULLPOS, CVF_WNEGONE);
	runner.PerformBasic("FULL NEG x -1", CVF_FULLNEG, CVF_WNEGONE);
	runner.PerformBasic("HALF POS x -1", CVF_HALFPOS, CVF_WNEGONE);
	runner.PerformBasic("HALF NEG x -1", CVF_HALFNEG, CVF_WNEGONE);
	runner.PerformBasic("HALF NEG x  0", CVF_HALFNEG, CVF_WZERO);
	runner.PerformBasic("HALF NEG x -0", CVF_HALFNEG, CVF_WNEGZERO);
	runner.PerformBasic(" 0 x -0", CVF_WZERO, CVF_WNEGZERO);
	runner.PerformBasic("-0 x  0", CVF_WNEGZERO, CVF_WZERO);
	runner.PerformBasic("FULL POS x FULL POS", CVF_FULLPOS, CVF_FULLPOS);
	runner.PerformBasic("HALF NEG x HALF NEG", CVF_HALFNEG, CVF_HALFNEG);
	runner.PerformBasic("MIN x 0", CVF_MIN, CVF_WZERO);
	runner.PerformBasic("MAX x 0", CVF_MAX, CVF_WZERO);
	runner.PerformBasic("0 x MIN", CVF_WZERO, CVF_MIN);
	runner.PerformBasic("0 x MAX", CVF_WZERO, CVF_MAX);
	runner.PerformMulti();
	runner.PerformMulti2();
	
	printf("-- TEST END\n");
	return 0;
}
