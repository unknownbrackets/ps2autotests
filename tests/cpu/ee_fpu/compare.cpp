#include <stdio.h>
#include <tamtypes.h>

union FloatBits {
	float f;
	u32 u;
};

static u32 CF_ZERO[1] = {0x00000000};
static u32 CF_NEGZERO[1] = {0x80000000};
static u32 CF_MAX[1] = {0x7FFFFFFF};
static u32 CF_MIN[1] = {0xFFFFFFFF};
static u32 CF_MAX_MANTISSA[1] = {0x3FFFFFFF};
static u32 CF_MAX_EXP[1] = {0x7F800001};
static u32 CF_MIN_EXP[1] = {0x00000001};
static u32 CF_NEGONE[1] = {0xBF800000};
static u32 CF_ONE[1] = {0x3F800000};
static u32 CF_GARBAGE1[4] = {0x00001337};
static u32 CF_GARBAGE2[4] = {0xDEADBEEF};

#define FCMP_OP_FUNC(OP) \
static inline void FCMP_##OP(register float &fd, const register float &fs) { \
	asm volatile ( \
		"c." #OP ".s %0, %1\n" \
		: "+f"(fd) : "f"(fs) \
	); \
}

template <u32 i>
static inline void SET_U32(register float &fd) {
	asm volatile (
		"lui $t6, %1\n"
		"ori $t6, $t6, %2\n"
		"mtc1 $t6, %0\n"
		: "+f"(fd) : "K"((i >> 16) & 0xFFFF), "K"(i & 0xFFFF) : "t6"
	);
}

static inline void SET_M(register float &fd, void *p) {
	fd = *(float *)p;
}

static inline void printFloatString(const FloatBits &bits) {
	// We want -0.0 and -NAN to show as negative.
	// So, let's just print the sign manually with an absolute value.
	FloatBits positive = bits;
	positive.u &= ~0x80000000;

	char sign = '+';
	if (bits.u & 0x80000000) {
		sign = '-';
	}

	printf("%c%0.2f", sign, positive.f);
}

static inline void PRINT_F(const register float &ft, bool newline) {
	static FloatBits result = {0};

	asm volatile (
		"swc1 %0, 0(%1)\n"
		: : "f"(ft), "r"((u32)&result.f)
	);

	printf("%08x/", result.u);
	printFloatString(result);
	if (newline) {
		printf("\n");
	}
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
