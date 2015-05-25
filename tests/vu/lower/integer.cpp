#include <common-ee.h>
#include "../test_runner.h"

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

static u32 *const CVI_ZERO = (u32 *)(vu0_mem + 0x0000);
static u32 *const CVI_S16_MAX = (u32 *)(vu0_mem + 0x0010);
static u32 *const CVI_S16_MIN = (u32 *)(vu0_mem + 0x0020);
static u32 *const CVI_S32_MAX = (u32 *)(vu0_mem + 0x0030);
static u32 *const CVI_S32_MIN = (u32 *)(vu0_mem + 0x0040);
static u32 *const CVI_S64_MAX = (u32 *)(vu0_mem + 0x0050);
static u32 *const CVI_S64_MIN = (u32 *)(vu0_mem + 0x0060);
static u32 *const CVI_NEGONE = (u32 *)(vu0_mem + 0x0070);
static u32 *const CVI_ONE = (u32 *)(vu0_mem + 0x0080);
static u32 *const CVI_GARBAGE1 = (u32 *)(vu0_mem + 0x0090);
static u32 *const CVI_GARBAGE2 = (u32 *)(vu0_mem + 0x00A0);

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

class IntegerTestRunner : public TestRunner {
public:
	IntegerTestRunner(int vu) : TestRunner(vu) {
	}

	void PerformDDD_III(const char *name, VU::LowerOp (*func)(VU::Reg, VU::Reg, VU::Reg), u32 d, u32 s, u32 t) {
		using namespace VU;

		Reset();

		WrSetIntegerRegister(VI01, d);
		WrSetIntegerRegister(VI02, s);
		WrSetIntegerRegister(VI03, t);
		Wr(func(VI01, VI02, VI03));

		Execute();

		printf("  %s %d, %d: ", name, s, t);
		PrintRegister(VI01, false);
		PrintStatus(true);
	}

	void PerformDDD_MMM(const char *name, VU::LowerOp (*func)(VU::Reg, VU::Reg, VU::Reg), const char *dname, const u32 *d, const char *sname, const u32 *s, const char *tname, const u32 *t) {
		using namespace VU;

		Reset();

		WrLoadIntegerRegister(DEST_X, VI01, d);
		WrLoadIntegerRegister(DEST_X, VI02, s);
		WrLoadIntegerRegister(DEST_X, VI03, t);
		Wr(func(VI01, VI02, VI03));

		Execute();

		printf("  %s %s, %s: ", name, sname, tname);
		PrintRegister(VI01, false);
		PrintStatus(true);
	}

	template <typename TType>
	void PerformDDI_III(const char *name, VU::LowerOp (*func)(VU::Reg, VU::Reg, TType), u32 d, u32 s, int t) {
		using namespace VU;

		Reset();

		WrSetIntegerRegister(VI01, d);
		WrSetIntegerRegister(VI02, s);
		Wr(func(VI01, VI02, t));

		Execute();

		printf("  %s %d, %d: ", name, s, t);
		PrintRegister(VI01, false);
		PrintStatus(true);
	}

	template <typename TType>
	void PerformDDI_MMI(const char *name, VU::LowerOp (*func)(VU::Reg, VU::Reg, TType), const char *dname, const u32 *d, const char *sname, const u32 *s, int t) {
		using namespace VU;

		Reset();

		WrLoadIntegerRegister(DEST_X, VI01, d);
		WrLoadIntegerRegister(DEST_X, VI02, s);
		Wr(func(VI01, VI02, t));

		Execute();

		printf("  %s %s, %d: ", name, sname, t);
		PrintRegister(VI01, false);
		PrintStatus(true);
	}

#define PerformDDD_MMM(name, func, d, s, t) PerformDDD_MMM(name, func, #d, d, #s, s, #t, t)
#define PerformDDI_MMI(name, func, d, s, t) PerformDDI_MMI(name, func, #d, d, #s, s, t)

	void PerformDDD(const char *name, VU::LowerOp (*func)(VU::Reg, VU::Reg, VU::Reg)) {
		printf("%s\n", name);

		PerformDDD_III(name, func, 0x1337, 0, 0);
		PerformDDD_III(name, func, 0x1337, 0, 1);
		PerformDDD_III(name, func, 0x1337, 1, 1);
		PerformDDD_III(name, func, 0x1337, 1, 0);
		PerformDDD_III(name, func, 0x1337, 2, 2);
		PerformDDD_III(name, func, 0x1337, 0xFFFF, 1);
		PerformDDD_III(name, func, 0x1337, 0xFFFF, 0xFFFF);
		PerformDDD_III(name, func, 0x1337, 0xDEAD, 0xFFFF);
		PerformDDD_III(name, func, 0x1337, 0xDEAD, 0x0F0F);

		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_ZERO, CVI_ZERO);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_ZERO, CVI_ONE);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_ONE, CVI_ZERO);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_ONE, CVI_ONE);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_ONE, CVI_NEGONE);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_S16_MAX, CVI_S16_MAX);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_S16_MIN, CVI_S16_MIN);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_S32_MAX, CVI_S32_MAX);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_S32_MIN, CVI_S32_MIN);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_S64_MAX, CVI_S64_MAX);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_S64_MIN, CVI_S64_MIN);
		PerformDDD_MMM(name, func, CVI_GARBAGE1, CVI_GARBAGE1, CVI_GARBAGE2);

		using namespace VU;

		Reset();

		WrSetIntegerRegister(VI01, 0x1337);
		WrSetIntegerRegister(VI02, 0xDEAD);
		WrSetIntegerRegister(VI03, 0x7331);
		Wr(func(VI00, VI02, VI03));
		Wr(IOR(VI01, VI00, VI00));

		Execute();

		printf("  %s -> $0: ", name);
		PrintRegister(VI01, true);

		printf("\n");
	}

	void PerformDDI(const char *name, VU::LowerOp (*func)(VU::Reg, VU::Reg, s8)) {
		printf("%s\n", name);

		PerformDDI_III(name, func, 0x1337, 0, 0);
		PerformDDI_III(name, func, 0x1337, 0, 1);
		PerformDDI_III(name, func, 0x1337, 1, 1);
		PerformDDI_III(name, func, 0x1337, 0, 15);
		PerformDDI_III(name, func, 0x1337, 0, -16);
		PerformDDI_III(name, func, 0x1337, 1233, 1);

		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ZERO, 0);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ZERO, 1);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ONE, 0);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ONE, 1);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ONE, -1);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S16_MAX, 15);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S16_MIN, -16);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S32_MAX, 15);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S32_MIN, -16);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S64_MAX, 15);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S64_MIN, -16);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_GARBAGE1, 3);

		using namespace VU;

		Reset();

		WrSetIntegerRegister(VI01, 0x1337);
		WrSetIntegerRegister(VI02, 0xDEAD);
		Wr(func(VI00, VI02, 7));
		Wr(IOR(VI01, VI00, VI00));

		Execute();

		printf("  %s -> $0: ", name);
		PrintRegister(VI01, true);

		printf("\n");
	}

	void PerformDDI(const char *name, VU::LowerOp (*func)(VU::Reg, VU::Reg, u16)) {
		printf("%s\n", name);

		PerformDDI_III(name, func, 0x1337, 0, 0);
		PerformDDI_III(name, func, 0x1337, 0, 1);
		PerformDDI_III(name, func, 0x1337, 1, 1);
		PerformDDI_III(name, func, 0x1337, 1, 0);
		PerformDDI_III(name, func, 0x1337, 2, 2);
		PerformDDI_III(name, func, 0x1337, 0xFFFF, 1);
		PerformDDI_III(name, func, 0x1337, 0xFFFF, 0x7FFF);
		PerformDDI_III(name, func, 0x1337, 0xDEAD, 0x7FFF);
		PerformDDI_III(name, func, 0x1337, 0xDEAD, 0x0F0F);

		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ZERO, 0);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ZERO, 1);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ONE, 0);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ONE, 1);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_ONE, 0x7FFF);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S16_MAX, 0x7FFF);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S16_MIN, 0x7FFF);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S32_MAX, 0x7FFF);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S32_MIN, 0x7FFF);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S64_MAX, 0x7FFF);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_S64_MIN, 0x7FFF);
		PerformDDI_MMI(name, func, CVI_GARBAGE1, CVI_GARBAGE1, 0x1337);

		using namespace VU;

		Reset();

		WrSetIntegerRegister(VI01, 0x1337);
		WrSetIntegerRegister(VI02, 0xDEAD);
		Wr(func(VI00, VI02, 0x7331));
		Wr(IOR(VI01, VI00, VI00));

		Execute();

		printf("  %s -> $0: ", name);
		PrintRegister(VI01, true);

		printf("\n");
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	setup_vi_constants();

	IntegerTestRunner runner(0);

	runner.PerformDDD("iadd", VU::IADD);
	runner.PerformDDI("iaddi", VU::IADDI);
	runner.PerformDDI("iaddiu", VU::IADDIU);
	runner.PerformDDD("iand", VU::IAND);
	runner.PerformDDD("ior", VU::IOR);
	runner.PerformDDD("isub", VU::ISUB);
	runner.PerformDDI("isubiu", VU::ISUBIU);

	printf("-- TEST END\n");
	return 0;
}
