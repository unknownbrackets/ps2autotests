#include <common-ee.h>
#include "../dma/dmaregs.h"
#include "../dma/dmasend.h"
#include "emit_giftag.h"
#include "gsregs.h"

void testSetBits() {
	//Signal must be cleared to unstall GIF
	*GS::CSR = GS::CSR_SIGNAL;
	*GS::SIGLBLID = 0x7654321000000000ULL;
	
	GIF::Tag tag;
	tag.SetLoops(1);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(64);
	packet.WriteTag(tag);
	packet.AD(GS::REG_SIGNAL, GS::SIGNAL(0xF0, 0xF0));
	
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());

	printf("Set signal bits -> CSR[SIGNAL] = %016lx, SIGLBLID: %016lx\n", 
		*GS::CSR & GS::CSR_SIGNAL, *GS::SIGLBLID);
}

void testClearBits() {
	//Signal must be cleared to unstall GIF
	*GS::CSR = GS::CSR_SIGNAL;
	*GS::SIGLBLID = 0x3333333333333333ULL;
	
	GIF::Tag tag;
	tag.SetLoops(1);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);

	GIF::PackedPacket packet(64);
	packet.WriteTag(tag);
	packet.AD(GS::REG_SIGNAL, GS::SIGNAL(0x00, 0xF0));
	
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());

	printf("Clear signal bits -> CSR[SIGNAL] = %016lx, SIGLBLID: %016lx\n", 
		*GS::CSR & GS::CSR_SIGNAL, *GS::SIGLBLID);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	testSetBits();
	testClearBits();
	
	printf("-- TEST END\n");
	return 0;
}
