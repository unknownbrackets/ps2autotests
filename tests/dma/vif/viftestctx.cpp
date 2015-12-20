#include "viftestctx.h"

TEST_CONTEXT testContext0 = { (u8*)0x11004000, VIF::VIF0, VIF::VIF0_FIFO, DMA::D0 };
TEST_CONTEXT testContext1 = { (u8*)0x1100C000, VIF::VIF1, VIF::VIF1_FIFO, DMA::D1 };
