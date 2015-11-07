#include <common-iop.h>
#include <thbase.h>
#include <thmsgbx.h>
#include "mbx-common.h"

#define STACK_SIZE 0x800

typedef struct
{
	s32 id;
	s32 mbxId;
} THREAD_PARAM;

void messageWaitThreadProc(THREAD_PARAM *param) {
	MSG* msg = NULL;
	ReceiveMbx((void**)&msg, param->mbxId);
	schedf("(thread: %d -> message: %d) ", param->id, msg->payload);
}

s32 createTestThread(u32 priority) {
	iop_thread_t threadInfo = {};
	threadInfo.attr      = TH_C;
	threadInfo.thread    = (void*)&messageWaitThreadProc;
	threadInfo.priority  = priority;
	threadInfo.stacksize = STACK_SIZE;
	return CreateThread(&threadInfo);
}

void testMultipleReceive(u32 mbxAttr) {
	static const int msgCount = 3;
	int i = 0;
	MSG msg[msgCount];
	THREAD_PARAM threadParams[msgCount];
	s32 threadIds[msgCount];
	
	iop_mbx_t mbxInfo = {};
	mbxInfo.option = 0;
	mbxInfo.attr   = mbxAttr;
	s32 mbxId = CreateMbx(&mbxInfo);

	for(i = 0; i < msgCount; i++) {
		memset(&threadParams[i], 0, sizeof(MSG));
		threadParams[i].id = i + 1;
		threadParams[i].mbxId = mbxId;
		threadIds[i] = createTestThread(getThreadPriority(TH_SELF) - (i + 1));
		StartThread(threadIds[i], &threadParams[i]);
	}
	
	printf("  box contents: ");
	printMbx(mbxId);
	
	schedf("  order: ");
	for(i = 0; i < msgCount; i++) {
		memset(&msg[i], 0, sizeof(MSG));
		msg[i].header.priority = msgCount - i;
		msg[i].payload         = i + 1;
		SendMbx(mbxId, &msg[i]);
	}
	schedf("\n");
	flushschedf();
	
	for(i = 0; i < msgCount; i++) {
		TerminateThread(threadIds[i]);
		DeleteThread(threadIds[i]);
	}
	
	DeleteMbx(mbxId);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	printf("receiving (fifo thread queuing):\n");
	testMultipleReceive(MBA_THFIFO | MBA_MSFIFO);

	printf("receiving (priority thread queuing):\n");
	testMultipleReceive(MBA_THPRI | MBA_MSFIFO);
	
	printf("invalid params:\n");
	{
		int result = ReceiveMbx(NULL, ~0);
		printf("  receiving message from invalid box: %d\n", result);
	}
	
	printf("-- TEST END\n");
	return 0;
}
