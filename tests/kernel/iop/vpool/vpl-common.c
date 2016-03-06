#include <thpool.h>
#include "vpl-common.h"

void printVpl(s32 vplId) {
	iop_vpl_info_t vplInfo;
	memset(&vplInfo, 0, sizeof(iop_vpl_info_t));
	s32 result = ReferVplStatus(vplId, &vplInfo);
	if(result >= 0) {
		printf("attr: %x, option: %x, wait threads: %d, size: %d, free size: %d\n", 
			vplInfo.attr, vplInfo.option, vplInfo.numWaitThreads, vplInfo.size, vplInfo.freeSize);
	} else {
		printf("failed (%d)\n", result);
	}
}
