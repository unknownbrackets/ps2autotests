#include "common.h"
#include <stdarg.h>

char schedfBuffer[65536];
unsigned int schedfBufferPos = 0;

void schedf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	schedfBufferPos += vsprintf(schedfBuffer + schedfBufferPos, format, args);
	va_end(args);
}

void flushschedf() {
	printf("%s", schedfBuffer);
	schedfBuffer[0] = '\0';
	schedfBufferPos = 0;
}
