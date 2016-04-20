#include <common-iop.h>
#include <thpool.h>
#include "vpl-common.h"

void createVpl(u32 attr, u32 option, s32 size) {
	iop_vpl_param vplParam = {};
	vplParam.attr   = attr;
	vplParam.option = option;
	vplParam.size   = size;
	
	s32 vplId = CreateVpl(&vplParam);

	if(vplId > 0) {
		printf("  succeeded -> ");
		printVpl(vplId);
		DeleteVpl(vplId);
	} else {
		printf("  failed -> result: %d\n", vplId);
	}
}

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	printf("valid:\n");
	createVpl(VA_THFIFO | VA_MEMBTM, 0x1234, 0x2000);
	
	printf("bad attr (2):\n");
	createVpl(2, 0x1234, 0x2000);
	
	printf("bad attr (~0):\n");
	createVpl(~0, 0x1234, 0x2000);
	
	printf("bad alloc (huge):\n");
	createVpl(VA_THFIFO | VA_MEMBTM, 0x1234, 0x20000000);
	
	printf("bad alloc (negative):\n");
	createVpl(VA_THFIFO | VA_MEMBTM, 0x1234, -0x2000);
	
	printf("bad attr & bad alloc:\n");
	createVpl(~0, 0x1234, -0x2000);
	
	printf("-- TEST END\n");
	return 0;
}
