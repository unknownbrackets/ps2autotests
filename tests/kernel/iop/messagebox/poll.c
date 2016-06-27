#include <common-iop.h>
#include <thmsgbx.h>
#include "mbx-common.h"

typedef struct {
	s32 id;
	s32 mbxId;
} THREAD_PARAM;

void basicPollingTest() {
	static const int msgCount = 3;
	int i = 0;
	MSG msg[msgCount];
	
	iop_mbx_t mbxInfo = {};
	mbxInfo.option = 0;
	mbxInfo.attr   = MBA_THFIFO | MBA_MSFIFO;
	s32 mbxId = CreateMbx(&mbxInfo);
	
	{
		s32 result = PollMbx(NULL, mbxId);
		printf("  polling from empty box: %d\n", result);
	}
	
	for (i = 0; i < msgCount; i++) {
		memset(&msg[i], 0, sizeof(MSG));
		msg[i].header.priority = msgCount - i;
		msg[i].payload         = i + 1;
		SendMbx(mbxId, &msg[i]);
	}
	
	printf("  box contents before polling: ");
	printMbx(mbxId);
	
	{
		MSG* msg = NULL;
		s32 result = PollMbx((void**)&msg, mbxId);
		printf("  polling from non-empty box: %d, message: %d\n", 
			result, msg->payload);
	}
	
	printf("  box contents after polling: ");
	printMbx(mbxId);
	
	DeleteMbx(mbxId);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	printf("basic polling:\n");
	basicPollingTest();

	printf("invalid params:\n");
	{
		s32 result = PollMbx(NULL, ~0);
		printf("  polling message from invalid box: %d\n", result);
		
		//Polling into a null pointer seems to work (returns 0 and message is dequeued)
		//but it will probably have dangerous side effects because memory at address 0
		//will be modified.
		//result = PollMbx(NULL, mbxId);
	}
	
	printf("-- TEST END\n");
	return 0;
}
