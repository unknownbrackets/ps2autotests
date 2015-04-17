#include <stdio.h>
#include <tamtypes.h>
#include "../assemble.h"

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	using namespace VU;
	Block block((LIW *)0x11000000);
	block.Wr(IADDI(VI01, VI00, 5));
	block.SafeExit();

	register u32 vi01 = 0;
	asm volatile (
		"vcallms 0\n"
		"vnop\n"
		"cfc2 %0, vi01\n"
		: "=r"(vi01)
	);

	printf("Running works: %04x\n", vi01);

	printf("-- TEST END\n");
	return 0;
}
