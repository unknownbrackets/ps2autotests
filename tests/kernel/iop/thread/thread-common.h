#ifndef THREAD_COMMON_H
#define THREAD_COMMON_H

s32 getThreadPriority(s32 threadId);
s32 createTestThread(void *entry, s32 prio, u32 stackSize);

#endif
