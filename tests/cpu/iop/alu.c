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

#define RRR(OP, rd, rs, rt) \
	asm volatile ( \
		#OP " %0, %1, %2\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	);
#define ZRR(OP, rd, rs, rt) \
	asm volatile ( \
		#OP " $0, %1, %2\n" \
		"ori %0, $0, 0\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	);

#define RRI(OP, rd, rs, i) \
	asm volatile ( \
		#OP " %0, %1, %2\n" \
		: "+r"(rd) : "r"(rs), "i"(i) \
	);
#define ZRI(OP, rd, rs, i) \
	asm volatile ( \
		#OP " $0, %1, %2\n" \
		"ori %0, $0, 0\n" \
		: "+r"(rd) : "r"(rs), "i"(i) \
	);

#define RI(OP, rd, i) \
	asm volatile ( \
		#OP " %0, %1\n" \
		: "+r"(rd) : "i"(i) \
	);
#define ZI(OP, rd, i) \
	asm volatile ( \
		#OP " $0, %1\n" \
		"ori %0, $0, 0\n" \
		: "+r"(rd) : "i"(i) \
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

#define RRR_OP_DO_III(OP, d, s, t) \
	SET_U32(rd, d); SET_U32(rs, s); SET_U32(rt, t); \
	RRR(OP, rd, rs, rt); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)rt); PRINT_R(rd, 1);

#define RRR_OP_DO_MMM(OP, d, s, t) \
	SET_M(rd, d); SET_M(rs, s); SET_M(rt, t); \
	RRR(OP, rd, rs, rt); \
	printf("  %s %s, %s: ", #OP, #s, #t); PRINT_R(rd, 1);

#define TEST_RRR_FUNC(OP) \
static void test_##OP() { \
	register u32 rd, rs, rt; \
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
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); SET_M(rt, C_GARBAGE1); \
	ZRR(OP, rd, rs, rt); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, 1); \
	printf("\n"); \
}

#define RRI_OP_DO_III(OP, d, s, t) \
	SET_U32(rd, d); SET_U32(rs, s); \
	RRI(OP, rd, rs, t); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)t); PRINT_R(rd, 1);

#define RRI_OP_DO_MMI(OP, d, s, t) \
	SET_M(rd, d); SET_M(rs, s); \
	RRI(OP, rd, rs, t); \
	printf("  %s %s, %d: ", #OP, #s, (u32)t); PRINT_R(rd, 1);

#define TEST_RRI_FUNC(OP) \
static void test_##OP() { \
	register u32 rd, rs; \
	 \
	printf("%s:\n", #OP); \
	 \
	RRI_OP_DO_III(OP, 0x1337, 0, 0); \
	RRI_OP_DO_III(OP, 0x1337, 0, 1); \
	RRI_OP_DO_III(OP, 0x1337, 1, 1); \
	RRI_OP_DO_III(OP, 0x1337, 1, 0); \
	RRI_OP_DO_III(OP, 0x1337, 2, 2); \
	RRI_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 1); \
	RRI_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0xFFFF); \
	 \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ZERO, 0); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ZERO, 1); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 0); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 1); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 0xFFFF); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S16_MAX, 0x7FFF); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S16_MIN, 0x8000); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S32_MAX, 0x7FFF); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S32_MIN, 0x8000); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S64_MAX, 0x7FFF); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S64_MIN, 0x8000); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_GARBAGE1, 0xDEAD); \
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); \
	ZRI(OP, rd, rs, 0xDEAD); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, 1); \
	printf("\n"); \
}

#define TEST_RRS_FUNC(OP) \
static void test_##OP() { \
	register u32 rd, rs; \
	 \
	printf("%s:\n", #OP); \
	 \
	RRI_OP_DO_III(OP, 0x1337, 0, 0); \
	RRI_OP_DO_III(OP, 0x1337, 0, 1); \
	RRI_OP_DO_III(OP, 0x1337, 1, 1); \
	RRI_OP_DO_III(OP, 0x1337, 1, 0); \
	RRI_OP_DO_III(OP, 0x1337, 2, 2); \
	RRI_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 1); \
	RRI_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 31); \
	 \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ZERO, 0); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ZERO, 1); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 0); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 1); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 2); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 30); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_ONE, 31); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S16_MAX, 3); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S16_MIN, 3); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S32_MAX, 3); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S32_MIN, 3); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S64_MAX, 3); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_S64_MIN, 3); \
	RRI_OP_DO_MMI(OP, C_GARBAGE1, C_GARBAGE1, 5); \
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); \
	ZRI(OP, rd, rs, 5); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, 1); \
	printf("\n"); \
}

#define RI_OP_DO_II(OP, d, t) \
	SET_U32(rd, d); \
	RI(OP, rd, t); \
	printf("  %s %d: ", #OP, (u32)t); PRINT_R(rd, 1);

#define RI_OP_DO_MI(OP, d, t) \
	SET_M(rd, d); \
	RI(OP, rd, t); \
	printf("  %s %d: ", #OP, (u32)t); PRINT_R(rd, 1);

#define TEST_RI_FUNC(OP) \
static void test_##OP() { \
	register u32 rd; \
	 \
	printf("%s:\n", #OP); \
	 \
	RI_OP_DO_II(OP, 0x1337, 0); \
	RI_OP_DO_II(OP, 0x1337, 1); \
	RI_OP_DO_II(OP, 0x1337, 2); \
	RI_OP_DO_II(OP, 0x1337, 0xFFFF); \
	RI_OP_DO_II(OP, 0x1337, 0x7FFF); \
	RI_OP_DO_II(OP, 0x1337, 0x8000); \
	 \
	RI_OP_DO_MI(OP, C_GARBAGE1, 0); \
	RI_OP_DO_MI(OP, C_GARBAGE1, 1); \
	RI_OP_DO_MI(OP, C_GARBAGE1, 0xFFFF); \
	RI_OP_DO_MI(OP, C_GARBAGE1, 0x7FFF); \
	RI_OP_DO_MI(OP, C_GARBAGE1, 0x8000); \
	RI_OP_DO_MI(OP, C_GARBAGE1, 0xDEAD); \
	\
	SET_M(rd, C_GARBAGE1); \
	ZI(OP, rd, 0xDEAD); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, 1); \
	printf("\n"); \
}

// Not covering:
//   - Trapping ops (add, dadd, tne, etc.)
//   - Branching and jumping ops
//   - Multiply, divide, and HI/LO ops
//   - Load/store ops
//   - Sync
//   - Syscalls

TEST_RRR_FUNC(addu);
TEST_RRI_FUNC(addiu);
TEST_RRR_FUNC(and);
TEST_RRI_FUNC(andi);
TEST_RI_FUNC(lui);
TEST_RRR_FUNC(nor);
TEST_RRR_FUNC(or);
TEST_RRI_FUNC(ori);
TEST_RRS_FUNC(sll);
TEST_RRR_FUNC(sllv);
TEST_RRR_FUNC(slt);
TEST_RRS_FUNC(slti);
TEST_RRS_FUNC(sltiu);
TEST_RRR_FUNC(sltu);
TEST_RRS_FUNC(sra);
TEST_RRR_FUNC(srav);
TEST_RRS_FUNC(srl);
TEST_RRR_FUNC(srlv);
TEST_RRR_FUNC(subu);
TEST_RRR_FUNC(xor);
TEST_RRI_FUNC(xori);

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_addu();
	test_addiu();
	test_and();
	test_andi();
	test_lui();
	test_nor();
	test_or();
	test_ori();
	test_sll();
	test_sllv();
	test_slt();
	test_slti();
	test_sltiu();
	test_sltu();
	test_sra();
	test_srav();
	test_srl();
	test_srlv();
	test_subu();
	test_xor();
	test_xori();

	printf("-- TEST END\n");

	return 0;
}
