#include <common-ee.h>

static u32 __attribute__((aligned(16), unused)) C_ZERO[4] = {0, 0, 0, 0};
static u32 __attribute__((aligned(16), unused)) C_S16_MAX[4] = {0x7FFF, 0, 0, 0};
static u32 __attribute__((aligned(16), unused)) C_S16_MIN[4] = {0xFFFF8000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16), unused)) C_S32_MAX[4] = {0x7FFFFFFF, 0, 0, 0};
static u32 __attribute__((aligned(16), unused)) C_S32_MIN[4] = {0x80000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16), unused)) C_S64_MAX[4] = {0xFFFFFFFF, 0x7FFFFFFF, 0, 0 };
static u32 __attribute__((aligned(16), unused)) C_S64_MIN[4] = {0, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16), unused)) C_NEGONE[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16), unused)) C_ONE[4] = {1, 0, 0, 0};
static u32 __attribute__((aligned(16), unused)) C_GARBAGE1[4] = {0x1337, 0x1338, 0x1339, 0x133A};
static u32 __attribute__((aligned(16), unused)) C_GARBAGE2[4] = {0xDEADBEEF, 0xDEADBEEE, 0xDEADBEED, 0xDEADBEEC};
static u32 __attribute__((aligned(16), unused)) C_P_S8_A[4] = {0x80808080, 0xFFFFFFFF, 0x12345678, 0x7F7F7F7F};
static u32 __attribute__((aligned(16), unused)) C_P_S8_B[4] = {0x80FF127F, 0xFF807F34, 0x567F80FF, 0x7F78FF80};
static u32 __attribute__((aligned(16), unused)) C_P_S16_A[4] = {0x80008000, 0xFFFFFFFF, 0x12345678, 0x7FFF7FFF};
static u32 __attribute__((aligned(16), unused)) C_P_S16_B[4] = {0x8000FFFF, 0x12347FFF, 0xFFFF8000, 0x7FFF5678};
static u32 __attribute__((aligned(16), unused)) C_P_S32_A[4] = {0x80000000, 0xFFFFFFFF, 0x12345678, 0x7FFFFFFF};
static u32 __attribute__((aligned(16), unused)) C_P_S32_B[4] = {0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0x12345678};
static u32 __attribute__((aligned(16), unused)) C_P_S32_C[4] = {0x80000000, 0xFFFFFFFF, 0x12345678, 0x7FFFFFFF};
static u32 __attribute__((aligned(16), unused)) C_P_S32_D[4] = {0x7FFFFFFF, 0x12345678, 0xFFFFFFFF, 0x80000000};
static u32 __attribute__((aligned(16), unused)) C_P_S32_E[4] = {0x00000000, 0x7FFFFFFF, 0xFFFFFFFF, 0x80000000};
static u32 __attribute__((aligned(16), unused)) C_P_S32_F[4] = {0xFFFFFFFF, 0x80000000, 0x00000000, 0x7FFFFFFF};

template <u32 i>
static inline void SET_U32(register u128 &rd) {
	asm volatile (
		"lui %0, %1\n"
		"ori %0, %0, %2\n"
		"pcpyld %0, %0, %0\n"
		: "+r"(rd) : "K"((i >> 16) & 0xFFFF), "K"(i & 0xFFFF)
	);
}

static inline void SET_M(register u128 &rd, u32 *p) {
	rd = *(vu128 *)p;
}

static inline void PRINT_R(const register u128 &rt, bool newline) {
	static u32 __attribute__((aligned(16))) result[4] = {0, 0, 0, 0};
	*(vu128 *)result = rt;

	printf("%08x %08x %08x %08x", result[3], result[2], result[1], result[0]);
	if (newline) {
		printf("\n");
	}
}
