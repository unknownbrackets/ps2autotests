#ifndef PS2AUTOTESTS_DMA_DMATAGS_H
#define PS2AUTOTESTS_DMA_DMATAGS_H

#include <common-ee.h>

namespace DMA {
	enum SrcChainType {
		SRC_REFE = 0,
		SRC_CNT = 1,
		SRC_NEXT = 2,
		SRC_REF = 3,
		SRC_REFS = 4,
		SRC_CALL = 5,
		SRC_RET = 6,
		SRC_END = 7,
	};

	struct SrcChainTag {
		union {
			struct {
				u16 QWC : 16;
				u16 : 10;
				u8 PCE : 2;
				SrcChainType ID : 3;
				u8 IRQ : 1;
			};
			u32 bits;
		};
		void *addr;
		u32 pad[2];
	} __attribute__((aligned(16)));

	class SrcChainPacket {
	public:
		SrcChainPacket(u32 size);
		~SrcChainPacket();

		void Reset();

		void CNT(u16 size);
		void NEXT(u16 size, void *nextdma, u16 nextsize);
		void REF(void *addr, u16 size);
		void REFS(void *addr, u16 size);
		void REFE(void *addr, u16 size);
		void CALL(u16 size, void *nextdma, u16 nextsize);
		void RET(u16 size);
		void END(u16 size);

		void DataQ(u128 v);
		void DataQPtr(const void *p, u16 size);

		void *Raw() {
			return start_;
		}

	protected:
		void Emit(u16 QWC, SrcChainType ID, void *addr);

		SrcChainTag *start_;
		SrcChainTag *addr_;
	};

	enum DstChainType {
		DST_CNTS = 0,
		DST_CNT = 1,
		DST_END = 7,
	};

	struct DstChainTag {
		union {
			struct {
				u16 QWC : 16;
				u16 : 10;
				u8 PCE : 2;
				DstChainType ID : 3;
				u8 IRQ : 1;
			};
			u32 bits;
		};
		void *addr;
		u32 pad[2];
	} __attribute__((aligned(16)));

	class DstChainPacket {
	public:
		DstChainPacket(u32 size);
		~DstChainPacket();

		void Reset();

		void CNT(void *addr, u16 size);
		void CNTS(void *addr, u16 size);
		void END(void *addr, u16 size);

		void DataQ(u128 v);
		void DataQPtr(const void *p, u16 size);

		void *Raw() {
			return start_;
		}

	protected:
		void Emit(u16 QWC, DstChainType ID, void *addr);

		DstChainTag *start_;
		DstChainTag *addr_;
	};
}

#endif
