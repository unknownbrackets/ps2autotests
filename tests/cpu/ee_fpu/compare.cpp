#include "shared.h"

#define FCMP_OP_FUNC(OP) \
static inline void FCMP_##OP(register float &fd, const register float &fs) { \
	asm volatile ( \
		"c." #OP ".s %0, %1\n" \
		: "+f"(fd) : "f"(fs) \
	); \
}

static inline void PRINT_FC(bool newline) {
	register u32 fcr31 = 0;

	asm volatile (
		"cfc1 %0, $31\n"
		: "+r"(fcr31)
	);

	if ((fcr31 >> 23) & 1) {
		printf("true");
	} else {
		printf("false");
	}
	if (newline) {
		printf("\n");
	}
}

#define FCMP_OP_DO_II(OP, s, t) \
	SET_U32<s>(fs); SET_U32<t>(ft); \
	FCMP_##OP(fs, ft); \
	printf("  %s ", #OP); \
	PRINT_F(fs, false); \
	printf(", "); \
	PRINT_F(ft, false); \
	printf(": "); \
	PRINT_FC(true);

#define FCMP_OP_DO_MM(OP, s, t) \
	SET_M(fs, s); SET_M(ft, t); \
	FCMP_##OP(fs, ft); \
	printf("  %s %s, %s: ", #OP, #s, #t); \
	PRINT_FC(true);

#define TEST_FCMP_FUNC(OP) \
FCMP_OP_FUNC(OP); \
static void test_c_##OP() { \
	register float fs, ft; \
	 \
	printf("%s:\n", #OP); \
	 \
	FCMP_OP_DO_II(OP, 0x00000000, 0x00000000); \
	FCMP_OP_DO_II(OP, 0x00000000, 0x80000000); \
	FCMP_OP_DO_II(OP, 0x80000000, 0x00000000); \
	FCMP_OP_DO_II(OP, 0x80000000, 0x80000000); \
	FCMP_OP_DO_II(OP, 0x00000000, 0x3F800000); \
	FCMP_OP_DO_II(OP, 0x3F800000, 0x3F800000); \
	FCMP_OP_DO_II(OP, 0x3F800000, 0x00000000); \
	FCMP_OP_DO_II(OP, 0x40000000, 0x40000000); \
	FCMP_OP_DO_II(OP, 0x40400000, 0x3F800000); \
	FCMP_OP_DO_II(OP, 0x40400000, 0x40400000); \
	FCMP_OP_DO_II(OP, 0x7FFFFFFF, 0x7FFFFFFF); \
	FCMP_OP_DO_II(OP, 0x7FFFFFFF, 0xFFFFFFFF); \
	FCMP_OP_DO_II(OP, 0xFFFFFFFF, 0x7FFFFFFF); \
	FCMP_OP_DO_II(OP, 0xFFFFFFFF, 0xFFFFFFFF); \
	FCMP_OP_DO_II(OP, 0x7FFFFFFF, 0x00000000); \
	FCMP_OP_DO_II(OP, 0x00000000, 0x00000001); \
	FCMP_OP_DO_II(OP, 0x00000001, 0x00000000); \
	FCMP_OP_DO_II(OP, 0x00000000, 0x7FFFFFFF); \
	FCMP_OP_DO_II(OP, 0x00000000, 0x7F800000); \
	FCMP_OP_DO_II(OP, 0x7F800000, 0x7F800000); \
	FCMP_OP_DO_II(OP, 0x7F800000, 0x7F800001); \
	FCMP_OP_DO_II(OP, 0x7F800001, 0x7F800000); \
	FCMP_OP_DO_II(OP, 0xFF800000, 0x7F800000); \
	 \
	FCMP_OP_DO_MM(OP, CF_ZERO, CF_ZERO); \
	FCMP_OP_DO_MM(OP, CF_ZERO, CF_NEGZERO); \
	FCMP_OP_DO_MM(OP, CF_ONE, CF_ONE); \
	FCMP_OP_DO_MM(OP, CF_ONE, CF_NEGONE); \
	FCMP_OP_DO_MM(OP, CF_MAX_MANTISSA, CF_ONE); \
	FCMP_OP_DO_MM(OP, CF_MAX_EXP, CF_ONE); \
	FCMP_OP_DO_MM(OP, CF_MIN_EXP, CF_ONE); \
	FCMP_OP_DO_MM(OP, CF_ONE, CF_MAX_MANTISSA); \
	FCMP_OP_DO_MM(OP, CF_ONE, CF_MAX_EXP); \
	FCMP_OP_DO_MM(OP, CF_ONE, CF_MIN_EXP); \
	FCMP_OP_DO_MM(OP, CF_MAX, CF_ONE); \
	FCMP_OP_DO_MM(OP, CF_MIN, CF_ONE); \
	FCMP_OP_DO_MM(OP, CF_ONE, CF_MAX); \
	FCMP_OP_DO_MM(OP, CF_ONE, CF_MIN); \
	FCMP_OP_DO_MM(OP, CF_MAX, CF_MAX); \
	FCMP_OP_DO_MM(OP, CF_MIN, CF_MIN); \
	FCMP_OP_DO_MM(OP, CF_GARBAGE1, CF_GARBAGE2); \
	\
	printf("\n"); \
}

TEST_FCMP_FUNC(eq)
TEST_FCMP_FUNC(f)
TEST_FCMP_FUNC(le)
TEST_FCMP_FUNC(lt)

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_c_eq();
	test_c_f();
	test_c_le();
	test_c_lt();

	printf("-- TEST END\n");

	return 0;
}
