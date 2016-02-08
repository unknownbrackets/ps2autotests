#ifndef PS2AUTOTESTS_GS_GSREGS_H
#define PS2AUTOTESTS_GS_GSREGS_H

#include <common-ee.h>

namespace GS
{
	enum RegCSRBits {
		CSR_SIGNAL = 1 << 0,
	};
	
	enum REGS
	{
		REG_SIGNAL = 0x60,
		REG_LABEL = 0x62
	};

	u64 SIGNAL(u32 id, u32 idmsk);
	u64 LABEL(u32 id, u32 idmsk);
	
	static volatile u64 *const CSR = (volatile u64 *)0x12001000;
	static volatile u64 *const SIGLBLID = (volatile u64 *)0x12001080;
}

#endif
