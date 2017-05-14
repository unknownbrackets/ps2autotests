#include <common-iop.h>
#include <stdio.h>
#include <tamtypes.h>
#include <thbase.h>
#include <thsemap.h>
#include <thevent.h>
#include <thmsgbx.h>
#include "thread-common.h"

#define STACK_SIZE 0x800

void nullThreadProc(u32 param) {
	//Do nothing
}

void sleepThreadProc(u32 param) {
	SleepThread();
}

void delayThreadProc(u32 param) {
	DelayThread(~0);
}

void semaThreadProc(u32 param) {
	iop_sema_t dummySemaInfo = {};
	dummySemaInfo.attr    = 0;
	dummySemaInfo.option  = 0;
	dummySemaInfo.initial = 0;
	dummySemaInfo.max     = 1;
	u32 dummySema = CreateSema(&dummySemaInfo);
	WaitSema(dummySema);
}

void eventFlagThreadProc(u32 param) {
	iop_event_t dummyEventFlagInfo = {};
	dummyEventFlagInfo.attr = 0;
	dummyEventFlagInfo.bits = 0;
	u32 dummyEventFlag = CreateEventFlag(&dummyEventFlagInfo);
	WaitEventFlag(dummyEventFlag, ~0, WEF_CLEAR | WEF_AND, NULL);
}

void messageBoxThreadProc(u32 param) {
	iop_mbx_t dummyMessageBoxInfo = {};
	dummyMessageBoxInfo.attr = 0;
	dummyMessageBoxInfo.option = 0;
	u32 dummyMessageBox = CreateMbx(&dummyMessageBoxInfo);
	ReceiveMbx(NULL, dummyMessageBox);
}

void printThreadStatus(int threadId) {
	iop_thread_info_t threadStat;
	memset(&threadStat, 0, sizeof(iop_thread_info_t));
	int result = ReferThreadStatus(threadId, &threadStat);
	if (result >= 0) {
		schedf("succeeded -> result: %d, init prio: %02x current prio: %02x, status: %02x", 
			result, threadStat.initPriority, threadStat.currentPriority, threadStat.status);
		if (threadStat.status & THS_WAIT) {
			schedf(", wait type: %d", threadStat.waitType);
		}
		schedf("\n");
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

void testWaitingState(const char *name, void *threadProc, u32 threadPrio) {
	int threadId = createTestThread(threadProc, threadPrio, STACK_SIZE);

	StartThread(threadId, 0);
	schedf("  stat after start (%s): ", name);
	printThreadStatus(threadId);
	
	TerminateThread(threadId);
	DeleteThread(threadId);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	int currentThreadPriority = getThreadPriority(TH_SELF);
	
	{
		schedf("self thread:\n");
		
		schedf("  stat (with TH_SELF): ");
		printThreadStatus(TH_SELF);
		
		schedf("  stat (with current tid): ");
		printThreadStatus(GetThreadId());
		
		flushschedf();
	}
	
	{
		schedf("low prio thread:\n");
		
		int threadId = createTestThread((void*)&nullThreadProc, currentThreadPriority + 1, STACK_SIZE);
		schedf("  stat before start: ");
		printThreadStatus(threadId);

		StartThread(threadId, 0);
		schedf("  stat after start: ");
		printThreadStatus(threadId);
		
		//Let the thread execute (terminate doesn't seem to work properly otherwise)
		DelayThread(1);
		
		TerminateThread(threadId);
		schedf("  stat after terminate: ");
		printThreadStatus(threadId);
		
		DeleteThread(threadId);

		flushschedf();
	}

	{
		schedf("waiting thread:\n");
		
		testWaitingState("sleep", (void*)&sleepThreadProc, currentThreadPriority - 1);
		testWaitingState("delay", (void*)&delayThreadProc, currentThreadPriority - 1);
		testWaitingState("semaphore", (void*)&semaThreadProc, currentThreadPriority - 1);
		testWaitingState("event flag", (void*)&eventFlagThreadProc, currentThreadPriority - 1);
		testWaitingState("message box", (void*)&messageBoxThreadProc, currentThreadPriority - 1);
		
		flushschedf();
	}
	
	{
		schedf("corner cases:\n");
		
		schedf("  stat with invalid thread id: ");
		printThreadStatus(~0);
		
		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
