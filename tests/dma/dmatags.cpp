#include <common.h>
#include <kernel.h>
#include <malloc.h>
#include <string.h>
#include "dmatags.h"

namespace DMA {
	SrcChainPacket::SrcChainPacket(u32 size) {
		start_ = (SrcChainTag *)memalign(16, size);
		addr_ = start_;
		memset(start_, 0, size);
	}
	SrcChainPacket::~SrcChainPacket() {
		free(start_);
	}

	void SrcChainPacket::Reset() {
		memset(start_, 0, (addr_ - start_) * sizeof(SrcChainTag));
		addr_ = start_;
	}

	void SrcChainPacket::CNT(u16 size) {
		Emit(size, SRC_CNT, 0);
	}

	void SrcChainPacket::NEXT(u16 size, void *nextdma, u16 nextsize) {
		if (nextsize != 0) {
			SyncDCache(nextdma, (u8 *)nextdma + nextsize * 16);
		}
		Emit(size, SRC_NEXT, nextdma);
	}

	void SrcChainPacket::REF(void *addr, u16 size) {
		SyncDCache(addr, (u8 *)addr + size * 16);
		Emit(size, SRC_REF, addr);
	}

	void SrcChainPacket::REFS(void *addr, u16 size) {
		SyncDCache(addr, (u8 *)addr + size * 16);
		Emit(size, SRC_REFS, addr);
	}

	void SrcChainPacket::REFE(void *addr, u16 size) {
		SyncDCache(addr, (u8 *)addr + size * 16);
		Emit(size, SRC_REFE, addr);
	}

	void SrcChainPacket::CALL(u16 size, void *nextdma, u16 nextsize) {
		if (nextsize != 0) {
			SyncDCache(nextdma, (u8 *)nextdma + nextsize * 16);
		}
		Emit(size, SRC_CALL, nextdma);
	}

	void SrcChainPacket::RET(u16 size) {
		Emit(size, SRC_RET, 0);
	}

	void SrcChainPacket::END(u16 size) {
		Emit(size, SRC_END, 0);
	}

	void SrcChainPacket::DataQ(u128 v) {
		*(u128 *)addr_ = v;
		addr_++;
	}
	void SrcChainPacket::DataQPtr(const void *p, u16 size) {
		memcpy(addr_, p, size * 16);
		addr_ += size;
	}

	void SrcChainPacket::Emit(u16 QWC, SrcChainType ID, void *addr) {
		addr_->QWC = QWC;
		addr_->ID = ID;
		addr_->addr = addr;
		addr_++;
	}

	DstChainPacket::DstChainPacket(u32 size) {
		start_ = (DstChainTag *)memalign(16, size);
		addr_ = start_;
		memset(start_, 0, size);
	}
	DstChainPacket::~DstChainPacket() {
		free(start_);
	}

	void DstChainPacket::Reset() {
		memset(start_, 0, (addr_ - start_) * sizeof(DstChainTag));
		addr_ = start_;
	}

	void DstChainPacket::CNT(u16 size) {
		Emit(size, DST_CNT, 0);
	}

	void DstChainPacket::CNTS(u16 size) {
		Emit(size, DST_CNTS, 0);
	}

	void DstChainPacket::END(u16 size) {
		Emit(size, DST_END, 0);
	}

	void DstChainPacket::DataQ(u128 v) {
		*(u128 *)addr_ = v;
		addr_++;
	}
	void DstChainPacket::DataQPtr(const void *p, u16 size) {
		memcpy(addr_, p, size * 16);
		addr_ += size;
	}

	void DstChainPacket::Emit(u16 QWC, DstChainType ID, void *addr) {
		addr_->QWC = QWC;
		addr_->ID = ID;
		addr_->addr = addr;
		addr_++;
	}
}
