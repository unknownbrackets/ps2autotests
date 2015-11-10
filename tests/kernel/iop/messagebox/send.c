#include <common-iop.h>
#include <thbase.h>
#include <thmsgbx.h>
#include "mbx-common.h"

#define STACK_SIZE 0x800

u32 g_receivedPayload = 0;

void messageWaitThreadProc(u32 mbxId) {
	MSG* msg = NULL;
	ReceiveMbx((void**)&msg, mbxId);
	g_receivedPayload = msg->payload;
}

void testMultipleSend(u32 mbxAttr) {
	static const int msgCount = 3;
	int i = 0;
	MSG msg[msgCount];
	
	iop_mbx_t mbxInfo = {};
	mbxInfo.option = 0;
	mbxInfo.attr   = mbxAttr;
	s32 mbxId = CreateMbx(&mbxInfo);

	for (i = 0; i < msgCount; i++) {
		memset(&msg[i], 0, sizeof(MSG));
		msg[i].header.priority = msgCount - i;
		msg[i].payload         = i + 1;
		SendMbx(mbxId, &msg[i]);
	}

	printf("  box contents: ");
	printMbx(mbxId);
	
	DeleteMbx(mbxId);
}

void testLinkUpdate() {
	iop_mbx_t mbxInfo = {};
	mbxInfo.option = 0;
	mbxInfo.attr   = MBA_THFIFO | MBA_MSFIFO;
	s32 mbxId = CreateMbx(&mbxInfo);

	MSG msg1, msg2;
	
	memset(&msg1, 0, sizeof(MSG));
	memset(&msg2, 0, sizeof(MSG));
	msg1.header.next = (struct iop_message*)&msg2;
	
	SendMbx(mbxId, &msg1);
	
	printf("  after sending, msg1's next pointer refers to: ");
	if (msg1.header.next == (struct iop_message*)&msg1) {
		printf("msg1\n");
	} else if (msg1.header.next == (struct iop_message*)&msg2) {
		printf("msg2\n");
	} else {
		printf("unknown (%p)\n", msg1.header.next);
	}

	DeleteMbx(mbxId);
}

void testSendWakeup() {
	iop_mbx_t mbxInfo = {};
	mbxInfo.option = 0;
	mbxInfo.attr   = MBA_THFIFO | MBA_MSFIFO;
	s32 mbxId = CreateMbx(&mbxInfo);

	iop_thread_t threadInfo = {};
	threadInfo.attr      = TH_C;
	threadInfo.thread    = (void*)&messageWaitThreadProc;
	threadInfo.priority  = getThreadPriority(TH_SELF) - 1;
	threadInfo.stacksize = STACK_SIZE;
	s32 thId = CreateThread(&threadInfo);
	StartThread(thId, (void*)mbxId);
	
	printf("  box contents after thread creation: ");
	printMbx(mbxId);

	MSG msg = {};
	msg.payload = 0xBEEF;
	SendMbx(mbxId, &msg);
	
	printf("  box contents after send: ");
	printMbx(mbxId);
	
	printf("  received payload: 0x%X\n", g_receivedPayload);
	
	DeleteMbx(mbxId);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	printf("sending (fifo message queuing):\n");
	testMultipleSend(MBA_THFIFO | MBA_MSFIFO);
	
	printf("sending (prio message queuing):\n");
	testMultipleSend(MBA_THFIFO | MBA_MSPRI);
	
	printf("send message linking behavior:\n");
	testLinkUpdate();
	
	printf("sending wakeup behavior:\n");
	testSendWakeup();
	
	printf("invalid params:\n");
	{
		int result = SendMbx(~0, NULL);
		printf("  sending message to invalid box: %d\n", result);

		//Sending a null message does seem to work even though it probably has
		//dangerous side effects since the message header at address 0 will be modified
		//int result = SendMbx(mbxId, NULL);
		//printf("sending null message: %d\n", result);
		//printMbx(mbxId);
	}
	
	printf("-- TEST END\n");
	return 0;
}
