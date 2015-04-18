#include <common.h>

union FloatBits {
	float f;
	u32 u;
};

static u32 __attribute__((unused)) CF_ZERO[1] = {0x00000000};
static u32 __attribute__((unused)) CF_NEGZERO[1] = {0x80000000};
static u32 __attribute__((unused)) CF_MAX[1] = {0x7FFFFFFF};
static u32 __attribute__((unused)) CF_MIN[1] = {0xFFFFFFFF};
static u32 __attribute__((unused)) CF_MAX_MANTISSA[1] = {0x3FFFFFFF};
static u32 __attribute__((unused)) CF_MAX_EXP[1] = {0x7F800001};
static u32 __attribute__((unused)) CF_MIN_EXP[1] = {0x00000001};
static u32 __attribute__((unused)) CF_NEGONE[1] = {0xBF800000};
static u32 __attribute__((unused)) CF_ONE[1] = {0x3F800000};
static u32 __attribute__((unused)) CF_GARBAGE1[4] = {0x00001337};
static u32 __attribute__((unused)) CF_GARBAGE2[4] = {0xDEADBEEF};

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

static inline void PRINT_R(const register u128 &rt, bool newline) {
	static u32 __attribute__((aligned(16))) result[4] = {0, 0, 0, 0};
	*(vu128 *)result = rt;

	printf("%08x %08x %08x %08x", result[3], result[2], result[1], result[0]);
	if (newline) {
		printf("\n");
	}
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
