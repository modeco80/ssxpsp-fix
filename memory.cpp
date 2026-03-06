#include <pspsdk.h>
#include <pspuser.h>

#include "memory.hpp"

namespace mem {

	// allocated objects add this header
	struct tMemoryBlockHeader {
		SceUID partitionMemoryUID;
		usize size;

		u8* memory() { return reinterpret_cast<u8*>(this+1); }
		// TODO memoryAligned
	};


	u8* allocate(size_t size) {
		auto uid = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_Low, size + sizeof(tMemoryBlockHeader), NULL);

		if (uid >= 0) {
			auto* p = reinterpret_cast<tMemoryBlockHeader*>(sceKernelGetBlockHeadAddr(uid));
			p->partitionMemoryUID = uid;
			p->size = size;
			return p->memory();
		}

		return nullptr;
	}

	void free(u8* ptr) {
		if (ptr) {
			auto* pBlockHeader = reinterpret_cast<tMemoryBlockHeader*>(ptr-sizeof(tMemoryBlockHeader));
			auto uid = pBlockHeader->partitionMemoryUID;
			sceKernelFreePartitionMemory(uid);
		}
	}
}
