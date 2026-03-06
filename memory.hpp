#pragma once
#include "types.hpp"

namespace mem {

	/// Initalize the memory subsystem
	void init(usize heapSize);

	/// Shutdown the memory subsystem.
	void shutdown();

	/// Allocates memory. The memory subsystem must be initalized with
	/// [mem::init()] beforehand.
	u8* allocate(usize count);

	/// Frees memory. The memory subsystem must be initalized with
	/// [mem::init()] beforehand.
	void free(u8* pptr);

} // namespace mem

// TODO: global new/delete override? would be nice
