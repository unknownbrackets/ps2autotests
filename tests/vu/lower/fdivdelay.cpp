#include <common-ee.h>
#include "../test_runner.h"

static VU::LowerOp DIV_adapter(VU::Field field, VU::Reg reg) {
	using namespace VU;
	return DIV(FIELD_W, VF00, field, reg);
}

static VU::LowerOp RSQRT_adapter(VU::Field field, VU::Reg reg) {
	using namespace VU;
	return RSQRT(FIELD_W, VF00, field, reg);
}

class FdivDelayTestRunner : public TestRunner {
public:
	FdivDelayTestRunner(int vu) : TestRunner(vu) {
	}

	typedef VU::LowerOp (*FdivFunction)(VU::Field field, VU::Reg reg);

	void PerformSingleWaitQ(const char *name, FdivFunction func) {
		using namespace VU;

		printf("%s single waitq:\n", name);

		Reset();

		//Prepare registers
		WrImm(4.0f);
		Wr(MULi(DEST_W, VF01, VF00));
		
		//Op
		Wr(func(FIELD_W, VF01));
		Wr(WAITQ());
		Wr(MULq(DEST_XYZW, VF01, VF00));
		
		Execute();

		printf("  VF01: "); PrintRegister(VF01, true);
	}

	void PerformMultipleWaitQ(const char *name, FdivFunction func) {
		using namespace VU;

		printf("%s multiple waitq:\n", name);

		Reset();

		//Prepare registers
		WrImm(4.0f);
		WrImm(MULi(DEST_W, VF01, VF00), 16.0f);
		Wr(MULi(DEST_W, VF02, VF00));
		
		//Op
		Wr(func(FIELD_W, VF01));
		Wr(WAITQ());
		Wr(MULq(DEST_XYZW, VF01, VF00));
		Wr(func(FIELD_W, VF02));
		Wr(WAITQ());
		Wr(MULq(DEST_XYZW, VF02, VF00));

		Execute();

		printf("  VF01: "); PrintRegister(VF01, true);
		printf("  VF02: "); PrintRegister(VF02, true);
	}

	void PerformSingleDelayed(const char *name, FdivFunction func) {
		using namespace VU;

		printf("%s single delayed:\n", name);

		Reset();

		//Set Q to 1.0f
		Wr(DIV(FIELD_W, VF00, FIELD_W, VF00));
		Wr(WAITQ());

		//Prepare registers
		WrImm(4.0f);
		Wr(MULi(DEST_W, VF01, VF00));
		
		//Op
		Wr(func(FIELD_W, VF01));
		Wr(MULq(DEST_XYZW, VF01, VF00));
		Wr(MULq(DEST_XYZW, VF02, VF00));
		Wr(MULq(DEST_XYZW, VF03, VF00));
		Wr(MULq(DEST_XYZW, VF04, VF00));
		Wr(MULq(DEST_XYZW, VF05, VF00));
		Wr(MULq(DEST_XYZW, VF06, VF00));
		Wr(MULq(DEST_XYZW, VF07, VF00));
		Wr(MULq(DEST_XYZW, VF08, VF00));
		Wr(MULq(DEST_XYZW, VF09, VF00));
		Wr(MULq(DEST_XYZW, VF10, VF00));
		Wr(MULq(DEST_XYZW, VF11, VF00));
		Wr(MULq(DEST_XYZW, VF12, VF00));
		Wr(MULq(DEST_XYZW, VF13, VF00));
		Wr(MULq(DEST_XYZW, VF14, VF00));
		
		Execute();

		printf("  VF01: "); PrintRegister(VF01, true);
		printf("  VF02: "); PrintRegister(VF02, true);
		printf("  VF03: "); PrintRegister(VF03, true);
		printf("  VF04: "); PrintRegister(VF04, true);
		printf("  VF05: "); PrintRegister(VF05, true);
		printf("  VF06: "); PrintRegister(VF06, true);
		printf("  VF07: "); PrintRegister(VF07, true);
		printf("  VF08: "); PrintRegister(VF08, true);
		printf("  VF09: "); PrintRegister(VF09, true);
		printf("  VF10: "); PrintRegister(VF10, true);
		printf("  VF11: "); PrintRegister(VF11, true);
		printf("  VF12: "); PrintRegister(VF12, true);
		printf("  VF13: "); PrintRegister(VF13, true);
		printf("  VF14: "); PrintRegister(VF14, true);
	}
	
	void PerformMultipleDelayed1(const char *name, FdivFunction func) {
		using namespace VU;

		printf("%s multiple delayed (1):\n", name);

		Reset();

		//Set Q to 1.0f
		Wr(DIV(FIELD_W, VF00, FIELD_W, VF00));
		Wr(WAITQ());

		//Prepare registers
		WrImm(4.0f);
		WrImm(MULi(DEST_W, VF01, VF00), 16.0f);
		Wr(MULi(DEST_W, VF02, VF00));
		
		//Op
		Wr(func(FIELD_W, VF01));
		Wr(func(FIELD_W, VF02));
		Wr(MULq(DEST_XYZW, VF01, VF00));
		Wr(MULq(DEST_XYZW, VF02, VF00));
		Wr(MULq(DEST_XYZW, VF03, VF00));
		Wr(MULq(DEST_XYZW, VF04, VF00));
		Wr(MULq(DEST_XYZW, VF05, VF00));
		Wr(MULq(DEST_XYZW, VF06, VF00));
		Wr(MULq(DEST_XYZW, VF07, VF00));
		Wr(MULq(DEST_XYZW, VF08, VF00));
		Wr(MULq(DEST_XYZW, VF09, VF00));
		Wr(MULq(DEST_XYZW, VF10, VF00));
		Wr(MULq(DEST_XYZW, VF11, VF00));
		Wr(MULq(DEST_XYZW, VF12, VF00));
		Wr(MULq(DEST_XYZW, VF13, VF00));
		Wr(MULq(DEST_XYZW, VF14, VF00));
		
		Execute();

		printf("  VF01: "); PrintRegister(VF01, true);
		printf("  VF02: "); PrintRegister(VF02, true);
		printf("  VF03: "); PrintRegister(VF03, true);
		printf("  VF04: "); PrintRegister(VF04, true);
		printf("  VF05: "); PrintRegister(VF05, true);
		printf("  VF06: "); PrintRegister(VF06, true);
		printf("  VF07: "); PrintRegister(VF07, true);
		printf("  VF08: "); PrintRegister(VF08, true);
		printf("  VF09: "); PrintRegister(VF09, true);
		printf("  VF10: "); PrintRegister(VF10, true);
		printf("  VF11: "); PrintRegister(VF11, true);
		printf("  VF12: "); PrintRegister(VF12, true);
		printf("  VF13: "); PrintRegister(VF13, true);
		printf("  VF14: "); PrintRegister(VF14, true);
	}
	
	void PerformMultipleDelayed2(const char *name, FdivFunction func) {
		using namespace VU;

		printf("%s multiple delayed (2):\n", name);

		Reset();

		//Set Q to 1.0f
		Wr(DIV(FIELD_W, VF00, FIELD_W, VF00));
		Wr(WAITQ());

		//Prepare registers
		WrImm(4.0f);
		WrImm(MULi(DEST_W, VF01, VF00), 16.0f);
		Wr(MULi(DEST_W, VF02, VF00));
		
		//Op
		Wr(func(FIELD_W, VF01));
		Wr(MULq(DEST_XYZW, VF01, VF00));
		Wr(func(FIELD_W, VF02));
		Wr(MULq(DEST_XYZW, VF02, VF00));
		Wr(MULq(DEST_XYZW, VF03, VF00));
		Wr(MULq(DEST_XYZW, VF04, VF00));
		Wr(MULq(DEST_XYZW, VF05, VF00));
		Wr(MULq(DEST_XYZW, VF06, VF00));
		Wr(MULq(DEST_XYZW, VF07, VF00));
		Wr(MULq(DEST_XYZW, VF08, VF00));
		Wr(MULq(DEST_XYZW, VF09, VF00));
		Wr(MULq(DEST_XYZW, VF10, VF00));
		Wr(MULq(DEST_XYZW, VF11, VF00));
		Wr(MULq(DEST_XYZW, VF12, VF00));
		Wr(MULq(DEST_XYZW, VF13, VF00));
		Wr(MULq(DEST_XYZW, VF14, VF00));
		
		Execute();

		printf("  VF01: "); PrintRegister(VF01, true);
		printf("  VF02: "); PrintRegister(VF02, true);
		printf("  VF03: "); PrintRegister(VF03, true);
		printf("  VF04: "); PrintRegister(VF04, true);
		printf("  VF05: "); PrintRegister(VF05, true);
		printf("  VF06: "); PrintRegister(VF06, true);
		printf("  VF07: "); PrintRegister(VF07, true);
		printf("  VF08: "); PrintRegister(VF08, true);
		printf("  VF09: "); PrintRegister(VF09, true);
		printf("  VF10: "); PrintRegister(VF10, true);
		printf("  VF11: "); PrintRegister(VF11, true);
		printf("  VF12: "); PrintRegister(VF12, true);
		printf("  VF13: "); PrintRegister(VF13, true);
		printf("  VF14: "); PrintRegister(VF14, true);
	}

	void Perform(const char *name, FdivFunction func) {
		using namespace VU;
		PerformSingleWaitQ(name, func);
		PerformMultipleWaitQ(name, func);
		PerformSingleDelayed(name, func);
		PerformMultipleDelayed1(name, func);
		PerformMultipleDelayed2(name, func);
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	FdivDelayTestRunner runner(0);

	runner.Perform("DIV",   &DIV_adapter);
	runner.Perform("SQRT",  &VU::SQRT);
	runner.Perform("RSQRT", &RSQRT_adapter);
	
	printf("-- TEST END\n");
	return 0;
}
