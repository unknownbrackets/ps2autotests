#include <common-iop.h>
#include <kerr.h>
#include <thevent.h>
#include <thbase.h>

#define STACK_SIZE 0x800

void waitThreadProc(u32 eventFlagId) {
	WaitEventFlag(eventFlagId, ~0, WEF_OR | WEF_CLEAR, NULL);
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

int createTestEventFlag(u32 attr, u32 option, u32 initPattern) {
	iop_event_t eventFlagParams;
	memset(&eventFlagParams, 0, sizeof(eventFlagParams));
	eventFlagParams.attr   = attr;
	eventFlagParams.option = option;
	eventFlagParams.bits   = initPattern;
	return CreateEventFlag(&eventFlagParams);
}

void doPollEventFlag(int eventFlagId, u32 waitPattern, int mode, u32 *resultPattern) {
	int result = PollEventFlag(eventFlagId, waitPattern, mode, resultPattern);
	if(result == KE_OK) {
		schedf("succeeded");
	} else {
		schedf("failed -> result: %d", result);
	}
	if(resultPattern != NULL) {
		schedf(", resultPattern: %08x", *resultPattern);
	}
	schedf("\n");
}

void testPollBusyEventFlag(u32 eventFlagMode) {
	int eventFlagId = createTestEventFlag(0, eventFlagMode, 0);
	u32 resultPattern = 0xCCCC;
	
	int currentThreadPriority = getThreadPriority(TH_SELF);
	int threadId = createTestThread((void*)&waitThreadProc, currentThreadPriority - 1, STACK_SIZE);
	StartThread(threadId, (void*)eventFlagId);
	
	doPollEventFlag(eventFlagId, 0x33, WEF_AND, &resultPattern);
	
	TerminateThread(threadId);
	DeleteThread(threadId);
	
	DeleteEventFlag(eventFlagId);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	{
		schedf("match success cases:\n");
		
		int eventFlagId = createTestEventFlag(EA_SINGLE, 0, 0xF);
		u32 resultPattern = 0xCCCC;
		
		schedf("  poll with matching pattern, OR wait mode: ");
		doPollEventFlag(eventFlagId, 0x1, WEF_OR, &resultPattern);
		
		schedf("  poll with matching pattern, AND wait mode: ");
		doPollEventFlag(eventFlagId, 0xF, WEF_AND, &resultPattern);
		
		DeleteEventFlag(eventFlagId);
		flushschedf();
	}
	
	{
		schedf("match failure cases:\n");
		
		int eventFlagId = createTestEventFlag(EA_SINGLE, 0, 0xF);
		u32 resultPattern = 0xCCCC;
		
		schedf("  poll with non-matching pattern, OR wait mode: ");
		doPollEventFlag(eventFlagId, 0x10, WEF_OR, &resultPattern);
		
		schedf("  poll with non-matching pattern, AND wait mode: ");
		doPollEventFlag(eventFlagId, 0x1F, WEF_AND, &resultPattern);
		
		DeleteEventFlag(eventFlagId);
		flushschedf();
	}
	
	{
		schedf("clear on success:\n");
		
		int eventFlagId = createTestEventFlag(EA_SINGLE, 0, 0x3333);
		u32 resultPattern = 0xCCCC;
		
		schedf("  poll with matching pattern, OR and CLEAR wait mode: ");
		doPollEventFlag(eventFlagId, 0x3, WEF_OR | WEF_CLEAR, &resultPattern);
		
		schedf("  poll a second time with matching pattern, OR wait mode: ");
		resultPattern = 0xCCCC;
		doPollEventFlag(eventFlagId, 0x30, WEF_OR, &resultPattern);
		
		DeleteEventFlag(eventFlagId);
		flushschedf();
	}
	
	{
		schedf("test busy event flag:\n");
		
		schedf("  single wait event flag: ");
		testPollBusyEventFlag(EA_SINGLE);
		
		schedf("  multi wait event flag: ");
		testPollBusyEventFlag(EA_MULTI);
	}
	
	{
		schedf("corner cases:\n");
		
		int eventFlagId = createTestEventFlag(EA_SINGLE, 0, ~0);
		u32 resultPattern = 0xCCCC;
		
		schedf("  poll with zero pattern: ");
		doPollEventFlag(eventFlagId, 0, WEF_AND, &resultPattern);
		
		schedf("  poll with null result pattern: ");
		doPollEventFlag(eventFlagId, 0x1, WEF_AND, NULL);
		
		DeleteEventFlag(eventFlagId);
		
		schedf("  poll with invalid tid: ");
		doPollEventFlag(0, ~0, 0, NULL);
		
		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
