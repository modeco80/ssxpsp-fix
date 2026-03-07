#pragma once

#include "types.hpp"

namespace mem::impl {

	/// A memory heap class. This class does not protect from multithreaded
	/// usage; you'll need to do that in a higher layer yourself.
	class Heap {
	   public:
		using SizeType = usize;
		using SignedSizeType = ptrdiff_t;
		using Pointer = u8*;
		using ConstPointer = const u8*;

		struct [[gnu::aligned(4)]] Segment {
		   protected:
			friend Heap;
			Segment* Prev;
			Segment* Next;

		   public:
			bool allocated {};
			SizeType size {};
		};

	   public:
		/// Create a heap at a given memory address. This (ab)uses placement new.
		static Heap* create(u8* begin, SizeType count);

		explicit Heap(SizeType count);

		/// Allocate somem
		Pointer allocate(SizeType count);

		void free(Pointer ptr);

	   private:
		void combineNodes(Segment* seg);

		Segment* head;

		inline Pointer usableChunkStart() { return reinterpret_cast<u8*>(this + 1); }

		inline Pointer usableChunkEnd() { return static_cast<Pointer>(usableChunkStart() + chunkSize); }

		/**
		 * Chunk size in bytes.
		 */
		size_t chunkSize;
	};

} // namespace mem::impl
