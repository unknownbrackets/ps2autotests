#include <common-ee.h>
#include <assert.h>
#include <kernel.h>
#include <malloc.h>
#include <string.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"

static volatile DMA::Channel *const toSPR = DMA::D9;

void testTagIrq(u32 *buf, bool tagIntEnable, bool tagIrq) {
	*DMA::D_STAT = DMA::D_STAT_CIS9;
	
	static const u32 dmaPacketSize = 512;
	DMA::SrcChainPacket srcTag(dmaPacketSize);
	
	srcTag.Reset();
	srcTag.REF(buf, 1, tagIrq);
	srcTag.REF(buf, 1);
	srcTag.END(0);
	
	if(tagIntEnable) {
		toSPR->chcr |= DMA::CHCR_TIE;
	} else {
		toSPR->chcr &= ~DMA::CHCR_TIE;
	}

	u32 cisBefore = *DMA::D_STAT & DMA::D_STAT_CIS9;
	DMA::SendChain(toSPR, srcTag.Raw(), dmaPacketSize);
	u32 cisAfter = *DMA::D_STAT & DMA::D_STAT_CIS9;
	
	u32 tadrOffset = (u8 *)toSPR->tadr - (u8 *)srcTag.Raw();
	
	printf("Tag interrupt %s, Tag IRQ %s:\n", 
		tagIntEnable ? "enabled" : "disabled",
		tagIrq ? "enabled" : "disabled");
	printf("  CHCR(tag): %08x, TADR offset: %08x, CIS(before): %08x, CIS(after): %08x\n", 
		toSPR->chcr & DMA::CHCR_TAG_MASK, tadrOffset, cisBefore, cisAfter);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	u32 *buf = (u32 *)memalign(16, 16 * 1024);
	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;
	
	testTagIrq(buf, true, true);
	testTagIrq(buf, false, true);
	testTagIrq(buf, false, false);
	
	free(buf);
	
	printf("-- TEST END\n");
	return 0;
}
