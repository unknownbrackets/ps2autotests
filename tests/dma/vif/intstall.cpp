#include <common-ee.h>
#include <string.h>
#include <kernel.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"
#include "emit_vifcode.h"
#include "vifunits.h"

static VIF::Unit* g_units[2] = { &VIF::Unit0, &VIF::Unit1 };

typedef void (*WriteIntCodeFunction)(VIF::Packet&);

void writeNopInt(VIF::Packet& vifcode) {
	vifcode.NOP(0, 0, VIF::CMD_ENABLE_INT);
}

void writeMarkInt(VIF::Packet& vifcode) {
	vifcode.MARK(0xBEEF, VIF::CMD_ENABLE_INT);
}

void writeStMaskInt(VIF::Packet& vifcode) {
	vifcode.STMASK(0xFFFFFFFF, VIF::CMD_ENABLE_INT);
}

void doTest(VIF::Unit* unit, const char* testName, WriteIntCodeFunction writeIntCode) {
	static const u32 vifPacketQwc = 3;
	static const u32 vifPacketSize = vifPacketQwc * 0x10;
	
	unit->regs->fbrst = VIF::FBRST_RST;
	
	VIF::Packet vifcode(vifPacketSize);

	vifcode.STMASK(0xAAAAAAAA);
	
	writeIntCode(vifcode);
	vifcode.STMASK(0xBBBBBBBB);
	
	vifcode.NOP();
	vifcode.NOP();
	vifcode.STMASK(0xCCCCCCCC);
	
	DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), vifPacketSize);
	
	printf("%s:\n", testName);
	printf("  Status after transfer - MASK: %08x, STAT: %08x\n", unit->regs->mask, unit->regs->stat);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	for(u32 i = 0; i < 2; i++) {
		VIF::Unit* unit = g_units[i];
		printf("== VIF%d ==\n", i);
		doTest(unit, "NOP (I)", &writeNopInt);
		doTest(unit, "MARK (I)", &writeMarkInt);
		doTest(unit, "STMASK (I)", &writeStMaskInt);
		printf("\n");
	}
	
	printf("-- TEST END\n");
	return 0;
}
