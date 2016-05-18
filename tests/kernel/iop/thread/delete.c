#include <common-iop.h>
#include <kerr.h>
#include <thbase.h>

#define STACK_SIZE 0x800

void nullThreadProc(u32 param) {
	//Do nothing
}

void sleepThreadProc(u32 param) {
	SleepThread();
}

int createTestThread(void *entry, int prio, u32 stackSize) {
	iop_thread_t threadParam;
	memset(&threadParam, 0, sizeof(iop_thread_t));
	threadParam.attr      = TH_C;
	threadParam.thread    = entry;
	threadParam.priority  = prio;
	threadParam.stacksize = stackSize;
	return CreateThread(&threadParam);
}

int getThreadPriority(int threadId) {
	iop_thread_info_t threadStat;
	memset(&threadStat, 0, sizeof(iop_thread_info_t));
	int result = ReferThreadStatus(threadId, &threadStat);
	if (result >= 0) {
		return threadStat.currentPriority;
	} else {
		return -1;
	}
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
