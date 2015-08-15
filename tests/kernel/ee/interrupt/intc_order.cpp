#include <common-ee.h>
#include <kernel.h>
#include <string.h>

#define MAX_INTC_RESULTS 9
#define MAX_INTC_HANDLERS 10

static u32 g_intcSema = 0;
static u32 g_intcHandlers[MAX_INTC_HANDLERS];
static u32 g_intcHandlerCount = 0;
static u32 g_orderPos = 0;
static u32 g_order[MAX_INTC_RESULTS];

typedef s32 (*IntcHandlerFunction)(s32);
typedef s32 (*IntcHandlerWithParamFunction)(s32, void *, void *);

void appendResult(u32 result) {
	if(g_orderPos != MAX_INTC_RESULTS) {
		iSignalSema(g_intcSema);
		g_order[g_orderPos] = result;
		g_orderPos++;
	}
}

template <s32 number, s32 returnValue>
s32 intcHandler(s32 cause) {
	appendResult(number);
	return returnValue;
}

template <s32 returnValue>
s32 intcHandlerWithParam(s32 cause, void *arg, void *addr) {
	appendResult((u32)arg);
	return returnValue;
}

void initializeTest() {
	g_orderPos = 0;
	g_intcHandlerCount = 0;
	{
		ee_sema_t intcSemaInfo;
		memset(&intcSemaInfo, 0, sizeof(intcSemaInfo));
		intcSemaInfo.init_count = 0;
		intcSemaInfo.max_count  = MAX_INTC_RESULTS;
		intcSemaInfo.option     = 0;
		g_intcSema = CreateSema(&intcSemaInfo);
	}
}

void addTestIntcHandler(IntcHandlerFunction handler, s32 next) {
	int handlerId = AddIntcHandler(INTC_VBLANK_S, handler, next);
	g_intcHandlers[g_intcHandlerCount] = handlerId;
	g_intcHandlerCount++;
}

void addTestIntcHandlerWithParam(IntcHandlerWithParamFunction handler, s32 next, void *arg) {
	int handlerId = AddIntcHandler2(INTC_VBLANK_S, handler, next, arg);
	g_intcHandlers[g_intcHandlerCount] = handlerId;
	g_intcHandlerCount++;
}

void removeTestIntcHandler(int index) {
	RemoveIntcHandler(INTC_VBLANK_S, g_intcHandlers[index]);
	for(u32 i = index + 1; i < g_intcHandlerCount; i++) {
		g_intcHandlers[i - 1] = g_intcHandlers[i];
	}
	g_intcHandlerCount--;
}

void waitForTestResults() {
	EnableIntc(INTC_VBLANK_S);
	for(u32 i = 0; i < MAX_INTC_RESULTS; i++) {
		WaitSema(g_intcSema);
	}
	DisableIntc(INTC_VBLANK_S);
}

void finishTest() {
	for(u32 i = 0; i < MAX_INTC_RESULTS; i++) {
		if(i != 0) {
			printf(", %d", g_order[i]);
		} else {
			printf("%d", g_order[i]);
		}
	}
	printf("\n");
	for(u32 i = 0; i < g_intcHandlerCount; i++) {
		RemoveIntcHandler(INTC_VBLANK_S, g_intcHandlers[i]);
	}
	DeleteSema(g_intcSema);
}

void pushBackTest() {
	printf("push back: ");
	initializeTest();
	addTestIntcHandler(&intcHandler<1, 0>, -1);
	addTestIntcHandler(&intcHandler<2, 0>, -1);
	addTestIntcHandler(&intcHandler<3, 0>, -1);
	waitForTestResults();
	finishTest();
}

void pushFrontTest() {
	printf("push front: ");
	initializeTest();
	addTestIntcHandler(&intcHandler<1, 0>, 0);
	addTestIntcHandler(&intcHandler<2, 0>, 0);
	addTestIntcHandler(&intcHandler<3, 0>, 0);
	waitForTestResults();
	finishTest();
}

void mixedFrontBackTest() {
	printf("mixed front back: ");
	initializeTest();
	addTestIntcHandler(&intcHandler<1, 0>, -1);
	addTestIntcHandler(&intcHandler<2, 0>,  0);
	addTestIntcHandler(&intcHandler<3, 0>, -1);
	addTestIntcHandler(&intcHandler<4, 0>,  0);
	waitForTestResults();
	finishTest();
}

void relativeTest() {
	printf("relative: ");
	initializeTest();
	addTestIntcHandler(&intcHandler<1, 0>, -1);
	addTestIntcHandler(&intcHandler<2, 0>, -1);
	addTestIntcHandler(&intcHandler<3, 0>, -1);
	addTestIntcHandler(&intcHandler<4, 0>, g_intcHandlers[1]);
	waitForTestResults();
	finishTest();
}

void removeTest() {
	printf("remove: ");
	initializeTest();
	addTestIntcHandler(&intcHandler<1, 0>, -1);
	addTestIntcHandler(&intcHandler<2, 0>, -1);
	addTestIntcHandler(&intcHandler<3, 0>, -1);
	removeTestIntcHandler(1);
	waitForTestResults();
	finishTest();
}

void cancelTest() {
	printf("cancel: ");
	initializeTest();
	addTestIntcHandler(&intcHandler<1, 0>,  -1);
	addTestIntcHandler(&intcHandler<2, -1>, -1);
	addTestIntcHandler(&intcHandler<3, 0>,  -1);
	waitForTestResults();
	finishTest();
}

void returnValueTest() {
	printf("return value: ");
	initializeTest();
	addTestIntcHandler(&intcHandler<1, 0x01234567>, -1);
	addTestIntcHandler(&intcHandler<2, 0x76543210>, -1);
	addTestIntcHandler(&intcHandler<3, 0x89ABCDEF>, -1);
	addTestIntcHandler(&intcHandler<4, 0xDEADBEEF>, -1);
	waitForTestResults();
	finishTest();
}

void returnValueTest2() {
	printf("return value (using AddIntcHandler2): ");
	initializeTest();
	addTestIntcHandlerWithParam(&intcHandlerWithParam<0x01234567>, -1, (void*)1);
	addTestIntcHandlerWithParam(&intcHandlerWithParam<0x76543210>, -1, (void*)2);
	addTestIntcHandlerWithParam(&intcHandlerWithParam<0x89ABCDEF>, -1, (void*)3);
	addTestIntcHandlerWithParam(&intcHandlerWithParam<0xDEADBEEF>, -1, (void*)4);
	waitForTestResults();
	finishTest();
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	pushBackTest();
	pushFrontTest();
	mixedFrontBackTest();
	relativeTest();
	removeTest();
	cancelTest();
	returnValueTest();
	returnValueTest2();
	
	printf("-- TEST END\n");
	
	return 0;
}
