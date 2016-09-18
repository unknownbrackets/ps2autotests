#include <common-ee.h>
#include <kernel.h>
#include <string.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"
#include "emit_vifcode.h"
#include "vifunits.h"

void testSimpleUnpack(VIF::Unit *unit, const char *description, VIF::UnpackType unpackType, bool zeroExtend, u32 value0, u32 value1 = 0, u32 value2 = 0, u32 value3 = 0) {
	unit->regs->fbrst = VIF::FBRST_RST;
	
	static const u32 packetSize = 16 * 2;
	static const u32 vectorSize = 16 * 1;
	
	u8 *vuMem = unit->vuMem;
	volatile u32 *vu_32 = (volatile u32 *)vuMem;
	
	memset(vuMem, 0xFF, vectorSize);
	SyncDCache(vuMem, vuMem + vectorSize);
	
	VIF::Packet vifcode(packetSize);
	vifcode.STCYCL(4, 4);
	vifcode.UNPACK(unpackType, 1, 0, zeroExtend ? VIF::UNPACK_ZERO_EXTEND : VIF::UNPACK_NORMAL);
	vifcode.Data32(value0);
	vifcode.Data32(value1);
	vifcode.Data32(value2);
	vifcode.Data32(value3);
	
	DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), packetSize);
	
	SyncDCache(vuMem, vuMem + vectorSize);
	
	printf("Simple UNPACK %s - %08x - %08x - %08x - %08x\n", 
		description, vu_32[0], vu_32[1], vu_32[2], vu_32[3]);
}

void testSetUnpack(VIF::Unit *unit) {
	unit->regs->fbrst = VIF::FBRST_RST;
	
	static const u32 packetSize = 16 * 4;
	static const u32 vectorSize = 16 * 256;
	
	u8 *vuMem = unit->vuMem;
	volatile u32 *vu_32 = (volatile u32 *)vuMem;
	
	memset(vuMem, 0xFF, vectorSize);
	SyncDCache(vuMem, vuMem + vectorSize);
	
	VIF::Packet vifcode(packetSize);
	vifcode.STCOL(0xFFEEDDCC, 0xBBAA9988, 0x77665544, 0x33221100);
	vifcode.STMASK(0xAAAAAAAA); //Use COL value for all writes
	vifcode.STCYCL(1, 0);
	vifcode.UNPACK(VIF::UNPACK_TYPE_S8, 256, 0, VIF::UNPACK_ENABLE_MASKS);
	
	DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), packetSize);
	
	SyncDCache(vuMem, vuMem + vectorSize);
	
	printf("Set 4096 bytes UNPACK - %08x - %08x - %08x - %08x\n", 
		vu_32[0], vu_32[1], vu_32[1022], vu_32[1023]);
}

void testNoDataUnpack(VIF::Unit *unit, u32 num) {
	unit->regs->fbrst = VIF::FBRST_RST;
	
	u32 vpsBefore = unit->regs->stat & VIF::STAT_VPS_MASK;
	
	static const u32 packetSize = 16;
	
	VIF::Packet vifcode(packetSize);
	vifcode.NOP();
	vifcode.NOP();
	vifcode.STCYCL(4, 4);
	vifcode.UNPACK(VIF::UNPACK_TYPE_S8, num, 0);
	
	DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), packetSize);
	
	u32 vpsAfter = unit->regs->stat & VIF::STAT_VPS_MASK;
	
	printf("No data UNPACK(NUM: %d) -> VPS before: %d, VPS after: %d, CODE: %08x, NUM: %d\n", 
	    num, vpsBefore, vpsAfter, unit->regs->code, unit->regs->num);
}

void doTest(VIF::Unit *unit) {
	testSimpleUnpack(unit, "S8(signExtend)",  VIF::UNPACK_TYPE_S8,  false, 0x88);
	testSimpleUnpack(unit, "S8(zeroExtend)",  VIF::UNPACK_TYPE_S8,  true,  0x88);
	testSimpleUnpack(unit, "S16(signExtend)", VIF::UNPACK_TYPE_S16, false, 0x8888);
	testSimpleUnpack(unit, "S16(zeroExtend)", VIF::UNPACK_TYPE_S16, true,  0x8888);
	testSimpleUnpack(unit, "S32",             VIF::UNPACK_TYPE_S32, false, 0x88888888);
	
	testSimpleUnpack(unit, "V2-8(signExtend)",  VIF::UNPACK_TYPE_V2_8,  false, 0x9988);
	testSimpleUnpack(unit, "V2-8(zeroExtend)",  VIF::UNPACK_TYPE_V2_8,  true,  0x9988);
	testSimpleUnpack(unit, "V2-16(signExtend)", VIF::UNPACK_TYPE_V2_16, false, 0x99998888);
	testSimpleUnpack(unit, "V2-16(zeroExtend)", VIF::UNPACK_TYPE_V2_16, true,  0x99998888);
	testSimpleUnpack(unit, "V2-32",             VIF::UNPACK_TYPE_V2_32, false, 0x88888888, 0x99999999);
	
	testSimpleUnpack(unit, "V3-8(signExtend)",  VIF::UNPACK_TYPE_V3_8,  false, 0xAA9988);
	testSimpleUnpack(unit, "V3-8(zeroExtend)",  VIF::UNPACK_TYPE_V3_8,  true,  0xAA9988);
	testSimpleUnpack(unit, "V3-16(signExtend)", VIF::UNPACK_TYPE_V3_16, false, 0x99998888, 0xAAAA);
	testSimpleUnpack(unit, "V3-16(zeroExtend)", VIF::UNPACK_TYPE_V3_16, true,  0x99998888, 0xAAAA);
	testSimpleUnpack(unit, "V3-32",             VIF::UNPACK_TYPE_V3_32, false, 0x88888888, 0x99999999, 0xAAAAAAAA);
	
	testSimpleUnpack(unit, "V4-8(signExtend)",  VIF::UNPACK_TYPE_V4_8,       false, 0xBBAA9988);
	testSimpleUnpack(unit, "V4-8(zeroExtend)",  VIF::UNPACK_TYPE_V4_8,       true,  0xBBAA9988);
	testSimpleUnpack(unit, "V4-16(signExtend)", VIF::UNPACK_TYPE_V4_16,      false, 0x99998888, 0xBBBBAAAA);
	testSimpleUnpack(unit, "V4-16(zeroExtend)", VIF::UNPACK_TYPE_V4_16,      true,  0x99998888, 0xBBBBAAAA);
	testSimpleUnpack(unit, "V4-32",             VIF::UNPACK_TYPE_V4_32,      false, 0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB);
	testSimpleUnpack(unit, "V4-5",              VIF::UNPACK_TYPE_V4_5_5_5_1, false, 0xFFFF);
	
	testSetUnpack(unit);
	
	testNoDataUnpack(unit, 1);
	testNoDataUnpack(unit, 255);
	testNoDataUnpack(unit, 256);
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
