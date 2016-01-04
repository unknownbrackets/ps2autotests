#include <common-ee.h>
#include <string.h>
#include <kernel.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"
#include "emit_vifcode.h"
#include "vifunits.h"

void testMode(VIF::Unit *unit, u32 mode, bool maskEnabled, u32 mask = 0) {
	u8 *vuMem = unit->vuMem;
	volatile u32 *vuMem_32 = (volatile u32 *)vuMem;
	
	memset(vuMem, 0, 16 * 4);
	SyncDCache(vuMem, vuMem + 16 * 4);
	
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
	DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), 16 * 7);
	
	SyncDCache(vuMem, vuMem + 16 * 4);
	
	for (int i = 0; i < 4; i++) {
		printf("  vumem(%02x): %08x - %08x - %08x - %08x\n", i * 0x10, 
		vuMem_32[(i * 4) + 0], vuMem_32[(i * 4) + 1], vuMem_32[(i * 4) + 2], vuMem_32[(i * 4) + 3]);
	}
	
	printf("  row: %08x - %08x - %08x - %08x\n", 
		unit->regs->r0, unit->regs->r1, unit->regs->r2, unit->regs->r3);
	printf("  mode: %08x\n", unit->regs->mode);
	
	printf("\n");
}

void doTest(VIF::Unit *unit) {
	printf("normal (no mask):\n");
	testMode(unit, VIF::MODE_NORMAL, false);
	
	printf("normal (mask diagonal):\n");
	testMode(unit, VIF::MODE_NORMAL, true, 0x40100401);
	
	printf("offset (no mask):\n");
	testMode(unit, VIF::MODE_OFFSET, false);
	
	printf("offset (mask diagonal):\n");
	testMode(unit, VIF::MODE_OFFSET, true, 0x40100401);
	
	printf("difference (no mask):\n");
	testMode(unit, VIF::MODE_DIFFERENCE, false);
	
	printf("difference (mask diagonal):\n");
	testMode(unit, VIF::MODE_DIFFERENCE, true, 0x40100401);
	
	printf("mode 3 (no mask):\n");
	testMode(unit, 3, false);
	
	printf("mode 3 (mask diagonal):\n");
	testMode(unit, 3, true, 0x40100401);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	printf("== VIF0 ==\n");
	doTest(&VIF::Unit0);
	printf("\n");
	
	printf("== VIF1 ==\n");
	doTest(&VIF::Unit1);
	printf("\n");

	printf("-- TEST END\n");
	return 0;
}
