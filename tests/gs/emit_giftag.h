#ifndef PS2AUTOTESTS_GS_EMIT_GIFTAG_H
#define PS2AUTOTESTS_GS_EMIT_GIFTAG_H

#include <common-ee.h>

namespace GIF {
	
	enum RegisterDescriptor {
		REG_INVALID = -1,
		REG_PRIM    = 0x0,
		REG_RGBAQ   = 0x1,
		REG_ST      = 0x2,
		REG_UV      = 0x3,
		REG_XYZF2   = 0x4,
		REG_XYZ2    = 0x5,
		REG_TEX0_1  = 0x6,
		REG_TEX0_2  = 0x7,
		REG_CLAMP_1 = 0x8,
		REG_CLAMP_2 = 0x9,
		REG_FOG     = 0xA,
		REG_XYZF3   = 0xC,
		REG_XYZ3    = 0xD,
		REG_AD      = 0xE,
		REG_NOP     = 0xF
	};
	
	enum DataFormat {
		FORMAT_PACKED,
		FORMAT_REGLIST,
		FORMAT_IMAGE,
		FORMAT_IMAGE_3,
	};
	
	class Tag {
	public:
		Tag();
		
		void SetLoops(u16 nloop);
		void SetEop(bool eop = true);
		void SetPrimitive(u8 prim);
		void SetFormat(DataFormat fmt);
		void SetRegDescs(
			RegisterDescriptor r0  = REG_INVALID, RegisterDescriptor r1  = REG_INVALID, RegisterDescriptor r2  = REG_INVALID, RegisterDescriptor r3  = REG_INVALID,
			RegisterDescriptor r4  = REG_INVALID, RegisterDescriptor r5  = REG_INVALID, RegisterDescriptor r6  = REG_INVALID, RegisterDescriptor r7  = REG_INVALID,
			RegisterDescriptor r8  = REG_INVALID, RegisterDescriptor r9  = REG_INVALID, RegisterDescriptor r10 = REG_INVALID, RegisterDescriptor r11 = REG_INVALID,
			RegisterDescriptor r12 = REG_INVALID, RegisterDescriptor r13 = REG_INVALID, RegisterDescriptor r14 = REG_INVALID, RegisterDescriptor r15 = REG_INVALID
		);
		
		union {
			struct {
				u64 v0;
				u64 v1;
			} raw;
			struct {
				u32 nloop    : 15;
				u32 eop      : 1;
				u32 padding0 : 16;
				u32 padding1 : 14;
				u32 pre      : 1;
				u32 prim     : 11;
				u32 flg      : 2;
				u32 nreg     : 4;
				u64 regs;
			} typed;
		} tag_;
	};
	
	class Packet {
	public:
		Packet(u32 size);
		virtual ~Packet();

		void Reset();
		
		virtual void WriteTag(const Tag&);

		void *Raw() {
			return start_;
		}
		
		u32 Size() {
			return (u32)(addr_ - start_) * 8;
		}

		u32 Qwc() {
			return (Size() + 0xF) / 0x10;
		}

		void Emit(u64 v) {
			*addr_++ = v;
		}

	protected:
		u64 *start_;
		u64 *addr_;
	};

	class PackedPacket : public Packet {
	public:
		PackedPacket(u32 size);
		
		virtual void WriteTag(const Tag&);
		void AD(u8 reg, u64 value);
	};
}

#endif
