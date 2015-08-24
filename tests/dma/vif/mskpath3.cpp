#include <common-ee.h>
#include <assert.h>
#include <kernel.h>
#include <malloc.h>
#include <string.h>
#include <ee_regs.h>
#include "emit_vifcode.h"
#include "vifregs.h"

#define EE_GIF_STAT_M3P (0x02)

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

	printf("mask value (initial): %08X\n", *R_EE_GIF_STAT & EE_GIF_STAT_M3P);
	
	{
		VIF::Packet vifcode(16);
		vifcode.MSKPATH3(true);

		send_simple_fifo_vif(VIF::VIF1_FIFO, vifcode.Raw(), 16);

		printf("mask value (after mskpath3(true))): %08X\n", *R_EE_GIF_STAT & EE_GIF_STAT_M3P);
	}

	{
		VIF::Packet vifcode(16);
		vifcode.MSKPATH3(false);

		send_simple_fifo_vif(VIF::VIF1_FIFO, vifcode.Raw(), 16);

		printf("mask value (after mskpath3(false))): %08X\n", *R_EE_GIF_STAT & EE_GIF_STAT_M3P);
	}

	printf("-- TEST END\n");
	return 0;
}
