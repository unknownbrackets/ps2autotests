#ifndef PS2AUTOTESTS_DMA_VIF_VIFTESTCTX_H
#define PS2AUTOTESTS_DMA_VIF_VIFTESTCTX_H

#include <common-ee.h>
#include "../dmaregs.h"
#include "vifregs.h"

struct TEST_CONTEXT {
	u8 *vuMem;
	volatile VIF::VIFRegs *const vifRegs;
	volatile u128 *vifFifo;
	volatile DMA::Channel *dmaChannel;
};

extern TEST_CONTEXT testContext0;
extern TEST_CONTEXT testContext1;

#endif
