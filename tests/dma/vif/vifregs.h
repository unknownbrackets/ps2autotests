#ifndef PS2AUTOTESTS_DMA_VIF_VIFREGS_H
#define PS2AUTOTESTS_DMA_VIF_VIFREGS_H

#include <common.h>
#include "../dmaregs.h"

namespace VIF {

#define BITWISE_ENUM(Enum) \
	static inline Enum operator | (const Enum &lhs, const Enum &rhs) { \
		return Enum((u32)lhs | (u32)rhs); \
	} \
	static inline Enum operator ~ (const Enum &rhs) { \
		return Enum(~(u32)rhs); \
	}

	enum RegSTATBits {
		STAT_VPS_MASK = 3 << 0,
		STAT_VPS_IDLE = 0 << 0,
		STAT_VPS_DATAWAIT = 1 << 0,
		STAT_VPS_DECODE = 2 << 0,
		STAT_VPS_TRANSFER = 3 << 0,

		STAT_VEW = 1 << 2,
		// VIF1 only.
		STAT_VGW = 1 << 3,
		STAT_MRK = 1 << 6,
		// VIF1 only.
		STAT_DBF = 1 << 7,
		STAT_VSS = 1 << 8,
		STAT_VFS = 1 << 9,
		STAT_VIS = 1 << 10,
		STAT_INT = 1 << 11,
		STAT_ER0 = 1 << 12,
		STAT_ER1 = 1 << 13,
		// VIF1 only.
		STAT_FDR = 1 << 23,

		STAT_FQC_SHIFT = 24,
	};
	BITWISE_ENUM(RegSTATBits)

	enum RegFBRSTBits {
		FBRST_RST = 1 << 0,
		FBRST_FBK = 1 << 1,
		FBRST_STP = 1 << 2,
		FBRST_STC = 1 << 3,
	};
	BITWISE_ENUM(RegFBRSTBits)

	enum RegERRBits {
		ERR_MII = 1 << 0,
		ERR_ME0 = 1 << 1,
		ERR_ME1 = 1 << 2,
	};
	BITWISE_ENUM(RegERRBits)

	typedef DMA::Reg<RegSTATBits> RegSTAT;
	typedef DMA::Reg<RegFBRSTBits> RegFBRST;
	typedef DMA::Reg<RegERRBits> RegERR;

	struct VIFRegs {
		RegSTAT stat;
		u32 stat_pad[3];
		RegFBRST fbrst;
		u32 fbrst_pad[3];
		RegERR err;
		u32 err_pad[3];
		u32 mrk;
		u32 mrk_pad[3];

		u32 cycle;
		u32 cycle_pad[3];
		u32 mode;
		u32 mode_pad[3];
		u32 num;
		u32 num_pad[3];
		u32 mask;
		u32 mask_pad[3];
		u32 code;
		u32 code_pad[3];
		u32 itops;
		u32 itops_pad[3];
		// VIF1 only.
		u32 base;
		u32 base_pad[3];
		// VIF1 only.
		u32 ofst;
		u32 ofst_pad[3];
		// VIF1 only.
		u32 tops;
		u32 tops_pad[3];
		u32 itop;
		u32 itop_pad[3];
		u32 top;
		u32 top_pad[3];
		u32 pad2[4];
		u32 r0;
		u32 r0_pad[3];
		u32 r1;
		u32 r1_pad[3];
		u32 r2;
		u32 r2_pad[3];
		u32 r3;
		u32 r3_pad[3];
		u32 c0;
		u32 c0_pad[3];
		u32 c1;
		u32 c1_pad[3];
		u32 c2;
		u32 c2_pad[3];
		u32 c3;
		u32 c3_pad[3];
	};

	static volatile VIFRegs *const VIF0 = (volatile VIFRegs *)0x10003800;
	static volatile VIFRegs *const VIF1 = (volatile VIFRegs *)0x10003C00;

	static volatile u128 *const VIF0_FIFO = (volatile u128 *)0x10004000;
	static volatile u128 *const VIF1_FIFO = (volatile u128 *)0x10005000;

#undef BITWISE_ENUM
}

#endif
