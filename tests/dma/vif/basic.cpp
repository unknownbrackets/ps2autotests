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

static u8 *const vu0_mem = (u8 *)0x11004000;

void send_simple_fifo_vif(volatile u128 *fifo, void *vifcode, int size) {
	// Must be aligned to 16 bytes.
	assert((size & 0xf) == 0 && (((u32)vifcode) & 0xf) == 0);

	u128 *vif128 = (u128 *)vifcode;
	for (int i = 0; i < size / 16; ++i) {
		*fifo = vif128[i];
	}
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	volatile u32 *vu0_32 = (volatile u32 *)vu0_mem;

	VIF::Packet vifcode(64);
	vifcode.UNPACK(VIF::UNPACK_TYPE_S32, 4, 0);
	vifcode.Data32(0x1337C0DE);
	vifcode.Data32(0x1337C1DE);
	vifcode.Data32(0x1337C2DE);
	vifcode.Data32(0x1337C3DE);

	DMA::SrcChainPacket srcpacket(512);
	srcpacket.REFE(vifcode.Raw(), 2);

	memset(vu0_mem, 0, 16 * 4);
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);

	send_simple_fifo_vif(VIF::VIF0_FIFO, vifcode.Raw(), 32);
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);

	printf("VIF0 FIFO: %08x (%08x) - %08x - %08x - %08x\n", vu0_32[0], vu0_32[1], vu0_32[4], vu0_32[8], vu0_32[12]);

	memset(vu0_mem, 0, 16 * 4);
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);

	DMA::SendSimple(DMA::D0, vifcode.Raw(), 32);
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);

	printf("VIF0 DMA simple: %08x (%08x) - %08x - %08x - %08x\n", vu0_32[0], vu0_32[1], vu0_32[4], vu0_32[8], vu0_32[12]);

	memset(vu0_mem, 0, 16 * 4);
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);

	DMA::SendChain(DMA::D0, srcpacket.Raw(), 512);
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);

	printf("VIF0 DMA chain (basic): %08x (%08x) - %08x - %08x - %08x\n", vu0_32[0], vu0_32[1], vu0_32[4], vu0_32[8], vu0_32[12]);

	printf("-- TEST END\n");
	return 0;
}
