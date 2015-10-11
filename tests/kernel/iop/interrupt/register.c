#include <stdio.h>
#include <intrman.h>
#include <thsemap.h>

static int g_handlerResult = -1;
static int g_handlerSema = 0;

void intrHandler(int value) {
	g_handlerResult = value;
	iSignalSema(g_handlerSema);
}

void createHandlerSema() {
	iop_sema_t semaInfo = {};
	semaInfo.attr    = 0;
	semaInfo.option  = 0;
	semaInfo.initial = 0;
	semaInfo.max     = 1;
	g_handlerSema = CreateSema(&semaInfo);
}

void testMultipleRegister() {
	int oldStat = 0, oldIntr = 0;

	createHandlerSema();

	printf("multiple register:\n");
	CpuSuspendIntr(&oldStat);
	{
		//Make sure no handler is currently registered
		ReleaseIntrHandler(IOP_IRQ_VBLANK);
		DisableIntr(IOP_IRQ_VBLANK, &oldIntr);

		int firstRegisterResult = RegisterIntrHandler(IOP_IRQ_VBLANK, 0, (void *)&intrHandler, (void *)0xAAAA);
		printf("\tregister when no handler is present: %d\n", firstRegisterResult);
		
		int secondRegisterResult = RegisterIntrHandler(IOP_IRQ_VBLANK, 0, (void *)&intrHandler, (void *)0xBBBB);
		printf("\tregister when handler is already present: %d\n", secondRegisterResult);
		
		EnableIntr(IOP_IRQ_VBLANK);
	}
	CpuResumeIntr(oldStat);
	
	//Wait for handler to be called at least once
	WaitSema(g_handlerSema);
	
	CpuSuspendIntr(&oldStat);
	{
		printf("\thandler result: %d\n", g_handlerResult);

		ReleaseIntrHandler(IOP_IRQ_VBLANK);
		DisableIntr(IOP_IRQ_VBLANK, &oldIntr);
	}
	CpuResumeIntr(oldStat);
	
	DeleteSema(g_handlerSema);
}

void testInvalidParams() {
	int oldStat = 0;

	printf("invalid params:\n");

	CpuSuspendIntr(&oldStat);
	{
		int invalidIntrLineResult = RegisterIntrHandler(~0, 0, (void *)&intrHandler, NULL);
		printf("\tinvalid interrupt line: %d\n", invalidIntrLineResult);
		
		int invalidModeResult = RegisterIntrHandler(IOP_IRQ_VBLANK, ~0, (void *)&intrHandler, NULL);
		ReleaseIntrHandler(IOP_IRQ_VBLANK);
		printf("\tinvalid mode line: %d\n", invalidModeResult);

		int nullHandlerResult = RegisterIntrHandler(IOP_IRQ_VBLANK, 0, NULL, NULL);
		ReleaseIntrHandler(IOP_IRQ_VBLANK);
		printf("\tnull handler line: %d\n", nullHandlerResult);
	}
	CpuResumeIntr(oldStat);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	testMultipleRegister();
	testInvalidParams();

	printf("-- TEST END\n");
	
	return 0;
}
