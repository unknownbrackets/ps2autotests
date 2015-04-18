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
	FFF_OP_DO_III(OP, 0x1337, 0x3F800000, 0x41C80000); \
	FFF_OP_DO_III(OP, 0x1337, 0x40A00000, 0x41C80000); \
	FFF_OP_DO_III(OP, 0x1337, 0x41C80000, 0x41C80000); \
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
	FF_OP_DO_II(OP, 0x1337, 0x41C80000); \
	 \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_ZERO); \
	FF_OP_DO_MM(OP, CF_GARBAGE1, CF_NEGZERO); \
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

TEST_FFF_FUNC(rsqrt)
TEST_FF_FUNC(sqrt)

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_rsqrt();
	test_sqrt();

	printf("-- TEST END\n");

	return 0;
}
