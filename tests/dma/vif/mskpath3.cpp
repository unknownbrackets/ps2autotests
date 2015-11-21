#include <common-ee.h>
#include <ee_regs.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "emit_vifcode.h"

#define EE_GIF_STAT_M3P (0x02)

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	printf("mask value (initial): %08X\n", *R_EE_GIF_STAT & EE_GIF_STAT_M3P);
	
	{
		VIF::Packet vifcode(16);
		vifcode.MSKPATH3(true);
		DMA::SendSimple(DMA::D1, vifcode.Raw(), 16);
	
		printf("mask value (after mskpath3(true))): %08X\n", *R_EE_GIF_STAT & EE_GIF_STAT_M3P);
	}

	{
		VIF::Packet vifcode(16);
		vifcode.MSKPATH3(false);
		DMA::SendSimple(DMA::D1, vifcode.Raw(), 16);

		printf("mask value (after mskpath3(false))): %08X\n", *R_EE_GIF_STAT & EE_GIF_STAT_M3P);
	}

	printf("-- TEST END\n");
	return 0;
}
