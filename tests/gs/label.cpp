#include <common-ee.h>
#include "../dma/dmaregs.h"
#include "../dma/dmasend.h"
#include "emit_giftag.h"
#include "gsregs.h"

void testSetBits() {
	*GS::SIGLBLID = 0x0000000076543210ULL;
	
	GIF::Tag tag;
	tag.SetLoops(1);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(64);
	packet.WriteTag(tag);
	packet.AD(GS::REG_LABEL, GS::LABEL(0xF0, 0xF0));
	
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());

	printf("Set label bits -> SIGLBLID: %016lx\n", *GS::SIGLBLID);
}

void testClearBits() {
	*GS::SIGLBLID = 0x3333333333333333ULL;
	
	GIF::Tag tag;
	tag.SetLoops(1);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(64);
	packet.WriteTag(tag);
	packet.AD(GS::REG_LABEL, GS::LABEL(0x00, 0xF0));
	
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());

	printf("Clear label bits -> SIGLBLID: %016lx\n", *GS::SIGLBLID);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	
	testSetBits();
	testClearBits();
	
	printf("-- TEST END\n");
	return 0;
}
