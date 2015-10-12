#include <stdio.h>
#include <intrman.h>

void intrHandler(int value) {
}

void testNormalOperation() {
	int oldStat = 0;

	printf("normal operation:\n");

	CpuSuspendIntr(&oldStat);
	{
		ReleaseIntrHandler(IOP_IRQ_VBLANK);
		
		RegisterIntrHandler(IOP_IRQ_VBLANK, 0, (void *)&intrHandler, NULL);
		int result = ReleaseIntrHandler(IOP_IRQ_VBLANK);
		
		printf("  release after handler registered: %d\n", result);
	}
	CpuResumeIntr(oldStat);
}

void testReleaseAfterRelease() {
	int oldStat = 0;

	printf("release after release:\n");

	CpuSuspendIntr(&oldStat);
	{
		ReleaseIntrHandler(IOP_IRQ_VBLANK);
		int result = ReleaseIntrHandler(IOP_IRQ_VBLANK);
		printf("  release after handler released: %d\n", result);
	}
	CpuResumeIntr(oldStat);
}

void testInvalidParams() {
	int oldStat = 0;

	printf("invalid params:\n");

	CpuSuspendIntr(&oldStat);
	{
		int invalidIntrLineResult = ReleaseIntrHandler(~0);
		printf("  invalid interrupt line: %d\n", invalidIntrLineResult);
	}
	CpuResumeIntr(oldStat);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	testNormalOperation();
	testReleaseAfterRelease();
	testInvalidParams();

	printf("-- TEST END\n");
	
	return 0;
}
