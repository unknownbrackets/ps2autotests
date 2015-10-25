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

static void testSADROffsets(u32 *buf) {
	printf("SADR offsets:\n");

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;

	// And then set some data later into it.
	toSPR->sadr = 0x20;

	printf("  toSPR SADR (0x10): %08x -> ", toSPR->sadr);
	DMA::SendSimple(toSPR, buf, 16);
	printf("%08x\n", toSPR->sadr);
	
	// Now let's read it back (from channel 8.)
	fromSPR->sadr = 0x10;

	printf("  fromSPR SADR (0x20): %08x -> ", fromSPR->sadr);
	DMA::SendSimple(fromSPR, buf, 32);
	printf("%08x\n", fromSPR->sadr);

	printf("  SADR +0x10: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  SADR +0x20: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);
	printf("\n");
	
	// How about misaligned SADR values?
	memset(buf, 0xCC, 16 * 1024);
	fromSPR->sadr = 0x14;

	printf("  fromSPR SADR (0x20): %08x -> ", fromSPR->sadr);
	DMA::SendSimple(fromSPR, buf, 32);
	printf("%08x\n", fromSPR->sadr);
	
	printf("  SADR +0x14: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  SADR +0x24: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);

	fromSPR->sadr = 0;
	toSPR->sadr = 0;
}

static void testSADRHighBits(u32 *buf) {
	printf("SADR high bits:\n");

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;

	toSPR->sadr = 0x81234028;

	printf("  toSPR SADR (0x10): %08x -> ", toSPR->sadr);
	DMA::SendSimple(toSPR, buf, 16);
	printf("%08x\n", toSPR->sadr);

	memset(buf, 0, 16);

	fromSPR->sadr = 0x00000020;

	printf("  fromSPR SADR (0x10): %08x -> ", fromSPR->sadr);
	DMA::SendSimple(fromSPR, buf, 16);
	printf("%08x\n", fromSPR->sadr);

	printf("  SADR send with ignored bits: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("\n");

	memset(buf, 0, 16);

	fromSPR->sadr = 0x6703802C;

	printf("  fromSPR SADR (0x10): %08x -> ", fromSPR->sadr);
	DMA::SendSimple(fromSPR, buf, 16);
	printf("%08x\n", fromSPR->sadr);

	printf("  SADR receive with ignored bits: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);

	fromSPR->sadr = 0;
	toSPR->sadr = 0;
}

static void testMADRBit(u32 *buf) {
	printf("MADR with high bit:\n");

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;
	SyncDCache(buf, (u8 *)buf + 16);

	toSPR->sadr = 0;

	DMA::SendSimple(toSPR, DMA::SPR_ADDR(buf), 16);
	printf("  toSPR MADR: high bit set? %s\n", (((u32)toSPR->madr & 0x80000000) != 0) ? "yes" : "no");

	memset(buf, 0, 16);
	SyncDCache(buf, (u8 *)buf + 16);

	fromSPR->sadr = 0;
	DMA::SendSimple(fromSPR, buf, 16);

	printf("  MADR send with SPR bit: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("\n");
	
	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;
	SyncDCache(buf, (u8 *)buf + 16);

	toSPR->sadr = 0x100;
	DMA::SendSimple(toSPR, buf, 16);

	memset(buf, 0, 16);
	SyncDCache(buf, (u8 *)buf + 16);

	fromSPR->sadr = 0x100;

	DMA::SendSimple(fromSPR, DMA::SPR_ADDR(buf), 16);
	printf("  fromSPR MADR: high bit set? %s\n", (((u32)fromSPR->madr & 0x80000000) != 0) ? "yes" : "no");

	printf("  MADR receive with SPR bit: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);

	// Trying to do an "SPR -> SPR" transfer via toSPR seems to get a deterministic (non zero) result.
	// But, it's also a weird one... so, for now, not testing that.

	fromSPR->sadr = 0;
	toSPR->sadr = 0;
}

static void testSizeZero(u32 *buf) {
	printf("Size zero:\n");

	memset(buf, 0xDD, 16 * 1024);

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	toSPR->sadr = 0;
	DMA::SendSimple(toSPR, buf, 0);

	SyncDCache(buf, (u8 *)buf + 16 * 1024);
	memset(buf, 0xCC, 16 * 1024);
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	fromSPR->sadr = 0;
	DMA::SendSimple(fromSPR, buf, 16 * 1024);

	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	// It seems like this sometimes does, sometimes doesn't write 16 bytes.
	// But it never writes more.  So we skip the first 16.
	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	//printf("  Send zero 0x0000: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  Send zero 0x0010: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);

	for (u32 i = 16 / sizeof(u32); i < 16 * 1024 / sizeof(u32); ++i) {
		if (buf[i] != 0) {
			printf("  Read zeros until: 0x%04x %08x\n", i * sizeof(u32), buf[i]);
			break;
		}
	}

	clearSPR(buf);

	memset(buf, 0xDD, 16 * 1024);

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	toSPR->sadr = 0;
	DMA::SendSimple(toSPR, buf, 16);

	memset(buf, 0xCC, 16 * 1024);
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	fromSPR->sadr = 0;
	DMA::SendSimple(fromSPR, buf, 0);
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  Read zero 0x0000: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  Read zero 0x0010: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);

	// This is a bit of an odd value, but it seems deterministic.
	for (u32 i = 16 / sizeof(u32); i < 16 * 1024 / sizeof(u32); ++i) {
		if (buf[i] != 0) {
			printf("  Read zeros until: 0x%04x %08x\n", i * sizeof(u32), buf[i]);
			break;
		}
	}

	fromSPR->sadr = 0;
	toSPR->sadr = 0;
}

static void testNearEnd(u32 *buf) {
	printf("Near end (wrapping):\n");

	for (u32 i = 0; i < 16 * 1024 / sizeof(u32); ++i) {
		u32 c = i & 0xFF;
		buf[i] = c | (c << 8) | (c << 16) | (c << 24);
		if (i > 256) {
			buf[i] |= 0xFF000000;
		}
	}

	toSPR->sadr = 0;
	DMA::SendSimple(toSPR, buf, 16 * 1024);

	memset(buf, 0xCC, 16 * 1024);

	fromSPR->sadr = 0x3ff0;
	DMA::SendSimple(fromSPR, buf, 16 * 1024);

	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  Send near end 0x0000: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  Send near end 0x0010: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);

	clearSPR(buf);

	for (u32 i = 0; i < 16 * 1024 / sizeof(u32); ++i) {
		u32 c = i & 0xFF;
		buf[i] = c | (c << 8) | (c << 16) | (c << 24);
		if (i > 256) {
			buf[i] |= 0xFF000000;
		}
	}

	toSPR->sadr = 0x3ff0;
	DMA::SendSimple(toSPR, buf, 16 * 1024);

	memset(buf, 0xCC, 16 * 1024);

	fromSPR->sadr = 0;
	DMA::SendSimple(fromSPR, buf, 16 * 1024);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  Read near end 0x0000: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  Read near end 0x0010: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);

	fromSPR->sadr = 0;
	toSPR->sadr = 0;
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	// We'll reuse this buffer for the SPR in all the tests.
	u32 *buf = (u32 *)memalign(16, 16 * 1024);

	// Offsets should work.
	clearSPR(buf);
	testSADROffsets(buf);
	printf("\n");

	// Certain bits of SADR are ignored.
	clearSPR(buf);
	testSADRHighBits(buf);
	printf("\n");

	// What does the SPR bit of MADR do?
	clearSPR(buf);
	testMADRBit(buf);
	printf("\n");

	// How many bytes does a zero-sized transfer copy?
	// Results seem inconsistent.
	//clearSPR(buf);
	//testSizeZero(buf);
	//printf("\n");

	// What about near the end / wraparound?
	clearSPR(buf);
	testNearEnd(buf);

	free(buf);

	printf("-- TEST END\n");
	return 0;
}
