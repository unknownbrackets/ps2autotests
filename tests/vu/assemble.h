#ifndef PS2AUTOTESTS_VU_ASSEMBLE_H
#define PS2AUTOTESTS_VU_ASSEMBLE_H

#include "assemble/types.h"
#include "assemble/block.h"
#include "assemble/ops.h"

static VU::LIW *const vu0_micro = (VU::LIW *)0x11000000;
static const u32 vu0_micro_size = 0x00001000;
static VU::LIW *const vu1_micro = (VU::LIW *)0x11008000;
static const u32 vu1_micro_size = 0x00004000;

static u8 *const vu0_mem = (u8 *)0x11004000;
static const u32 vu0_mem_size = 0x1000;
static u8 *const vu1_mem = (u8 *)0x1100C000;
static const u32 vu1_mem_size = 0x4000;

#endif
