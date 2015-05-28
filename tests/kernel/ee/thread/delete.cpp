#include <common-ee.h>
#include <kernel.h>
#include <string.h>

#define STACK_SIZE 0x8000
#define TEST_THREAD_PRIORITY 0x40

char nullThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void nullThreadProc(u32) {
	//Do nothing
}

int createTestThread(void *entry, int prio, void *stack, u32 stackSize) {
	ee_thread_t threadParam;
	memset(&threadParam, 0, sizeof(ee_thread_t));
	threadParam.func = entry;
	threadParam.initial_priority = prio;
	threadParam.stack = stack;
	threadParam.stack_size = stackSize;
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

	{
		schedf("self thread:\n");

		schedf("  delete (with current tid): ");
		doDeleteThread(GetThreadId());
		
		flushschedf();
	}

	{
		schedf("low prio thread:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, TEST_THREAD_PRIORITY + 0x10, nullThreadStack, STACK_SIZE);
		schedf("  delete before start: ");
		doDeleteThread(threadId);

		threadId = createTestThread((void*)&nullThreadProc, TEST_THREAD_PRIORITY + 0x10, nullThreadStack, STACK_SIZE);
		StartThread(threadId, NULL);
		schedf("  delete after start: ");
		doDeleteThread(threadId);

		flushschedf();
	}

	{
		schedf("high prio thread:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, TEST_THREAD_PRIORITY - 0x10, nullThreadStack, STACK_SIZE);
		StartThread(threadId, NULL);
		schedf("  delete after start: ");
		doDeleteThread(threadId);

		flushschedf();
	}

	//Various corner cases
	{
		schedf("corner cases:\n");
		
		schedf("  delete with invalid tid: ");
		doDeleteThread(~0);

		flushschedf();
	}
	
	printf("-- TEST END\n");
}
