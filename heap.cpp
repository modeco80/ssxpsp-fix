
#include "heap.hpp"

#include <new>

namespace mem::impl {

	Heap* Heap::create(u8* begin, Heap::SizeType size) {
		return new(begin) Heap(size);
	}

	Heap::Heap(SizeType size) : chunkSize(size) {
		// Initialize the head segment of the heap.
		head = new(usableChunkStart()) Segment;
		head->size = size;
	}

	Heap::Pointer Heap::allocate(SizeType count) {
		Segment* current {};
		Segment* prev {};
		Segment* best {};

		SignedSizeType difference {};
		SignedSizeType bestDifference { 0x7fffffff };

		count += sizeof(Segment);
		// Align size to 16 (naive, could probably do this better with bit twiddling)
		count += count % 16 ? 16 - (count % 16) : 0;

		// Find the allocation that is closest in bytes to this request
		for(current = head; current != nullptr; current = current->Next) {
			difference = current->size - count;
			if(!current->allocated && difference < bestDifference && difference >= 0) {
				best = current;
				bestDifference = difference;
			}
			prev = current;
		}

		// Couldn't find a fitting allocation, give up.
		if(best == nullptr) {
#if 0
			if(reinterpret_cast<u8*>(prev) + prev->size >= reinterpret_cast<u8*>(head) + this->chunkSize) {
				// Out of heap memory.
			}
#endif

			return nullptr;
		}

		// If the best difference we could come up with was large, split up this segment into two.
		if(bestDifference > static_cast<SignedSizeType>(2 * sizeof(Segment))) {
			auto splitSegment = new(reinterpret_cast<u8*>(best) + count) Segment;

			current = best->Next;

			best->Next = splitSegment;
			best->Next->Next = current;
			best->Next->Prev = best;
			best->Next->size = best->size - count;
			best->size = count;
		}

		// Mark the chunk we just allocated as being allocated (obviously)
		best->allocated = true;
		return reinterpret_cast<Pointer>(reinterpret_cast<u8*>(best) + sizeof(Segment));
	}

	void Heap::free(Pointer ptr) {
		if(ptr == nullptr)
			return;

		auto* seg = reinterpret_cast<Segment*>(reinterpret_cast<u8*>(ptr) - sizeof(Segment));
		seg->allocated = false;

		// Try combining nodes.
		combineNodes(seg);
	}

	void Heap::combineNodes(Segment* seg) {
		// Try to coalesce segments to the left of us.
		while(seg->Prev != nullptr && !seg->Prev->allocated) {
			seg->Prev->Next = seg->Next;
			seg->Prev->size += seg->size;
			seg = seg->Prev;
		}

		// Try to coalesce segments to the right of us.
		while(seg->Next != nullptr && !seg->Next->allocated) {
			seg->size += seg->Next->size;
			seg->Next = seg->Next->Next;
		}
	}

} // namespace mem::impl
