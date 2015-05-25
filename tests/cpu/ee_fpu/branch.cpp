#include <common-ee.h>

enum BranchResultFlags {
	BRANCH_SKIPPED = 0,
	BRANCH_FOLLOWED = 1,
	BRANCH_SET_RA = 2,
	BRANCH_DELAY_SLOT = 4,
};

#define BO_OP_FUNC(OP) \
static inline void BO_##OP(u32 &result) { \
	asm volatile ( \
		".set noreorder\n" \
		"" \
		"lui %0, 0\n" \
		"or $t9, $0, $ra\n" \
		"" \
		#OP " " #OP "_branch%=\n" \
		"ori %0, %0, 4\n" \
		"" \
		"b " #OP "_skip%=\n" \
		"nop\n" \
		"" \
		#OP "_branch%=:\n" \
		"ori %0, %0, 1\n" \
		"" \
		#OP "_skip%=:\n" \
		"beq $t9, $ra, " #OP "_done%=\n" \
		"nop\n" \
		"" \
		"or $ra, $0, $t9\n" \
		"ori %0, %0, 2\n" \
		"" \
		#OP "_done%=:\n" \
		"" \
		: "=&r"(result) : : "t9" \
	); \
}

static inline void PRINT_BRANCH(const u32 result, bool newline) {
	printf("%s, ", (result & BRANCH_FOLLOWED) != 0 ? "followed" : "skipped");
	printf("%s, ", (result & BRANCH_SET_RA) != 0 ? "set ra" : "ignored ra");
	printf("%s", (result & BRANCH_DELAY_SLOT) != 0 ? "ran delay slot" : "skipped delay slot");
	if (newline) {
		printf("\n");
	}
}

static inline void SET_FC_I(int flag) {
	register u32 mask = ~(1 << 23);
	register u32 bit = flag << 23;

	asm volatile (
		"cfc1 $t6, $31\n"
		"and $t6, $t6, %0\n"
		"or $t6, $t6, %1\n"
		"ctc1 $t6, $31\n"
		: : "r"(mask), "r"(bit) : "t6"
	);
}

static inline void SET_FC_CMP_TRUE() {
	asm volatile (
		"c.eq.s $f0, $f0\n"
	);
}

static inline void SET_FC_CMP_FALSE() {
	asm volatile (
		"c.f.s $f0, $f0\n"
	);
}

#define TEST_BO_FUNC(OP) \
BO_OP_FUNC(OP); \
static void test_##OP() { \
	u32 res; \
	 \
	printf("%s:\n", #OP); \
	 \
	SET_FC_I(1); \
	BO_##OP(res); \
	printf("  %s 1: ", #OP); PRINT_BRANCH(res, true); \
	 \
	SET_FC_I(0); \
	BO_##OP(res); \
	printf("  %s 0: ", #OP); PRINT_BRANCH(res, true); \
	 \
	SET_FC_CMP_TRUE(); \
	BO_##OP(res); \
	printf("  %s c.eq: ", #OP); PRINT_BRANCH(res, true); \
	 \
	SET_FC_CMP_FALSE(); \
	BO_##OP(res); \
	printf("  %s c.f: ", #OP); PRINT_BRANCH(res, true); \
	\
	printf("\n"); \
}

TEST_BO_FUNC(bc1f);
TEST_BO_FUNC(bc1fl);
TEST_BO_FUNC(bc1t);
TEST_BO_FUNC(bc1tl);

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_bc1f();
	test_bc1fl();
	test_bc1t();
	test_bc1tl();

	printf("-- TEST END\n");

	return 0;
}
