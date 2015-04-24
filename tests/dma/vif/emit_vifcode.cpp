#include <common.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "emit_vifcode.h"

namespace VIF {
	enum CmdType {
		CMD_NOP = 0x00 << 24,
		CMD_STCYCL = 0x01 << 24,
		CMD_OFFSET = 0x02 << 24,
		CMD_BASE = 0x03 << 24,
		CMD_ITOP = 0x04 << 24,
		CMD_STMOD = 0x05 << 24,
		CMD_MSKPATH3 = 0x06 << 24,
		CMD_MARK = 0x07 << 24,
		CMD_FLUSHE = 0x10 << 24,
		CMD_FLUSH = 0x11 << 24,
		CMD_FLUSHA = 0x13 << 24,
		CMD_MSCAL = 0x14 << 24,
		CMD_MSCALF = 0x15 << 24,
		CMD_MSCNT = 0x17 << 24,
		CMD_STMASK = 0x20 << 24,
		CMD_STROW = 0x30 << 24,
		CMD_STCOL = 0x31 << 24,
		CMD_MPG = 0x46 << 24,
		CMD_DIRECT = 0x50 << 24,
		CMD_DIRECTHL = 0x51 << 24,
	};

	Packet::Packet(u32 size) {
		start_ = (u32 *)memalign(16, size);
		addr_ = start_;
		memset(start_, 0, size);
	}
	Packet::~Packet() {
		free(start_);
	}

	void Packet::Reset() {
		memset(start_, 0, (addr_ - start_) * sizeof(u32));
		addr_ = start_;
	}

	void Packet::NOP(u16 imm, u8 num, CmdFlags flags) {
		Emit(CMD_NOP | flags | (num << 16) | imm);
	}

	void Packet::STCYCL(u8 cl, u8 wl, CmdFlags flags) {
		Emit(CMD_STCYCL | flags | (wl << 8) | cl);
	}

	void Packet::OFFSET(u16 off, CmdFlags flags) {
		Emit(CMD_OFFSET | flags | off);
	}

	void Packet::BASE(u16 off, CmdFlags flags) {
		Emit(CMD_BASE | flags | off);
	}

	void Packet::ITOP(u16 addr, CmdFlags flags) {
		Emit(CMD_ITOP | flags | addr);
	}

	void Packet::STMOD(u8 mode, CmdFlags flags) {
		Emit(CMD_STMOD | flags | mode);
	}

	void Packet::MSKPATH3(bool masked, CmdFlags flags) {
		Emit(CMD_MSKPATH3 | flags | (masked ? (1 << 15) : 0));
	}

	void Packet::MARK(u16 mark, CmdFlags flags) {
		Emit(CMD_MARK | flags | mark);
	}

	void Packet::FLUSHE(CmdFlags flags) {
		Emit(CMD_FLUSHE | flags);
	}

	void Packet::FLUSH(CmdFlags flags) {
		Emit(CMD_FLUSH | flags);
	}

	void Packet::FLUSHA(CmdFlags flags) {
		Emit(CMD_FLUSHA | flags);
	}

	void Packet::MSCAL(u16 addr, CmdFlags flags) {
		Emit(CMD_MSCAL | flags | addr);
	}

	void Packet::MSCALF(u16 addr, CmdFlags flags) {
		Emit(CMD_MSCALF | flags | addr);
	}

	void Packet::MSCNT(u16 addr, CmdFlags flags) {
		Emit(CMD_MSCNT | flags | addr);
	}

	void Packet::STMASK(u32 mask, CmdFlags flags) {
		Emit(CMD_STMASK | flags);
		Emit(mask);
	}

	void Packet::STROW(u32 r0, u32 r1, u32 r2, u32 r3, CmdFlags flags) {
		Emit(CMD_STROW | flags);
		Emit(r0);
		Emit(r1);
		Emit(r2);
		Emit(r3);
	}

	void Packet::STCOL(u32 c0, u32 c1, u32 c2, u32 c3, CmdFlags flags) {
		Emit(CMD_STCOL | flags);
		Emit(c0);
		Emit(c1);
		Emit(c2);
		Emit(c3);
	}

	void Packet::MPG(u16 addr, u16 size, CmdFlags flags) {
		assert(size != 0 && size <= 256);

		Emit(CMD_MPG | flags | ((size & 0xff) << 16) | addr);
	}

	void Packet::DIRECT(u32 size, CmdFlags flags) {
		assert(size != 0 && size <= 65536);

		Emit(CMD_DIRECT | flags | (size & 0xffff));
	}

	void Packet::DIRECTHL(u32 size, CmdFlags flags) {
		assert(size != 0 && size <= 65536);

		Emit(CMD_DIRECTHL | flags | (size & 0xffff));
	}

	void Packet::UNPACK(UnpackType cmd, u16 size, u16 addr, UnpackFlags flags) {
		// Technically 0x3ff, but we're just verifying the encoding.
		assert((addr & 0x7ff) == addr);
		assert(size != 0 && size <= 256);

		// Size 256 is encoded as 0.
		Emit(cmd | flags | ((size & 0xff) << 16) | addr);
	}

	void Packet::Data32(u32 v) {
		Emit(v);
	}

	void Packet::DataPtr(const void *p, u32 size) {
		assert((size & 3) == 0);
		memcpy(addr_, p, size);
		addr_ += size / 4;
	}
}
