#include "shared.h"

struct HILOREGS {
	u64 hi;
	u64 lo;
	u64 hi1;
	u64 lo1;
};

static HILOREGS __attribute__((aligned(16))) C_HILO = {0x0123456789ABCDEF, 0x123456789ABCDEF0, 0x23456789ABCDEF01, 0x456789ABCDEF0123};

static u32 __attribute__((aligned(16))) C_ONEONEONEONE[4] = {1, 1, 1, 1};
static u32 __attribute__((aligned(16))) C_TWOTWOTWOTWO[4] = {2, 2, 2, 2};

#define RRHL_OP_FUNC(OP) \
static inline void RRHL_##OP(const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " %0, %1\n" \
		: : "r"(rs), "r"(rt) \
	); \
}

// gas automatically inserts a divide by zero trap, annoyingly.
static inline void RRHL_div(const register u128 &rs, const register u128 &rt) {
	asm volatile (
		"por $t8, $0, %0\n"
		"por $t9, $0, %1\n"
		".word 0x0319001A\n" // "div $t8, $t9\n"
		: : "r"(rs), "r"(rt) : "t8", "t9"
	);
}

static inline void RRHL_divu(const register u128 &rs, const register u128 &rt) {
	asm volatile (
		"por $t8, $0, %0\n"
		"por $t9, $0, %1\n"
		".word 0x0319001B\n" // "divu $t8, $t9\n"
		: : "r"(rs), "r"(rt) : "t8", "t9"
	);
}

#define RRRHL_OP_FUNC(OP) \
static inline void RRRHL_##OP(register u128 &rd, const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " %0, %1, %2\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
} \
static inline void ZRRHL_##OP(register u128 &rd, const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " $0, %1, %2\n" \
		"ori %0, $0, 0\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
}

#define RHL_OP_FUNC(OP) \
static inline void RHL_##OP(register u128 &rd) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " %0\n" \
		: "+r"(rd) \
	); \
} \
static inline void ZHL_##OP(register u128 &rd) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " $0\n" \
		"ori %0, $0, 0\n" \
		: "+r"(rd) \
	); \
}

static inline void GET_HILO(HILOREGS &regs) {
	asm volatile (
		"mfhi %0\n"
		"mflo %1\n"
		"mfhi1 %2\n"
		"mflo1 %3\n"
		: "+r"(regs.hi), "+r"(regs.lo), "+r"(regs.hi1), "+r"(regs.lo1)
	);
}

static inline void SET_HILO(const HILOREGS &regs) {
	asm volatile (
		"mthi %0\n"
		"mtlo %1\n"
		"mthi1 %2\n"
		"mtlo1 %3\n"
		: : "r"(regs.hi), "r"(regs.lo), "r"(regs.hi1), "r"(regs.lo1)
	);
}

static inline void PRINT_HILO(const HILOREGS &regs, bool newline) {
	printf("H: %016lx %016lx L: %016lx %016lx", regs.hi, regs.hi1, regs.lo, regs.lo1);
	if (newline) {
		printf("\n");
	}
}

#define RRHL_OP_DO_II(OP, s, t) \
	SET_U32<s>(rs); SET_U32<t>(rt); \
	SET_HILO(C_HILO); \
	RRHL_##OP(rs, rt); \
	GET_HILO(hilo); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)rt); PRINT_HILO(hilo, true);

#define RRHL_OP_DO_MM(OP, s, t) \
	SET_M(rs, s); SET_M(rt, t); \
	SET_HILO(C_HILO); \
	RRHL_##OP(rs, rt); \
	GET_HILO(hilo); \
	printf("  %s %s, %s: ", #OP, #s, #t); PRINT_HILO(hilo, true);

#define TEST_RRHL_FUNC(OP) \
static void test_##OP() { \
	register u128 rs, rt; \
	HILOREGS hilo; \
	 \
	printf("%s:\n", #OP); \
	 \
	RRHL_OP_DO_II(OP, 0, 0); \
	RRHL_OP_DO_II(OP, 0, 1); \
	RRHL_OP_DO_II(OP, 1, 0); \
	RRHL_OP_DO_II(OP, 1, 1); \
	RRHL_OP_DO_II(OP, 2, 2); \
	RRHL_OP_DO_II(OP, 53, 2); \
	RRHL_OP_DO_II(OP, 53, 0); \
	RRHL_OP_DO_II(OP, 0xFFFFFFFF, 1); \
	RRHL_OP_DO_II(OP, 1, 0xFFFFFFFF); \
	RRHL_OP_DO_II(OP, 0xFFFFFFFF, 0xFFFFFFFF); \
	RRHL_OP_DO_II(OP, 0x80000000, 0xFFFFFFFF); \
	RRHL_OP_DO_II(OP, 0x80000000, 0); \
	RRHL_OP_DO_II(OP, 0x12341234, 0x56785678); \
	 \
	RRHL_OP_DO_MM(OP, C_ZERO, C_ONE); \
	RRHL_OP_DO_MM(OP, C_ONE, C_ONE); \
	RRHL_OP_DO_MM(OP, C_ONE, C_NEGONE); \
	RRHL_OP_DO_MM(OP, C_S16_MAX, C_S16_MAX); \
	RRHL_OP_DO_MM(OP, C_S16_MIN, C_S16_MIN); \
	RRHL_OP_DO_MM(OP, C_S32_MAX, C_S32_MAX); \
	RRHL_OP_DO_MM(OP, C_S32_MIN, C_S32_MIN); \
	RRHL_OP_DO_MM(OP, C_S64_MAX, C_S64_MAX); \
	RRHL_OP_DO_MM(OP, C_S64_MIN, C_S64_MIN); \
	RRHL_OP_DO_MM(OP, C_GARBAGE1, C_GARBAGE2); \
	RRHL_OP_DO_MM(OP, C_TWOTWOTWOTWO, C_ONEONEONEONE); \
	\
	printf("\n"); \
}

#define RRRHL_OP_DO_III(OP, d, s, t) \
	SET_U32<d>(rd); SET_U32<s>(rs); SET_U32<t>(rt); \
	SET_HILO(C_HILO); \
	RRRHL_##OP(rd, rs, rt); \
	GET_HILO(hilo); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)rt); PRINT_R(rd, false); printf(" "); PRINT_HILO(hilo, true);

#define RRRHL_OP_DO_MMM(OP, d, s, t) \
	SET_M(rd, d); SET_M(rs, s); SET_M(rt, t); \
	SET_HILO(C_HILO); \
	RRRHL_##OP(rd, rs, rt); \
	GET_HILO(hilo); \
	printf("  %s %s, %s: ", #OP, #s, #t); PRINT_R(rd, false); printf(" "); PRINT_HILO(hilo, true);

#define TEST_RRRHL_FUNC(OP) \
RRRHL_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd, rs, rt; \
	HILOREGS hilo; \
	 \
	printf("%s:\n", #OP); \
	 \
	RRRHL_OP_DO_III(OP, 0x1337, 0, 0); \
	RRRHL_OP_DO_III(OP, 0x1337, 0, 1); \
	RRRHL_OP_DO_III(OP, 0x1337, 1, 0); \
	RRRHL_OP_DO_III(OP, 0x1337, 1, 1); \
	RRRHL_OP_DO_III(OP, 0x1337, 2, 2); \
	RRRHL_OP_DO_III(OP, 0x1337, 53, 2); \
	RRRHL_OP_DO_III(OP, 0x1337, 53, 0); \
	RRRHL_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 1); \
	RRRHL_OP_DO_III(OP, 0x1337, 1, 0xFFFFFFFF); \
	RRRHL_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0xFFFFFFFF); \
	RRRHL_OP_DO_III(OP, 0x1337, 0x80000000, 0xFFFFFFFF); \
	RRRHL_OP_DO_III(OP, 0x1337, 0x80000000, 0); \
	RRRHL_OP_DO_III(OP, 0x1337, 0x12341234, 0x56785678); \
	 \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_ZERO, C_ONE); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_ONE); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_NEGONE); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_S16_MAX, C_S16_MAX); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_S16_MIN, C_S16_MIN); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_S32_MAX, C_S32_MAX); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_S32_MIN, C_S32_MIN); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_S64_MAX, C_S64_MAX); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_S64_MIN, C_S64_MIN); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_GARBAGE1, C_GARBAGE2); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_TWOTWOTWOTWO, C_ONEONEONEONE); \
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); SET_M(rt, C_GARBAGE1); \
	ZRRHL_##OP(rd, rs, rt); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

#define RHL_OP_DO_I(OP, d) \
	SET_U32<d>(rd); \
	SET_HILO(C_HILO); \
	RHL_##OP(rd); \
	GET_HILO(hilo); \
	printf("  %s %d: ", #OP, (u32)d); PRINT_R(rd, false); printf(" "); PRINT_HILO(hilo, true);

#define RHL_OP_DO_M(OP, d) \
	SET_M(rd, d); \
	SET_HILO(C_HILO); \
	RHL_##OP(rd); \
	GET_HILO(hilo); \
	printf("  %s %s: ", #OP, #d); PRINT_R(rd, false); printf(" "); PRINT_HILO(hilo, true);

#define TEST_RHL_FUNC(OP) \
RHL_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd; \
	HILOREGS hilo; \
	 \
	printf("%s:\n", #OP); \
	 \
	RHL_OP_DO_I(OP, 0); \
	RHL_OP_DO_I(OP, 1); \
	RHL_OP_DO_I(OP, 2); \
	RHL_OP_DO_I(OP, 53); \
	RHL_OP_DO_I(OP, 0xFFFFFFFF); \
	RHL_OP_DO_I(OP, 0x80000000); \
	RHL_OP_DO_I(OP, 0x12341234); \
	 \
	RHL_OP_DO_M(OP, C_ZERO); \
	RHL_OP_DO_M(OP, C_ONE); \
	RHL_OP_DO_M(OP, C_NEGONE); \
	RHL_OP_DO_M(OP, C_S16_MAX); \
	RHL_OP_DO_M(OP, C_S16_MIN); \
	RHL_OP_DO_M(OP, C_S32_MAX); \
	RHL_OP_DO_M(OP, C_S32_MIN); \
	RHL_OP_DO_M(OP, C_S64_MAX); \
	RHL_OP_DO_M(OP, C_S64_MIN); \
	RHL_OP_DO_M(OP, C_GARBAGE1); \
	RHL_OP_DO_M(OP, C_TWOTWOTWOTWO); \
	\
	SET_M(rd, C_GARBAGE1); \
	ZHL_##OP(rd); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

RRHL_OP_FUNC(div1);
RRHL_OP_FUNC(divu1);

TEST_RRHL_FUNC(div)
TEST_RRHL_FUNC(divu)
TEST_RRHL_FUNC(div1)
TEST_RRHL_FUNC(divu1)

TEST_RRRHL_FUNC(madd)
TEST_RRRHL_FUNC(maddu)
TEST_RRRHL_FUNC(madd1)
TEST_RRRHL_FUNC(maddu1)
TEST_RRRHL_FUNC(mult)
TEST_RRRHL_FUNC(multu)
TEST_RRRHL_FUNC(mult1)
TEST_RRRHL_FUNC(multu1)

TEST_RHL_FUNC(mfhi)
TEST_RHL_FUNC(mflo)
TEST_RHL_FUNC(mthi)
TEST_RHL_FUNC(mtlo)
TEST_RHL_FUNC(mfhi1)
TEST_RHL_FUNC(mflo1)
TEST_RHL_FUNC(mthi1)
TEST_RHL_FUNC(mtlo1)

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_div();
	test_divu();
	test_div1();
	test_divu1();

	test_madd();
	test_maddu();
	test_madd1();
	test_maddu1();
	test_mult();
	test_multu();
	test_mult1();
	test_multu1();

	test_mfhi();
	test_mflo();
	test_mthi();
	test_mtlo();
	test_mfhi1();
	test_mflo1();
	test_mthi1();
	test_mtlo1();

	printf("-- TEST END\n");

	return 0;
}
