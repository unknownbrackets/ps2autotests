#include <stdio.h>
#include <tamtypes.h>

static void test_mfhilo_wait() {
	register u32 hi = 0x1337;
	register u32 lo = 0x1337;

	asm volatile (
		"ori $t8, $0, 0x10\n"
		"mthi $t8\n"
		"ori $t8, $0, 0x20\n"
		"mtlo $t8\n"
		"nop\n"

		"ori $t8, $0, 0x01\n"
		"mfhi $t8\n"
		"or %0, $t8, $0\n"

		"ori $t8, $0, 0x02\n"
		"mflo $t8\n"
		"or %1, $t8, $0\n"
		: "+&r"(hi), "+&r"(lo) : : "t8"
	);

	printf("mfhi delay: %x\n", hi);
	printf("mflo delay: %x\n", lo);
}

static void test_div_wait() {
	register u32 hi = 0x1337;
	register u32 lo = 0x1337;

	asm volatile (
		"ori $t8, $0, 0x10\n"
		"mthi $t8\n"
		"ori $t8, $0, 0x20\n"
		"mtlo $t8\n"
		"nop\n"

		"ori $t8, $0, 0x03\n"
		"ori $t9, $0, 0x02\n"

		".word 0x0319001A\n" /* "div $t8, $t9\n" */
		"mfhi %0\n"

		".word 0x0319001A\n" /* "div $t8, $t9\n" */
		"mflo %1\n"
		: "+&r"(hi), "+&r"(lo) : : "t8", "t9"
	);

	printf("div delay: %x %x\n", hi, lo);
}

static void test_mult_wait() {
	register u32 hi = 0x1337;
	register u32 lo = 0x1337;

	asm volatile (
		"ori $t8, $0, 0x10\n"
		"mthi $t8\n"
		"ori $t8, $0, 0x20\n"
		"mtlo $t8\n"
		"nop\n"

		"ori $t8, $0, 0x03\n"
		"ori $t9, $0, 0x02\n"

		"mult $t8, $t9\n"
		"mfhi %0\n"

		"mult $t8, $t9\n"
		"mflo %1\n"
		: "+&r"(hi), "+&r"(lo) : : "t8", "t9"
	);

	printf("mult delay: %x %x\n", hi, lo);
}

int _start(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_mfhilo_wait();
	test_div_wait();
	test_mult_wait();

	printf("-- TEST END\n");

	return 0;
}
