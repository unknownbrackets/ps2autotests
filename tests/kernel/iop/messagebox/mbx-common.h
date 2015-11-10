#ifndef MBX_COMMON_H
#define MBX_COMMON_H

typedef struct {
	iop_message_t header;
	u32           payload;
} MSG;

u32 getThreadPriority(s32 threadId);
void printMbx(s32 mbxId);

#endif
