#include <stdio.h>
#include <tamtypes.h>

static u32 __attribute__((aligned(16))) C_GARBAGE1[4] = {0x1337, 0x1338, 0x1339, 0x133A};
static u32 __attribute__((aligned(16))) C_PATTERN[3][4] = {
	{0x45678123, 0x9ABCDEF0, 0xDEADBEEF, 0xC0DE1337},
	{0x23456789, 0xABCDEF01, 0xBEEFDEAD, 0xC0DEC0DE},
	{0x8899AABB, 0xCCDDEEFF, 0x00112233, 0x44556677},
};

#define LOAD_OP(OP, res, address, offset) \
	asm volatile ( \
		"move $t7, $0\n" \
		#OP " $t6, %2(%1)\n" \
		"nop\n" \
		"move %0, $t6\n" \
		"xor %0, %0, $t7\n" \
		: "+&r"(res) : "r"(address), "i"(offset) : "t6", "t7" \
	);
#define LOADZ_OP(OP, res, address, offset) \
	asm volatile ( \
		#OP " $0, %2(%1)\n" \
		"nop\n" \
		"ori %0, $0, 0\n" \
		: "+&r"(res) : "r"(address), "i"(offset) \
	);

#define STORE_OP(OP, val, address, offset) \
	asm volatile ( \
		"move $t6, %0\n" \
		"move $t7, $0\n" \
		#OP " $t6, %2(%1)\n" \
		"nop\n" \
		: : "r"(val), "r"(address), "i"(offset) : "t6", "t7" \
	);

#define SET_U32(rd, i) \
	asm volatile ( \
		"lui %0, %1\n" \
		"ori %0, %0, %2\n" \
		: "+r"(rd) : "K"((i >> 16) & 0xFFFF), "K"(i & 0xFFFF) \
	);

#define SET_M(rd, p) \
	rd = *(u32 *)(p);

static inline void PRINT_R(const register u32 rt, int newline) {
	printf("%08x", rt);
	if (newline) {
		printf("\n");
	}
}

static inline void PRINT_M(const void *m, int newline) {
	const u32 *result = (const u32 *)m;

	printf("%08x %08x", result[0], result[1]);
	if (newline) {
		printf("\n");
	}
}

#define LOAD_OP_DO_I(OP, t, s) \
	SET_U32(rt, t); \
	LOAD_OP(OP, rt, C_PATTERN[1], s); \
	printf("  %s %+d: ", #OP, (u32)s); PRINT_R(rt, 1);

#define LOAD_OP_DO_M(OP, t, s) \
	SET_M(rt, t); \
	LOAD_OP(OP, rt, C_PATTERN[1], s); \
	printf("  %s +%d: ", #OP, (u32)s); PRINT_R(rt, 1);

#define TEST_LOAD_FUNC(OP) \
static void test_##OP() { \
	register u32 rt; \
	SET_M(rt, C_GARBAGE1); \
	 \
	printf("%s:\n", #OP); \
	 \
	LOAD_OP_DO_I(OP, 0xABCD4321, 0); \
	LOAD_OP_DO_I(OP, 0xABCD4321, 4); \
	LOAD_OP_DO_I(OP, 0xABCD4321, -4); \
	 \
	LOAD_OP_DO_M(OP, C_GARBAGE1, 0); \
	LOAD_OP_DO_M(OP, C_GARBAGE1, 4); \
	LOAD_OP_DO_M(OP, C_GARBAGE1, -4); \
	\
	SET_M(rt, C_GARBAGE1); \
	LOADZ_OP(OP, rt, C_PATTERN[1], 0); \
	printf("  %s -> $0: ", #OP); PRINT_R(rt, 1); \
	printf("\n"); \
}

#define LOADLR_OP_DO_I(LOP, ROP, t, l, r) \
	SET_U32(rt, t); \
	LOAD_OP(LOP, rt, C_PATTERN[1], l); \
	LOAD_OP(ROP, rt, C_PATTERN[1], r); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_R(rt, 1);

#define LOADLR_OP_DO_M(LOP, ROP, t, l, r) \
	SET_M(rt, t); \
	LOAD_OP(LOP, rt, C_PATTERN[1], l); \
	LOAD_OP(ROP, rt, C_PATTERN[1], r); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_R(rt, 1);

#define TEST_LOADLR_FUNC(LOP, ROP, SIZE) \
static void test_##LOP##_##ROP() { \
	register u32 rt; \
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
	LOADZ_OP(LOP, rt, C_PATTERN[1], 0); \
	printf("  %s -> $0: ", #LOP); PRINT_R(rt, 1); \
	SET_M(rt, C_GARBAGE1); \
	LOADZ_OP(ROP, rt, C_PATTERN[1], 0); \
	printf("  %s -> $0: ", #ROP); PRINT_R(rt, 1); \
	printf("\n"); \
}

#define STORE_OP_DO_I(OP, t, s) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_U32(rt, t); \
	STORE_OP(OP, rt, &buffer[1], s); \
	printf("  %s %+d: ", #OP, (u32)s); PRINT_M(&buffer[1 + (s) / 4], 1);

#define STORE_OP_DO_M(OP, t, s) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_M(rt, t); \
	STORE_OP(OP, rt, &buffer[1], s); \
	printf("  %s +%d: ", #OP, (u32)s); PRINT_M(&buffer[1 + (s) / 4], 1);

#define TEST_STORE_FUNC(OP) \
static void test_##OP() { \
	u32 buffer[6]; \
	register u32 rt; \
	SET_M(rt, C_GARBAGE1); \
	 \
	printf("%s:\n", #OP); \
	 \
	STORE_OP_DO_I(OP, 0xABCD4321, 0); \
	STORE_OP_DO_I(OP, 0xABCD4321, 4); \
	STORE_OP_DO_I(OP, 0xABCD4321, -4); \
	 \
	STORE_OP_DO_M(OP, C_GARBAGE1, 0); \
	STORE_OP_DO_M(OP, C_GARBAGE1, 4); \
	STORE_OP_DO_M(OP, C_GARBAGE1, -4); \
	\
	printf("\n"); \
}

#define STORELR_OP_DO_I(LOP, ROP, t, l, r) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_U32(rt, t); \
	STORE_OP(LOP, rt, &buffer[1], l); \
	STORE_OP(ROP, rt, &buffer[1], r); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_M(&buffer[1 + (l) / 4], 1);

#define STORELR_OP_DO_M(LOP, ROP, t, l, r) \
	memcpy(buffer, C_PATTERN, sizeof(buffer)); \
	SET_M(rt, t); \
	STORE_OP(LOP, rt, &buffer[1], l); \
	STORE_OP(ROP, rt, &buffer[1], r); \
	printf("  %s %+d/%s %+d: ", #LOP, (u32)l, #ROP, (u32)r); PRINT_M(&buffer[1 + (l) / 4], 1);

#define TEST_STORELR_FUNC(LOP, ROP, SIZE) \
static void test_##LOP##_##ROP() { \
	u32 buffer[6]; \
	register u32 rt; \
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
TEST_LOAD_FUNC(lh)
TEST_LOAD_FUNC(lhu)
TEST_LOAD_FUNC(lw)
TEST_LOADLR_FUNC(lwl, lwr, 4)

TEST_STORE_FUNC(sb)
TEST_STORE_FUNC(sd)
TEST_STORE_FUNC(sh)
TEST_STORE_FUNC(sw)
TEST_STORELR_FUNC(swl, swr, 4)

int _start(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_lb();
	test_lbu();
	test_ld();
	test_lh();
	test_lhu();
	test_lw();
	test_lwl_lwr();

	test_sb();
	test_sd();
	test_sh();
	test_sw();
	test_swl_swr();

	printf("-- TEST END\n");

	return 0;
}
