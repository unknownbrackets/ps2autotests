#include "gsregs.h"

namespace GS {
	u64 SIGNAL(u32 id, u32 idmsk) {
		return ((u64)id) | ((u64)idmsk << 32);
	}
	
	u64 LABEL(u32 id, u32 idmsk) {
		return ((u64)id) | ((u64)idmsk << 32);
	}
}
