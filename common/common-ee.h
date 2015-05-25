#ifndef PS2AUTOTESTS_COMMON_EE_H
#define PS2AUTOTESTS_COMMON_EE_H

// Defines for MSVC highlighting.  Not intended to actually compile with msc.
#ifdef _MSC_VER
#define __STDC__
#define _EE
#define __attribute__(x)
#endif

#include <stdio.h>
#include <tamtypes.h>

#undef main
#define main test_main

void schedf(const char *format, ...);
void flushschedf();

#endif
