#pragma once

#include <common-ee.h>

struct Vu0Flags {
	Vu0Flags();

	void PrintChanges(bool newline = false);

	u32 status_;
	u32 mac_;
	u32 clipping_;
};
