#include <kernel.h>
#include <string.h>
#include "kernel_test_base.h"

#define STACK_SIZE 0x8000

u32 g_kernelTestThreadDoneSema = 0;

extern void kernelTestMain();

char kernelTestThreadStack[STACK_SIZE] __attribute__ ((aligned(16)));
void kernelTestThreadProc() {
	kernelTestMain();
	SignalSema(g_kernelTestThreadDoneSema);
}

int main(int argc, char** argv) {
	ee_sema_t semaInfo;
	memset(&semaInfo, 0, sizeof(ee_sema_t));
	semaInfo.init_count	= 0;
	semaInfo.max_count	= 1;
	semaInfo.option 	= 0;
	g_kernelTestThreadDoneSema = CreateSema(&semaInfo);
	
	u32 kernelTestThreadId = 0;
	ee_thread_t threadParam;
	memset(&threadParam, 0, sizeof(ee_thread_t));
	threadParam.func 				= (void*)&kernelTestThreadProc;
	threadParam.initial_priority 	= TEST_THREAD_PRIORITY;
	threadParam.stack 				= kernelTestThreadStack;
	threadParam.stack_size 			= STACK_SIZE;
	kernelTestThreadId = CreateThread(&threadParam);
	
	StartThread(kernelTestThreadId, NULL);
	WaitSema(g_kernelTestThreadDoneSema);
}
