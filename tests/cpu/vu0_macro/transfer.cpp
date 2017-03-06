#include <stdio.h>
#include <string.h>
#include "shared.h"

static u32 __attribute__ ((aligned(16))) CF_PATTERN[3][4] = {
	{0x45678123, 0x9ABCDEF0, 0xDEADBEEF, 0xC0DE1337},
	{0x23456789, 0xABCDEF01, 0xBEEFDEAD, 0xC0DEC0DE},
	{0x8899AABB, 0xCCDDEEFF, 0x00112233, 0x44556677},
};

static u128 __attribute__ ((aligned(16))) WRITE_BUFFER[3];

#define LOADF_OP_FUNC(OP) \
template <int offset> \
inline void LOADF_##OP(register u128 &res, void *address) { \
	asm volatile ( \
		"qmtc2 %0, $vf1\n" \
		#OP " $vf1, %2(%1)\n" \
		"qmfc2 %0, $vf1\n" \
		"sync\n" \
		: "+&r"(res) : "r"(address), "i"(offset) \
	); \
}

#define STOREF_OP_FUNC(OP) \
template <int offset> \
inline void STOREF_##OP(const register u128 &val, void *address) { \
	asm volatile ( \
		"qmtc2 %0, $vf1\n" \
		#OP " $vf1, %2(%1)\n" \
		"sync\n" \
		: : "r"(val), "r"(address), "i"(offset) \
	); \
}

#define SET_U128(f, t) \
	f = (t); \
	asm volatile ( \
		"dsll32 $t6, %0, 0\n" \
		"or $t6, $t6, %0\n" \
		"pcpyld %0, $t6, $t6\n" \
		: "+&r"(f) : : "t6" \
	);

#define TEST_CTC2(reg) \
	do { \
		register u32 x__printvi; \
		asm volatile ( \
			"addiu $t0, $0, -1\n" \
			"vnop\n" \
			"ctc2 $t0, " #reg "\n" \
			"vnop\n" \
			"cfc2 %0, " #reg "\n" \
			"sync\n" \
			: "=r"(x__printvi) : : "t0" \
		); \
		printf("ctc2 -> " #reg ": %08x\n", x__printvi); \
	} while (false)

void test_cfc2() {
	register u128 vi21 = 0;
	
	//Prepare tests (move ~0 in vi21)
	asm volatile (
		"addiu $t0, $0, -1\n"
		"vnop\n"
		"ctc2 $t0, vi21\n"
		: : : "t0"
	);
	
	SET_U128(vi21, 0x12345678);
	asm volatile (
		"vnop\n"
		"cfc2 %0, vi21\n"
		"sync\n"
		: "+&r"(vi21)
	);

	printf("cfc2: ");
	PRINT_R(vi21, true);
	
	asm volatile (
		"vnop\n"
		"cfc2 $0, vi21\n"
		"sync\n"
		"por %0, $0, $0\n"
		: "+&r"(vi21)
	);
	
	printf("cfc2 -> $0: ");
	PRINT_R(vi21, true);
}

void test_ctc2() {
	TEST_CTC2(vi00);
	TEST_CTC2(vi01);
	//Assuming the same for vi02 -> vi15
	
	TEST_CTC2(vi16);    //Status flag
	TEST_CTC2(vi17);    //MAC flag
	TEST_CTC2(vi18);    //Clipping flag
	TEST_CTC2(vi19);    //Reserved
	TEST_CTC2(vi20);    //R
	TEST_CTC2(vi21);    //I
	TEST_CTC2(vi22);    //Q
	TEST_CTC2(vi23);    //Reserved
	TEST_CTC2(vi24);    //Reserved
	TEST_CTC2(vi25);    //Reserved 
	//TEST_CTC2(vi26);    //TPC - Not printed because it always changes
	TEST_CTC2(vi27);    //CMSAR0
	TEST_CTC2(vi28);    //FBRST
	TEST_CTC2(vi29);    //VPU-STAT
	//TEST_CTC2(vi30);    //Reserved - Not printed because it always changes
	TEST_CTC2(vi31);    //CMSAR1
}

void test_qmfc2() {
	register u128 vf = 0;
	
	SET_U128(vf, 0x12345678);
	asm volatile (
		"vnop\n"
		"qmfc2 %0, vf0\n"
		"sync\n"
		: "+&r"(vf)
	);
	
	printf("qmfc2: ");
	PRINT_R(vf, true);
	
	asm volatile (
		"vnop\n"
		"qmfc2 $0, vf0\n"
		"sync\n"
		"por %0, $0, $0\n"
		: "+&r"(vf)
	);
	
	printf("qmfc2 -> $0: ");
	PRINT_R(vf, true);
}

void test_qmtc2() {
	register u128 vf = 0;
	
	SET_U128(vf, 0x12345678);
	asm volatile (
		"vnop\n"
		"qmtc2 %0, $vf1\n"
		"vnop\n"
		"qmfc2 %0, $vf1\n"
		"sync\n"
		: "+&r"(vf)
	);

	printf("qmtc2: ");
	PRINT_R(vf, true);
	
	SET_U128(vf, 0x12345678);
	asm volatile (
		"vnop\n"
		"qmtc2 %0, $vf0\n"
		"vnop\n"
		"qmfc2 %0, $vf0\n"
		"sync\n"
		: "+&r"(vf)
	);
	
	printf("qmtc2 -> $vf0: ");
	PRINT_R(vf, true);
}

LOADF_OP_FUNC(lqc2)
STOREF_OP_FUNC(sqc2)

#define LOADF_OP_DO_I(OP, t, s) \
	{ \
		register u128 vf; \
		SET_U128(vf, t); \
		LOADF_##OP<s>(vf, CF_PATTERN[1]); \
		printf("  %s %+d: ", #OP, (u32)s); PRINT_R(vf, true); \
	}

#define STOREF_OP_DO_I(OP, t, s) \
	{ \
		register u128 vf; \
		memcpy(WRITE_BUFFER, CF_PATTERN, sizeof(WRITE_BUFFER)); \
		SET_U128(vf, t); \
		STOREF_##OP<s>(vf, &WRITE_BUFFER[1]); \
		printf("  %s %+d: ", #OP, (u32)s); PRINT_R(WRITE_BUFFER[1 + (s) / 16], true); \
	}
	
void test_lqc2() {
	printf("lqc2:\n");
	
	LOADF_OP_DO_I(lqc2, 0x01234567, 0)
	LOADF_OP_DO_I(lqc2, 0x01234567, 16)
	LOADF_OP_DO_I(lqc2, 0x01234567, -16)
	
	{
		const void* address = CF_PATTERN;
		register u128 vf;
		asm volatile (
			"lqc2 $vf0, 0(%1)\n"
			"vnop\n"
			"qmfc2 %0, $vf0\n"
			"sync\n"
			: "+&r"(vf) : "r"(address)
		);
		
		printf("  lqc2 -> $vf0: ");
		PRINT_R(vf, true);
	}
	
	printf("\n");
}

void test_sqc2() {
	printf("sqc2:\n");
	
	STOREF_OP_DO_I(sqc2, 0x01234567, 0)
	STOREF_OP_DO_I(sqc2, 0x89ABCDEF, 16)
	STOREF_OP_DO_I(sqc2, 0x76543210, -16)
	
	printf("\n");
}

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	test_lqc2();
	test_sqc2();
	
	test_cfc2();
	test_ctc2();
	
	test_qmfc2();
	test_qmtc2();
	
	printf("-- TEST END\n");
	
	return 0;
}
