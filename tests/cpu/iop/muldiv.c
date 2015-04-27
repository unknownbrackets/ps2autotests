#include <stdio.h>
#include <tamtypes.h>

struct HILOREGS {
	u32 hi;
	u32 lo;
};

static struct HILOREGS __attribute__((aligned(16))) C_HILO = {0x01234567, 0x89ABCDEF};

static u32 __attribute__((aligned(16))) C_ZERO[4] = {0, 0, 0, 0};
static u32 __attribute__((aligned(16))) C_S16_MAX[4] = {0x7FFF, 0, 0, 0};
static u32 __attribute__((aligned(16))) C_S16_MIN[4] = {0xFFFF8000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16))) C_S32_MAX[4] = {0x7FFFFFFF, 0, 0, 0};
static u32 __attribute__((aligned(16))) C_S32_MIN[4] = {0x80000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16))) C_S64_MAX[4] = {0xFFFFFFFF, 0x7FFFFFFF, 0, 0 };
static u32 __attribute__((aligned(16))) C_S64_MIN[4] = {0, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16))) C_NEGONE[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static u32 __attribute__((aligned(16))) C_ONE[4] = {1, 0, 0, 0};
static u32 __attribute__((aligned(16))) C_GARBAGE1[4] = {0x1337, 0x1338, 0x1339, 0x133A};
static u32 __attribute__((aligned(16))) C_GARBAGE2[4] = {0xDEADBEEF, 0xDEADBEEE, 0xDEADBEED, 0xDEADBEEC};
static u32 __attribute__((aligned(16))) C_ONEONEONEONE[4] = {1, 1, 1, 1};
static u32 __attribute__((aligned(16))) C_TWOTWOTWOTWO[4] = {2, 2, 2, 2};

#define RRHL(OP, rs, rt) \
	if (#OP[0] == 'd' && #OP[1] == 'i' && #OP[2] == 'v') { \
		/* gas automatically inserts a divide by zero trap, annoyingly. */ \
		if (#OP[3] == 'u') { \
			asm volatile ( \
				"or $t8, $0, %0\n" \
				"or $t9, $0, %1\n" \
				".word 0x0319001B\n" /* "divu $t8, $t9\n" */ \
				: : "r"(rs), "r"(rt) : "t8", "t9" \
			); \
		} else { \
			asm volatile ( \
				"or $t8, $0, %0\n" \
				"or $t9, $0, %1\n" \
				".word 0x0319001A\n" /* "div $t8, $t9\n" */ \
				: : "r"(rs), "r"(rt) : "t8", "t9" \
			); \
		} \
	} else { \
		asm volatile ( \
			".set noreorder\n" \
			".set nomacro\n" \
			#OP " %0, %1\n" \
			".set reorder\n" \
			".set macro\n" \
			: : "r"(rs), "r"(rt) \
		); \
	}

#define RHL(OP, rd) \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " %0\n" \
		".set reorder\n" \
		".set macro\n" \
		: "+r"(rd) \
	);
#define ZHL(OP, rd) \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " $0\n" \
		"ori %0, $0, 0\n" \
		".set reorder\n" \
		".set macro\n" \
		: "+r"(rd) \
	);

static inline void GET_HILO(struct HILOREGS *regs) {
	asm volatile (
		"mfhi %0\n"
		"mflo %1\n"
		: "+r"(regs->hi), "+r"(regs->lo)
	);
}

static inline void SET_HILO(const struct HILOREGS regs) {
	asm volatile (
		"mthi %0\n"
		"mtlo %1\n"
		: : "r"(regs.hi), "r"(regs.lo)
	);
}

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

static inline void PRINT_HILO(const struct HILOREGS regs, int newline) {
	printf("H: %08x L: %08x", regs.hi, regs.lo);
	if (newline) {
		printf("\n");
	}
}

#define RRHL_OP_DO_II(OP, s, t) \
	SET_U32(rs, s); SET_U32(rt, t); \
	SET_HILO(C_HILO); \
	RRHL(OP, rs, rt); \
	GET_HILO(&hilo); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)rt); PRINT_HILO(hilo, 1);

#define RRHL_OP_DO_MM(OP, s, t) \
	SET_M(rs, s); SET_M(rt, t); \
	SET_HILO(C_HILO); \
	RRHL(OP, rs, rt); \
	GET_HILO(&hilo); \
	printf("  %s %s, %s: ", #OP, #s, #t); PRINT_HILO(hilo, 1);

#define TEST_RRHL_FUNC(OP) \
static void test_##OP() { \
	register u32 rs, rt; \
	struct HILOREGS hilo; \
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
	RRHL_OP_DO_II(OP, 0xFFFFFFFF, 0); \
	RRHL_OP_DO_II(OP, 0xFFFFFFCB, 0); \
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
	RRHL_OP_DO_MM(OP, C_NEGONE, C_TWOTWOTWOTWO); \
	\
	printf("\n"); \
}

#define RHL_OP_DO_I(OP, d) \
	SET_U32(rd, d); \
	SET_HILO(C_HILO); \
	RHL(OP, rd); \
	GET_HILO(&hilo); \
	printf("  %s %d: ", #OP, (u32)d); PRINT_R(rd, 0); printf(" "); PRINT_HILO(hilo, 1);

#define RHL_OP_DO_M(OP, d) \
	SET_M(rd, d); \
	SET_HILO(C_HILO); \
	RHL(OP, rd); \
	GET_HILO(&hilo); \
	printf("  %s %s: ", #OP, #d); PRINT_R(rd, 0); printf(" "); PRINT_HILO(hilo, 1);

#define TEST_RHL_FUNC(OP) \
static void test_##OP() { \
	register u32 rd; \
	struct HILOREGS hilo; \
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
	ZHL(OP, rd); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, 1); \
	printf("\n"); \
}

TEST_RRHL_FUNC(div)
TEST_RRHL_FUNC(divu)
TEST_RRHL_FUNC(mult)
TEST_RRHL_FUNC(multu)

TEST_RHL_FUNC(mfhi)
TEST_RHL_FUNC(mflo)
TEST_RHL_FUNC(mthi)
TEST_RHL_FUNC(mtlo)

int _start(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_div();
	test_divu();
	test_mult();
	test_multu();

	test_mfhi();
	test_mflo();
	test_mthi();
	test_mtlo();

	printf("-- TEST END\n");

	return 0;
}
