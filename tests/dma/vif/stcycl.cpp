#include <common-ee.h>
#include <string.h>
#include <kernel.h>
#include <ee_regs.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"
#include "emit_vifcode.h"
#include "vifregs.h"

static u8 *const vu0_mem = (u8 *)0x11004000;

void testCycleNormal(u32 cl, u32 wl) {
	volatile u32 *vu0_32 = (volatile u32 *)vu0_mem;
	
	static const u32 outputSize = 8;
	static const u32 displaySize = 64;
	static const u32 vifPacketSize = (outputSize + 4);
	
	memset(vu0_mem, 0, 16 * displaySize);
	SyncDCache(vu0_mem, vu0_mem + 16 * displaySize);

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
	
	DMA::SendSimple(DMA::D0, vifcode.Raw(), 16 * vifPacketSize);
	
	SyncDCache(vu0_mem, vu0_mem + 16 * displaySize);
	
	for (u32 i = 0; i < displaySize / 4; i++) {
		printf("%x%x%x%x",  
			vu0_32[(i * 0x10) + 0x0], vu0_32[(i * 0x10) + 0x4], vu0_32[(i * 0x10) + 0x8], vu0_32[(i * 0x10) + 0xC]);
	}
	
	printf("\n");
}

void testCycleTruncated(u32 cl, u32 wl) {
	volatile u32 *vu0_32 = (volatile u32 *)vu0_mem;
	
	static const u32 outputSize = 8;
	static const u32 displaySize = 64;
	
	memset(vu0_mem, 0, 16 * displaySize);
	SyncDCache(vu0_mem, vu0_mem + 16 * displaySize);

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
		
		DMA::SendSimple(DMA::D0, vifcode.Raw(), 16 * vifPacketSize);
	}

	{
		static const u32 vifPacketSize = (outputSize / 2);
		VIF::Packet vifcode(16 * vifPacketSize);

		//(outputSize / 2) qwords
		for (u32 i = outputSize * 2; i < outputSize * 4; i++) {
			vifcode.Data32(i);
		}

		DMA::SendSimple(DMA::D0, vifcode.Raw(), 16 * vifPacketSize);
	}
	
	SyncDCache(vu0_mem, vu0_mem + 16 * displaySize);
	
	for (u32 i = 0; i < displaySize / 4; i++) {
		printf("%x%x%x%x",  
			vu0_32[(i * 0x10) + 0x0], vu0_32[(i * 0x10) + 0x4], vu0_32[(i * 0x10) + 0x8], vu0_32[(i * 0x10) + 0xC]);
	}
	
	printf("\n");
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	for(u32 cl = 0; cl <= 8; cl++) {
		for(u32 wl = 0; wl <= 8; wl++) {
			printf("normal (cl %d, wl %d): ", cl, wl);
			testCycleNormal(cl, wl);
		}
	}
	
	for(u32 cl = 0; cl <= 8; cl++) {
		for(u32 wl = 0; wl <= 8; wl++) {
			printf("truncated (cl %d, wl %d): ", cl, wl);
			testCycleTruncated(cl, wl);
		}
	}
	
	printf("-- TEST END\n");
	return 0;
}
