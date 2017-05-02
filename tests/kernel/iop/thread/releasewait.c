#include <common-iop.h>
#include <thbase.h>
#include <thsemap.h>
#include <thevent.h>
#include <thmsgbx.h>
#include "thread-common.h"

#define STACK_SIZE 0x800

void nullThreadProc(u32 param) {
	//Do nothing
}

void sleepThreadProc(s32 *result) {
	*result = SleepThread();
}

void delayThreadProc(s32 *result) {
	*result = DelayThread(~0);
}

void semaThreadProc(s32 *result) {
	iop_sema_t dummySemaInfo = {};
	dummySemaInfo.attr    = 0;
	dummySemaInfo.option  = 0;
	dummySemaInfo.initial = 0;
	dummySemaInfo.max     = 1;
	u32 dummySema = CreateSema(&dummySemaInfo);
	*result = WaitSema(dummySema);
}

void eventFlagThreadProc(s32 *result) {
	iop_event_t dummyEventFlagInfo = {};
	dummyEventFlagInfo.attr = 0;
	dummyEventFlagInfo.bits = 0;
	u32 dummyEventFlag = CreateEventFlag(&dummyEventFlagInfo);
	*result = WaitEventFlag(dummyEventFlag, ~0, WEF_CLEAR | WEF_AND, NULL);
}

void messageBoxThreadProc(s32 *result) {
	iop_mbx_t dummyMessageBoxInfo = {};
	dummyMessageBoxInfo.attr = 0;
	dummyMessageBoxInfo.option = 0;
	u32 dummyMessageBox = CreateMbx(&dummyMessageBoxInfo);
	*result = ReceiveMbx(NULL, dummyMessageBox);
}

void doReleaseWaitThread(s32 threadId) {
	s32 result = ReleaseWaitThread(threadId);
	if(result >= 0) {
		schedf("succeeded -> result: %d\n", result);
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

void testRunningThread() {
	schedf("running thread: ");
	
	s32 currentThreadPriority = getThreadPriority(TH_SELF);
	s32 threadId = createTestThread(&nullThreadProc, currentThreadPriority + 1, STACK_SIZE);
	StartThread(threadId, 0);
	
	doReleaseWaitThread(threadId);
	
	//Let the thread execute (terminate doesn't seem to work properly otherwise)
	DelayThread(1);
	
	TerminateThread(threadId);
	DeleteThread(threadId);
	
	flushschedf();
}

void testDormantThread() {
	schedf("dormant thread: ");
	
	s32 threadId = createTestThread(&nullThreadProc, 0x20, STACK_SIZE);
	
	doReleaseWaitThread(threadId);
	
	DeleteThread(threadId);
	
	flushschedf();
}

void testInvalidThread() {
	schedf("invalid thread: ");
	
	doReleaseWaitThread(~0);
	
	flushschedf();
}

void testWaitingThread(const char *waitType, void *threadProc) {
	schedf("%s thread:\n", waitType);
	
	s32 waitResult = 0;
	s32 currentThreadPriority = getThreadPriority(TH_SELF);
	s32 threadId = createTestThread(threadProc, currentThreadPriority - 1, STACK_SIZE);
	StartThread(threadId, &waitResult);
	
	schedf("  ReleaseWaitThread returned: ");
	doReleaseWaitThread(threadId);
	
	schedf("  wait function returned: %d\n", waitResult);
	
	TerminateThread(threadId);
	DeleteThread(threadId);
	
	flushschedf();
}

void testSelf() {
	schedf("self thread:\n");
	
	schedf("  With TH_SELF: ");
	doReleaseWaitThread(TH_SELF);
	
	schedf("  With current thread id: ");
	doReleaseWaitThread(GetThreadId());
	
	flushschedf();
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	testRunningThread();
	testDormantThread();
	testInvalidThread();
	testWaitingThread("sleeping", &sleepThreadProc);
	testWaitingThread("delayed", &delayThreadProc);
	testWaitingThread("waiting semaphore", &semaThreadProc);
	testWaitingThread("waiting event flag", &eventFlagThreadProc);
	testWaitingThread("waiting message box", &messageBoxThreadProc);
	testSelf();
	
	printf("-- TEST END\n");
	return 0;
}
