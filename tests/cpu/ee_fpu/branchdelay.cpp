#include <common-ee.h>

void __attribute__((noinline)) test_bc1t() {
	int result = -1;

	asm volatile (
		".set    noreorder\n"

		"c.eq.s  $f0, $f0\n"
		"bc1t    target1_%=\n"
		"bc1t    target2_%=\n"
		"bc1t    target3_%=\n"
		"bc1t    target4_%=\n"
		"nop\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"li      %0, 2\n"
		"j       skip_%=\n"
		"nop\n"

		"target3_%=:\n"
		"li      %0, 3\n"
		"j       skip_%=\n"
		"nop\n"

		"target4_%=:\n"
		"li      %0, 4\n"
		"j       skip_%=\n"
		"nop\n"

		"skip_%=:\n"

		: "+r"(result)
	);

	printf("bc1t: delay branch: %08x\n", result);
}

void __attribute__((noinline)) test_bc1tl() {
	int result = -1;

	asm volatile (
		".set    noreorder\n"

		"c.eq.s  $f0, $f0\n"
		"bc1tl   target1_%=\n"
		"bc1tl   target2_%=\n"
		"bc1tl   target3_%=\n"
		"bc1tl   target4_%=\n"
		"nop\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"li      %0, 2\n"
		"j       skip_%=\n"
		"nop\n"

		"target3_%=:\n"
		"li      %0, 3\n"
		"j       skip_%=\n"
		"nop\n"

		"target4_%=:\n"
		"li      %0, 4\n"
		"j       skip_%=\n"
		"nop\n"

		"skip_%=:\n"

		: "+r"(result)
	);

	printf("bc1tl: delay branch: %08x\n", result);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_bc1t();
	test_bc1tl();

	printf("-- TEST END\n");

	return 0;
}
