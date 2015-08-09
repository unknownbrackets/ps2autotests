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

typedef s32 (*IntcHandlerFunction)(s32, void *, void *);

void appendResult(u32 result) {
	if(g_orderPos != MAX_INTC_RESULTS) {
		iSignalSema(g_intcSema);
		g_order[g_orderPos] = result;
		g_orderPos++;
	}
}

s32 continueIntcHandler(s32 cause, void *arg, void *addr) {
	appendResult((u32)arg);
	return 0;
}

s32 cancelIntcHandler(s32 cause, void *arg, void *addr) {
	appendResult((u32)arg);
	return -1;
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

void addTestIntcHandler(IntcHandlerFunction handler, s32 next, void *arg) {
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
	addTestIntcHandler(&continueIntcHandler, -1, (void*)1);
	addTestIntcHandler(&continueIntcHandler, -1, (void*)2);
	addTestIntcHandler(&continueIntcHandler, -1, (void*)3);
	waitForTestResults();
	finishTest();
}

void pushFrontTest() {
	printf("push front: ");
	initializeTest();
	addTestIntcHandler(&continueIntcHandler, 0, (void*)1);
	addTestIntcHandler(&continueIntcHandler, 0, (void*)2);
	addTestIntcHandler(&continueIntcHandler, 0, (void*)3);
	waitForTestResults();
	finishTest();
}

void mixedFrontBackTest() {
	printf("mixed front back: ");
	initializeTest();
	addTestIntcHandler(&continueIntcHandler, -1, (void*)1);
	addTestIntcHandler(&continueIntcHandler,  0, (void*)2);
	addTestIntcHandler(&continueIntcHandler, -1, (void*)3);
	addTestIntcHandler(&continueIntcHandler,  0, (void*)4);
	waitForTestResults();
	finishTest();
}

void relativeTest() {
	printf("relative: ");
	initializeTest();
	addTestIntcHandler(&continueIntcHandler, -1,                (void*)1);
	addTestIntcHandler(&continueIntcHandler, -1,                (void*)2);
	addTestIntcHandler(&continueIntcHandler, -1,                (void*)3);
	addTestIntcHandler(&continueIntcHandler, g_intcHandlers[1], (void*)4);
	waitForTestResults();
	finishTest();
}

void removeTest() {
	printf("remove: ");
	initializeTest();
	addTestIntcHandler(&continueIntcHandler, -1, (void*)1);
	addTestIntcHandler(&continueIntcHandler, -1, (void*)2);
	addTestIntcHandler(&continueIntcHandler, -1, (void*)3);
	removeTestIntcHandler(1);
	waitForTestResults();
	finishTest();
}

void cancelTest() {
	printf("cancel: ");
	initializeTest();
	addTestIntcHandler(&continueIntcHandler, -1, (void*)1);
	addTestIntcHandler(&cancelIntcHandler,   -1, (void*)2);
	addTestIntcHandler(&continueIntcHandler, -1, (void*)3);
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
	
	printf("-- TEST END\n");
	
	return 0;
}
