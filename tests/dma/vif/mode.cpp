#include <common-ee.h>
#include <string.h>
#include <kernel.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"
#include "emit_vifcode.h"
#include "vifregs.h"

static u8 *const vu0_mem = (u8 *)0x11004000;

void testMode(u32 mode, bool maskEnabled, u32 mask = 0) {
	volatile u32 *vu0_32 = (volatile u32 *)vu0_mem;
	
	memset(vu0_mem, 0, 16 * 4);
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);
	
	VIF::Packet vifcode(16 * 7);

	vifcode.NOP();
	vifcode.NOP();
	vifcode.STCYCL(4, 4);
	vifcode.STROW(0x1000, 0x1000, 0x1000, 0x1000);

	vifcode.STMASK(mask);
	vifcode.STMOD(mode);
	vifcode.UNPACK(VIF::UNPACK_TYPE_V4_32, 4, 0, maskEnabled ? VIF::UNPACK_ENABLE_MASKS : VIF::UNPACK_NORMAL);
	for (int i = 0; i < 16; i++) {
		vifcode.Data32(i);
	}
	DMA::SendSimple(DMA::D0, vifcode.Raw(), 16 * 7);
	
	SyncDCache(vu0_mem, vu0_mem + 16 * 4);
	
	for (int i = 0; i < 4; i++) {
		printf("  vumem(%02x): %08x - %08x - %08x - %08x\n", i * 0x10, vu0_32[(i * 4) + 0], vu0_32[(i * 4) + 1], vu0_32[(i * 4) + 2], vu0_32[(i * 4) + 3]);
	}
	
	printf("  row: %08x - %08x - %08x - %08x\n", VIF::VIF0->r0, VIF::VIF0->r1, VIF::VIF0->r2, VIF::VIF0->r3);
	
	printf("\n");
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	printf("normal (no mask):\n");
	testMode(VIF::MODE_NORMAL, false);
	
	printf("normal (mask diagonal):\n");
	testMode(VIF::MODE_NORMAL, true, 0x40100401);
	
	printf("offset (no mask):\n");
	testMode(VIF::MODE_OFFSET, false);
	
	printf("offset (mask diagonal):\n");
	testMode(VIF::MODE_OFFSET, true, 0x40100401);
	
	printf("difference (no mask):\n");
	testMode(VIF::MODE_DIFFERENCE, false);
	
	printf("difference (mask diagonal):\n");
	testMode(VIF::MODE_DIFFERENCE, true, 0x40100401);
	
	printf("mode 3 (no mask):\n");
	testMode(3, false);
	
	printf("mode 3 (mask diagonal):\n");
	testMode(3, true, 0x40100401);
	
	printf("-- TEST END\n");
	return 0;
}
