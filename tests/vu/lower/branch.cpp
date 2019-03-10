#include <common-ee.h>
#include "../test_runner.h"

class BranchTestRunner : public TestRunner {
public:
	BranchTestRunner(int vu) : TestRunner(vu) {
	}

	void Perform_JALR(const char *name, VU::Reg targetReg, VU::Reg returnReg, VU::Reg resultReg) {
		using namespace VU;

		Reset();

		Wr(IADDI(resultReg, VI00, 0x0));
		Wr(IADDI(targetReg, VI00, 0x6));
		JALR(returnReg, targetReg);
		Label exitLabel = B();
		//6 * 8
		Wr(IADDI(resultReg, VI00, 0x1));
		L(exitLabel);
		Wr(NOP());
		
		Execute();

		printf("  %s ->", name);
		printf(" targetReg: ");
		PrintRegister(targetReg, false);
		printf(" returnReg: ");
		PrintRegister(returnReg, false);
		printf(" resultReg: ");
		PrintRegister(resultReg, false);
		
		printf("\n");
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	BranchTestRunner runner(0);
	runner.Perform_JALR("JALR /w diff regs", VU::VI01, VU::VI02, VU::VI03);
	runner.Perform_JALR("JALR /w same regs", VU::VI01, VU::VI01, VU::VI03);
	
	printf("-- TEST END\n");
	return 0;
}
