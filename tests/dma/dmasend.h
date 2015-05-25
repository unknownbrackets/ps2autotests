#ifndef PS2AUTOTESTS_DMA_DMASEND_H
#define PS2AUTOTESTS_DMA_DMASEND_H

#include <common-ee.h>

namespace DMA {
	void SendSimple(volatile DMA::Channel *chan, void *data, int size);
	void SendChain(volatile DMA::Channel *chan, void *dmatag, int size);
}

#endif
