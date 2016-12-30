#include <common-iop.h>
#include <kerr.h>
#include <thevent.h>
#include <thbase.h>

#define STACK_SIZE 0x800

typedef struct {
	int eventFlagId;
	s32 waitEventFlagResult;
} WAIT_THREAD_PARAMS;

void waitThreadProc(WAIT_THREAD_PARAMS* params) {
	params->waitEventFlagResult = WaitEventFlag(params->eventFlagId, ~0, WEF_OR | WEF_CLEAR, NULL);
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

int createTestEventFlag() {
	iop_event_t eventFlagParams;
	memset(&eventFlagParams, 0, sizeof(eventFlagParams));
	return CreateEventFlag(&eventFlagParams);
}

int generateDeletedEventFlagId() {
	int eventFlagId = createTestEventFlag();
	DeleteEventFlag(eventFlagId);
	return eventFlagId;
}

void doDeleteEventFlag(int eventFlagId) {
	int result = DeleteEventFlag(eventFlagId);
	if(result == KE_OK) {
		schedf("succeeded\n");
	} else {
		schedf("failed -> result: %d\n", result);
	}
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	{
		schedf("unused event flag:\n");
		
		int eventFlagId = createTestEventFlag();
		
		schedf("  delete after creation: ");
		doDeleteEventFlag(eventFlagId);
		
		flushschedf();
	}
	
	{
		schedf("used event flag:\n");
		
		int eventFlagId = createTestEventFlag();
		
		WAIT_THREAD_PARAMS waitThreadParams;
		waitThreadParams.eventFlagId         = eventFlagId;
		waitThreadParams.waitEventFlagResult = 0;
		
		int currentThreadPriority = getThreadPriority(TH_SELF);
		int threadId = createTestThread((void*)&waitThreadProc, currentThreadPriority - 1, STACK_SIZE);
		StartThread(threadId, &waitThreadParams);
		
		schedf("  delete while thread is waiting: ");
		doDeleteEventFlag(eventFlagId);
		
		schedf("  WaitEventFlag returned: %d\n", waitThreadParams.waitEventFlagResult);
		
		//DeleteThread should succeed since WaitEventFlag will abort due to the
		//event flag being deleted and the thread being put in a dormant state
		int deleteThreadResult = DeleteThread(threadId);
		schedf("  DeleteThread returned: %d\n", deleteThreadResult);
	}
	
	{
		schedf("corner cases:\n");
		
		schedf("  delete with invalid id: ");
		doDeleteEventFlag(0);
		
		schedf("  delete with deleted id: ");
		int eventFlagId = generateDeletedEventFlagId();
		doDeleteEventFlag(eventFlagId);
		
		flushschedf();
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
