#include <common-ee.h>
#include <malloc.h>

static const char *describePtr(void *p) {
	u32 v = (u32)p;
	if (v >= 0x80000000 && v < 0x80100000) {
		return "EE Kernel";
	}
	if (v >= 0x80100000 && v < 0x82000000) {
		return "EE KSEG0";
	}
	if (v >= 0xA0100000 && v < 0xA2000000) {
		return "EE KSEG1";
	}
	if (v >= 0x00100000 && v < 0x02000000) {
		return "EE Cached";
	}
	if (v >= 0x10000000 && v < 0x14000000) {
		return "HW Registers";
	}
	if (v >= 0x20100000 && v < 0x22000000) {
		return "EE Uncached";
	}
	if (v >= 0x30100000 && v < 0x32000000) {
		return "EE UCAB";
	}
	if (v >= 0xBC000000 && v < 0xBC200000) {
		return "IOP";
	}
	if (v >= 0x70000000 && v < 0x70004000) {
		return "Scratchpad";
	}
	return "Other";
}

static void tryAlloc(u32 mb) {
	void *p = malloc(mb * 1024 * 1024);
	printf("  %d MB: %s\n", mb, p ? "success" : "fail");
	free(p);
}

static void verifyMirrors() {
	void *p = calloc(1, 1024 * 1024);

	printf("  Allocated in %s\n", describePtr(p));
	printf("  Pointer is %s\n", ((u32)p & 0xF) == 0 ? "aligned" : "not aligned");

	u8 *cached = (u8 *)((u32)p & 0x00FFFFFF);
	u8 *uncached = (u8 *)((u32)cached | 0x20000000);
	u8 *ucab = (u8 *)((u32)cached | 0x30000000);

	*uncached = 42;

	printf("  Mirrors: cached=%d, uncached=%d, ucab=%d\n", *cached, *uncached, *ucab);

	free(p);
}

void verifyScratchpad() {
	// TODO: Is there any system to allocate the scratchpad?  Thread flag?

	u8 *sram = (u8 *)0x70000000;

	sram[0x0000] = 42;
	sram[0x3FFF] = 1;

	printf("  Scratchpad start: %d, end: %d\n", sram[0x0000], sram[0x3FFF]);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	printf("Memory allocation limits:\n");
	// Let's not be to exact, since the program size or other things may make this inexact.
	tryAlloc(32);
	tryAlloc(30);
	tryAlloc(24);
	tryAlloc(16);

	printf("Mirrors:\n");
	verifyMirrors();

	printf("Scratchpad:\n");
	verifyScratchpad();

	printf("-- TEST END\n");
	return 0;
}