#include <common-ee.h>
#include <assert.h>
#include <kernel.h>
#include "dmaregs.h"
#include "dmasend.h"

namespace DMA {
	void SendSimple(volatile Channel *chan, void *data, int size) {
		// Must be aligned to 16 bytes.
		assert((size & 0xf) == 0 && (((u32)data) & 0xf) == 0);

		// First, we need to flush the dcache for this data.
		SyncDCache(data, (u8 *)data + size);

		// Enable DMA, and turn off release signal/cycle, MFIFO, and stall control.
		*D_CTRL = D_CTRL_DMAE;

		// Now set the packet address and size.
		chan->madr = data;
		chan->qwc = size / 16;
		chan->tadr = 0;

		if (chan->madr != data) {
			printf("WARNING: DMA transfer did not accept MADR: %08x\n", (u32)chan->madr ^ (u32)data);
		}
		const char *madrStart = (const char *)chan->madr;

		// And start the transfer.  Let's keep it simple.
		chan->chcr |= CHCR_STR | CHCR_DIR;

		int i = 100000;
		while (--i > 0 && chan->chcr.Ongoing()) {
			continue;
		}

		if (chan->madr != madrStart + size) {
			printf("WARNING: DMA transfer MADR did not increase as expected: %08x\n", (u32)chan->madr - (u32)madrStart);
		}

		if (i == 0) {
			printf("ERROR: DMA transfer timed out.\n");
		}
	}

	void SendChain(volatile Channel *chan, void *dmatag, int size) {
		// Must be aligned to 16 bytes.
		assert((size & 0xf) == 0 && (((u32)dmatag) & 0xf) == 0);

		// We need to flush the dcache for the tag.  The caller will need to flush for the data.
		SyncDCache(dmatag, (u8 *)dmatag + size);

		// Here, we only need the tag address.
		chan->madr = 0;
		chan->qwc = 0;
		chan->tadr = dmatag;

		// Start the transfer in chain mode.
		chan->chcr = (chan->chcr & ~CHCR_MOD_ANY) | CHCR_MOD_CHAIN | CHCR_STR;

		int i = 100000;
		while (--i > 0 && chan->chcr.Ongoing()) {
			continue;
		}

		// Clear the mode back for next time.
		chan->chcr &= ~CHCR_MOD_ANY;

		if (i == 0) {
			printf("ERROR: DMA transfer timed out.\n");
		}
	}
}
