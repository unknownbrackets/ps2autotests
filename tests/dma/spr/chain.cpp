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

	DMA::SrcChainPacket srcTag(512);

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;

	srcTag.Reset();
	srcTag.REFE(buf, 1);

	// And then set some data later into it.
	toSPR->sadr = 0x20;
	DMA::SendChain(toSPR, srcTag.Raw(), 512);

	// Now let's read it back (from channel 8.)
	fromSPR->sadr = 0x10;
	DMA::SendSimple(fromSPR, buf, 32);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  Tag data updated: MADR=+%08x, TADR=+%08x\n", (const u8 *)toSPR->madr - (const u8 *)buf, (const u8 *)toSPR->tadr - (const u8 *)srcTag.Raw());
	printf("  SADR +0x10: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  SADR +0x20: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);
	printf("\n");

	// How about misaligned SADR values?
	memset(buf, 0xCC, 16 * 1024);
	buf[0] = 0xDEADBEEF;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0x01234567;
	buf[3] = 0x1337C0DE;

	srcTag.Reset();
	srcTag.REFE(buf, 1);

	toSPR->sadr = 0x14;
	DMA::SendChain(toSPR, srcTag.Raw(), 512);

	memset(buf, 0xCC, 16 * 1024);

	fromSPR->sadr = 0x10;
	DMA::SendSimple(fromSPR, buf, 32);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  Tag data updated: MADR=+%08x, TADR=+%08x\n", (const u8 *)toSPR->madr - (const u8 *)buf, (const u8 *)toSPR->tadr - (const u8 *)srcTag.Raw());
	printf("  SADR +0x14: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  SADR +0x24: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);

	fromSPR->sadr = 0;
	toSPR->sadr = 0;
}

static void testSADRHighBits(u32 *buf) {
	printf("SADR high bits:\n");

	DMA::SrcChainPacket srcTag(512);

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;

	srcTag.Reset();
	srcTag.REFE(buf, 16);

	toSPR->sadr = 0x81234028;
	DMA::SendChain(toSPR, srcTag.Raw(), 512);

	memset(buf, 0, 16);

	fromSPR->sadr = 0x00000020;
	DMA::SendSimple(fromSPR, buf, 16);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  Tag data updated: MADR=+%08x, TADR=+%08x\n", (const u8 *)toSPR->madr - (const u8 *)buf, (const u8 *)toSPR->tadr - (const u8 *)srcTag.Raw());
	printf("  SADR send with ignored bits: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("\n");

	memset(buf, 0, 16);

	fromSPR->sadr = 0x6703802C;
	DMA::SendSimple(fromSPR, buf, 16);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  SADR receive with ignored bits: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);

	fromSPR->sadr = 0;
	toSPR->sadr = 0;
}

static void testSizeZero(u32 *buf) {
	printf("Size zero:\n");

	DMA::SrcChainPacket srcTag(512);

	memset(buf, 0xDD, 16 * 1024);

	buf[0] = 0x01234567;
	buf[1] = 0x89ABCDEF;
	buf[2] = 0xDEADBEEF;
	buf[3] = 0x1337C0DE;

	srcTag.Reset();
	srcTag.REFE(buf, 0);

	toSPR->sadr = 0;
	DMA::SendChain(toSPR, srcTag.Raw(), 512);

	SyncDCache(buf, (u8 *)buf + 16 * 1024);
	memset(buf, 0xCC, 16 * 1024);
	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	fromSPR->sadr = 0;
	DMA::SendSimple(fromSPR, buf, 16 * 1024);

	SyncDCache(buf, (u8 *)buf + 16 * 1024);

	printf("  SADR updated: to=%08x, from=%08x\n", toSPR->sadr, fromSPR->sadr);
	printf("  Send zero 0x0000: %08x %08x %08x %08x\n", buf[0], buf[1], buf[2], buf[3]);
	printf("  Send zero 0x0010: %08x %08x %08x %08x\n", buf[4], buf[5], buf[6], buf[7]);

	for (u32 i = 16 / sizeof(u32); i < 16 * 1024 / sizeof(u32); ++i) {
		if (buf[i] != 0) {
			printf("  Read zeros until: 0x%04x %08x\n", i * sizeof(u32), buf[i]);
			break;
		}
	}

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

	// Setting the SPR bit in chains for fromSPR/toSPR seems to cause crashes.

	// How many bytes does a zero-sized transfer copy?
	clearSPR(buf);
	testSizeZero(buf);

	free(buf);

	printf("-- TEST END\n");
	return 0;
}
