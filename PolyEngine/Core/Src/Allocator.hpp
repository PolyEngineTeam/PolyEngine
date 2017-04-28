#pragma once

#include "Defines.hpp"

namespace Poly {

#if defined(_WIN32)
	inline void* DefaultAlloc(size_t size) { return _aligned_malloc(size, 16); }
	inline void* DefaultRealloc(void* ptr, size_t size) { return _aligned_realloc(ptr, size, 16); }
	inline void DefaultFree(void* ptr) { _aligned_free(ptr); }
#elif defined(__GNUC__) || defined(__clang__)
	namespace Impl {
		constexpr size_t alignment = 16;
	}

#include <malloc.h>

	inline void* DefaultAlloc(size_t size) {
		using namespace Impl;
		void* fresh = aligned_alloc(alignment, size);

#ifdef __has_feature
#if __has_feature(memory_sanitizer)
		//we'll be possibly reading this memory later during reallocation, so write over it to avoid triggering unitialized reads detection
		size_t diff = malloc_usable_size(fresh) - size;
		memset(static_cast<uint8_t*>(fresh) + size - diff, 0, diff);
#endif
#endif
		return fresh;
	}

	inline void* DefaultRealloc(void* ptr, size_t size) {
		using namespace Impl;
		//POSIX provides no aligned_realloc, so always do a fresh allocation, then copy :(
		void* fresh = DefaultAlloc(size);

		if (ptr) {
			memcpy(fresh, ptr, malloc_usable_size(ptr));
			free(ptr);
		}

		return fresh;
	}

	inline void DefaultFree(void* ptr) {
		free(ptr);
	}

#if 0
	inline void* ManualBookkeeping_DefaultAlloc(size_t size) {
		using namespace Impl;
		static_assert(sizeof(size) <= alignment);
		void* fresh = aligned_alloc(alignment, alignment + size);
		if (fresh == nullptr) {
			return fresh;
		}

		memcpy(fresh, &size, sizeof(size)); //will be needed by realloc

		return static_cast<uint8_t*>(fresh) + alignment;
	}

	inline void* ManualBookkeeping_DefaultRealloc(void* ptr, size_t size) {
		using namespace Impl;
		static_assert(sizeof(size) <= alignment);
		//POSIX provides no aligned_realloc, so always do a memcpy :(
		void* fresh = ManualBookkeeping_DefaultAlloc(size);

		if (ptr) {
			decltype(size) old_size;
			memcpy(&old_size, static_cast<uint8_t*>(ptr) - alignment, sizeof(size));

			//copy stuff over
			memcpy(fresh, ptr, old_size);
		}

		return fresh;

	}

	inline void ManualBookkeeping_DefaultFree(void* ptr) {
		using namespace Impl;
		free(static_cast<uint8_t*>(ptr) - alignment);
	}
#endif

#else
	#error [ERROR] Unsupported platform! You are trying to compile for unsupported platform. This won't work.'
#endif
} //namespace Poly
