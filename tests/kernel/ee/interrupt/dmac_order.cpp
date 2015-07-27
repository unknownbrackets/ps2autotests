#include <common-ee.h>
#include <kernel.h>
#include <ee_regs.h>
#include <string.h>

#define MAX_DMAC_RESULTS 9
#define MAX_DMAC_HANDLERS 10

static u32 g_dmacSema = 0;
static u32 g_dmacHandlers[MAX_DMAC_HANDLERS];
static u32 g_dmacHandlerCount = 0;
static u32 g_orderPos = 0;
static u32 g_order[MAX_DMAC_RESULTS];

static const u32 __attribute__((aligned(16))) g_dummyTransferData[4] = {0, 0, 0, 0};

typedef s32 (*DmacHandlerFunction)(s32);

void appendResult(u32 result) {
	if(g_orderPos != MAX_DMAC_RESULTS) {
		iSignalSema(g_dmacSema);
		g_order[g_orderPos] = result;
		g_orderPos++;
	}
}

template <s32 number>
s32 continueDmacHandler(s32 channel) {
	appendResult(number);
	return 0;
}

template <s32 number>
s32 cancelDmacHandler(s32 channel) {
	appendResult(number);
	return -1;
}

void initializeTest() {
	g_orderPos = 0;
	g_dmacHandlerCount = 0;
	{
		ee_sema_t dmacSemaInfo;
		memset(&dmacSemaInfo, 0, sizeof(dmacSemaInfo));
		dmacSemaInfo.init_count = 0;
		dmacSemaInfo.max_count  = MAX_DMAC_RESULTS;
		dmacSemaInfo.option     = 0;
		g_dmacSema = CreateSema(&dmacSemaInfo);
	}
}

void addTestDmacHandler(DmacHandlerFunction handler, s32 next) {
	int handlerId = AddDmacHandler(DMAC_TO_SPR, handler, next);
	g_dmacHandlers[g_dmacHandlerCount] = handlerId;
	g_dmacHandlerCount++;
}

void removeTestDmacHandler(int index) {
	RemoveDmacHandler(DMAC_TO_SPR, g_dmacHandlers[index]);
	for(u32 i = index + 1; i < g_dmacHandlerCount; i++) {
		g_dmacHandlers[i - 1] = g_dmacHandlers[i];
	}
	g_dmacHandlerCount--;
}

void waitForTestResults() {
	EnableDmac(DMAC_TO_SPR);
	for(u32 i = 0; i < MAX_DMAC_RESULTS; i++) {
		*(R_EE_D9_MADR) = (u32)g_dummyTransferData;
		*(R_EE_D9_SADR) = 0;
		*(R_EE_D9_QWC)  = 1;
		*(R_EE_D9_CHCR) = 0x100;
		while(*(R_EE_D9_CHCR) & 0x100) {}
		WaitSema(g_dmacSema);
	}
	DisableDmac(DMAC_TO_SPR);
}

void finishTest() {
	for(u32 i = 0; i < MAX_DMAC_RESULTS; i++) {
		if(i != 0) {
			printf(", %d", g_order[i]);
		} else {
			printf("%d", g_order[i]);
		}
	}
	printf("\n");
	for(u32 i = 0; i < g_dmacHandlerCount; i++) {
		RemoveDmacHandler(DMAC_TO_SPR, g_dmacHandlers[i]);
	}
}

void pushBackTest() {
	printf("push back: ");
	initializeTest();
	addTestDmacHandler(&continueDmacHandler<1>, -1);
	addTestDmacHandler(&continueDmacHandler<2>, -1);
	addTestDmacHandler(&continueDmacHandler<3>, -1);
	waitForTestResults();
	finishTest();
}

void pushFrontTest() {
	printf("push front: ");
	initializeTest();
	addTestDmacHandler(&continueDmacHandler<1>, 0);
	addTestDmacHandler(&continueDmacHandler<2>, 0);
	addTestDmacHandler(&continueDmacHandler<3>, 0);
	waitForTestResults();
	finishTest();
}

void mixedFrontBackTest() {
	printf("mixed front back: ");
	initializeTest();
	addTestDmacHandler(&continueDmacHandler<1>, -1);
	addTestDmacHandler(&continueDmacHandler<2>,  0);
	addTestDmacHandler(&continueDmacHandler<3>, -1);
	addTestDmacHandler(&continueDmacHandler<4>,  0);
	waitForTestResults();
	finishTest();
}

void relativeTest() {
	printf("relative: ");
	initializeTest();
	addTestDmacHandler(&continueDmacHandler<1>, -1);
	addTestDmacHandler(&continueDmacHandler<2>, -1);
	addTestDmacHandler(&continueDmacHandler<3>, -1);
	addTestDmacHandler(&continueDmacHandler<4>, g_dmacHandlers[1]);
	waitForTestResults();
	finishTest();
}

void removeTest() {
	printf("remove: ");
	initializeTest();
	addTestDmacHandler(&continueDmacHandler<1>, -1);
	addTestDmacHandler(&continueDmacHandler<2>, -1);
	addTestDmacHandler(&continueDmacHandler<3>, -1);
	removeTestDmacHandler(1);
	waitForTestResults();
	finishTest();
}

void cancelTest() {
	printf("cancel: ");
	initializeTest();
	addTestDmacHandler(&continueDmacHandler<1>, -1);
	addTestDmacHandler(&cancelDmacHandler<2>,   -1);
	addTestDmacHandler(&continueDmacHandler<3>, -1);
	waitForTestResults();
	finishTest();
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	pushBackTest();
	pushFrontTest();
	mixedFrontBackTest();
	removeTest();
	relativeTest();
	cancelTest();
	
	printf("-- TEST END\n");
	
	return 0;
}
