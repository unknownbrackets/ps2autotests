#include "shared.h"

#define FF_OP_FUNC(OP, IDENT) \
static inline void FF_##IDENT(register float &fd, const register float &fs) { \
	asm volatile ( \
		OP " %0, %1\n" \
		: "+f"(fd) : "f"(fs) \
	); \
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

#define FF_OP_DO_II(OP, IDENT, d, s) \
	SET_U32<d>(fd); SET_U32<s>(fs); \
	FF_##IDENT(fd, fs); \
	printf("  %s ", OP); \
	PRINT_F(fs, false); \
	printf(": "); \
	PRINT_F(fd, true);

#define FF_OP_DO_MM(OP, IDENT, d, s) \
	SET_M(fd, d); SET_M(fs, s); \
	FF_##IDENT(fd, fs); \
	printf("  %s %s: ", OP, #s); PRINT_F(fd, true);

#define TEST_FF_FUNC(OP, IDENT) \
FF_OP_FUNC(OP, IDENT); \
static void test_##IDENT() { \
	register float fd, fs; \
	 \
	printf("%s:\n", OP); \
	 \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x00000000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x80000000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x3F800000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x40000000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x40400000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x7FFFFFFF); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0xFFFFFFFF); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x7F800000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0xFF800000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x00000005); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x0000FFFF); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x4F000000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0xCF000000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x4EFFFFFF); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0xCEFFFFFF); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x3FBEB852); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x3FC00000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0x3FC147AE); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0xBFBEB852); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0xBFC00000); \
	FF_OP_DO_II(OP, IDENT, 0x1337, 0xBFC147AE); \
	 \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_ZERO); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_NEGZERO); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_ONE); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_NEGONE); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_MAX_MANTISSA); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_MAX_EXP); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_MIN_EXP); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_MAX); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_MIN); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_GARBAGE1); \
	FF_OP_DO_MM(OP, IDENT, CF_GARBAGE1, CF_GARBAGE2); \
	\
	printf("\n"); \
}

TEST_FF_FUNC("cvt.w.s", cvtws)
TEST_FF_FUNC("cvt.s.w", cvtsw)

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_cvtws();
	test_cvtsw();

	printf("-- TEST END\n");

	return 0;
}
