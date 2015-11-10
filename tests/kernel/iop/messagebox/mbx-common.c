#include <thbase.h>
#include <thmsgbx.h>
#include "mbx-common.h"

u32 getThreadPriority(s32 threadId) {
	iop_thread_info_t threadInfo = {};
	ReferThreadStatus(threadId, &threadInfo);
	return threadInfo.currentPriority;
}

void printMbx(s32 mbxId) {
	iop_mbx_status_t mbxInfo;
	s32 result = ReferMbxStatus(mbxId, &mbxInfo);
	if (result >= 0) {
		MSG* message = (MSG*)mbxInfo.topPacket;
		u32 i = 0;
		printf("wait threads: %d, messages: %d", mbxInfo.numWaitThreads, mbxInfo.numMessage);
		if (mbxInfo.numMessage != 0) {
			printf(" -> ");
		}
		for (i = 0; i < mbxInfo.numMessage; i++) {
			printf("{ payload: %d, priority: %d }", message->payload, message->header.priority);
			if((i + 1) != mbxInfo.numMessage) {
				printf(", ");
			}
			message = (MSG*)message->header.next;
		}
		printf("\n");
	} else {
		printf("  failed (%d)\n", result);
	}
}
