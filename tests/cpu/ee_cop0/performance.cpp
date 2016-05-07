#include <common-ee.h>

enum RegPCCR
{
	PCCR_EXL0 = 1 << 1,
	PCCR_K0 = 1 << 2,
	PCCR_S0 = 1 << 3,
	PCCR_U0 = 1 << 4,
	
	PCCR_EVENT0_MASK = 31 << 5,
	PCCR_EVENT0_PROCESSOR_CYCLE = 1 << 5,
	PCCR_EVENT0_NO_EVENT = 16 << 5,
	
	PCCR_EXL1 = 1 << 11,
	PCCR_K1 = 1 << 12,
	PCCR_S1 = 1 << 13,
	PCCR_U1 = 1 << 14,
	
	PCCR_EVENT1_MASK = 31 << 15,
	PCCR_EVENT1_PROCESSOR_CYCLE = 1 << 15,
	PCCR_EVENT1_NO_EVENT = 16 << 15,
	
	PCCR_CTE = 1 << 31
};

template <int index>
static u32 getEventSpecifier() {
	u32 value = 0;
	asm volatile (
		"mfps    %0, %1\n"
		: "+r"(value) : "i"(index)
	);
	return value;
}

template <int index>
static void setEventSpecifier(u32 value) {
	asm volatile (
		"mtps    %0, %1\n"
		"sync.p\n"
		: "+r"(value) : "i"(index)
	);
}

template <int index>
static u32 getCounter() {
	u32 value = 0;
	asm volatile (
		"mfpc    %0, %1\n"
		: "+r"(value) : "i"(index)
	);
	return value;
}

template <int index>
static void setCounter(u32 value) {
	asm volatile (
		"mtpc    %0, %1\n"
		"sync.p\n"
		: "+r"(value) : "i"(index)
	);
}

typedef u32 (*getCounterFunction)();

static const getCounterFunction getCounterTable[32] = {
	&getCounter<0>,  &getCounter<1>,  &getCounter<2>,  &getCounter<3>,
	&getCounter<4>,  &getCounter<5>,  &getCounter<6>,  &getCounter<7>,
	&getCounter<8>,  &getCounter<9>,  &getCounter<10>, &getCounter<11>,
	&getCounter<12>, &getCounter<13>, &getCounter<14>, &getCounter<15>,
	&getCounter<16>, &getCounter<17>, &getCounter<18>, &getCounter<19>,
	&getCounter<20>, &getCounter<21>, &getCounter<22>, &getCounter<23>,
	&getCounter<24>, &getCounter<25>, &getCounter<26>, &getCounter<27>,
	&getCounter<28>, &getCounter<29>, &getCounter<30>, &getCounter<31>
};

void printCounterValues() {
	for(u32 i = 0; i < 32; i++) {
		u32 counterValue = getCounterTable[i]();
		printf("%d", counterValue);
	}
	printf("\n");
}

void testCounterChange(u32 eventSpecifier) {
	//Reset counter registers
	setEventSpecifier<0>(0);
	setCounter<0>(0);
	setCounter<1>(0);
	
	//Start counter
	setEventSpecifier<0>(PCCR_CTE | eventSpecifier);
	for(u32 i = 0; i < 0x100; i++) { asm volatile("nop\nnop\nnop\n"); }
	
	//Stop counter
	setEventSpecifier<0>(0);
	
	//Compare counter values
	u32 pcr0 = getCounter<0>();
	u32 pcr1 = getCounter<1>();
	printf("pcr0 -> %s, pcr1 -> %s\n", 
		(pcr0 != 0) ? "changed" : "unchanged", 
		(pcr1 != 0) ? "changed" : "unchanged");
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	//Disable counters
	setEventSpecifier<0>(0);
	
	{
		printf("setting pcr0: ");
		setCounter<0>(0x89ABCDEF);
		u32 value = getCounter<0>();
		printf("%08x\n", value);
	}
	
	{
		printf("setting pcr1: ");
		setCounter<1>(0x01234567);
		u32 value = getCounter<1>();
		printf("%08x\n", value);
	}
	
	{
		printf("setting pccr: ");
		setEventSpecifier<0>(0x7FFFFFFF);
		u32 value = getEventSpecifier<0>();
		printf("%08x\n", value);
	}
	
	{
		printf("reading from even/odd numbered pcr specifier maps to pcr0/pcr1: ");
		setCounter<0>(1);
		setCounter<1>(2);
		printCounterValues();
	}
	
	{
		printf("writing to even/odd numbered pcr specifier maps to pcr0/pcr1: ");
		setCounter<30>(4);
		setCounter<31>(5);
		printCounterValues();
	}
	
	{
		printf("reading from any pccr specifier maps to pccr: ");
		setEventSpecifier<0>(PCCR_EVENT0_PROCESSOR_CYCLE | PCCR_EVENT1_PROCESSOR_CYCLE);
		u32 value = getEventSpecifier<31>();
		printf("%08x\n", value);
	}
	
	{
		printf("writing to any pccr specifier other than 0 is no-op: ");
		setEventSpecifier<0>(0);
		setEventSpecifier<15>(PCCR_EVENT0_NO_EVENT | PCCR_EVENT1_NO_EVENT);
		u32 value = getEventSpecifier<0>();
		printf("%08x\n", value);
	}
	
	{
		printf("counter 0 (all modes, processor cycles), counter 1 (no modes, 0): ");
		testCounterChange(PCCR_EXL0 | PCCR_K0 | PCCR_S0 | PCCR_U0 | PCCR_EVENT0_PROCESSOR_CYCLE);
	}
	
	{
		printf("counter 0 (no modes, 0), counter 1 (all modes, processor cycles): ");
		testCounterChange(PCCR_EXL1 | PCCR_K1 | PCCR_S1 | PCCR_U1 | PCCR_EVENT1_PROCESSOR_CYCLE);
	}
	
	{
		printf("counter 0 (all modes, processor cycles), counter 1 (all modes, processor cycles): ");
		testCounterChange(
			PCCR_EXL0 | PCCR_K0 | PCCR_S0 | PCCR_U0 | PCCR_EVENT0_PROCESSOR_CYCLE |
			PCCR_EXL1 | PCCR_K1 | PCCR_S1 | PCCR_U1 | PCCR_EVENT1_PROCESSOR_CYCLE);
	}
	
	{
		printf("counter 0 (no modes, processor cycles), counter 1 (no modes, processor cycles): ");
		testCounterChange(PCCR_EVENT0_PROCESSOR_CYCLE | PCCR_EVENT1_PROCESSOR_CYCLE);
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
