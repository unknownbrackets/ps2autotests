#include <stdio.h>
#include <stdarg.h>
#include <xprintf.h>

#define SCHEDF_BUFFER_SIZE 65536

char schedfBuffer[65536];
unsigned int schedfBufferPos = 0;

void schedf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	schedfBufferPos += vsnprintf(schedfBuffer + schedfBufferPos, SCHEDF_BUFFER_SIZE, format, args);
	va_end(args);
}

void flushschedf() {
	printf("%s", schedfBuffer);
	schedfBuffer[0] = '\0';
	schedfBufferPos = 0;
}
