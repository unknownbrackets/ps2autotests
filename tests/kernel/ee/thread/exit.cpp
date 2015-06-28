#include <common-ee.h>
#include <kernel.h>
#include <string.h>

#define STACK_SIZE 0x8000
#define TEST_THREAD_PRIORITY 0x40

void printResult(u32 threadId, u32 result) {
	if(threadId == result) {
		schedf("returned thread id\n");
	} else {
		schedf("returned %d\n", result);
	}
}

void printThreadStatus(int threadId) {
	ee_thread_status_t threadStat;
	memset(&threadStat, 0, sizeof(ee_thread_status_t));
	int result = ReferThreadStatus(threadId, &threadStat);
	schedf("result: %02x, init prio: %02x current prio: %02x, status: %02x\n", 
		result, threadStat.initial_priority, threadStat.current_priority, threadStat.status);
}

char exitThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void exitThreadProc(u32) {
	ExitThread();
	SleepThread();
}

char exitThreadChangePriorityStack[STACK_SIZE] __attribute__ ((aligned(16)));
void exitThreadChangePriorityProc(u32 newPriority) {
	ChangeThreadPriority(GetThreadId(), newPriority);
	schedf("  stat after changing priority -> ");
	printThreadStatus(GetThreadId());
	ExitThread();
	SleepThread();
}

char exitDeleteThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void exitDeleteThreadProc(u32) {
	ExitDeleteThread();
	SleepThread();
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

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	{
		schedf("exit thread:\n");
		
		int threadId = createTestThread((void*)&exitThreadProc, TEST_THREAD_PRIORITY - 0x10, exitThreadStack, STACK_SIZE);
		StartThread(threadId, NULL);
		
		s32 result = StartThread(threadId, NULL);
		schedf("  start after thread exited -> ");
		printResult(threadId, result);

		result = DeleteThread(threadId);
		schedf("  delete after thread exited -> ");
		printResult(threadId, result);

		flushschedf();
	}

	{
		schedf("exit thread (priority change):\n");

		int threadId = createTestThread((void*)&exitThreadChangePriorityProc, TEST_THREAD_PRIORITY - 0x10, exitThreadChangePriorityStack, STACK_SIZE);
		StartThread(threadId, (void*)(TEST_THREAD_PRIORITY - 0x20));
		
		schedf("  stat after thread exited -> ");
		printThreadStatus(threadId);
		
		flushschedf();
	}
	
	{
		schedf("exit delete thread:\n");
		
		int threadId = createTestThread((void*)&exitDeleteThreadProc, TEST_THREAD_PRIORITY - 0x10, exitDeleteThreadStack, STACK_SIZE);
		StartThread(threadId, NULL);

		s32 result = StartThread(threadId, NULL);
		schedf("  start after thread exited -> ");
		printResult(threadId, result);

		result = DeleteThread(threadId);
		schedf("  delete after thread exited -> ");
		printResult(threadId, result);

		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
