#include "shared.h"

#define FFF_OP_FUNC(OP) \
static inline void FFF_##OP(register float &fd, const register float &fs, const register float &ft) { \
	asm volatile ( \
		#OP ".s %0, %1, %2\n" \
		: "+f"(fd) : "f"(fs), "f"(ft) \
	); \
}

#define FF_OP_FUNC(OP) \
static inline void FF_##OP(register float &fd, const register float &fs) { \
	asm volatile ( \
		#OP ".s %0, %1\n" \
		: "+f"(fd) : "f"(fs) \
	); \
}

template <u32 i>
static inline void SET_ACC_U32() {
	static const float negz = -0.0f;

	asm volatile (
		"lui $t6, %0\n"
		"ori $t6, $t6, %1\n"
		"mtc1 $t6, $f4\n"
		// This sets ACC to $f4 + -0.  We use negative to avoid changing $f4 = -0 to ACC = +0.
		"adda.s $f4, %2\n"
		: : "K"((i >> 16) & 0xFFFF), "K"(i & 0xFFFF), "f"(negz) : "t6", "$f4"
	);
}

static inline void SET_ACC_M(void *p) {
	static const float negz = -0.0f;

	asm volatile (
		"lwc1 $f4, 0(%0)\n"
		// This sets ACC to $f4 + -0.  We use negative to avoid changing $f4 = -0 to ACC = +0.
		"adda.s $f4, %1\n"
		: : "r"(p), "f"(negz) : "$f4"
	);
}

static inline void PRINT_ACC(bool newline) {
	static FloatBits result = {0};
	static const float negz = -0.0f;
	static const float posz = 0.0f;

	asm volatile (
		// To grab ACC, we use madd.s 0, 0 -> fd = ACC + 0 * 0.
		// We use negative zero to avoid changing the sign of ACC (note that it's a multiply, -0 * -0 = +0.)
		"madd.s $f4, %0, %1\n"
		"swc1 $f4, 0(%2)\n"
		: : "f"(negz), "f"(posz), "r"((u32)&result.f) : "$f4"
	);

	printf("%08x/", result.u);
	printFloatString(result);
	if (newline) {
		printf("\n");
	}
}

#define FFF_OP_DO_III(OP, d, s, t) \
	SET_U32<d>(fd); SET_U32<s>(fs); SET_U32<t>(ft); \
	FFF_##OP(fd, fs, ft); \
	printf("  %s ", #OP); \
	PRINT_F(fs, false); \
	printf(", "); \
	PRINT_F(ft, false); \
	printf(": "); \
	PRINT_F(fd, true);

#define FFF_OP_DO_MMM(OP, d, s, t) \
	SET_M(fd, d); SET_M(fs, s); SET_M(ft, t); \
	FFF_##OP(fd, fs, ft); \
	printf("  %s %s, %s: ", #OP, #s, #t); PRINT_F(fd, true);

#define TEST_FFF_FUNC(OP) \
FFF_OP_FUNC(OP); \
static void test_##OP() { \
	register float fd, fs, ft; \
	 \
	printf("%s:\n", #OP); \
	 \
	FFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x00000000); \
	FFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x80000000); \
	FFF_OP_DO_III(OP, 0x1337, 0x80000000, 0x00000000); \
	FFF_OP_DO_III(OP, 0x1337, 0x80000000, 0x80000000); \
	FFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x3F800000); \
	FFF_OP_DO_III(OP, 0x1337, 0x3F800000, 0x3F800000); \
	FFF_OP_DO_III(OP, 0x1337, 0x3F800000, 0x00000000); \
	FFF_OP_DO_III(OP, 0x1337, 0x40000000, 0x40000000); \
	FFF_OP_DO_III(OP, 0x1337, 0x40400000, 0x3F800000); \
	FFF_OP_DO_III(OP, 0x1337, 0x40400000, 0x40400000); \
	FFF_OP_DO_III(OP, 0x1337, 0x7FFFFFFF, 0x7FFFFFFF); \
	FFF_OP_DO_III(OP, 0x1337, 0x7FFFFFFF, 0xFFFFFFFF); \
	FFF_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0x7FFFFFFF); \
	FFF_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0xFFFFFFFF); \
	FFF_OP_DO_III(OP, 0x1337, 0x7FFFFFFF, 0x00000000); \
	FFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x7FFFFFFF); \
	FFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x7F800000); \
	FFF_OP_DO_III(OP, 0x1337, 0x7F800000, 0x7F800000); \
	FFF_OP_DO_III(OP, 0x1337, 0xFF800000, 0x7F800000); \
	 \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ZERO, CF_ZERO); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ZERO, CF_NEGZERO); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_ONE); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_NEGONE); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX_MANTISSA, CF_ONE); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX_EXP, CF_ONE); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MIN_EXP, CF_ONE); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MAX_MANTISSA); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MAX_EXP); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MIN_EXP); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX, CF_ONE); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MIN, CF_ONE); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MAX); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MIN); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX, CF_MAX); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MIN, CF_MIN); \
	FFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_GARBAGE1, CF_GARBAGE2); \
	\
	printf("\n"); \
}

#define FF_OP_DO_II(OP, d, s) \
	SET_U32<d>(fd); SET_U32<s>(fs); \
	FF_##OP(fd, fs); \
	printf("  %s ", #OP); \
	PRINT_F(fs, false); \
	printf(": "); \
	PRINT_F(fd, true);

#define FF_OP_DO_MM(OP, d, s) \
	SET_M(fd, d); SET_M(fs, s); \
	FF_##OP(fd, fs); \
	printf("  %s %s: ", #OP, #s); PRINT_F(fd, true);

#define TEST_FF_FUNC(OP) \
FF_OP_FUNC(OP); \
static void test_##OP() { \
	register float fd, fs; \
	 \
	printf("%s:\n", #OP); \
	 \
	FF_OP_DO_II(OP, 0x1337, 0x00000000); \
	FF_OP_DO_II(OP, 0x1337, 0x80000000); \
	FF_OP_DO_II(OP, 0x1337, 0x3F800000); \
	FF_OP_DO_II(OP, 0x1337, 0x40000000); \
	FF_OP_DO_II(OP, 0x1337, 0x40400000); \
	FF_OP_DO_II(OP, 0x1337, 0x7FFFFFFF); \
	FF_OP_DO_II(OP, 0x1337, 0xFFFFFFFF); \
	FF_OP_DO_II(OP, 0x1337, 0x7F800000); \
	FF_OP_DO_II(OP, 0x1337, 0xFF800000); \
	 \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_ZERO); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_ONE); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_NEGONE); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_MAX_MANTISSA); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_MAX_EXP); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_MIN_EXP); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_MAX); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_MIN); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_GARBAGE1); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_GARBAGE2); \
	\
	printf("\n"); \
}

#define AFF_OP_DO_III(OP, d, s, t) \
	SET_ACC_U32<d>(); \
	SET_U32<s>(fs); SET_U32<t>(ft); \
	FF_##OP(fs, ft); \
	printf("  %s ", #OP); \
	PRINT_F(fs, false); \
	printf(", "); \
	PRINT_F(ft, false); \
	printf(": "); \
	PRINT_ACC(true);

#define AFF_OP_DO_MMM(OP, d, s, t) \
	SET_ACC_M(d); \
	SET_M(fs, s); SET_M(ft, t); \
	FF_##OP(fs, ft); \
	printf("  %s %s, %s: ", #OP, #s, #t); \
	PRINT_ACC(true);

#define TEST_AFF_FUNC(OP) \
FF_OP_FUNC(OP); \
static void test_##OP() { \
	register float fs, ft; \
	 \
	printf("%s:\n", #OP); \
	 \
	AFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x00000000); \
	AFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x80000000); \
	AFF_OP_DO_III(OP, 0x1337, 0x80000000, 0x00000000); \
	AFF_OP_DO_III(OP, 0x1337, 0x80000000, 0x80000000); \
	AFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x3F800000); \
	AFF_OP_DO_III(OP, 0x1337, 0x3F800000, 0x3F800000); \
	AFF_OP_DO_III(OP, 0x1337, 0x3F800000, 0x00000000); \
	AFF_OP_DO_III(OP, 0x1337, 0x40000000, 0x40000000); \
	AFF_OP_DO_III(OP, 0x1337, 0x40400000, 0x3F800000); \
	AFF_OP_DO_III(OP, 0x1337, 0x40400000, 0x40400000); \
	AFF_OP_DO_III(OP, 0x1337, 0x7FFFFFFF, 0x7FFFFFFF); \
	AFF_OP_DO_III(OP, 0x1337, 0x7FFFFFFF, 0xFFFFFFFF); \
	AFF_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0x7FFFFFFF); \
	AFF_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0xFFFFFFFF); \
	AFF_OP_DO_III(OP, 0x1337, 0x7FFFFFFF, 0x00000000); \
	AFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x7FFFFFFF); \
	AFF_OP_DO_III(OP, 0x1337, 0x00000000, 0x7F800000); \
	AFF_OP_DO_III(OP, 0x1337, 0x7F800000, 0x7F800000); \
	AFF_OP_DO_III(OP, 0x1337, 0xFF800000, 0x7F800000); \
	 \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ZERO, CF_ZERO); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ZERO, CF_NEGZERO); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_ONE); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_NEGONE); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX_MANTISSA, CF_ONE); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX_EXP, CF_ONE); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MIN_EXP, CF_ONE); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MAX_MANTISSA); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MAX_EXP); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MIN_EXP); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX, CF_ONE); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MIN, CF_ONE); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MAX); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_ONE, CF_MIN); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MAX, CF_MAX); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_MIN, CF_MIN); \
	AFF_OP_DO_MMM(OP, CF_GARBAGE1, CF_GARBAGE1, CF_GARBAGE2); \
	\
	printf("\n"); \
}

TEST_FF_FUNC(abs)
TEST_FFF_FUNC(add)
TEST_AFF_FUNC(adda)
TEST_FFF_FUNC(max)
TEST_FFF_FUNC(min)
TEST_FF_FUNC(mov)
TEST_FF_FUNC(neg)
TEST_FFF_FUNC(sub)
TEST_AFF_FUNC(suba)

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_abs();
	test_add();
	test_adda();
	test_max();
	test_min();
	test_mov();
	test_neg();
	test_sub();
	test_suba();

	printf("-- TEST END\n");

	return 0;
}
