#ifndef PS2AUTOTESTS_COMMON_H
#define PS2AUTOTESTS_COMMON_H

#include <stdio.h>
#include <tamtypes.h>

// Defines for MSVC highlighting.  Not intended to actually compile with msc.
#ifdef _MSC_VER
void printf(const char *s, ...);
#define __attribute__(x)
#endif

#endif
