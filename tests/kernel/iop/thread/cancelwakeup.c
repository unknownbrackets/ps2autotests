#include <common-iop.h>
#include <thbase.h>
#include "thread-common.h"

#define STACK_SIZE 0x800

void doCancelWakeupThread(int threadId) {
	int result = CancelWakeupThread(threadId);
	if(result >= 0) {
		schedf("succeeded -> result: %d\n", result);
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

void nullThreadProc(u32 param) {
	//Do nothing
}

void cancelThreadProc(u32 param) {
	schedf("  cancel wakeup self thread: ");
	doCancelWakeupThread(TH_SELF);
	
	schedf("  cancel wakeup self thread after cancelling: ");
	doCancelWakeupThread(TH_SELF);
}

int generateDeletedThreadId() {
	int threadId = createTestThread((void*)&nullThreadProc, 64, STACK_SIZE);
	StartThread(threadId, 0);
	
	//Let the thread execute (terminate doesn't seem to work properly otherwise)
	DelayThread(1);
	
	TerminateThread(threadId);
	DeleteThread(threadId);
	
	return threadId;
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	{
		schedf("self thread:\n");
		
		int threadId = createTestThread((void*)&cancelThreadProc, 64, STACK_SIZE);
		
		StartThread(threadId, 0);
		WakeupThread(threadId);
		WakeupThread(threadId);
		WakeupThread(threadId);
		WakeupThread(threadId);
		DelayThread(100);
		
		TerminateThread(threadId);
		DeleteThread(threadId);
		
		flushschedf();
	}
	
	{
		schedf("standard usage:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, 64, STACK_SIZE);
		StartThread(threadId, 0);
		
		schedf("  cancel wakeup after starting: ");
		doCancelWakeupThread(threadId);
		
		WakeupThread(threadId);
		WakeupThread(threadId);
		WakeupThread(threadId);
		
		schedf("  cancel wakeup after wakeups: ");
		doCancelWakeupThread(threadId);
		
		schedf("  cancel wakeup after cancelling: ");
		doCancelWakeupThread(threadId);
		
		TerminateThread(threadId);
		DeleteThread(threadId);
		
		flushschedf();
	}
	
	{
		int deletedThreadId = generateDeletedThreadId();
		
		schedf("corner cases:\n");
		
		schedf("  cancel wakeup with invalid thread id: ");
		doCancelWakeupThread(deletedThreadId);
		
		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
