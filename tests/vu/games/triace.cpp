#include <common.h>
#include "../test_runner.h"

static const u32 __attribute__((aligned(16))) C_INIT[4] = {0xB063B75B, 0x8701CE82, 0x46FCC888, 0x793CC535};

static u32 *const CVI_INIT = (u32 *)(vu0_mem + 0x0000);

static void setup_constants() {
	*(vu128 *)CVI_INIT = *(vu128 *)C_INIT;
}

class TriAceTestRunner : public TestRunner {
public:
	TriAceTestRunner(int vu) : TestRunner(vu) {
	}

	void Perform() {
		using namespace VU;

		Reset();

		WrLoadFloatRegister(DEST_XYZW, VF01, CVI_INIT);
		
		WrImm(                          0x42FECCCD);
		WrImm(MULi(DEST_X, VF02, VF01), 0x42546666);
		WrImm(ADDi(DEST_X, VF03, VF02), 0x43D80D3E);
		WrImm(MULi(DEST_Y, VF02, VF01), 0xC7F079B3);
		WrImm(ADDi(DEST_Y, VF03, VF02), 0x43A0DA10);
		WrImm(MULi(DEST_Z, VF02, VF01), 0x43A02666);
		WrImm(ADDi(DEST_Z, VF03, VF02), 0x43546E14);
		WrImm(MULi(DEST_W, VF02, VF01), 0x42F23333);
		WrImm(ADDi(DEST_W, VF03, VF02), 0x3DD3DD98);
		
		Execute();

		PrintRegister(VF01, true);
		PrintRegister(VF02, true);
		PrintRegister(VF03, true);
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	setup_constants();

	TriAceTestRunner runner(0);

	runner.Perform();

	printf("-- TEST END\n");
	return 0;
}
