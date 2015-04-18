#include "shared.h"

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
		"ori %0, $0, 0\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
}

#define RRI_OP_FUNC(OP) \
template <int i> \
static inline void RRI_##OP(register u128 &rd, const register u128 &rs) { \
	asm volatile ( \
		#OP " %0, %1, %2\n" \
		: "+r"(rd) : "r"(rs), "i"(i) \
	); \
} \
template <int i> \
static inline void ZRI_##OP(register u128 &rd, const register u128 &rs) { \
	asm volatile ( \
		#OP " $0, %1, %2\n" \
		"ori %0, $0, 0\n" \
		: "+r"(rd) : "r"(rs), "i"(i) \
	); \
}

#define RI_OP_FUNC(OP) \
template <int i> \
static inline void RI_##OP(register u128 &rd) { \
	asm volatile ( \
		#OP " %0, %1\n" \
		: "+r"(rd) : "i"(i) \
	); \
} \
template <int i> \
static inline void ZI_##OP(register u128 &rd) { \
	asm volatile ( \
		#OP " $0, %1\n" \
		"ori %0, $0, 0\n" \
		: "+r"(rd) : "i"(i) \
	); \
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
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); SET_M(rt, C_GARBAGE1); \
	ZRR_##OP(rd, rs, rt); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

#define RRI_OP_DO_III(OP, d, s, t) \
	SET_U32<d>(rd); SET_U32<s>(rs); \
	RRI_##OP<t>(rd, rs); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)t); PRINT_R(rd, true);

#define RRI_OP_DO_MMI(OP, d, s, t) \
	SET_M(rd, d); SET_M(rs, s); \
	RRI_##OP<t>(rd, rs); \
	printf("  %s %s, %d: ", #OP, #s, (u32)t); PRINT_R(rd, true);

#define TEST_RRI_FUNC(OP) \
RRI_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd, rs; \
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
	ZRI_##OP<0xDEAD>(rd, rs); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

#define TEST_RRS_FUNC(OP) \
RRI_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd, rs; \
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
	ZRI_##OP<5>(rd, rs); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

#define RI_OP_DO_II(OP, d, t) \
	SET_U32<d>(rd); \
	RI_##OP<t>(rd); \
	printf("  %s %d: ", #OP, (u32)t); PRINT_R(rd, true);

#define RI_OP_DO_MI(OP, d, t) \
	SET_M(rd, d); \
	RI_##OP<t>(rd); \
	printf("  %s %d: ", #OP, (u32)t); PRINT_R(rd, true);

#define TEST_RI_FUNC(OP) \
RI_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd; \
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
	ZI_##OP<0xDEAD>(rd); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
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
TEST_RRR_FUNC(daddu);
TEST_RRI_FUNC(daddiu);
TEST_RRS_FUNC(dsll);
TEST_RRS_FUNC(dsll32);
TEST_RRR_FUNC(dsllv);
TEST_RRS_FUNC(dsra);
TEST_RRS_FUNC(dsra32);
TEST_RRR_FUNC(dsrav);
TEST_RRS_FUNC(dsrl);
TEST_RRS_FUNC(dsrl32);
TEST_RRR_FUNC(dsrlv);
TEST_RRR_FUNC(dsubu);
TEST_RI_FUNC(lui);
TEST_RRR_FUNC(movn);
TEST_RRR_FUNC(movz);
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

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_addu();
	test_addiu();
	test_and();
	test_andi();
	test_daddu();
	test_daddiu();
	test_dsll();
	test_dsll32();
	test_dsllv();
	test_dsra();
	test_dsra32();
	test_dsrav();
	test_dsrl();
	test_dsrl32();
	test_dsrlv();
	test_dsubu();
	test_lui();
	test_movn();
	test_movz();
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
