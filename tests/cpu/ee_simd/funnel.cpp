#include "shared.h"

static u32 __attribute__((aligned(16))) C_SHIFT_PATTERN1[4] = {0x12345678, 0x9ABCDEF0, 0x11223344, 0xDEADBEEF};
static u32 __attribute__((aligned(16))) C_SHIFT_PATTERN2[4] = {0xAABBCCDD, 0x12121212, 0xFFFFFFFF, 0x1337C0DE};

template <int i>
static void SET_SA_I() {
	asm volatile (
		"lui $t6, %0\n"
		"ori $t6, $t6, %1\n"
		"pcpyld $t6, $t6, $t6\n"
		"mtsa $t6\n"
		: : "K"((i >> 16) & 0xFFFF), "K"(i & 0xFFFF) : "t6"
	);
}

static void SET_SA_M(void *p) {
	asm volatile (
		"ld $t6, 0(%0)\n"
		"sync\n"
		"nop\n"
		"mtsa $t6\n"
		: : "r"(p) : "t6"
	);
}

static void PRINT_SA(bool newline) {
	register u128 r = 0;

	asm volatile (
		"lui %0, 0x1234\n"
		"ori %0, %0, 0xABCD\n"
		"pcpyld %0, %0, %0\n"
		"mfsa %0\n"
		: "+r"(r)
	);

	PRINT_R(r, newline);
}

static void test_mfsa() {
	printf("mfsa:\n");

	SET_SA_I<0>();
	printf("  mfsa 0: ");
	PRINT_SA(true);

	SET_SA_I<1>();
	printf("  mfsa 1: ");
	PRINT_SA(true);

	SET_SA_I<8>();
	printf("  mfsa 8: ");
	PRINT_SA(true);

	SET_SA_I<16>();
	printf("  mfsa 16: ");
	PRINT_SA(true);

	SET_SA_I<0xFFFF>();
	printf("  mfsa 0xFFFF: ");
	PRINT_SA(true);

	SET_SA_M(C_ZERO);
	printf("  mfsa C_ZERO: ");
	PRINT_SA(true);

	SET_SA_M(C_S16_MAX);
	printf("  mfsa C_S16_MAX: ");
	PRINT_SA(true);

	printf("\n");
}

static void test_mtsa() {
	printf("mtsa:\n");

	// Already largely tested mtsa in mfsa.  Let's test edge cases only.
	register u128 rd = 0;

	asm volatile (
		".set noreorder\n"
		".set nomacro\n"

		"mtsa $0\n"

		"lq $t6, 0(%1)\n"
		"sync\n"
		"nop\n"

		"ori $t7, $0, 1\n"

		"mtsa $t7\n"
		"qfsrv %0, $t6, $t6\n"
		: "+r"(rd) : "r"(C_SHIFT_PATTERN1) : "t6", "t7"
	);

	printf("  mtsa immediately before qfsrv: ");
	PRINT_R(rd, true);

	asm volatile (
		".set noreorder\n"
		".set nomacro\n"

		"mtsa $0\n"

		"lq $t6, 0(%1)\n"
		"sync\n"
		"nop\n"

		"ori $t7, $0, 1\n"

		"mtsa $t7\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"qfsrv %0, $t6, $t6\n"
		: "+r"(rd) : "r"(C_SHIFT_PATTERN1) : "t6", "t7"
	);

	printf("  mtsa with nops before qfsrv: ");
	PRINT_R(rd, true);

	printf("\n");
}

static void test_mtsab() {
	printf("mtsab:\n");

	register u128 rd = 0;
	asm volatile (
		".set noreorder\n"
		".set nomacro\n"

		"mtsa $0\n"
		"nop\n"
		"nop\n"

		"mfsa %0\n"
		"mtsab $0, 1\n"
		: "+r"(rd) : : "t6"
	);

	printf("  mtsab next to mfsa: ");
	PRINT_R(rd, false);
	printf(" (");
	PRINT_SA(false);
	printf(")\n");

	asm volatile (
		".set noreorder\n"
		".set nomacro\n"

		"mtsa $0\n"
		"nop\n"
		"nop\n"

		"mfsa %0\n"
		"nop\n"
		"nop\n"
		"mtsab $0, 1\n"
		: "+r"(rd) : : "t6"
	);

	printf("  mtsab with space after mfsa: ");
	PRINT_R(rd, false);
	printf(" (");
	PRINT_SA(false);
	printf(")\n");

	asm volatile (
		"mtsab $0, 15\n"
	);
	printf("  mtsab 15: ");
	PRINT_SA(true);

	asm volatile (
		"mtsab $0, 16\n"
	);
	printf("  mtsab 16: ");
	PRINT_SA(true);

	asm volatile (
		"mtsab $0, 17\n"
	);
	printf("  mtsab 17: ");
	PRINT_SA(true);

	asm volatile (
		"ori $t6, $0, 4\n"
		"mtsab $t6, 1\n"
		: : : "t6"
	);
	printf("  mtsab 4 ^ 1: ");
	PRINT_SA(true);

	asm volatile (
		"ori $t6, $0, 5\n"
		"mtsab $t6, 1\n"
		: : : "t6"
	);
	printf("  mtsab 5 ^ 1: ");
	PRINT_SA(true);

	printf("\n");
}

static void test_mtsah() {
	printf("mtsah:\n");

	register u128 rd = 0;
	asm volatile (
		".set noreorder\n"
		".set nomacro\n"

		"mtsa $0\n"
		"nop\n"
		"nop\n"

		"mfsa %0\n"
		"mtsah $0, 1\n"
		: "+r"(rd) : : "t6"
	);

	printf("  mtsah next to mfsa: ");
	PRINT_R(rd, false);
	printf(" (");
	PRINT_SA(false);
	printf(")\n");

	asm volatile (
		".set noreorder\n"
		".set nomacro\n"

		"mtsa $0\n"
		"nop\n"
		"nop\n"

		"mfsa %0\n"
		"nop\n"
		"nop\n"
		"mtsah $0, 1\n"
		: "+r"(rd) : : "t6"
	);

	printf("  mtsah with space after mfsa: ");
	PRINT_R(rd, false);
	printf(" (");
	PRINT_SA(false);
	printf(")\n");

	asm volatile (
		"mtsah $0, 7\n"
	);
	printf("  mtsah 7: ");
	PRINT_SA(true);

	asm volatile (
		"mtsah $0, 8\n"
	);
	printf("  mtsah 8: ");
	PRINT_SA(true);

	asm volatile (
		"mtsah $0, 9\n"
	);
	printf("  mtsah 9: ");
	PRINT_SA(true);

	asm volatile (
		"ori $t6, $0, 4\n"
		"mtsah $t6, 1\n"
		: : : "t6"
	);
	printf("  mtsah 4 ^ 1: ");
	PRINT_SA(true);

	asm volatile (
		"ori $t6, $0, 5\n"
		"mtsah $t6, 1\n"
		: : : "t6"
	);
	printf("  mtsah 5 ^ 1: ");
	PRINT_SA(true);

	printf("\n");
}

template <int i>
static void do_qdsrv_i() {
	register u128 rd = 0;
	asm volatile (
		".set noreorder\n"
		".set nomacro\n"

		"lq $t6, 0(%1)\n"
		"lq $t7, 0(%2)\n"
		"sync\n"

		"mtsab $0, %3\n"
		"nop\n"
		"nop\n"
		"nop\n"
		"qfsrv %0, $t6, $t7\n"
		: "+r"(rd) : "r"(C_SHIFT_PATTERN1), "r"(C_SHIFT_PATTERN2), "i"(i) : "t6", "t7"
	);

	printf("  qdsrv %d: ", i);
	PRINT_R(rd, true);
}

static void test_qdsrv() {
	printf("qdsrv:\n");

	do_qdsrv_i<0>();
	do_qdsrv_i<1>();
	do_qdsrv_i<2>();
	do_qdsrv_i<3>();
	do_qdsrv_i<4>();
	do_qdsrv_i<7>();
	do_qdsrv_i<8>();
	do_qdsrv_i<9>();
	do_qdsrv_i<15>();
	do_qdsrv_i<16>();
	do_qdsrv_i<17>();

	printf("\n");
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_mfsa();
	test_mtsa();
	test_mtsab();
	test_mtsah();
	test_qdsrv();

	printf("-- TEST END\n");

	return 0;
}
