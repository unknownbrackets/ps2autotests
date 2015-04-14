#include <stdio.h>
#include <tamtypes.h>

struct HILOREGS {
	u64 hi;
	u64 lo;
	u64 hi1;
	u64 lo1;
};

static HILOREGS __attribute__((aligned(16))) C_HILO = {0x0123456789ABCDEF, 0x123456789ABCDEF0, 0x23456789ABCDEF01, 0x456789ABCDEF0123};
static HILOREGS __attribute__((aligned(16))) C_HILO_ZERO = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000};
static HILOREGS __attribute__((aligned(16))) C_HILO_PATTERN = {0x0000000000000000, 0x0000000010001234, 0x0000000000000000, 0x00000000F000ABCD};

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
static u32 __attribute__((aligned(16))) C_P_S8_A[4] = {0x80808080, 0xFFFFFFFF, 0x12345678, 0x7F7F7F7F};
static u32 __attribute__((aligned(16))) C_P_S8_B[4] = {0x80FF127F, 0xFF807F34, 0x567F80FF, 0x7F78FF80};
static u32 __attribute__((aligned(16))) C_P_S16_A[4] = {0x80008000, 0xFFFFFFFF, 0x12345678, 0x7FFF7FFF};
static u32 __attribute__((aligned(16))) C_P_S16_B[4] = {0x8000FFFF, 0x12347FFF, 0xFFFF8000, 0x7FFF5678};
static u32 __attribute__((aligned(16))) C_P_S32_A[4] = {0x80000000, 0xFFFFFFFF, 0x12345678, 0x7FFFFFFF};
static u32 __attribute__((aligned(16))) C_P_S32_B[4] = {0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0x12345678};
static u32 __attribute__((aligned(16))) C_P_S32_C[4] = {0x80000000, 0xFFFFFFFF, 0x12345678, 0x7FFFFFFF};
static u32 __attribute__((aligned(16))) C_P_S32_D[4] = {0x7FFFFFFF, 0x12345678, 0xFFFFFFFF, 0x80000000};
static u32 __attribute__((aligned(16))) C_P_S32_E[4] = {0x00000000, 0x7FFFFFFF, 0xFFFFFFFF, 0x80000000};
static u32 __attribute__((aligned(16))) C_P_S32_F[4] = {0xFFFFFFFF, 0x80000000, 0x00000000, 0x7FFFFFFF};

#define RRHL_OP_FUNC(OP) \
static inline void RRHL_##OP(const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		#OP " %0, %1\n" \
		: : "r"(rs), "r"(rt) \
	); \
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
		"por %0, $0, $0\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
}

#define RHL_OP_FUNC(OP, IDENT) \
static inline void RHL_##IDENT(register u128 &rd) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		OP " %0\n" \
		: "+r"(rd) \
	); \
} \
static inline void ZHL_##IDENT(register u128 &rd) { \
	asm volatile ( \
		".set noreorder\n" \
		".set nomacro\n" \
		OP " $0\n" \
		"por %0, $0, $0\n" \
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
RRHL_OP_FUNC(OP); \
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
	RRHL_OP_DO_MM(OP, C_P_S8_A, C_P_S8_B); \
	RRHL_OP_DO_MM(OP, C_P_S16_A, C_P_S16_B); \
	RRHL_OP_DO_MM(OP, C_P_S32_A, C_P_S32_B); \
	RRHL_OP_DO_MM(OP, C_P_S32_B, C_P_S32_A); \
	RRHL_OP_DO_MM(OP, C_P_S32_C, C_P_S32_D); \
	RRHL_OP_DO_MM(OP, C_P_S32_D, C_P_S32_C); \
	RRHL_OP_DO_MM(OP, C_P_S32_E, C_P_S32_F); \
	RRHL_OP_DO_MM(OP, C_P_S32_F, C_P_S32_E); \
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
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S8_A, C_P_S8_B); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S16_A, C_P_S16_B); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_A, C_P_S32_B); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_B, C_P_S32_A); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_C, C_P_S32_D); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_D, C_P_S32_C); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_E, C_P_S32_F); \
	RRRHL_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_F, C_P_S32_E); \
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); SET_M(rt, C_GARBAGE1); \
	ZRRHL_##OP(rd, rs, rt); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

#define RHL_OP_DO_I(OP, IDENT, hl, d) \
	SET_U32<d>(rd); \
	SET_HILO(hl); \
	RHL_##IDENT(rd); \
	GET_HILO(hilo); \
	printf("  %s %d (%s): ", OP, (u32)d, #hl); PRINT_R(rd, false); printf(" "); PRINT_HILO(hilo, true);

#define RHL_OP_DO_M(OP, IDENT, hl, d) \
	SET_M(rd, d); \
	SET_HILO(hl); \
	RHL_##IDENT(rd); \
	GET_HILO(hilo); \
	printf("  %s %s (%s): ", OP, #d, #hl); PRINT_R(rd, false); printf(" "); PRINT_HILO(hilo, true);

#define TEST_RHL_FUNC(OP, IDENT) \
RHL_OP_FUNC(OP, IDENT); \
static void test_##IDENT() { \
	register u128 rd; \
	HILOREGS hilo; \
	 \
	printf("%s:\n", OP); \
	 \
	RHL_OP_DO_I(OP, IDENT, C_HILO, 0); \
	RHL_OP_DO_I(OP, IDENT, C_HILO, 1); \
	RHL_OP_DO_I(OP, IDENT, C_HILO, 2); \
	RHL_OP_DO_I(OP, IDENT, C_HILO, 53); \
	RHL_OP_DO_I(OP, IDENT, C_HILO, 0xFFFFFFFF); \
	RHL_OP_DO_I(OP, IDENT, C_HILO, 0x80000000); \
	RHL_OP_DO_I(OP, IDENT, C_HILO, 0x12341234); \
	RHL_OP_DO_I(OP, IDENT, C_HILO_ZERO, 0x00000000); \
	RHL_OP_DO_I(OP, IDENT, C_HILO_ZERO, 0x12341234); \
	RHL_OP_DO_I(OP, IDENT, C_HILO_ZERO, 0xFFFFFFFF); \
	RHL_OP_DO_I(OP, IDENT, C_HILO_PATTERN, 0x00000000); \
	RHL_OP_DO_I(OP, IDENT, C_HILO_PATTERN, 0x12341234); \
	RHL_OP_DO_I(OP, IDENT, C_HILO_PATTERN, 0xFFFFFFFF); \
	 \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_ZERO); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_ONE); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_NEGONE); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_S16_MAX); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_S16_MIN); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_S32_MAX); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_S32_MIN); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_S64_MAX); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_S64_MIN); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_GARBAGE1); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_TWOTWOTWOTWO); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_P_S8_A); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_P_S16_A); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_P_S32_A); \
	RHL_OP_DO_M(OP, IDENT, C_HILO, C_P_S32_E); \
	\
	SET_M(rd, C_GARBAGE1); \
	ZHL_##IDENT(rd); \
	printf("  %s -> $0: ", OP); PRINT_R(rd, true); \
	printf("\n"); \
}

TEST_RRHL_FUNC(pdivbw)
TEST_RRHL_FUNC(pdivuw)
TEST_RRHL_FUNC(pdivw)

TEST_RRRHL_FUNC(phmadh)
TEST_RRRHL_FUNC(phmsbh)
TEST_RRRHL_FUNC(pmaddh)
TEST_RRRHL_FUNC(pmadduw)
TEST_RRRHL_FUNC(pmaddw)
TEST_RRRHL_FUNC(pmsubh)
TEST_RRRHL_FUNC(pmsubw)
TEST_RRRHL_FUNC(pmulth)
TEST_RRRHL_FUNC(pmultuw)
TEST_RRRHL_FUNC(pmultw)

TEST_RHL_FUNC("pmfhi", pmfhi)
TEST_RHL_FUNC("pmfhl.lh", pmfhl_lh)
TEST_RHL_FUNC("pmfhl.lw", pmfhl_lw)
TEST_RHL_FUNC("pmfhl.sh", pmfhl_sh)
TEST_RHL_FUNC("pmfhl.slw", pmfhl_slw)
TEST_RHL_FUNC("pmfhl.uw", pmfhl_uw)
TEST_RHL_FUNC("pmflo", pmflo)
TEST_RHL_FUNC("pmthi", pmthi)
TEST_RHL_FUNC("pmthl.lw", pmthl_lw)
TEST_RHL_FUNC("pmtlo", pmtlo)

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_pdivbw();
	test_pdivuw();
	test_pdivw();

	test_phmadh();
	test_phmsbh();
	test_pmaddh();
	test_pmadduw();
	test_pmaddw();
	test_pmsubh();
	test_pmsubw();
	test_pmulth();
	test_pmultuw();
	test_pmultw();

	test_pmfhi();
	test_pmfhl_lh();
	test_pmfhl_lw();
	test_pmfhl_sh();
	test_pmfhl_slw();
	test_pmfhl_uw();
	test_pmflo();
	test_pmthi();
	test_pmthl_lw();
	test_pmtlo();

	printf("-- TEST END\n");

	return 0;
}
