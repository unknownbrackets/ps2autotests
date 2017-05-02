#include <common-iop.h>
#include <kerr.h>
#include <thbase.h>
#include "thread-common.h"

#define STACK_SIZE 0x800

void nullThreadProc(u32 param) {
	//Do nothing
}

void sleepThreadProc(u32 param) {
	SleepThread();
}

void doDeleteThread(int threadId) {
	int result = DeleteThread(threadId);
	if(result == KE_OK) {
		schedf("succeeded\n");
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	int currentThreadPriority = getThreadPriority(TH_SELF);
	
	{
		schedf("self thread:\n");
		
		schedf("  delete (with TH_SELF): ");
		doDeleteThread(TH_SELF);
		
		schedf("  delete (with current tid): ");
		doDeleteThread(GetThreadId());
		
		flushschedf();
	}
	
	{
		schedf("low prio thread:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, currentThreadPriority + 1, STACK_SIZE);
		schedf("  delete before start: ");
		doDeleteThread(threadId);
		
		threadId = createTestThread((void*)&nullThreadProc, currentThreadPriority + 1, STACK_SIZE);
		StartThread(threadId, 0);
		schedf("  delete after start: ");
		doDeleteThread(threadId);
		
		//Let the thread execute (terminate doesn't seem to work properly otherwise)
		DelayThread(1);
		
		TerminateThread(threadId);
		schedf("  delete after terminate: ");
		doDeleteThread(threadId);
		
		schedf("  delete after delete: ");
		doDeleteThread(threadId);
		
		flushschedf();
	}
	
	{
		schedf("waiting thread:\n");
		
		int threadId = createTestThread((void*)&sleepThreadProc, currentThreadPriority - 1, STACK_SIZE);
		StartThread(threadId, 0);
		
		schedf("  delete after start: ");
		doDeleteThread(threadId);
		
		TerminateThread(threadId);
		schedf("  delete after terminate: ");
		doDeleteThread(threadId);
		
		flushschedf();
	}
	
	{
		schedf("corner cases:\n");
		
		schedf("  delete with invalid tid: ");
		doDeleteThread(~0);

		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
