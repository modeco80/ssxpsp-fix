#include "memory.hpp"

#include <pspsdk.h>
#include <pspuser.h>

#include "heap.hpp"
#include "log.hpp"

namespace mem {

	SceUID gMemoryUID = -1;
	impl::Heap* gpMemHeap = nullptr;

	void init(usize memSize) {
		auto uid = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "memmgr", PSP_SMEM_Low, memSize, NULL);
		if(uid >= 0) {
			// The kernel allocated memory successfully. Let's create our heap on top of it.
			gMemoryUID = uid;
			gpMemHeap = impl::Heap::create(reinterpret_cast<u8*>(sceKernelGetBlockHeadAddr(uid)), memSize);
			logc::print(logc::Info, "Heap (%u bytes) initialized successfully.", memSize);
		}
	}

	void shutdown() {
		if(gpMemHeap) {
			// Destroy the heap.
			gpMemHeap->~Heap();

			// Free the kernel allocated memory.
			sceKernelFreePartitionMemory(gMemoryUID);
			gMemoryUID = -1;
		}
	}

	u8* allocate(size_t size) {
		if(gpMemHeap == nullptr)
			return nullptr;
		return reinterpret_cast<u8*>(gpMemHeap->allocate(size));
	}

	void free(u8* ptr) {
		if(gpMemHeap == nullptr)
			return;
		gpMemHeap->free(ptr);
	}
} // namespace mem
