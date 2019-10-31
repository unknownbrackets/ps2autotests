#include <common-iop.h>
#include <string.h>

//vsprintf is not reliable on a real PS2, we can't use it. We rely on this macro instead.
#define EXECUTE_TEST(output, format, ...) sprintf(output, format, __VA_ARGS__); printf("%s: %s\n", format, output);

int _start(int argc, char **argv) {
	printf("-- TEST BEGIN\n");
	
	char buffer[256];

	EXECUTE_TEST(buffer, "%d", 32);
	EXECUTE_TEST(buffer, "%03d", 45);
	EXECUTE_TEST(buffer, "%3d", 12);
	EXECUTE_TEST(buffer, "%3.3d", 87);
	EXECUTE_TEST(buffer, "%+6d", 948);
	EXECUTE_TEST(buffer, "%+6d", -948);
	EXECUTE_TEST(buffer, "%6d", -948);
	EXECUTE_TEST(buffer, "%06d", -948);
	EXECUTE_TEST(buffer, "%+06d", 948);
	EXECUTE_TEST(buffer, "%u", -948);
	EXECUTE_TEST(buffer, "%s%3.3d", "myfile", 0);
	EXECUTE_TEST(buffer, "%x", 0xdeadbeef);
	EXECUTE_TEST(buffer, "%X", 0xDEADBEEF);
	EXECUTE_TEST(buffer, "%08x", 23);
	EXECUTE_TEST(buffer, "%8x", 12);
	EXECUTE_TEST(buffer, "%p", (void*)0x01234567);
	EXECUTE_TEST(buffer, "%c", 'j');
	
	printf("-- TEST END\n");
	return 0;
}
