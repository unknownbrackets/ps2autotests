#include <common-ee.h>
#include "../test_runner.h"

static const u32 __attribute__((aligned(16))) C_ZERO[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
static const u32 __attribute__((aligned(16))) C_NEGZERO[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
static const u32 __attribute__((aligned(16))) C_MAX[4] = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
static const u32 __attribute__((aligned(16))) C_MIN[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static const u32 __attribute__((aligned(16))) C_MAX_MANTISSA[4] = {0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF};
static const u32 __attribute__((aligned(16))) C_MAX_EXP[4] = {0x7F800001, 0x7F800001, 0x7F800001, 0x7F800001};
static const u32 __attribute__((aligned(16))) C_MIN_EXP[4] = {0x00000001, 0x00000001, 0x00000001, 0x00000001};
static const u32 __attribute__((aligned(16))) C_ONE[4] = {0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000};
static const u32 __attribute__((aligned(16))) C_NEGONE[4] = {0xBF800000, 0xBF800000, 0xBF800000, 0xBF800000};
static const u32 __attribute__((aligned(16))) C_GARBAGE1[4] = {0x00001337, 0x00001337, 0x00001337, 0x00001337};
static const u32 __attribute__((aligned(16))) C_GARBAGE2[4] = {0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF};
static const u32 __attribute__((aligned(16))) C_INCREASING[4] = {0x3F800000, 0x40000000, 0x40400000, 0x40800000};
static const u32 __attribute__((aligned(16))) C_DECREASING[4] = {0x40800000, 0x40400000, 0x40000000, 0x3F800000};

static u32 *const CVF_ZERO         = (u32 *)(vu1_mem + 0x0000);
static u32 *const CVF_NEGZERO      = (u32 *)(vu1_mem + 0x0010);
static u32 *const CVF_MAX          = (u32 *)(vu1_mem + 0x0020);
static u32 *const CVF_MIN          = (u32 *)(vu1_mem + 0x0030);
static u32 *const CVF_MAX_MANTISSA = (u32 *)(vu1_mem + 0x0040);
static u32 *const CVF_MAX_EXP      = (u32 *)(vu1_mem + 0x0050);
static u32 *const CVF_MIN_EXP      = (u32 *)(vu1_mem + 0x0060);
static u32 *const CVF_ONE          = (u32 *)(vu1_mem + 0x0070);
static u32 *const CVF_NEGONE       = (u32 *)(vu1_mem + 0x0080);
static u32 *const CVF_GARBAGE1     = (u32 *)(vu1_mem + 0x0090);
static u32 *const CVF_GARBAGE2     = (u32 *)(vu1_mem + 0x00A0);
static u32 *const CVF_INCREASING   = (u32 *)(vu1_mem + 0x00B0);
static u32 *const CVF_DECREASING   = (u32 *)(vu1_mem + 0x00C0);

static void setup_vf_constants() {
	*(vu128 *)CVF_ZERO = *(vu128 *)C_ZERO;
	*(vu128 *)CVF_NEGZERO = *(vu128 *)C_NEGZERO;
	*(vu128 *)CVF_MAX = *(vu128 *)C_MAX;
	*(vu128 *)CVF_MIN = *(vu128 *)C_MIN;
	*(vu128 *)CVF_MAX_MANTISSA = *(vu128 *)C_MAX_MANTISSA;
	*(vu128 *)CVF_MAX_EXP = *(vu128 *)C_MAX_EXP;
	*(vu128 *)CVF_MIN_EXP = *(vu128 *)C_MIN_EXP;
	*(vu128 *)CVF_ONE = *(vu128 *)C_ONE;
	*(vu128 *)CVF_NEGONE = *(vu128 *)C_NEGONE;
	*(vu128 *)CVF_GARBAGE1 = *(vu128 *)C_GARBAGE1;
	*(vu128 *)CVF_GARBAGE2 = *(vu128 *)C_GARBAGE2;
	*(vu128 *)CVF_INCREASING = *(vu128 *)C_INCREASING;
	*(vu128 *)CVF_DECREASING = *(vu128 *)C_DECREASING;
}

class EfuTestRunner : public TestRunner {
public:
	EfuTestRunner(int vu) : TestRunner(vu) {
	}

	typedef VU::LowerOp (*EfuFunctionScalar)(VU::Field field, VU::Reg reg);
	typedef VU::LowerOp (*EfuFunctionVector)(VU::Reg reg);

	void PerformS_M(const char *name, EfuFunctionScalar func, const char *tname, const u32 *t) {
		using namespace VU;
		
		Reset();
		
		WrLoadFloatRegister(DEST_XYZW, VF01, t);
		
		//Op
		Wr(func(FIELD_Z, VF01));
		Wr(WAITP());
		Wr(MFP(DEST_XYZW, VF02));
		
		Execute();

		printf("  %s %s: ", name, tname); 
		PrintRegisterField(VF02, FIELD_X, true);
	}
	
	void PerformV_M(const char *name, EfuFunctionVector func, const char *tname, const u32 *t) {
		using namespace VU;
		
		Reset();
		
		WrLoadFloatRegister(DEST_XYZW, VF01, t);
		
		//Op
		Wr(func(VF01));
		Wr(WAITP());
		Wr(MFP(DEST_XYZW, VF02));
		
		Execute();

		printf("  %s %s: ", name, tname); 
		PrintRegisterField(VF02, FIELD_X, true);
	}
	
#define PerformS_M(name, func, t) PerformS_M(name, func, #t, t)
#define PerformV_M(name, func, t) PerformV_M(name, func, #t, t)
	
	void PerformS(const char *name, EfuFunctionScalar func) {
		printf("%s:\n", name);
		
		PerformS_M(name, func, CVF_ZERO);
		PerformS_M(name, func, CVF_NEGZERO);
		PerformS_M(name, func, CVF_MAX);
		PerformS_M(name, func, CVF_MIN);
		PerformS_M(name, func, CVF_MAX_MANTISSA);
		PerformS_M(name, func, CVF_MAX_EXP);
		PerformS_M(name, func, CVF_MIN_EXP);
		PerformS_M(name, func, CVF_ONE);
		PerformS_M(name, func, CVF_NEGONE);
		PerformS_M(name, func, CVF_GARBAGE1);
		PerformS_M(name, func, CVF_GARBAGE2);
		PerformS_M(name, func, CVF_INCREASING);
		PerformS_M(name, func, CVF_DECREASING);
	}
	
	void PerformV(const char *name, EfuFunctionVector func) {
		printf("%s:\n", name);
		
		PerformV_M(name, func, CVF_ZERO);
		PerformV_M(name, func, CVF_NEGZERO);
		PerformV_M(name, func, CVF_MAX);
		PerformV_M(name, func, CVF_MIN);
		PerformV_M(name, func, CVF_MAX_MANTISSA);
		PerformV_M(name, func, CVF_MAX_EXP);
		PerformV_M(name, func, CVF_MIN_EXP);
		PerformV_M(name, func, CVF_ONE);
		PerformV_M(name, func, CVF_NEGONE);
		PerformV_M(name, func, CVF_GARBAGE1);
		PerformV_M(name, func, CVF_GARBAGE2);
		PerformV_M(name, func, CVF_INCREASING);
		PerformV_M(name, func, CVF_DECREASING);
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	setup_vf_constants();
	
	EfuTestRunner runner(1);

	runner.PerformS("EATAN", &VU::EATAN);
	runner.PerformV("EATANxy", &VU::EATANxy);
	runner.PerformV("EATANxz", &VU::EATANxz);
	runner.PerformS("EEXP", &VU::EEXP);
	runner.PerformV("ELENG", &VU::ELENG);
	runner.PerformS("ERCPR", &VU::ERCPR);
	runner.PerformV("ERLENG", &VU::ERLENG);
	runner.PerformV("ERSADD", &VU::ERSADD);
	runner.PerformS("ERSQRT", &VU::ERSQRT);
	runner.PerformV("ESADD", &VU::ESADD);
	runner.PerformS("ESIN", &VU::ESIN);
	runner.PerformS("ESQRT", &VU::ESQRT);
	runner.PerformV("ESUM", &VU::ESUM);
	
	printf("-- TEST END\n");
	return 0;
}
