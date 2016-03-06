#include <common-iop.h>
#include <thpool.h>
#include "vpl-common.h"

#define MAX_ALLOC_SIZES 24

static const s32 g_allocSizes[MAX_ALLOC_SIZES] = {
	0x80, 0x81, 0x82, 0x83, 
	0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8A, 0x8B,
	0x8C, 0x8D, 0x8E, 0x8F,
	0x100, 0x800, 0x1000, 0x8000,
	-0x1, -0x5, -0x8, -0x80
};

void doAllocate(u32 vplId, s32 size) {
	void *ptr = pAllocateVpl(vplId, size);
	s32 result = (s32)ptr;
	if(result >= 0) {
		printf("  succeeded -> ");
		printVpl(vplId);
		FreeVpl(vplId, ptr);
	} else {
		printf("  failed -> result (%d)\n", result);
	}
} 

void testSingleAllocs() {
	iop_vpl_param vplParam = {};
	vplParam.attr   = VA_THFIFO;
	vplParam.option = 0;
	vplParam.size   = 0x1000;
	
	s32 vplId = CreateVpl(&vplParam);
	
	s32 i = 0;
	for(i = 0; i < MAX_ALLOC_SIZES; i++) {
		s32 allocSize = g_allocSizes[i];
		printf("  allocate %d bytes:\n", allocSize);
		printf("  "); doAllocate(vplId, allocSize);
	}
	
	DeleteVpl(vplId);
}

void testMultiAllocs() {
	iop_vpl_param vplParam = {};
	vplParam.attr   = VA_THFIFO;
	vplParam.option = 0;
	vplParam.size   = 0x1000;
	
	s32 vplId = CreateVpl(&vplParam);
	
	pAllocateVpl(vplId, 0x800);
	pAllocateVpl(vplId, 0x400);
	pAllocateVpl(vplId, 0x200);
	pAllocateVpl(vplId, 0x100);
	
	s32 i = 0;
	for(i = 0; i < MAX_ALLOC_SIZES; i++) {
		s32 allocSize = g_allocSizes[i];
		printf("  allocate %d bytes:\n", allocSize);
		printf("  "); doAllocate(vplId, allocSize);
	}
	
	DeleteVpl(vplId);
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	printf("single allocations:\n");
	testSingleAllocs();
	
	printf("multiple allocations:\n");
	testMultiAllocs();
	
	printf("alloc from invalid vpl:\n");
	doAllocate(~0, 0x100);
	
	printf("alloc negative from invalid vpl:\n");
	doAllocate(~0, -0x80);
	
	printf("-- TEST END\n");
	return 0;
}
