#include <common-ee.h>
#include <string.h>

void __attribute__((noinline)) printfcrs(const char *title, int all) {
	unsigned int fcrs[32];
	memset(fcrs, 0xCC, sizeof(fcrs));

#define ASM_READ_FCR(R) \
		"lui $t0, 0\n" \
		"cfc1 $t0, $" #R "\n" \
		"nop\n" \
		"sw $t0, 4 * " #R "(%0)\n"

	asm volatile (
		".set noreorder\n"

		ASM_READ_FCR(0)
		ASM_READ_FCR(1)
		ASM_READ_FCR(2)
		ASM_READ_FCR(3)
		ASM_READ_FCR(4)
		ASM_READ_FCR(5)
		ASM_READ_FCR(6)
		ASM_READ_FCR(7)
		ASM_READ_FCR(8)
		ASM_READ_FCR(9)
		ASM_READ_FCR(10)
		ASM_READ_FCR(11)
		ASM_READ_FCR(12)
		ASM_READ_FCR(13)
		ASM_READ_FCR(14)
		ASM_READ_FCR(15)
		ASM_READ_FCR(16)
		ASM_READ_FCR(17)
		ASM_READ_FCR(18)
		ASM_READ_FCR(19)
		ASM_READ_FCR(20)
		ASM_READ_FCR(21)
		ASM_READ_FCR(22)
		ASM_READ_FCR(23)
		ASM_READ_FCR(24)
		ASM_READ_FCR(25)
		ASM_READ_FCR(26)
		ASM_READ_FCR(27)
		ASM_READ_FCR(28)
		ASM_READ_FCR(29)
		ASM_READ_FCR(30)
		ASM_READ_FCR(31)

		".set    reorder\n"
		: : "r"(fcrs) : "t0"
	);

	printf("%s:\n  ", title);
	if (all) {
		int i;
		for (i = 0; i < 31; i++) {
			printf("fcr%d: %08x, ", i, fcrs[i]);
		}
	} else {
		printf("fcr0: %08x, ", fcrs[0]);
	}
	printf("fcr31: %08x\n", fcrs[31]);
}

void __attribute__((noinline)) setfcr0(u32 v) {
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"ctc1    %0, $0\n"
		"nop\n"

		".set    reorder\n"
		".set    macro\n"
		: : "r"(v)
	);
}

void __attribute__((noinline)) updatefcr31(u32 v) {
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"or      $v0, $0, %0\n"
		"ctc1    $v0, $31\n"
		"nop\n"

		".set    reorder\n"
		".set    macro\n"
		: : "r"(v) : "v0"
	);
}

static float sqrtneg() {
	float res;
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"lui     $v0, 0xBF80\n" // 0xBF800000 = -1
		"mtc1    $v0, %0\n"
		"nop\n"
		"sqrt.s  %0, %0\n"

		".set    reorder\n"
		".set    macro\n"
		: "=f"(res) : : "v0"
	);
	return res;
}

static float zerodivzero() {
	float res;
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"mtc1    $0, %0\n"
		"nop\n"
		"div.s   %0, %0, %0\n"

		".set    reorder\n"
		".set    macro\n"
		: "=f"(res)
	);
	return res;
}

static float numdivzero() {
	float res;
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"lui     $v0, 0x3F80\n" // 0x3F800000 = 1
		"mtc1    $v0, $f1\n"
		"mtc1    $0, %0\n"
		"nop\n"
		"div.s   %0, $f1, %0\n"

		".set    reorder\n"
		".set    macro\n"
		: "=f"(res) : : "$f1"
	);
	return res;
}

static float nanmath() {
	float res;
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"lui     $v0, 0x7F80\n" // 0x7F800001 = NAN
		"ori     $v0, 0x0001\n"
		"mtc1    $v0, %0\n"
		"nop\n"
		"add.s   %0, %0, %0\n"

		".set    reorder\n"
		".set    macro\n"
		: "=f"(res) : : "v0"
	);
	return res;
}

static float fltoverflow() {
	float res;
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"lui     $v0, 0x7F7F\n" // 0x7F7FFFFF = FLT_MAX
		"ori     $v0, 0xFFFF\n"
		"mtc1    $v0, %0\n"
		"nop\n"
		"mul.s   %0, %0, %0\n"

		".set    reorder\n"
		".set    macro\n"
		: "=f"(res) : : "v0"
	);
	return res;
}

static float fltunderflow() {
	float res;
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"lui     $v0, 0x0080\n" // 0x00800000 = FLT_MIN
		"mtc1    $v0, $f1\n"
		"lui     $v0, 0x4040\n" // 0x40400000 = 3
		"mtc1    $v0, %0\n"
		"nop\n"
		"div.s   %0, $f1, %0\n"

		".set    reorder\n"
		".set    macro\n"
		: "=f"(res) : : "v0", "$f1"
	);
	return res;
}

static float fltinexact() {
	float res;
	asm volatile (
		".set    noreorder\n"
		".set    nomacro\n"

		"lui     $v0, 0x3F80\n" // 0x3F800000 = 1
		"mtc1    $v0, $f1\n"
		"lui     $v0, 0x4040\n" // 0x40400000 = 3
		"ori     $v0, 0xFFFF\n"
		"mtc1    $v0, %0\n"
		"nop\n"
		"div.s   %0, $f1, %0\n"

		".set    reorder\n"
		".set    macro\n"
		: "=f"(res) : : "v0", "$f1"
	);
	return res;
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	printfcrs("Initial", 1);

	setfcr0(0xDEADBEEF);
	printfcrs("Update fcr0", 0);
	updatefcr31(0x00000003);
	printfcrs("Update rounding mode (RM)", 0);
	updatefcr31(0x0000007C);
	printfcrs("Update flags", 0);
	printfcrs("All regs", 1);
	updatefcr31(0x00000F80);
	printfcrs("Update enables", 0);
	updatefcr31(0x0001F000);
	printfcrs("Update cause", 0);
	updatefcr31(0x00020000);
	printfcrs("Update unimplemented (E)", 0);
	updatefcr31(0x01000000);
	printfcrs("Update flushing (FS)", 0);
	updatefcr31(0x00400000);
	printfcrs("Update flushing (FO)", 0);
	updatefcr31(0x00200000);
	printfcrs("Update flushing (FN)", 0);
	updatefcr31(0x00800000);
	printfcrs("Update FCC", 0);
	updatefcr31(0xFE000000);
	printfcrs("Update FCC1-7", 0);
	updatefcr31(0x001C0000);
	printfcrs("Update unknown", 0);

	printf("\nFlag situations:\n");

	float f1;

	updatefcr31(0x00000000);
	f1 = sqrtneg();
	printfcrs("sqrt(-1)", 0);
	printf("  result = %f\n", f1);

	updatefcr31(0x00000000);
	f1 = zerodivzero();
	printfcrs("Divide zero by zero", 0);
	printf("  result = %f\n", f1);

	updatefcr31(0x00000000);
	f1 = numdivzero();
	printfcrs("Divide one by zero", 0);
	printf("  result = %f\n", f1);

	updatefcr31(0x00000000);
	f1 = nanmath();
	printfcrs("NAN math", 0);
	printf("  result = %f\n", f1);

	updatefcr31(0x00000000);
	f1 = fltoverflow();
	printfcrs("Overflow", 0);
	printf("  result = %f\n", f1);

	updatefcr31(0x00000000);
	// Can't seem to make underflow happen.
	f1 = fltunderflow();
	printfcrs("Underflow", 0);
	printf("  result = %f\n", f1);

	updatefcr31(0x00000000);
	f1 = fltinexact();
	printfcrs("Inexact", 0);
	printf("  result = %f\n", f1);

	printf("-- TEST END\n");

	return 0;
}
