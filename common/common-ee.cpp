#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <kernel.h>

#define TEST_THREAD_PRIORITY   0x40
#define TEST_THREAD_STACK_SIZE 0x8000

char schedfBuffer[65536];
unsigned int schedfBufferPos = 0;

void schedf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	schedfBufferPos += vsprintf(schedfBuffer + schedfBufferPos, format, args);
	va_end(args);
}

void flushschedf() {
	printf("%s", schedfBuffer);
	schedfBuffer[0] = '\0';
	schedfBufferPos = 0;
}

u32 g_testThreadDoneSema = 0;

extern int test_main(int argc, char *argv[]);

char testThreadStack[TEST_THREAD_STACK_SIZE] __attribute__ ((aligned(16)));
void testThreadProc() {
	//TODO: Print begin/end traces
	//TODO: Pass arguments from main thread to the test function
	test_main(0, NULL);
	SignalSema(g_testThreadDoneSema);
}

int main(int argc, char *argv[]) {
	//A thread is created to ensure the execution environment is the same
	//across all possible boot methods (direct hw boot, ps2link, emulator, etc.)
	
	ee_sema_t semaInfo;
	memset(&semaInfo, 0, sizeof(ee_sema_t));
	semaInfo.init_count = 0;
	semaInfo.max_count  = 1;
	semaInfo.option     = 0;
	g_testThreadDoneSema = CreateSema(&semaInfo);
	
	u32 testThreadId = 0;
	ee_thread_t threadParam;
	memset(&threadParam, 0, sizeof(ee_thread_t));
	threadParam.func               = (void*)&testThreadProc;
	threadParam.initial_priority   = TEST_THREAD_PRIORITY;
	threadParam.stack_size         = TEST_THREAD_STACK_SIZE;
	threadParam.stack              = testThreadStack;
	testThreadId = CreateThread(&threadParam);
	
	StartThread(testThreadId, NULL);
	WaitSema(g_testThreadDoneSema);
	
	return 0;
}
