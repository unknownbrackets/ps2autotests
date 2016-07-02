#include <common-iop.h>
#include <thbase.h>

void nullThreadProc(u32 param) {
	//Do nothing
}

void printThreadInfo(s32 threadId) {
	iop_thread_info_t threadStat;
	memset(&threadStat, 0, sizeof(iop_thread_info_t));
	s32 result = ReferThreadStatus(threadId, &threadStat);
	if (result >= 0) {
		printf("attr: %08x, option: %08x, stack size: %08x, init prio: %02x, curr prio: %02x, status: %02x, ", 
			threadStat.attr, threadStat.option, threadStat.stackSize, 
			threadStat.initPriority, threadStat.currentPriority, threadStat.status);
		if(threadStat.entry == &nullThreadProc) {
			printf("entry: &nullThreadProc");
		} else {
			printf("entry: %08x", threadStat.entry);
		}
		printf("\n");
	} else {
		printf("refer failed -> result: %d\n", result);
	}
}


void createThread(s32 attr, void *entry, s32 prio, s32 stackSize, u32 option) {
	iop_thread_t threadParam;
	memset(&threadParam, 0, sizeof(iop_thread_t));
	threadParam.attr      = attr;
	threadParam.thread    = entry;
	threadParam.priority  = prio;
	threadParam.stacksize = stackSize;
	threadParam.option    = option;
	
	s32 thId = CreateThread(&threadParam);
	
	if(thId > 0) {
		printf("  succeeded -> ");
		printThreadInfo(thId);
		DeleteThread(thId);
	} else {
		printf("  failed -> result: %d\n", thId);
	}
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	printf("valid:\n");
	createThread(TH_C, &nullThreadProc, 10, 0x800, 0x01234567);
	
	printf("huge stack size (~0):\n");
	createThread(0, &nullThreadProc, 10, ~0, 0);
	
	printf("small stack size (300):\n");
	createThread(0, &nullThreadProc, 10, 300, 0);
	
	printf("invalid attr (~0):\n");
	createThread(~0, &nullThreadProc, 10, 0x800, 0);
	
	//TH_NO_FILLSTACK and TH_CLEAR_STACK are defined in ps2sdk, but don't seem to be supported
	//(Might be due to BIOS version differences)
	
	printf("known attribute (TH_NO_FILLSTACK):\n");
	createThread(TH_NO_FILLSTACK, &nullThreadProc, 10, 0x800, 0);
	
	printf("known attribute (TH_CLEAR_STACK):\n");
	createThread(TH_CLEAR_STACK, &nullThreadProc, 10, 0x800, 0);
	
	printf("known valid attributes (TH_UMODE | TH_ASM | TH_C):\n");
	createThread(TH_UMODE | TH_ASM | TH_C, &nullThreadProc, 10, 0x800, 0);
	
	printf("invalid entry point (NULL):\n");
	createThread(0, NULL, 10, 0x800, 0);
	
	printf("invalid entry point (3):\n");
	createThread(0, (void*)3, 10, 0x800, 0);
	
	printf("invalid entry point (~3):\n");
	createThread(0, (void*)~3, 10, 0x800, 0);
	
	printf("invalid priority (0):\n");
	createThread(0, &nullThreadProc, 0, 0x800, 0);
	
	printf("invalid priority (127):\n");
	createThread(0, &nullThreadProc, 127, 0x800, 0);
	
	printf("-- TEST END\n");
	return 0;
}
