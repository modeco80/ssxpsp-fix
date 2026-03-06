#pragma once
#include "types.hpp"

namespace mem {

	u8* allocate(usize count);

	void free(u8* pptr);

}
