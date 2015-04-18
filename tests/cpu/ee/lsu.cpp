#include <common.h>
#include <string.h>

static u32 __attribute__((aligned(16))) C_GARBAGE1[4] = {0x1337, 0x1338, 0x1339, 0x133A};
static u32 __attribute__((aligned(16))) C_PATTERN[3][4] = {
	{0x45678123, 0x9ABCDEF0, 0xDEADBEEF, 0xC0DE1337},
	{0x23456789, 0xABCDEF01, 0xBEEFDEAD, 0xC0DEC0DE},
	{0x8899AABB, 0xCCDDEEFF, 0x00112233, 0x44556677},
};

#define LOAD_OP_FUNC(OP) \
template <int offset> \
inline void LOAD_##OP(register u128 &res, void *address) { \
	asm volatile ( \
		#OP " %0, %2(%1)\n" \
		"sync\n" \
		: "+&r"(res) : "r"(address), "i"(offset) \
	); \
} \
template <int offset> \
inline void LOADZ_##OP(register u128 &res, void *address) { \
	asm volatile ( \
		#OP " $0, %2(%1)\n" \
		"sync\n" \
		"ori %0, $0, 0\n" \
		: "+&r"(res) : "r"(address), "i"(offset) \
	); \
}

#define STORE_OP_FUNC(OP) \
template <int offset> \
inline void STORE_##OP(const register u128 &val, void *address) { \
	asm volatile ( \
		#OP " %0, %2(%1)\n" \
		"sync\n" \
		: : "r"(val), "r"(address), "i"(offset) \
	); \
}

template <u32 i>
static inline void SET_U32(register u128 &rd) {
	asm volatile (
		"lui %0, %1\n"
		"ori %0, %0, %2\n"
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

static inline void PRINT_M(const void *m, bool newline) {
	const u32 *result = (const u32 *)m;

	printf("%08x %08x %08x %08x", result[3], result[2], result[1], result[0]);
	if (newline) {
		printf("\n");
	}
}

#define LOAD_OP_DO_I(OP, t, s) \
	SET_U32<t>(rt); \
	LOAD_##OP<s>(rt, C_PATTERN[1]); \
	printf("  %s %+d: ", #OP, (u32)s); PRINT_R(rt, true);

#define LOAD_OP_DO_M(OP, t, s) \
	SET_M(rt, t); \
	LOAD_##OP<s>(rt, C_PATTERN[1]); \
	printf("  %s +%d: ", #OP, (u32)s); PRINT_R(rt, true);

#define TEST_LOAD_FUNC(OP) \
LOAD_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rt; \
	SET_M(rt, C_GARBAGE1); \
	 \
	printf("%s:\n", #OP); \
	 \
	LOAD_OP_DO_I(OP, 0xABCD4321, 0); \
	LOAD_OP_DO_I(OP, 0xABCD4321, 16); \
	LOAD_OP_DO_I(OP, 0xABCD4321, -16); \
	 \
	LOAD_OP_DO_M(OP, C_GARBAGE1, 0); \
	LOAD_OP_DO_M(OP, C_GARBAGE1, 16); \
	LOAD_OP_DO_M(OP, C_GARBAGE1, -16); \
	\
	SET_M(rt, C_GARBAGE1); \
	LOADZ_##OP<0>(rt, C_PATTERN[1]); \
	printf("  %s -> $0: ", #OP); PRINT_R(rt, true); \
	printf("\n"); \
}

#define LOADLR_OP_DO_I(LOP, ROP, t, l, r) \
	SET_U32<t>(rt); \
	LOAD_##LOP<l>(rt, C_PATTERN[1]); \
	LOAD_##ROP<r>(rt, C_PATTERN[1]); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_R(rt, true);

#define LOADLR_OP_DO_M(LOP, ROP, t, l, r) \
	SET_M(rt, t); \
	LOAD_##LOP<l>(rt, C_PATTERN[1]); \
	LOAD_##ROP<r>(rt, C_PATTERN[1]); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_R(rt, true);

#define TEST_LOADLR_FUNC(LOP, ROP, SIZE) \
LOAD_OP_FUNC(LOP); \
LOAD_OP_FUNC(ROP); \
static void test_##LOP##_##ROP() { \
	register u128 rt; \
	SET_M(rt, C_GARBAGE1); \
	 \
	printf("%s / %s:\n", #LOP, #ROP); \
	 \
	LOAD_OP_DO_I(LOP, 0xABCD4321, 0); \
	LOAD_OP_DO_I(ROP, 0xABCD4321, 0); \
	LOAD_OP_DO_I(LOP, 0xABCD4321, 1); \
	LOAD_OP_DO_I(ROP, 0xABCD4321, 1); \
	LOADLR_OP_DO_I(LOP, ROP, 0xABCD4321, 0, SIZE - 1); \
	 \
	LOAD_OP_DO_M(LOP, C_GARBAGE1, 0); \
	LOAD_OP_DO_M(ROP, C_GARBAGE1, 0); \
	LOAD_OP_DO_M(LOP, C_GARBAGE1, 1); \
	LOAD_OP_DO_M(ROP, C_GARBAGE1, 1); \
	LOADLR_OP_DO_M(LOP, ROP, C_GARBAGE1, 0, SIZE - 1); \
	\
	SET_M(rt, C_GARBAGE1); \
	LOADZ_##LOP<0>(rt, C_PATTERN[1]); \
	printf("  %s -> $0: ", #LOP); PRINT_R(rt, true); \
	SET_M(rt, C_GARBAGE1); \
	LOADZ_##ROP<0>(rt, C_PATTERN[1]); \
	printf("  %s -> $0: ", #ROP); PRINT_R(rt, true); \
	printf("\n"); \
}

#define STORE_OP_DO_I(OP, t, s) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_U32<t>(rt); \
	STORE_##OP<s>(rt, &buffer[1]); \
	printf("  %s %+d: ", #OP, (u32)s); PRINT_M(&buffer[1 + (s) / 16], true);

#define STORE_OP_DO_M(OP, t, s) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_M(rt, t); \
	STORE_##OP<s>(rt, &buffer[1]); \
	printf("  %s +%d: ", #OP, (u32)s); PRINT_M(&buffer[1 + (s) / 16], true);

#define TEST_STORE_FUNC(OP) \
STORE_OP_FUNC(OP); \
static void test_##OP() { \
	u128 buffer[3]; \
	register u128 rt; \
	SET_M(rt, C_GARBAGE1); \
	 \
	printf("%s:\n", #OP); \
	 \
	STORE_OP_DO_I(OP, 0xABCD4321, 0); \
	STORE_OP_DO_I(OP, 0xABCD4321, 16); \
	STORE_OP_DO_I(OP, 0xABCD4321, -16); \
	 \
	STORE_OP_DO_M(OP, C_GARBAGE1, 0); \
	STORE_OP_DO_M(OP, C_GARBAGE1, 16); \
	STORE_OP_DO_M(OP, C_GARBAGE1, -16); \
	\
	printf("\n"); \
}

#define STORELR_OP_DO_I(LOP, ROP, t, l, r) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_U32<t>(rt); \
	STORE_##LOP<l>(rt, &buffer[1]); \
	STORE_##ROP<r>(rt, &buffer[1]); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_M(&buffer[1 + (l) / 16], true);

#define STORELR_OP_DO_M(LOP, ROP, t, l, r) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_M(rt, t); \
	STORE_##LOP<l>(rt, &buffer[1]); \
	STORE_##ROP<r>(rt, &buffer[1]); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_M(&buffer[1 + (l) / 16], true);

#define TEST_STORELR_FUNC(LOP, ROP, SIZE) \
STORE_OP_FUNC(LOP); \
STORE_OP_FUNC(ROP); \
static void test_##LOP##_##ROP() { \
	u128 buffer[3]; \
	register u128 rt; \
	SET_M(rt, C_GARBAGE1); \
	 \
	printf("%s / %s:\n", #LOP, #ROP); \
	 \
	STORE_OP_DO_I(LOP, 0xABCD4321, 0); \
	STORE_OP_DO_I(ROP, 0xABCD4321, 0); \
	STORE_OP_DO_I(LOP, 0xABCD4321, 1); \
	STORE_OP_DO_I(ROP, 0xABCD4321, 1); \
	STORELR_OP_DO_I(LOP, ROP, 0xABCD4321, 0, SIZE - 1); \
	 \
	STORE_OP_DO_M(LOP, C_GARBAGE1, 0); \
	STORE_OP_DO_M(ROP, C_GARBAGE1, 0); \
	STORE_OP_DO_M(LOP, C_GARBAGE1, 1); \
	STORE_OP_DO_M(ROP, C_GARBAGE1, 1); \
	STORELR_OP_DO_M(LOP, ROP, C_GARBAGE1, 0, SIZE - 1); \
	\
	printf("\n"); \
}

TEST_LOAD_FUNC(lb)
TEST_LOAD_FUNC(lbu)
TEST_LOAD_FUNC(ld)
TEST_LOADLR_FUNC(ldl, ldr, 8)
TEST_LOAD_FUNC(lh)
TEST_LOAD_FUNC(lhu)
TEST_LOAD_FUNC(lw)
TEST_LOADLR_FUNC(lwl, lwr, 4)
TEST_LOAD_FUNC(lwu)
TEST_LOAD_FUNC(lq)

TEST_STORE_FUNC(sb)
TEST_STORE_FUNC(sd)
TEST_STORELR_FUNC(sdl, sdr, 8)
TEST_STORE_FUNC(sh)
TEST_STORE_FUNC(sw)
TEST_STORELR_FUNC(swl, swr, 4)
TEST_STORE_FUNC(sq)

static void test_pref() {
	// No guarantees that it will even prefetch, let's just make sure it doesn't wig out.
	register u32 res = 0;
	asm volatile (
		"pref 0, 0(%1)\n"
		"ori %0, $0, 0\n"
		: "+&r"(res) : "r"(C_PATTERN)
	);
	printf("pref: %s\n", res == 0 ? "OK" : "FAIL");
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_lb();
	test_lbu();
	test_ld();
	test_ldl_ldr();
	test_lh();
	test_lhu();
	test_lw();
	test_lwl_lwr();
	test_lwu();
	test_lq();

	test_sb();
	test_sd();
	test_sdl_sdr();
	test_sh();
	test_sw();
	test_swl_swr();
	test_sq();

	test_pref();

	printf("-- TEST END\n");

	return 0;
}
