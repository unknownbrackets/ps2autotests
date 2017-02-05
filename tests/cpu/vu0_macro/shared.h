#pragma once

#include <common-ee.h>

static inline void PRINT_R(const register u128 &rt, bool newline) {
	static u32 __attribute__((aligned(16))) result[4] = {0, 0, 0, 0};
	*(vu128 *)result = rt;

	printf("%08x %08x %08x %08x", result[3], result[2], result[1], result[0]);
	if (newline) {
		printf("\n");
	}
}

struct Vu0Flags {
	Vu0Flags();

	void PrintChanges(bool newline = false);

	u32 status_;
	u32 mac_;
	u32 clipping_;
};
