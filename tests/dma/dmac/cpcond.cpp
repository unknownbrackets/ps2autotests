#include <common-ee.h>
#include <assert.h>
#include <kernel.h>
#include <malloc.h>
#include <string.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"

static volatile DMA::Channel *const toSPR = DMA::D9;

u32 getCpCond() {
	u32 result = 0;
	asm volatile (
		".set noreorder\n"
		"sync.l\n"
		"sync.p\n"
		"bc0t done\n"
		"addiu %0, $0, 1\n"
		"b done\n"
		"addiu %0, $0, 0\n"
		"done:\n"
		: : "r"(result)
	);
	return result;
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	u32 *buf = (u32 *)memalign(16, 16 * 1024);
	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;
	
	u32 result = 0;
	
	result = getCpCond();
	printf("initial value: %d\n", result);
	
	*DMA::D_PCR = DMA::D_PCR_CPC9;
	
	result = getCpCond();
	printf("value after setting PCR.CPC: %d\n", result);
	
	DMA::SendSimple(toSPR, buf, 16 * 1024);
	
	result = getCpCond();
	printf("value after sending DMA: %d\n", result);
	
	*DMA::D_PCR &= ~DMA::D_PCR_CPC9;
	
	result = getCpCond();
	printf("value after clearing PCR.CPC: %d\n", result);
	
	*DMA::D_PCR = DMA::D_PCR_CPC9;

	result = getCpCond();
	printf("value after restoring PCR.CPC: %d\n", result);
	
	*DMA::D_STAT = DMA::D_STAT_CIS9;
	
	result = getCpCond();
	printf("value after resetting STAT.CIS: %d\n", result);
	
	free(buf);
	
	printf("-- TEST END\n");
	return 0;
}
