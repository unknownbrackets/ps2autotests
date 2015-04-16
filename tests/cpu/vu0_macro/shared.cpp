#include "shared.h"

Vu0Flags::Vu0Flags() {
	// Grab the previous values.
	asm volatile (
		// Try to clear them first, if possible.
		"vnop\n"
		"ctc2 $0, $16\n"
		"ctc2 $0, $17\n"
		"ctc2 $0, $18\n"
		// Then read.
		"vnop\n"
		"cfc2 %0, $16\n"
		"cfc2 %1, $17\n"
		"cfc2 %2, $18\n"
		: "=&r"(status_), "=&r"(mac_), "=&r"(clipping_)
	);

	if (clipping_ != 0) {
		printf("WARNING: Clipping flag was not cleared.\n");
	}
}

void Vu0Flags::PrintChanges(bool newline) {
	u32 status, mac, clipping;

	asm volatile (
		"vnop\n"
		"cfc2 %0, $16\n"
		"cfc2 %1, $17\n"
		"cfc2 %2, $18\n"
		: "=&r"(status), "=&r"(mac), "=&r"(clipping)
	);

	if (status == status_ && mac == mac_ && clipping == clipping_) {
		printf(" (no flag changes)");
	} else {
		printf(" st=+%04x,-%04x, mac=+%04x,-%04x, clip=%08x", status & ~status_, ~status & status_, mac & ~mac_, ~mac & mac_, clipping);
	}
	if (newline) {
		printf("\n");
	}
}
