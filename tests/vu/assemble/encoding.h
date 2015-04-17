#include "types.h"

namespace VU {

// TODO: <assert.h> doesn't link?
#define assert(cond) \
	if (!(cond)) { \
		printf("Assert failed: %s", #cond); \
		asm ("break 100"); \
	}

static inline Reg VI(Reg i) {
	assert(i >= VI00 && i <= VI15);
	return Reg(i - 32);
}

static inline u32 VD(Reg v) {
	assert(v >= VF00 && v <= VF31);
	return v << 6;
}

static inline u32 VS(Reg v) {
	assert(v >= VF00 && v <= VF31);
	return v << 11;
}

static inline u32 VT(Reg v) {
	assert(v >= VF00 && v <= VF31);
	return v << 16;
}

static inline u32 DEST(Dest dest) {
	return dest << 21;
}

static inline u32 BC(Field f) {
	return f;
}

static inline u32 FSF(Field f) {
	return f << 21;
}

static inline u32 FTF(Field f) {
	return f << 23;
}

static inline u32 IMM5(u8 imm) {
	assert((imm & 0x1F) == imm);
	return imm << 6;
}

static inline u32 IMM11(u16 imm) {
	assert((imm & 0x07FF) == imm);
	return imm;
}

static inline u32 IMM15(u16 imm) {
	assert((imm & 0x7FFF) == imm);
	// The second half is at position 21 (10 right of the 11 we zero out.)
	return (imm & 0x07FF) | ((imm & 0x7800) << 10);
}

static inline u32 IMM24(u32 imm) {
	assert((imm & 0x00FFFFFF) == imm);
	return imm;
}

#undef assert

}
