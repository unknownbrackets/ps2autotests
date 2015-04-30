#ifndef PS2AUTOTESTS_DMA_DMAREGS_H
#define PS2AUTOTESTS_DMA_DMAREGS_H

#include <common.h>

namespace DMA {

#define BITWISE_ENUM(Enum) \
	static inline Enum operator | (const Enum &lhs, const Enum &rhs) { \
		return Enum((u32)lhs | (u32)rhs); \
	} \
	static inline Enum operator ~ (const Enum &rhs) { \
		return Enum(~(u32)rhs); \
	}

	enum RegCTRLBits {
		D_CTRL_DMAE = 1 << 0,
		D_CTRL_RELE = 1 << 1,

		D_CTRL_MFD_VIF1 = 2 << 2,
		D_CTRL_MFD_GIF = 3 << 2,
		D_CTRL_MFD_ANY = 3 << 2,

		D_CTRL_STS_SIF0 = 1 << 4,
		D_CTRL_STS_FROMSPR = 2 << 4,
		D_CTRL_STS_FROMIPU = 3 << 4,
		D_CTRL_STS_ANY = 3 << 4,

		D_CTRL_STD_VIF1 = 1 << 6,
		D_CTRL_STD_GIF = 2 << 6,
		D_CTRL_STD_SIF1 = 3 << 6,
		D_CTRL_STD_ANY = 3 << 6,

		D_CTRL_RCYC_8 = 0 << 8,
		D_CTRL_RCYC_16 = 1 << 8,
		D_CTRL_RCYC_32 = 2 << 8,
		D_CTRL_RCYC_64 = 3 << 8,
		D_CTRL_RCYC_128 = 4 << 8,
		D_CTRL_RCYC_256 = 5 << 8,
		D_CTRL_RCYC_AY = 7 << 8,
	};
	BITWISE_ENUM(RegCTRLBits)

	enum RegSTATBits {
		D_STAT_CIS0 = 1 << 0,
		D_STAT_CIS1 = 1 << 1,
		D_STAT_CIS2 = 1 << 2,
		D_STAT_CIS3 = 1 << 3,
		D_STAT_CIS4 = 1 << 4,
		D_STAT_CIS5 = 1 << 5,
		D_STAT_CIS6 = 1 << 6,
		D_STAT_CIS7 = 1 << 7,
		D_STAT_CIS8 = 1 << 8,
		D_STAT_CIS9 = 1 << 9,

		D_STAT_SIS = 1 << 13,
		D_STAT_MEIS = 1 << 14,
		D_STAT_BEIS = 1 << 15,

		D_STAT_CIM0 = 1 << 16,
		D_STAT_CIM1 = 1 << 17,
		D_STAT_CIM2 = 1 << 18,
		D_STAT_CIM3 = 1 << 19,
		D_STAT_CIM4 = 1 << 20,
		D_STAT_CIM5 = 1 << 21,
		D_STAT_CIM6 = 1 << 22,
		D_STAT_CIM7 = 1 << 23,
		D_STAT_CIM8 = 1 << 24,
		D_STAT_CIM9 = 1 << 25,

		D_STAT_SIM = 1 << 29,
		D_STAT_MEIM = 1 << 30,
	};
	BITWISE_ENUM(RegSTATBits)

	enum RegPCRBits {
		D_PCR_CPC0 = 1 << 0,
		D_PCR_CPC1 = 1 << 1,
		D_PCR_CPC2 = 1 << 2,
		D_PCR_CPC3 = 1 << 3,
		D_PCR_CPC4 = 1 << 4,
		D_PCR_CPC5 = 1 << 5,
		D_PCR_CPC6 = 1 << 6,
		D_PCR_CPC7 = 1 << 7,
		D_PCR_CPC8 = 1 << 8,
		D_PCR_CPC9 = 1 << 9,

		D_PCR_CDE0 = 1 << 16,
		D_PCR_CDE1 = 1 << 17,
		D_PCR_CDE2 = 1 << 18,
		D_PCR_CDE3 = 1 << 19,
		D_PCR_CDE4 = 1 << 20,
		D_PCR_CDE5 = 1 << 21,
		D_PCR_CDE6 = 1 << 22,
		D_PCR_CDE7 = 1 << 23,
		D_PCR_CDE8 = 1 << 24,
		D_PCR_CDE9 = 1 << 25,

		D_PCR_PCE = 1 << 31,
	};
	BITWISE_ENUM(RegPCRBits)

	enum RegENABLERWBits {
		D_ENABLE_CPND = 1 << 16,
	};
	BITWISE_ENUM(RegENABLERWBits)

	enum ChannelRegCHCRBits {
		CHCR_DIR = 1 << 0,

		CHCR_MOD_NORMAL = 0 << 2,
		CHCR_MOD_CHAIN = 1 << 2,
		CHCR_MOD_INTERLEAVE = 2 << 2,
		CHCR_MOD_ANY = 3 << 2,

		CHCR_ASP_1 = 1 << 4,
		CHCR_ASP_2 = 2 << 4,
		CHCR_ASP_ANY = 3 << 4,

		CHCR_TTE = 1 << 6,
		CHCR_TIE = 1 << 7,
		CHCR_STR = 1 << 8,

		CHCR_TAG_MASK = 0xffff << 16,
	};
	BITWISE_ENUM(ChannelRegCHCRBits)

	template <typename RegBits>
	struct Reg {
		volatile u32 bits_;

		void operator = (const RegBits &bits) volatile {
			bits_ = bits;
		}

		void operator |= (const RegBits &bits) volatile {
			bits_ |= bits;
		}

		void operator &= (const RegBits &bits) volatile {
			bits_ &= bits;
		}

		RegBits operator & (const RegBits &bits) volatile const {
			return RegBits(bits_ & (u32)bits);
		}

		RegBits operator | (const RegBits &bits) volatile const {
			return RegBits(bits_ | (u32)bits);
		}
	};

	typedef Reg<RegCTRLBits> RegCTRL;
	typedef Reg<RegSTATBits> RegSTAT;
	typedef Reg<RegPCRBits> RegPCR;
	typedef Reg<RegENABLERWBits> RegENABLERW;

	struct RegSQWC {
		volatile u32 bits_;

		void operator = (const u32 &bits) volatile {
			bits_ = bits;
		}

		void SQWC(u16 bits) {
			bits_  = (bits_ & 0xFFFF0000) | bits;
		}

		void TQWC(u16 bits) {
			bits_  = (bits_ & 0x0000FFFF) | (bits << 16);
		}
	};

	struct ChannelRegCHCR : public Reg<ChannelRegCHCRBits> {
		void operator = (const ChannelRegCHCRBits &bits) volatile {
			bits_ = bits;
		}

		bool Ongoing() volatile {
			return (bits_ & CHCR_STR) != 0;
		}
	};

	struct Channel {
		ChannelRegCHCR chcr;
		u32 chcr_pad[3];
		void *madr;
		u32 madr_pad[3];
		u32 qwc;
		u32 qwc_pad[3];
		void *tadr;
		u32 tadr_pad[3];
		void *asr0;
		u32 asr0_pad[3];
		void *asr1;
		u32 asr1_pad[3];
		u32 pad1[8];
		u32 sadr;
		u32 sadr_pad[3];
		u32 pad2[28];
	};

	static inline void *SPR_ADDR(void *p) {
		return (void *)((u32)p | 0x80000000);
	}

	static volatile Channel *const D0 = (volatile Channel *)0x10008000;
	static volatile Channel *const D1 = (volatile Channel *)0x10009000;
	static volatile Channel *const D2 = (volatile Channel *)0x1000a000;
	static volatile Channel *const D3 = (volatile Channel *)0x1000b000;
	static volatile Channel *const D4 = (volatile Channel *)0x1000b400;
	static volatile Channel *const D5 = (volatile Channel *)0x1000c000;
	static volatile Channel *const D6 = (volatile Channel *)0x1000c400;
	static volatile Channel *const D7 = (volatile Channel *)0x1000c800;
	static volatile Channel *const D8 = (volatile Channel *)0x1000d000;
	static volatile Channel *const D9 = (volatile Channel *)0x1000d400;
	static volatile RegCTRL *const D_CTRL = (volatile RegCTRL *)0x1000e000;
	// Note: special behavior when written to.
	static volatile RegSTAT *const D_STAT = (volatile RegSTAT *)0x1000e010;
	static volatile RegPCR *const D_PCR = (volatile RegPCR *)0x1000e020;
	static volatile RegSQWC *const D_SQWC = (volatile RegSQWC *)0x1000e030;
	static volatile u32 *const D_RBSR = (volatile u32 *)0x1000e040;
	static void *volatile *const D_RBOR = (void *volatile *)0x1000e050;
	static void *volatile *const D_STADR = (void *volatile *)0x1000e060;
	static volatile RegENABLERW *const D_ENABLER = (volatile RegENABLERW *)0x1000f520;
	static volatile RegENABLERW *const D_ENABLEW = (volatile RegENABLERW *)0x1000f590;

#undef BITWISE_ENUM
}

#endif
