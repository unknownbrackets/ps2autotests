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

void printThreadStatus(int threadId) {
	ee_thread_status_t threadStat;
	memset(&threadStat, 0, sizeof(ee_thread_status_t));
	int result = ReferThreadStatus(threadId, &threadStat);
	schedf("result: %02x, init prio: %02x, current prio: %02x, status: %02x\n", 
		result, threadStat.initial_priority, threadStat.current_priority, threadStat.status);
}

void doResumeThread(int threadId) {
	int result = ResumeThread(threadId);
	if (result >= 0) {
		schedf("succeeded -> result: %s, status -> ", (result == threadId) ? "thread id" : "something else");
		printThreadStatus(threadId);
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	int testThreadPriority = getThreadPriority(GetThreadId());

	{
		schedf("self thread:\n");

		schedf("  resume (with current tid): ");
		doResumeThread(GetThreadId());
		
		flushschedf();
	}
	
	{
		schedf("worse prio thread:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, testThreadPriority + 0x10, nullThreadStack, STACK_SIZE);
		schedf("  resume before start: ");
		doResumeThread(threadId);

		StartThread(threadId, NULL);
		schedf("  resume after start: ");
		doResumeThread(threadId);

		SuspendThread(threadId);
		schedf("  resume after suspend: ");
		doResumeThread(threadId);
		
		TerminateThread(threadId);
		schedf("  resume after terminate: ");
		doResumeThread(threadId);
		
		DeleteThread(threadId);

		flushschedf();
	}

	{
		schedf("sleeping thread:\n");
		
		int threadId = createTestThread((void*)&sleepingThreadProc, testThreadPriority - 0x10, sleepingThreadStack, STACK_SIZE);

		StartThread(threadId, 0);
		schedf("  resume after start: ");
		doResumeThread(threadId);
		
		SuspendThread(threadId);
		schedf("  resume after suspend: ");
		doResumeThread(threadId);
		
		TerminateThread(threadId);
		DeleteThread(threadId);

		flushschedf();
	}
	
	{
		schedf("waiting thread:\n");

		int threadId = createTestThread((void*)&waitingThreadProc, testThreadPriority - 0x10, waitingThreadStack, STACK_SIZE);

		StartThread(threadId, 0);
		schedf("  resume after start: ");
		doResumeThread(threadId);
		
		SuspendThread(threadId);
		schedf("  resume after suspend: ");
		doResumeThread(threadId);

		TerminateThread(threadId);
		DeleteThread(threadId);

		flushschedf();
	}

	{
		schedf("corner cases:\n");
		
		schedf("  resume with invalid tid: ");
		doResumeThread(~0);

		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
