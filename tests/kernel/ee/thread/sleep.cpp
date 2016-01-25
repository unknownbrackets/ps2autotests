#include <common-ee.h>
#include <kernel.h>
#include <string.h>

#define STACK_SIZE 0x8000

u32 g_threadResult = 0;

char sleepingThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void sleepingThreadProc(u32) {
	g_threadResult = SleepThread();
}

char waitSleepThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void waitSleepThreadProc(u32 waitSemaId) {
	WaitSema(waitSemaId);
	g_threadResult = SleepThread();
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

u32 createSimpleSema() {
	ee_sema_t semaInfo;
	memset(&semaInfo, 0, sizeof(ee_sema_t));
	semaInfo.init_count = 0;
	semaInfo.max_count  = 1;
	semaInfo.option     = 0;
	return CreateSema(&semaInfo);
}

void printThreadResult(u32 threadId) {
	if(g_threadResult == threadId) {
		schedf("thread id\n");
	} else {
		schedf("something else (%d)\n", g_threadResult);
	}
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	int testThreadPriority = getThreadPriority(GetThreadId());

	{
		g_threadResult = 0;
		int threadId = createTestThread((void*)&sleepingThreadProc, testThreadPriority - 0x10, sleepingThreadStack, STACK_SIZE);
		StartThread(threadId, NULL);

		WakeupThread(threadId);

		TerminateThread(threadId);
		DeleteThread(threadId);

		schedf("sleep result (wakeupCount == 0): ");
		printThreadResult(threadId);
		flushschedf();
	}

	{
		g_threadResult = 0;
		u32 waitSemaId = createSimpleSema();
		int threadId = createTestThread((void*)&waitSleepThreadProc, testThreadPriority - 0x10, waitSleepThreadStack, STACK_SIZE);
		StartThread(threadId, (u32*)waitSemaId);

		//Wakeup twice to set wakeupCount to 2
		WakeupThread(threadId);
		WakeupThread(threadId);
		
		SignalSema(waitSemaId);

		TerminateThread(threadId);
		DeleteThread(threadId);

		schedf("sleep result (wakeupCount > 0): ");
		printThreadResult(threadId);
		flushschedf();
		
		DeleteSema(waitSemaId);
	}

	printf("-- TEST END\n");
	
	return 0;
}
