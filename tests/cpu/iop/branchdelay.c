#include <stdio.h>
#include <tamtypes.h>

void __attribute__((noinline)) test_beq() {
	register int result = -1;

	// This is technically not valid MIPS anyway.
	// But it seems to always take target 2?
	asm volatile (
		".set    noreorder\n"

		"li      $t0, 0\n"
		"beq     $t0, $t0, target1_%=\n"
		"beq     $t0, $t0, target2_%=\n"
		"beq     $t0, $t0, target3_%=\n"
		"beq     $t0, $t0, target4_%=\n"
		"beq     $t0, $t0, target5_%=\n"
		"beq     $t0, $t0, target6_%=\n"
		"nop\n"

		"li      %0, -1\n"
		"j       skip_%=\n"
		"nop\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"li      %0, 2\n"
		"j       skip_%=\n"
		"nop\n"

		"target3_%=:\n"
		"li      %0, 3\n"
		"j       skip_%=\n"
		"nop\n"

		"target4_%=:\n"
		"li      %0, 4\n"
		"j       skip_%=\n"
		"nop\n"

		"target5_%=:\n"
		"li      %0, 5\n"
		"j       skip_%=\n"
		"nop\n"

		"target6_%=:\n"
		"li      %0, 6\n"
		"j       skip_%=\n"
		"nop\n"

		"skip_%=:\n"

		".set    reorder\n"
		: "+r"(result) : : "t0"
	);

	/* This seems to be somewhat determistic, but only for specific code. */
	/*printf("beq: delay branch: %08x\n", result);*/
	printf("beq: delay branch did not crash\n");
}

void __attribute__((noinline)) test_jal() {
	register int result = -1;

	asm volatile (
		".set    noreorder\n"

		"move    $t2, $ra\n"
		"li      $t0, 0\n"
		"jal     target2_%=\n"
		"li      $ra, 2\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"move    %0, $ra\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result) : : "t0", "t2"
	);

	printf("jal: ra order: %08x\n", result);
}

void __attribute__((noinline)) test_jalr_ra_order() {
	register int result = -1;

	asm volatile (
		".set noreorder\n"

		"move    $t2, $ra\n"
		"la      $t0, target2_%=\n"
		"jalr    $t0\n"
		"li      $ra, 2\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"move    %0, $ra\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result) : : "t0", "t2"
	);

	printf("jalr: ra order: %08x\n", result);
}

void __attribute__((noinline)) test_jalr_non_ra_value() {
	register int result = -1;

	asm volatile (
		".set noreorder\n"

		"move    $t2, $ra\n"
		"la      $t1, target2_%=\n"
		"jalr    %0, $t1\n"
		"nop\n"

		"target1_%=:\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"nop\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result) : : "t1", "t2"
	);

	printf("jalr: non-ra: %08x\n", result);
}

void __attribute__((noinline)) test_jalr_non_ra_order() {
	register int result = -1;

	asm volatile (
		".set noreorder\n"

		"move    $t2, $ra\n"
		"la      $t1, target2_%=\n"
		"jalr    %0, $t1\n"
		"li      %0, 1\n"

		"target1_%=:\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"nop\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result) : : "t1", "t2"
	);

	printf("jalr: non-ra order: %08x\n", result);
}

void __attribute__((noinline)) test_jalr_rs_rd_match() {
	register int result = -1;

	asm volatile (
		".set noreorder\n"

		"move    $t2, $ra\n"
		"la      $t0, target2_%=\n"
		"jalr    $t0, $t0\n"
		"nop\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"li      %0, 2\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result) : : "t0", "t2"
	);

	printf("jalr: rs/rd match: %08x\n", result);
}

void __attribute__((noinline)) test_jalr_clobber() {
	register int result = -1;

	asm volatile (
		".set noreorder\n"

		"move    $t2, $ra\n"
		"la      $t0, target2_%=\n"
		"jalr    $t0\n"
		"ori     $t0, $0, 5\n"
		"nop\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"li      %0, 2\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result) : : "t0", "t2"
	);

	printf("jalr: clobber rs: %08x\n", result);
}

static void test_jalr() {
	test_jalr_ra_order();
	test_jalr_non_ra_value();
	test_jalr_non_ra_order();
	test_jalr_rs_rd_match();
	test_jalr_clobber();
}

void __attribute__((noinline)) test_bltzal() {
	register int result = -1;

	asm volatile (
		".set    noreorder\n"

		"move    $t2, $ra\n"
		"subu    $t0, $0, 1\n"
		"bltzal  $t0, target2_%=\n"
		"li      $ra, 2\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"move    %0, $ra\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result)
	);

	printf("bltzal: ra order: %08x\n", result);
}

void __attribute__((noinline)) test_bgezal() {
	register int result = -1;

	asm volatile (
		".set    noreorder\n"

		"move    $t2, $ra\n"
		"li      $t0, 0\n"
		"bgezal  $t0, target2_%=\n"
		"li      $ra, 2\n"

		"target1_%=:\n"
		"li      %0, 1\n"
		"j       skip_%=\n"
		"nop\n"

		"target2_%=:\n"
		"move    %0, $ra\n"

		"skip_%=:\n"
		"move    $ra, $t2\n"

		".set    reorder\n"
		: "+r"(result)
	);

	printf("bgezal: ra order: %08x\n", result);
}

int _start(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	test_beq();

	test_jal();
	test_jalr();
	test_bltzal();
	test_bgezal();

	printf("-- TEST END\n");

	return 0;
}
