#include <common-ee.h>
#include <assert.h>
#include <kernel.h>
#include <malloc.h>
#include <string.h>
#include "../dmaregs.h"
#include "../dmasend.h"
#include "../dmatags.h"

static volatile DMA::Channel *const fromSPR = DMA::D8;
static volatile DMA::Channel *const toSPR = DMA::D9;

static void clearSPR(u32 *buf) {
	memset(buf, 0, 16 * 1024);

	toSPR->sadr = 0;
	DMA::SendSimple(toSPR, buf, 16 * 1024);
}

static void doTestInterleaveTo(u32 *buf, u16 skip, u16 transfer, u16 qwc) {
	// First, let's clear it to 0xCC so we can see when it stops more easily.
	memset(buf, 0xCC, 16 * 1024);
	toSPR->sadr = 0;
	DMA::SendSimple(toSPR, buf, 16 * 1024);

	for (u32 i = 0; i < 16 * 1024 / sizeof(u32); ++i) {
		u32 c = i & 0xFF;
		buf[i] = c | (c << 8) | (c << 16) | (c << 24);
		if (i > 256) {
			buf[i] |= 0xFF000000;
		}
	}
	// We need to sync the d-cache outside the initial range (DMA::SendSimple doesn't account for interleave.)
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	DMA::D_SQWC->SQWC(skip);
	DMA::D_SQWC->TQWC(transfer);
	toSPR->sadr = 0;
	DMA::SendSimple(toSPR, buf, qwc * 0x10);

	memset(buf, 0xDD, 16 * 1024);
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	DMA::D_SQWC->SQWC(0);
	DMA::D_SQWC->TQWC(0);
	fromSPR->sadr = 0;
	DMA::SendSimple(fromSPR, buf, 16 * 1024);
}

static void printTestInterleaveTo(const char *title, u32 *buf, bool enable, u16 skip, u16 transfer, u16 qwc, bool newline, bool disp = true) {
	printf("Interleave upload %s:\n", title);
	if (enable) {
		toSPR->chcr |= DMA::CHCR_MOD_INTERLEAVE;
		doTestInterleaveTo(buf, skip, transfer, qwc);
		toSPR->chcr &= ~DMA::CHCR_MOD_INTERLEAVE;
	} else {
		doTestInterleaveTo(buf, skip, transfer, qwc);
	}
	printf("  Upload %s SADR: from=%08x, to=%08x\n", title, fromSPR->sadr, toSPR->sadr);
	if (disp) {
		for (u32 i = 0; i < 0x100 / sizeof(u32); i += 0x10 / sizeof(u32)) {
			printf("  Upload %s %02x: %08x %08x %08x %08x\n", title, i, buf[i + 0], buf[i + 1], buf[i + 2], buf[i + 3]);
		}
	}
	if (newline) {
		printf("\n");
	}
}

static void doTestInterleaveFrom(u32 *buf, u16 skip, u16 transfer, u16 qwc) {
	// Let's put something into the SPR to download.
	for (u32 i = 0; i < 16 * 1024 / sizeof(u32); ++i) {
		u32 c = i & 0xFF;
		buf[i] = c | (c << 8) | (c << 16) | (c << 24);
		if (i > 256) {
			buf[i] |= 0xFF000000;
		}
	}

	toSPR->sadr = 0;
	DMA::SendSimple(toSPR, buf, 16 * 1024);

	// Now, clear buf so we can tell what gets read.
	memset(buf, 0xCC, 16 * 1024);

	// We need to sync the d-cache outside the initial range (DMA::SendSimple doesn't account for interleave.)
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	DMA::D_SQWC->SQWC(skip);
	DMA::D_SQWC->TQWC(transfer);
	fromSPR->sadr = 0;
	DMA::SendSimple(fromSPR, buf, qwc * 0x10);

	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	DMA::D_SQWC->SQWC(0);
	DMA::D_SQWC->TQWC(0);
}

static void printTestInterleaveFrom(const char *title, u32 *buf, bool enable, u16 skip, u16 transfer, u16 qwc, bool newline, bool disp = true) {
	printf("Interleave download %s:\n", title);
	if (enable) {
		fromSPR->chcr |= DMA::CHCR_MOD_INTERLEAVE;
		doTestInterleaveFrom(buf, skip, transfer, qwc);
		fromSPR->chcr &= ~DMA::CHCR_MOD_INTERLEAVE;
	} else {
		doTestInterleaveFrom(buf, skip, transfer, qwc);
	}
	printf("  Download %s SADR: from=%08x, to=%08x\n", title, fromSPR->sadr, toSPR->sadr);
	if (disp) {
		for (u32 i = 0; i < 0x100 / sizeof(u32); i += 0x10 / sizeof(u32)) {
			printf("  Download %s %02x: %08x %08x %08x %08x\n", title, i, buf[i + 0], buf[i + 1], buf[i + 2], buf[i + 3]);
		}
	}
	if (newline) {
		printf("\n");
	}
}

static void testInterleaveTo(u32 *buf) {
	printTestInterleaveTo("(disabled, count = 3)", buf, false, 1, 1, 3, true);

	printTestInterleaveTo("(skip 0, count = 3)", buf, true, 0, 1, 3, true);
	printTestInterleaveTo("(skip 1, count = 3)", buf, true, 1, 1, 3, true);
	printTestInterleaveTo("(skip 2, count = 3)", buf, true, 2, 1, 3, true);
	printTestInterleaveTo("(skip 3, count = 3)", buf, true, 3, 1, 3, true);

	// Hangs.
	//printTestInterleaveTo("(transfer 0)", buf, true, 1, 0, 1, false);
	printTestInterleaveTo("(transfer 1, count 3)", buf, true, 1, 1, 3, true);
	printTestInterleaveTo("(transfer 2, count 4)", buf, true, 1, 2, 4, true);
	printTestInterleaveTo("(transfer 3, count 6)", buf, true, 1, 3, 6, true);
	
	printTestInterleaveTo("(transfer 2, skip 2, count 6)", buf, true, 2, 2, 6, true);
	printTestInterleaveTo("(transfer 3, skip 3, count 9)", buf, true, 3, 3, 9, true);
	printTestInterleaveTo("(transfer 1, skip 14, count 2)", buf, true, 14, 1, 2, true);

	// Just hangs.
	//printTestInterleaveTo("(transfer more than qwc)", buf, true, 1, 4, 2, true);

	printTestInterleaveTo("(transfer 1, skip 128, count 2)", buf, true, 128, 1, 2, true, true);
	printTestInterleaveTo("(transfer 1, skip 256, count 2)", buf, true, 256, 1, 2, true, true);
	printTestInterleaveTo("(transfer 1, skip 257, count 2)", buf, true, 257, 1, 2, true, true);

	printTestInterleaveTo("(transfer 128, skip 1, count 128)", buf, true, 1, 128, 128, true, true);
	printTestInterleaveTo("(transfer 257, skip 1, count 3)", buf, true, 1, 257, 3, true, true);

	printTestInterleaveTo("(transfer 1, skip 255, count 4)", buf, true, 255, 1, 4, true, true);
}

static void testInterleaveFrom(u32 *buf) {
	printTestInterleaveFrom("(disabled, count 3)", buf, false, 1, 1, 3, true);

	printTestInterleaveFrom("(skip 0, count 3)", buf, true, 0, 1, 3, true);
	printTestInterleaveFrom("(skip 1, count 3)", buf, true, 1, 1, 3, true);
	printTestInterleaveFrom("(skip 2, count 3)", buf, true, 2, 1, 3, true);
	printTestInterleaveFrom("(skip 3, count 3)", buf, true, 3, 1, 3, true);

	// Would just hang.
	//printTestInterleaveFrom("(transfer 0)", buf, true, 1, 0, 3, false);
	printTestInterleaveFrom("(transfer 1, count 3)", buf, true, 1, 1, 3, true);
	printTestInterleaveFrom("(transfer 2, count 4)", buf, true, 1, 2, 4, true);
	printTestInterleaveFrom("(transfer 3, count 6)", buf, true, 1, 3, 6, true);
	
	printTestInterleaveFrom("(transfer 2, skip 2, count 6)", buf, true, 2, 2, 6, true);
	printTestInterleaveFrom("(transfer 3, skip 3, count 9)", buf, true, 3, 3, 9, true);
	printTestInterleaveFrom("(transfer 1, skip 14, count 2)", buf, true, 14, 1, 2, true);

	// Just hangs, never ends.
	//printTestInterleaveFrom("(transfer more than qwc)", buf, true, 1, 4, 2, true);

	printTestInterleaveFrom("(transfer 1, skip 128, count 2)", buf, true, 128, 1, 2, true, false);
	printTestInterleaveFrom("(transfer 1, skip 256, count 2)", buf, true, 256, 1, 2, true, true);
	printTestInterleaveFrom("(transfer 1, skip 257, count 2)", buf, true, 257, 1, 2, true, true);

	printTestInterleaveFrom("(transfer 128, skip 1, count 128)", buf, true, 1, 128, 128, true, false);
	printTestInterleaveFrom("(transfer 257, skip 1, count 3)", buf, true, 1, 257, 3, true, true);

	printTestInterleaveFrom("(transfer 1, skip 255, count 4)", buf, true, 255, 1, 4, true, false);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	// Warning: if QWC is not a multiple of TWQC, unpredictable things can happen.
	// It generally copies the amount requested, but sometimes sets MADR to a much higher value or crashes.

	// We'll reuse this buffer for the SPR in all the tests.
	u32 *buf = (u32 *)memalign(16, 16 * 1024);

	clearSPR(buf);
	testInterleaveTo(buf);
	printf("\n");

	clearSPR(buf);
	testInterleaveFrom(buf);

	free(buf);

	printf("-- TEST END\n");
	return 0;
}
