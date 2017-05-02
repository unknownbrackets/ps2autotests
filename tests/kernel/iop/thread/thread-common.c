#include <thbase.h>
#include "thread-common.h"

s32 getThreadPriority(s32 threadId) {
	iop_thread_info_t threadStat;
	memset(&threadStat, 0, sizeof(iop_thread_info_t));
	int result = ReferThreadStatus(threadId, &threadStat);
	if (result >= 0) {
		return threadStat.currentPriority;
	} else {
		return -1;
	}
}

s32 createTestThread(void *entry, s32 prio, u32 stackSize) {
	iop_thread_t threadParam;
	memset(&threadParam, 0, sizeof(iop_thread_t));
	threadParam.attr      = TH_C;
	threadParam.thread    = entry;
	threadParam.priority  = prio;
	threadParam.stacksize = stackSize;
	return CreateThread(&threadParam);
}
