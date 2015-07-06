#include <stdio.h>
#include <tamtypes.h>
#include <thbase.h>

void printThreadStatus(int threadId) {
	iop_thread_info_t threadStat;
	memset(&threadStat, 0, sizeof(iop_thread_info_t));
	int result = ReferThreadStatus(threadId, &threadStat);
	if (result >= 0) {
		printf("succeeded -> result: %02x, init prio: %02x current prio: %02x, status: %02x\n", 
			result, threadStat.initPriority, threadStat.currentPriority, threadStat.status);
	} else {
		printf("failed -> result: %d\n", result);
	}
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	{
		printf("self thread:\n");
		
		printf("  stat (with TH_SELF): ");
		printThreadStatus(TH_SELF);
		
		printf("  stat (with current tid): ");
		printThreadStatus(GetThreadId());
	}
	
	printf("-- TEST END\n");
	
	return 0;
}
