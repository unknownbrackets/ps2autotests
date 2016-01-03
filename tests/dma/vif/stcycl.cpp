#include <common-ee.h>
#include <string.h>
#include <kernel.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"
#include "emit_vifcode.h"
#include "vifunits.h"

void testCycleNormal(VIF::Unit *unit, u32 cl, u32 wl) {
	u8 *vuMem = unit->vuMem;
	volatile u32 *vuMem_32 = (volatile u32 *)vuMem;
	
	static const u32 outputSize = 8;
	static const u32 displaySize = 64;
	static const u32 vifPacketSize = (outputSize + 4);
	
	memset(vuMem, 0, 16 * displaySize);
	SyncDCache(vuMem, vuMem + 16 * displaySize);

	VIF::Packet vifcode(16 * vifPacketSize);

	//1 qword
	vifcode.NOP();
	vifcode.STCYCL(cl, wl);
	vifcode.STMASK(0xAAAAAAAA);	//2 words

	//2 qwords
	vifcode.NOP();
	vifcode.NOP();
	vifcode.NOP();
	vifcode.STCOL(1, 2, 3, 4);	//5 words

	//1 qword
	vifcode.NOP();
	vifcode.NOP();
	vifcode.NOP();
	vifcode.UNPACK(VIF::UNPACK_TYPE_V4_32, outputSize, 0, VIF::UNPACK_ENABLE_MASKS);

	//outputSize qwords
	for (u32 i = 0; i < outputSize * 4; i++) {
		vifcode.Data32(i);
	}
	
	DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), 16 * vifPacketSize);
	
	SyncDCache(vuMem, vuMem + 16 * displaySize);
	
	u32 cycleReg = unit->regs->cycle;
	printf("CYCLE: %08x, ", cycleReg);
	
	for (u32 i = 0; i < displaySize / 4; i++) {
		printf("%x%x%x%x",  
			vuMem_32[(i * 0x10) + 0x0], vuMem_32[(i * 0x10) + 0x4], vuMem_32[(i * 0x10) + 0x8], vuMem_32[(i * 0x10) + 0xC]);
	}
	
	printf("\n");
}

void testCycleTruncated(VIF::Unit *unit, u32 cl, u32 wl) {
	u8 *vuMem = unit->vuMem;
	volatile u32 *vuMem_32 = (volatile u32 *)unit->vuMem;
	
	static const u32 outputSize = 8;
	static const u32 displaySize = 64;
	
	memset(vuMem, 0, 16 * displaySize);
	SyncDCache(vuMem, vuMem + 16 * displaySize);

	{
		static const u32 vifPacketSize = ((outputSize / 2) + 4);
		VIF::Packet vifcode(16 * vifPacketSize);

		//1 qword
		vifcode.NOP();
		vifcode.STCYCL(cl, wl);
		vifcode.STMASK(0xAAAAAAAA);	//2 words

		//2 qwords
		vifcode.NOP();
		vifcode.NOP();
		vifcode.NOP();
		vifcode.STCOL(1, 2, 3, 4);	//5 words

		//1 qword
		vifcode.NOP();
		vifcode.NOP();
		vifcode.NOP();
		vifcode.UNPACK(VIF::UNPACK_TYPE_V4_32, outputSize, 0, VIF::UNPACK_ENABLE_MASKS);

		//(outputSize / 2) qwords
		for (u32 i = 0; i < outputSize * 2; i++) {
			vifcode.Data32(i);
		}
		
		DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), 16 * vifPacketSize);
	}

	{
		static const u32 vifPacketSize = (outputSize / 2);
		VIF::Packet vifcode(16 * vifPacketSize);

		//(outputSize / 2) qwords
		for (u32 i = outputSize * 2; i < outputSize * 4; i++) {
			vifcode.Data32(i);
		}

		DMA::SendSimple(unit->dmaChannel, vifcode.Raw(), 16 * vifPacketSize);
	}
	
	SyncDCache(vuMem, vuMem + 16 * displaySize);
	
	u32 cycleReg = unit->regs->cycle;
	printf("CYCLE: %08x, ", cycleReg);
	
	for (u32 i = 0; i < displaySize / 4; i++) {
		printf("%x%x%x%x",  
			vuMem_32[(i * 0x10) + 0x0], vuMem_32[(i * 0x10) + 0x4], vuMem_32[(i * 0x10) + 0x8], vuMem_32[(i * 0x10) + 0xC]);
	}
	
	printf("\n");
}

void doTest(VIF::Unit *unit) {
	for(u32 cl = 0; cl <= 8; cl++) {
		for(u32 wl = 0; wl <= 8; wl++) {
			printf("normal (cl %d, wl %d): ", cl, wl);
			testCycleNormal(unit, cl, wl);
		}
	}
	
	for(u32 cl = 251; cl <= 255; cl++) {
		for(u32 wl = 0; wl <= 8; wl++) {
			printf("normal (cl %d, wl %d): ", cl, wl);
			testCycleNormal(unit, cl, wl);
		}
	}
	
	for(u32 cl = 0; cl <= 8; cl++) {
		for(u32 wl = 251; wl <= 255; wl++) {
			printf("normal (cl %d, wl %d): ", cl, wl);
			testCycleNormal(unit, cl, wl);
		}
	}
	
	for(u32 cl = 0; cl <= 8; cl++) {
		for(u32 wl = 0; wl <= 8; wl++) {
			printf("truncated (cl %d, wl %d): ", cl, wl);
			testCycleTruncated(unit, cl, wl);
		}
	}
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	printf("VIF0 -------------------------------------------------------------\n");
	doTest(&VIF::Unit0);
	printf("\n");
	
	printf("VIF1 -------------------------------------------------------------\n");
	doTest(&VIF::Unit1);
	printf("\n");

	printf("-- TEST END\n");
	return 0;
}
