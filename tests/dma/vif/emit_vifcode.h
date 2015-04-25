#ifndef PS2AUTOTESTS_DMA_VIF_EMIT_VIFCODE_H
#define PS2AUTOTESTS_DMA_VIF_EMIT_VIFCODE_H

#include <common.h>

namespace VIF {
	enum UnpackType {
		UNPACK_TYPE_S32 = 0x60 << 24,
		UNPACK_TYPE_S16 = 0x61 << 24,
		UNPACK_TYPE_S8 = 0x62 << 24,
		UNPACK_TYPE_V2_32 = 0x64 << 24,
		UNPACK_TYPE_V2_16 = 0x65 << 24,
		UNPACK_TYPE_V2_8 = 0x66 << 24,
		UNPACK_TYPE_V3_32 = 0x68 << 24,
		UNPACK_TYPE_V3_16 = 0x69 << 24,
		UNPACK_TYPE_V3_8 = 0x6A << 24,
		UNPACK_TYPE_V4_32 = 0x6C << 24,
		UNPACK_TYPE_V4_16 = 0x6D << 24,
		UNPACK_TYPE_V4_8 = 0x6E << 24,
		UNPACK_TYPE_V4_5_5_5_1 = 0x6F << 24,
	};

	enum CmdFlags {
		CMD_NORMAL = 0,
		CMD_ENABLE_INT = 0x80 << 24,
	};

	enum UnpackFlags {
		UNPACK_NORMAL = 0,
		UNPACK_VIF1_USE_TOPS = 1 << 15,
		UNPACK_ZERO_EXTEND = 1 << 14,
		UNPACK_ENABLE_MASKS = 0x10 << 24,
		UNPACK_ENABLE_INT = 0x80 << 24,
	};

	static inline UnpackFlags operator | (const UnpackFlags &lhs, const UnpackFlags &rhs) {
		return UnpackFlags((u32)lhs | (u32)rhs);
	}


	class Packet {
	public:
		Packet(u32 size);
		~Packet();

		void Reset();

		void NOP(u16 imm = 0, u8 num = 0, CmdFlags flags = CMD_NORMAL);
		void STCYCL(u8 cl, u8 wl, CmdFlags flags = CMD_NORMAL);
		// VIF1 only.
		void OFFSET(u16 off, CmdFlags flags = CMD_NORMAL);
		// VIF1 only.
		void BASE(u16 off, CmdFlags flags = CMD_NORMAL);
		void ITOP(u16 addr, CmdFlags flags = CMD_NORMAL);
		void STMOD(u8 mode, CmdFlags flags = CMD_NORMAL);
		// VIF1 only.
		void MSKPATH3(bool masked, CmdFlags flags = CMD_NORMAL);
		void MARK(u16 mark, CmdFlags flags = CMD_NORMAL);
		void FLUSHE(CmdFlags flags = CMD_NORMAL);
		// VIF1 only.  Waits also for GIF stuff?
		void FLUSH(CmdFlags flags = CMD_NORMAL);
		// VIF1 only.  Waits also for GIF stuff?
		void FLUSHA(CmdFlags flags = CMD_NORMAL);
		// Note that the address should be divided by 8 (like VU branches.)
		void MSCAL(u16 addr, CmdFlags flags = CMD_NORMAL);
		// VIF1 only? Same as MSCAL but waits for GIF first?
		void MSCALF(u16 addr, CmdFlags flags = CMD_NORMAL);
		// Resume execution after next / recent E bit.
		void MSCNT(u16 addr, CmdFlags flags = CMD_NORMAL);
		void STMASK(u32 mask, CmdFlags flags = CMD_NORMAL);
		void STROW(u32 r0, u32 r1, u32 r2, u32 r3, CmdFlags flags = CMD_NORMAL);
		void STCOL(u32 c0, u32 c1, u32 c2, u32 c3, CmdFlags flags = CMD_NORMAL);
		// Note that the address should be divided by 8 (like VU branches.)
		void MPG(u16 addr, u16 size, CmdFlags flags = CMD_NORMAL);
		// VIF1 only.  Number of qwords to transfer.
		void DIRECT(u32 size, CmdFlags flags = CMD_NORMAL);
		// VIF1 only.  Number of qwords to transfer.
		void DIRECTHL(u32 size, CmdFlags flags = CMD_NORMAL);
		void UNPACK(UnpackType cmd, u16 size, u16 addr, UnpackFlags flags = UNPACK_NORMAL);

		void Data32(u32 v);
		void DataPtr(const void *p, u32 size);

		void *Raw() {
			return start_;
		}

	protected:
		void Emit(u32 v) {
			*addr_++ = v;
		}

		u32 *start_;
		u32 *addr_;
	};
}

#endif
