#include <stdio.h>
#include <tamtypes.h>

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

#define RRR_OP_FUNC(OP) \
static inline void RRR_##OP(register u128 &rd, const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		#OP " %0, %1, %2\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
} \
static inline void ZRR_##OP(register u128 &rd, const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		#OP " $0, %1, %2\n" \
		"por %0, $0, $0\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
}

#define RR_OP_FUNC(OP) \
static inline void RR_##OP(register u128 &rd, const register u128 &rs) { \
	asm volatile ( \
		#OP " %0, %1\n" \
		: "+r"(rd) : "r"(rs) \
	); \
} \
static inline void ZR_##OP(register u128 &rd, const register u128 &rs) { \
	asm volatile ( \
		#OP " $0, %1\n" \
		"por %0, $0, $0\n" \
		: "+r"(rd) : "r"(rs) \
	); \
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

#define RRR_OP_DO_III(OP, d, s, t) \
	SET_U32<d>(rd); SET_U32<s>(rs); SET_U32<t>(rt); \
	RRR_##OP(rd, rs, rt); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)rt); PRINT_R(rd, true);

#define RRR_OP_DO_MMM(OP, d, s, t) \
	SET_M(rd, d); SET_M(rs, s); SET_M(rt, t); \
	RRR_##OP(rd, rs, rt); \
	printf("  %s %s, %s: ", #OP, #s, #t); PRINT_R(rd, true);

#define TEST_RRR_FUNC(OP) \
RRR_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd, rs, rt; \
	 \
	printf("%s:\n", #OP); \
	 \
	RRR_OP_DO_III(OP, 0x1337, 0, 0); \
	RRR_OP_DO_III(OP, 0x1337, 0, 1); \
	RRR_OP_DO_III(OP, 0x1337, 1, 1); \
	RRR_OP_DO_III(OP, 0x1337, 1, 0); \
	RRR_OP_DO_III(OP, 0x1337, 2, 2); \
	RRR_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 1); \
	RRR_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0xFFFFFFFF); \
	 \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ZERO, C_ZERO); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ZERO, C_ONE); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_ZERO); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_ONE); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_NEGONE); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S16_MAX, C_S16_MAX); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S16_MIN, C_S16_MIN); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S32_MAX, C_S32_MAX); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S32_MIN, C_S32_MIN); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S64_MAX, C_S64_MAX); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S64_MIN, C_S64_MIN); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_GARBAGE1, C_GARBAGE2); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S8_A, C_P_S8_B); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S16_A, C_P_S16_B); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_A, C_P_S32_B); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_B, C_P_S32_A); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_C, C_P_S32_D); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_D, C_P_S32_C); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_E, C_P_S32_F); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_F, C_P_S32_E); \
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); SET_M(rt, C_GARBAGE1); \
	ZRR_##OP(rd, rs, rt); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

#define RR_OP_DO_II(OP, d, s) \
	SET_U32<d>(rd); SET_U32<s>(rs); \
	RR_##OP(rd, rs); \
	printf("  %s %d: ", #OP, (u32)rs); PRINT_R(rd, true);

#define RR_OP_DO_MM(OP, d, s) \
	SET_M(rd, d); SET_M(rs, s); \
	RR_##OP(rd, rs); \
	printf("  %s %s: ", #OP, #s); PRINT_R(rd, true);

#define TEST_RR_FUNC(OP) \
RR_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd, rs; \
	 \
	printf("%s:\n", #OP); \
	 \
	RR_OP_DO_II(OP, 0x1337, 0); \
	RR_OP_DO_II(OP, 0x1337, 0); \
	RR_OP_DO_II(OP, 0x1337, 1); \
	RR_OP_DO_II(OP, 0x1337, 1); \
	RR_OP_DO_II(OP, 0x1337, 2); \
	RR_OP_DO_II(OP, 0x1337, 0xFFFFFFFF); \
	RR_OP_DO_II(OP, 0x1337, 0xFFFFFFFF); \
	 \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_ZERO); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_ZERO); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_ONE); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_ONE); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_ONE); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_S16_MAX); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_S16_MIN); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_S32_MAX); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_S32_MIN); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_S64_MAX); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_S64_MIN); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_GARBAGE1); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_P_S8_A); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_P_S16_A); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_P_S32_A); \
	RR_OP_DO_MM(OP, C_GARBAGE1, C_P_S32_E); \
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); \
	ZR_##OP(rd, rs); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

TEST_RR_FUNC(pcpyh)
TEST_RRR_FUNC(pcpyld)
TEST_RRR_FUNC(pcpyud)
TEST_RR_FUNC(pexch)
TEST_RR_FUNC(pexcw)
TEST_RR_FUNC(pexeh)
TEST_RR_FUNC(pexew)
TEST_RR_FUNC(pext5)
TEST_RRR_FUNC(pextlb)
TEST_RRR_FUNC(pextlh)
TEST_RRR_FUNC(pextlw)
TEST_RRR_FUNC(pextub)
TEST_RRR_FUNC(pextuh)
TEST_RRR_FUNC(pextuw)
TEST_RRR_FUNC(pinteh)
TEST_RRR_FUNC(pinth)
TEST_RR_FUNC(ppac5)
TEST_RRR_FUNC(ppacb)
TEST_RRR_FUNC(ppach)
TEST_RRR_FUNC(ppacw)
TEST_RR_FUNC(prevh)
TEST_RR_FUNC(prot3w)

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_pcpyh();
	test_pcpyld();
	test_pcpyud();
	test_pexch();
	test_pexcw();
	test_pexeh();
	test_pexew();
	test_pext5();
	test_pextlb();
	test_pextlh();
	test_pextlw();
	test_pextub();
	test_pextuh();
	test_pextuw();
	test_pinteh();
	test_pinth();
	test_ppac5();
	test_ppacb();
	test_ppach();
	test_ppacw();
	test_prevh();
	test_prot3w();

	printf("-- TEST END\n");

	return 0;
}
