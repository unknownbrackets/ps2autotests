#include <common-ee.h>
#include <kernel.h>
#include <string.h>

#define STACK_SIZE 0x8000

char nullThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void nullThreadProc(u32) {
	//Do nothing
}

char sleepingThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void sleepingThreadProc(u32) {
	SleepThread();
}

char waitingThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void waitingThreadProc(u32) {
	ee_sema_t dummySemaInfo;
	memset(&dummySemaInfo, 0, sizeof(ee_sema_t));
	dummySemaInfo.init_count = 0;
	dummySemaInfo.max_count  = 1;
	dummySemaInfo.option     = 0;
	u32 dummySema = CreateSema(&dummySemaInfo);
	WaitSema(dummySema);
}

int createTestThread(void *entry, int prio, void *stack, u32 stackSize) {
	ee_thread_t threadParam;
	memset(&threadParam, 0, sizeof(ee_thread_t));
	threadParam.func             = entry;
	threadParam.initial_priority = prio;
	threadParam.stack            = stack;
	threadParam.stack_size       = stackSize;
	return CreateThread(&threadParam);
}

void doDeleteThread(int threadId) {
	int result = DeleteThread(threadId);
	if(result >= 0) {
		schedf("succeeded -> result: %s\n", (result == threadId) ? "thread id" : "something else");
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	int testThreadPriority = getThreadPriority(GetThreadId());
	
	{
		schedf("self thread:\n");

		schedf("  delete (with current tid): ");
		doDeleteThread(GetThreadId());
		
		flushschedf();
	}

	{
		schedf("worse prio thread:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, testThreadPriority + 0x10, nullThreadStack, STACK_SIZE);
		schedf("  delete before start: ");
		doDeleteThread(threadId);

		schedf("  delete after delete: ");
		doDeleteThread(threadId);
		
		threadId = createTestThread((void*)&nullThreadProc, testThreadPriority + 0x10, nullThreadStack, STACK_SIZE);
		StartThread(threadId, NULL);
		schedf("  delete after start: ");
		doDeleteThread(threadId);

		SuspendThread(threadId);
		schedf("  delete after suspend: ");
		doDeleteThread(threadId);

		ResumeThread(threadId);
		schedf("  delete after resume: ");
		doDeleteThread(threadId);
		
		TerminateThread(threadId);
		schedf("  delete after terminate: ");
		doDeleteThread(threadId);
		
		flushschedf();
	}

	{
		schedf("sleeping thread:\n");
		
		int threadId = createTestThread((void*)&sleepingThreadProc, testThreadPriority - 0x10, sleepingThreadStack, STACK_SIZE);

		StartThread(threadId, 0);
		schedf("  delete after start: ");
		doDeleteThread(threadId);
		
		SuspendThread(threadId);
		schedf("  delete after suspend: ");
		doDeleteThread(threadId);
		
		ResumeThread(threadId);
		schedf("  delete after resume: ");
		doDeleteThread(threadId);
		
		TerminateThread(threadId);
		DeleteThread(threadId);

		flushschedf();
	}

	{
		schedf("waiting thread:\n");

		int threadId = createTestThread((void*)&waitingThreadProc, testThreadPriority - 0x10, waitingThreadStack, STACK_SIZE);

		StartThread(threadId, 0);
		schedf("  delete after start: ");
		doDeleteThread(threadId);
		
		SuspendThread(threadId);
		schedf("  delete after suspend: ");
		doDeleteThread(threadId);

		ResumeThread(threadId);
		schedf("  delete after resume: ");
		doDeleteThread(threadId);

		TerminateThread(threadId);
		DeleteThread(threadId);

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
