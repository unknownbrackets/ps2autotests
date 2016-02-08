#include <common-ee.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "emit_giftag.h"

namespace GIF {
	Tag::Tag() {
		tag_.raw.v0 = 0;
		tag_.raw.v1 = 0;
	}
	
	void Tag::SetLoops(u16 nloop) {
		tag_.typed.nloop = nloop;
	}
	
	void Tag::SetEop(bool eop) {
		tag_.typed.eop = eop ? 1 : 0;
	}
	
	void Tag::SetFormat(DataFormat fmt) {
		tag_.typed.flg = fmt;
	}
	
	void Tag::SetRegDescs(
		RegisterDescriptor r0,  RegisterDescriptor r1,  RegisterDescriptor r2,  RegisterDescriptor r3,
		RegisterDescriptor r4,  RegisterDescriptor r5,  RegisterDescriptor r6,  RegisterDescriptor r7,
		RegisterDescriptor r8,  RegisterDescriptor r9,  RegisterDescriptor r10, RegisterDescriptor r11,
		RegisterDescriptor r12, RegisterDescriptor r13, RegisterDescriptor r14, RegisterDescriptor r15
	) {
		u32 regCount = 0;
		s32 regDescs[16];
#define ASSIGN_REG(rid) regDescs[rid] = r##rid;
		ASSIGN_REG(0);  ASSIGN_REG(1);  ASSIGN_REG(2);  ASSIGN_REG(3);
		ASSIGN_REG(4);  ASSIGN_REG(5);  ASSIGN_REG(6);  ASSIGN_REG(7);
		ASSIGN_REG(8);  ASSIGN_REG(9);  ASSIGN_REG(10); ASSIGN_REG(11);
		ASSIGN_REG(12); ASSIGN_REG(13); ASSIGN_REG(14); ASSIGN_REG(15);
#undef ASSIGN_REG
		tag_.typed.regs = 0;
		for(u32 i = 0; i < 16; i++) {
			s32 regDesc = regDescs[i];
			if(regDesc == REG_INVALID) break;
			tag_.typed.regs |= (regDesc & 0xF) << (i * 4);
			regCount++;
		}
		regCount &= 0xF;
		tag_.typed.nreg = regCount;
	}
	
	Packet::Packet(u32 size) {
		start_ = (u64 *)memalign(16, size);
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

	void Packet::WriteTag(const Tag& tag) {
		Emit(tag.tag_.raw.v0);
		Emit(tag.tag_.raw.v1);
	}

	PackedPacket::PackedPacket(u32 size) 
	: Packet(size) {
		
	}

	void PackedPacket::WriteTag(const Tag& tag) {
		Tag copyTag = tag;
		copyTag.SetFormat(FORMAT_PACKED);
		Packet::WriteTag(copyTag);
	}
	
	void PackedPacket::AD(u8 reg, u64 value) {
		Emit(value);
		Emit(reg);
	}
}
