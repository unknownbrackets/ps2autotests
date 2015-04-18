#include <common.h>
#include <string.h>

void test_lw_delay() {
	const static u32 data = 0x13371337;
	register u32 res = 0;
	asm volatile (
		".set noreorder\n"
		"lui $t0, 0x1122\n"
		"ori $t0, $t0, 0x3344\n"
		"lw $t0, 0(%1)\n"
		"or %0, $0, $t0\n"
		"sync\n"
		: "+&r"(res) : "r"((u32)&data) : "t0"
	);
	printf("lw: %08x\n", res);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	// Just to show that the ee (r5900) has no load delay slots.
	test_lw_delay();

	printf("-- TEST END\n");

	return 0;
}
