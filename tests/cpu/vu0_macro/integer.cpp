#include <string.h>
#include "shared.h"

static u32 *const vu_mem0 = (u32 *)0x11004000;
static u32 vu_mem0_size = 0x1000;

static const u32 __attribute__((aligned(16))) C_ZERO[4] = {0, 0, 0, 0};
static const u32 __attribute__((aligned(16))) C_S16_MAX[4] = {0x7FFF, 0, 0, 0};
static const u32 __attribute__((aligned(16))) C_S16_MIN[4] = {0xFFFF8000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static const u32 __attribute__((aligned(16))) C_S32_MAX[4] = {0x7FFFFFFF, 0, 0, 0};
static const u32 __attribute__((aligned(16))) C_S32_MIN[4] = {0x80000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static const u32 __attribute__((aligned(16))) C_S64_MAX[4] = {0xFFFFFFFF, 0x7FFFFFFF, 0, 0 };
static const u32 __attribute__((aligned(16))) C_S64_MIN[4] = {0, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFF};
static const u32 __attribute__((aligned(16))) C_NEGONE[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static const u32 __attribute__((aligned(16))) C_ONE[4] = {1, 0, 0, 0};
static const u32 __attribute__((aligned(16))) C_GARBAGE1[4] = {0x1337, 0x1338, 0x1339, 0x133A};
static const u32 __attribute__((aligned(16))) C_GARBAGE2[4] = {0xDEADBEEF, 0xDEADBEEE, 0xDEADBEED, 0xDEADBEEC};

static u32 *const CVI_ZERO = vu_mem0 + 0x0000;
static u32 *const CVI_S16_MAX = vu_mem0 + 0x0010;
static u32 *const CVI_S16_MIN = vu_mem0 + 0x0020;
static u32 *const CVI_S32_MAX = vu_mem0 + 0x0030;
static u32 *const CVI_S32_MIN = vu_mem0 + 0x0040;
static u32 *const CVI_S64_MAX = vu_mem0 + 0x0050;
static u32 *const CVI_S64_MIN = vu_mem0 + 0x0060;
static u32 *const CVI_NEGONE = vu_mem0 + 0x0070;
static u32 *const CVI_ONE = vu_mem0 + 0x0080;
static u32 *const CVI_GARBAGE1 = vu_mem0 + 0x0090;
static u32 *const CVI_GARBAGE2 = vu_mem0 + 0x00A0;

static void setup_vi_constants() {
	*(vu128 *)CVI_ZERO = *(vu128 *)C_ZERO;
	*(vu128 *)CVI_S16_MAX = *(vu128 *)C_S16_MAX;
	*(vu128 *)CVI_S16_MIN = *(vu128 *)C_S16_MIN;
	*(vu128 *)CVI_S32_MAX = *(vu128 *)C_S32_MAX;
	*(vu128 *)CVI_S32_MIN = *(vu128 *)C_S32_MIN;
	*(vu128 *)CVI_S64_MAX = *(vu128 *)C_S64_MAX;
	*(vu128 *)CVI_S64_MIN = *(vu128 *)C_S64_MIN;
	*(vu128 *)CVI_NEGONE = *(vu128 *)C_NEGONE;
	*(vu128 *)CVI_ONE = *(vu128 *)C_ONE;
	*(vu128 *)CVI_GARBAGE1 = *(vu128 *)C_GARBAGE1;
	*(vu128 *)CVI_GARBAGE2 = *(vu128 *)C_GARBAGE2;
}

#define SET_VI(reg, i) \
	asm volatile ( \
		"lui $t0, %0\n" \
		"ori $t0, $t0, %1\n" \
		"ctc2 $t0, " #reg "\n" \
		: : "K"(((i) >> 16) & 0xFFFF), "K"((i) & 0xFFFF) : "t0" \
	)
#define SET_VI_M(reg, m, lane) \
	SET_VI(reg, (u16)(((u8 *)m - (u8 *)vu_mem0) / 16)); \
	asm volatile ( \
		"vilwr." #lane " " #reg ", (" #reg ")\n" \
		"vnop\n" \
	)

#define PRINT_VI(reg, newline) \
	do { \
		register u32 x__printvi; \
		asm volatile ( \
			"cfc2 %0, " #reg "\n" \
			: "=&r"(x__printvi) \
		); \
		printf("%04x", x__printvi); \
		if (newline) { \
			printf("\n"); \
		} \
	} while (false)

#define VDDD(OP, id, is, it) \
	asm volatile ( \
		#OP " " #id ", " #is ", " #it "\n" \
	)

#define VZDD(OP, id, is, it) \
	asm volatile ( \
		#OP " vi00, " #is ", " #it "\n" \
		"vior " #id ", vi00, vi00\n" \
	)

#define VDDI(OP, id, is, t) \
	asm volatile ( \
		#OP " " #id ", " #is ", " #t "\n" \
	)

#define VZDI(OP, id, is, it) \
	asm volatile ( \
		#OP " vi00, " #is ", " #it "\n" \
		"vior " #id ", vi00, vi00\n" \
	)

#define VDDD_OP_DO_III(OP, d, s, t) \
	do { \
		Vu0Flags flags; \
		SET_VI(vi01, d); \
		SET_VI(vi02, s); \
		SET_VI(vi03, t); \
		VDDD(OP, vi01, vi02, vi03); \
		printf("  %s %d, %d: ", #OP, s, t); \
		PRINT_VI(vi01, false); \
		flags.PrintChanges(true); \
	} while (false) \

#define VDDD_OP_DO_MMM(OP, d, s, t) \
	do { \
		Vu0Flags flags; \
		SET_VI_M(vi01, d, x); \
		SET_VI_M(vi02, s, x); \
		SET_VI_M(vi03, t, x); \
		VDDD(OP, vi01, vi02, vi03); \
		printf("  %s %s, %s: ", #OP, #s, #t); \
		PRINT_VI(vi01, false); \
		flags.PrintChanges(true); \
	} while (false) \

#define TEST_VDDD_FUNC(OP) \
static void test_##OP() { \
	printf("%s:\n", #OP); \
	 \
	VDDD_OP_DO_III(OP, 0x1337, 0, 0); \
	VDDD_OP_DO_III(OP, 0x1337, 0, 1); \
	VDDD_OP_DO_III(OP, 0x1337, 1, 1); \
	VDDD_OP_DO_III(OP, 0x1337, 1, 0); \
	VDDD_OP_DO_III(OP, 0x1337, 2, 2); \
	VDDD_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 1); \
	VDDD_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0xFFFFFFFF); \
	VDDD_OP_DO_III(OP, 0x1337, 0xDEAD, 0xFFFF); \
	VDDD_OP_DO_III(OP, 0x1337, 0xDEAD, 0x0F0F); \
	 \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_ZERO, CVI_ZERO); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_ZERO, CVI_ONE); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_ONE, CVI_ZERO); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_ONE, CVI_ONE); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_ONE, CVI_NEGONE); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_S16_MAX, CVI_S16_MAX); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_S16_MIN, CVI_S16_MIN); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_S32_MAX, CVI_S32_MAX); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_S32_MIN, CVI_S32_MIN); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_S64_MAX, CVI_S64_MAX); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_S64_MIN, CVI_S64_MIN); \
	VDDD_OP_DO_MMM(OP, CVI_GARBAGE1, CVI_GARBAGE1, CVI_GARBAGE2); \
	\
	SET_VI(vi01, 0x1337); \
	SET_VI(vi02, 0xDEAD); \
	SET_VI(vi03, 0x7331); \
	VZDI(OP, vi01, vi02, vi03); \
	printf("  %s -> $0: ", #OP); \
	PRINT_VI(vi01, true); \
	\
	printf("\n"); \
}

#define VDDI_OP_DO_III(OP, d, s, t) \
	do { \
		Vu0Flags flags; \
		SET_VI(vi01, d); \
		SET_VI(vi02, s); \
		VDDD(OP, vi01, vi02, t); \
		printf("  %s %d, %d: ", #OP, s, t); \
		PRINT_VI(vi01, false); \
		flags.PrintChanges(true); \
	} while (false)

#define VDDI_OP_DO_MMI(OP, d, s, t) \
	do { \
		Vu0Flags flags; \
		SET_VI_M(vi01, d, x); \
		SET_VI_M(vi02, s, x); \
		VDDD(OP, vi01, vi02, t); \
		printf("  %s %s, %s: ", #OP, #s, #t); \
		PRINT_VI(vi01, false); \
		flags.PrintChanges(true); \
	} while (false)

static void test_viaddi() {
	printf("viaddi:\n");

	VDDI_OP_DO_III(viaddi, 0x1337, 0, 0);
	VDDI_OP_DO_III(viaddi, 0x1337, 1, 1);
	VDDI_OP_DO_III(viaddi, 0x1337, 0, 15);
	VDDI_OP_DO_III(viaddi, 0x1337, 0, -16);
	VDDI_OP_DO_III(viaddi, 0x1337, 1233, 1);

	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_ZERO, 0);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_ZERO, 1);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_ONE, 0);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_ONE, 1);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_ONE, -1);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_S16_MAX, 15);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_S16_MIN, -16);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_S32_MAX, 15);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_S32_MIN, -16);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_S64_MAX, 15);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_S64_MIN, -15);
	VDDI_OP_DO_MMI(viaddi, CVI_GARBAGE1, CVI_GARBAGE1, 3);

	SET_VI_M(vi01, CVI_GARBAGE1, y);
	SET_VI_M(vi02, CVI_GARBAGE1, z);
	VZDI(viaddi, vi01, vi02, 4);
	printf("  viaddi -> $0: ");
	PRINT_VI(vi01, true);

	printf("\n");
}

TEST_VDDD_FUNC(viadd)
TEST_VDDD_FUNC(viand)
TEST_VDDD_FUNC(vior)
TEST_VDDD_FUNC(visub)

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	setup_vi_constants();

	test_viadd();
	test_viaddi();
	test_viand();
	test_vior();
	test_visub();

	printf("-- TEST END\n");

	return 0;
}
