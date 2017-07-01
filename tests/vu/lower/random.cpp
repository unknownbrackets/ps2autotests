#include <common-ee.h>
#include "../test_runner.h"

static const u32 __attribute__((aligned(16))) C_ZEROONE[4]      = {0x3F800000, 0x3FFFFFFF, 0x00000000, 0xFFFFFFFF};
static const u32 __attribute__((aligned(16))) C_GARBAGE[4]      = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};

static u32 *const CVF_ZEROONE      = (u32 *)(vu1_mem + 0x0000);
static u32 *const CVF_GARBAGE      = (u32 *)(vu1_mem + 0x0010);

static void setup_vf_constants() {
	*(vu128 *)CVF_ZEROONE      = *(vu128 *)C_ZEROONE;
	*(vu128 *)CVF_GARBAGE      = *(vu128 *)C_GARBAGE;
}

class RandomTestRunner : public TestRunner {
public:
	RandomTestRunner(int vu) : TestRunner(vu) {
	}
	
	void PerformInit() {
		using namespace VU;
		
		Reset();
		
		WrLoadFloatRegister(DEST_XYZW, VF16, CVF_GARBAGE);
		
		Wr(RINIT(FIELD_X, VF16));
		Wr(RGET(DEST_XYZW, VF01));
		Wr(RINIT(FIELD_Y, VF16));
		Wr(RGET(DEST_XYZW, VF02));
		Wr(RINIT(FIELD_Z, VF16));
		Wr(RGET(DEST_XYZW, VF03));
		Wr(RINIT(FIELD_W, VF16));
		Wr(RGET(DEST_XYZW, VF04));
		
		Execute();

		printf("RINIT: \n"); 
		printf("  "); PrintRegister(VF01, true);
		printf("  "); PrintRegister(VF02, true);
		printf("  "); PrintRegister(VF03, true);
		printf("  "); PrintRegister(VF04, true);
	}
	
	void PerformXor() {
		using namespace VU;
		
		Reset();
		
		WrLoadFloatRegister(DEST_XYZW, VF16, CVF_GARBAGE);
		
		Wr(RINIT(FIELD_X, VF00));
		Wr(RXOR(FIELD_X, VF16));
		Wr(RGET(DEST_XYZW, VF01));
		Wr(RXOR(FIELD_Y, VF16));
		Wr(RGET(DEST_XYZW, VF02));
		Wr(RXOR(FIELD_Z, VF16));
		Wr(RGET(DEST_XYZW, VF03));
		Wr(RXOR(FIELD_W, VF16));
		Wr(RGET(DEST_XYZW, VF04));
		
		Execute();

		printf("RXOR: \n"); 
		printf("  "); PrintRegister(VF01, true);
		printf("  "); PrintRegister(VF02, true);
		printf("  "); PrintRegister(VF03, true);
		printf("  "); PrintRegister(VF04, true);
	}
	
	void PerformNext_M(const char *fieldName, VU::Field field, const char *sname, const u32* s) {
		using namespace VU;
		
		printf("  %s[%s]: \n", sname, fieldName); 
		
		Reset();
		
		WrLoadFloatRegister(DEST_XYZW, VF16, s);
		
		Wr(RINIT(field, VF16));
		Wr(RNEXT(DEST_XYZW, VF01));
		Wr(RNEXT(DEST_XYZW, VF02));
		Wr(RNEXT(DEST_XYZW, VF03));
		Wr(RNEXT(DEST_XYZW, VF04));
		
		Execute();
		
		printf("    1: "); PrintRegister(VF01, true);
		printf("    2: "); PrintRegister(VF02, true);
		printf("    3: "); PrintRegister(VF03, true);
		printf("    4: "); PrintRegister(VF04, true);
	}
	
#define PerformNext_M(field, s) PerformNext_M(#field, field, #s, s)
	
	void PerformNext()
	{
		printf("RNEXT:\n");
		
		PerformNext_M(VU::FIELD_X, CVF_ZEROONE);
		PerformNext_M(VU::FIELD_Y, CVF_ZEROONE);
		PerformNext_M(VU::FIELD_Z, CVF_ZEROONE);
		PerformNext_M(VU::FIELD_W, CVF_ZEROONE);
		PerformNext_M(VU::FIELD_X, CVF_GARBAGE);
		PerformNext_M(VU::FIELD_Y, CVF_GARBAGE);
		PerformNext_M(VU::FIELD_Z, CVF_GARBAGE);
		PerformNext_M(VU::FIELD_W, CVF_GARBAGE);
	}
	
#undef PerformNext_M
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	setup_vf_constants();
	
	RandomTestRunner runner(1);
	runner.PerformInit();
	runner.PerformXor();
	runner.PerformNext();

	printf("-- TEST END\n");
	return 0;
}
