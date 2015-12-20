#include <common-ee.h>
#include <assert.h>
#include <kernel.h>
#include <malloc.h>
#include <string.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"
#include "emit_vifcode.h"
#include "vifregs.h"
#include "viftestctx.h"

void send_simple_fifo_vif(volatile u128 *fifo, void *vifcode, int size) {
	// Must be aligned to 16 bytes.
	assert((size & 0xf) == 0 && (((u32)vifcode) & 0xf) == 0);

	u128 *vif128 = (u128 *)vifcode;
	for (int i = 0; i < size / 16; ++i) {
		*fifo = vif128[i];
	}
}

void doTest(TEST_CONTEXT* context) {
	u8 *vuMem = context->vuMem;
	volatile u32 *vu_32 = (volatile u32 *)vuMem;

	VIF::Packet vifcode(64);
	vifcode.UNPACK(VIF::UNPACK_TYPE_S32, 4, 0);
	vifcode.Data32(0x1337C0DE);
	vifcode.Data32(0x1337C1DE);
	vifcode.Data32(0x1337C2DE);
	vifcode.Data32(0x1337C3DE);

	DMA::SrcChainPacket srcpacket(512);
	srcpacket.REFE(vifcode.Raw(), 2);

	memset(vuMem, 0, 16 * 4);
	SyncDCache(vuMem, vuMem + 16 * 4);

	send_simple_fifo_vif(context->vifFifo, vifcode.Raw(), 32);
	SyncDCache(vuMem, vuMem + 16 * 4);

	printf("FIFO: %08x (%08x) - %08x - %08x - %08x\n", vu_32[0], vu_32[1], vu_32[4], vu_32[8], vu_32[12]);

	memset(vuMem, 0, 16 * 4);
	SyncDCache(vuMem, vuMem + 16 * 4);

	DMA::SendSimple(context->dmaChannel, vifcode.Raw(), 32);
	SyncDCache(vuMem, vuMem + 16 * 4);

	printf("DMA simple: %08x (%08x) - %08x - %08x - %08x\n", vu_32[0], vu_32[1], vu_32[4], vu_32[8], vu_32[12]);

	memset(vuMem, 0, 16 * 4);
	SyncDCache(vuMem, vuMem + 16 * 4);

	DMA::SendChain(context->dmaChannel, srcpacket.Raw(), 512);
	SyncDCache(vuMem, vuMem + 16 * 4);

	printf("DMA chain (basic): %08x (%08x) - %08x - %08x - %08x\n", vu_32[0], vu_32[1], vu_32[4], vu_32[8], vu_32[12]);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	printf("VIF0 -------------------------------------------------------------\n");
	doTest(&testContext0);
	printf("\n");
	
	printf("VIF1 -------------------------------------------------------------\n");
	doTest(&testContext1);
	printf("\n");
	
	printf("-- TEST END\n");
	return 0;
}
