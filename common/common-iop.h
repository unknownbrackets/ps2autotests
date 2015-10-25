#ifndef PS2AUTOTESTS_COMMON_IOP_H
#define PS2AUTOTESTS_COMMON_IOP_H

// Defines for MSVC highlighting.  Not intended to actually compile with msc.
#ifdef _MSC_VER
#define __STDC__
#define _IOP
#define __attribute__(x)
#endif

#include <stdio.h>
#include <tamtypes.h>

void schedf(const char *format, ...);
void flushschedf();

#endif
