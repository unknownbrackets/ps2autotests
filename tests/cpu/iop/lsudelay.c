#include <stdio.h>
#include <tamtypes.h>

#define LOAD_DELAY0_FUNC(OP) \
void test_delay0_##OP() { \
	const static u32 data = 0x13371337; \
	register u32 res = 0; \
	asm volatile ( \
		".set noreorder\n" \
		"lui $t0, 0x1122\n" \
		"ori $t0, $t0, 0x3344\n" \
		#OP " $t0, 0(%1)\n" \
		"ori $t0, $0, 0\n" \
		"or %0, $0, $t0\n" \
		"nop\n" \
		: "+&r"(res) : "r"((u32)&data) : "t0" \
	); \
	printf("%s 0: %08x\n", #OP, res); \
}

#define LOAD_DELAY1_FUNC(OP) \
void test_delay1_##OP() { \
	const static u32 data = 0x13371337; \
	register u32 res = 0; \
	asm volatile ( \
		".set noreorder\n" \
		"lui $t0, 0x1122\n" \
		"ori $t0, $t0, 0x3344\n" \
		#OP " $t0, 0(%1)\n" \
		"or %0, $0, $t0\n" \
		"nop\n" \
		: "+&r"(res) : "r"((u32)&data) : "t0" \
	); \
	printf("%s 1: %08x\n", #OP, res); \
}

#define LOAD_DELAY2_FUNC(OP) \
void test_delay2_##OP() { \
	const static u32 data = 0x13371337; \
	register u32 res = 0; \
	asm volatile ( \
		".set noreorder\n" \
		"lui $t0, 0x1122\n" \
		"ori $t0, $t0, 0x3344\n" \
		#OP " $t0, 0(%1)\n" \
		"nop\n" \
		"or %0, $0, $t0\n" \
		"nop\n" \
		: "+&r"(res) : "r"((u32)&data) : "t0" \
	); \
	printf("%s 2: %08x\n", #OP, res); \
}

void test_delay_lw_branch() {
	const static u32 data = 0x13371337;
	register u32 res = 0;
	asm volatile (
		".set noreorder\n"
		"lui $t0, 0x1122\n"
		"ori $t0, $t0, 0x3344\n"
		"ori $t1, $t0, 0\n"
		"lw $t0, 0(%1)\n"
		"beq $t0, $t1, lw_branch_equal\n"
		"nop\n"

		"ori %0, $0, 1\n"
		"b lw_branch_done\n"
		"nop\n"

		"lw_branch_equal:\n"
		"ori %0, $0, 2\n"

		"lw_branch_done:\n"
		"nop\n"
		: "+&r"(res) : "r"((u32)&data) : "t0", "t1"
	);
	printf("lw then branch: %d\n", res);
}

void test_delay1_ld() {
	const static u32 data[2] = {0x13371337, 0x13371337};
	register u32 res1 = 0;
	register u32 res2 = 0;
	asm volatile (
		".set noreorder\n"
		"lui $t0, 0x1122\n"
		"ori $t0, $t0, 0x3344\n"
		"ori $t1, $t0, 0\n"
		"ld $t0, 0(%2)\n"
		"ori $t1, $0, 0\n"
		"move %0, $t0\n"
		"move %1, $t1\n"
		"nop\n"
		: "+&r"(res1), "+&r"(res2) : "r"((u32)&data[0]) : "t0", "t1"
	);
	printf("ld 1: %08x %08x\n", res1, res2);
}

void test_delay2_ld() {
	const static u32 data[2] = {0x13371337, 0x13371337};
	register u32 res1 = 0;
	register u32 res2 = 0;
	asm volatile (
		".set noreorder\n"
		"lui $t0, 0x1122\n"
		"ori $t0, $t0, 0x3344\n"
		"ori $t1, $t0, 0\n"
		"ld $t0, 0(%2)\n"
		"nop\n"
		"move %0, $t0\n"
		"move %1, $t1\n"
		"nop\n"
		: "+&r"(res1), "+&r"(res2) : "r"((u32)&data[0]) : "t0", "t1"
	);
	printf("ld 2: %08x %08x\n", res1, res2);
}

LOAD_DELAY0_FUNC(lb)
LOAD_DELAY0_FUNC(lw)
LOAD_DELAY1_FUNC(lb)
LOAD_DELAY1_FUNC(lw)
LOAD_DELAY2_FUNC(lb)
LOAD_DELAY2_FUNC(lw)

int _start(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	test_delay0_lb();
	test_delay0_lw();
	test_delay1_lb();
	test_delay1_lw();
	test_delay2_lb();
	test_delay2_lw();

	test_delay1_ld();
	test_delay2_ld();

	test_delay_lw_branch();

	printf("-- TEST END\n");

	return 0;
}
