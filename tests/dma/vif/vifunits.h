#ifndef PS2AUTOTESTS_DMA_VIF_VIFUNITS_H
#define PS2AUTOTESTS_DMA_VIF_VIFUNITS_H

#include <common-ee.h>
#include "vifregs.h"

namespace VIF {

	struct Unit {
		u8 *vuMem;
		volatile VIFRegs *const regs;
		volatile u128 *fifo;
		volatile DMA::Channel *dmaChannel;
	};
	
	static Unit Unit0 = { (u8*)0x11004000, VIF0, VIF0_FIFO, DMA::D0 };
	static Unit Unit1 = { (u8*)0x1100C000, VIF1, VIF1_FIFO, DMA::D1 };

}

#endif
