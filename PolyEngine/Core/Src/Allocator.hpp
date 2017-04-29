#pragma once

#include "Defines.hpp"

//TODO(vuko): possibly replace with std::aligned_storage for portability and soundness guarantee

namespace Poly {

#if defined(_WIN32)
	inline void* DefaultAlloc(size_t size) { return _aligned_malloc(size, 16); }
	inline void* DefaultRealloc(void* ptr, size_t size) { return _aligned_realloc(ptr, size, 16); }
	inline void DefaultFree(void* ptr) { _aligned_free(ptr); }
#elif defined(__GNUC__) || defined(__clang__)
	namespace Impl {
		constexpr size_t MEM_ALIGNMENT = 16;
	}

#include <malloc.h>

	inline void* DefaultAlloc(size_t size) {
		using namespace Impl;
		void* fresh = aligned_alloc(MEM_ALIGNMENT, size);

#ifdef __has_feature
#if __has_feature(memory_sanitizer)
		//we'll be possibly reading this memory later during reallocation, so write over it to avoid triggering unitialized reads detection
		size_t diff = malloc_usable_size(fresh) - size;
		memset(static_cast<uint8_t*>(fresh) + size - diff, 0, diff);
#endif
#endif
		return fresh;
	}

	inline void DefaultFree(void* ptr) {
		free(ptr); //NOTE(vuko): aligned_alloc'd pointers can be safely freed by free()
	}

	inline void* DefaultRealloc(void* ptr, size_t size) {
		using namespace Impl;
		//POSIX provides no aligned_realloc, so always do a fresh allocation, then copy :(
		void* fresh = DefaultAlloc(size);

		if (ptr) {
			memcpy(fresh, ptr, malloc_usable_size(ptr));
			DefaultFree(ptr);
		}

		return fresh;
	}

#else
	#error [ERROR] Unsupported platform! You are trying to compile for unsupported platform. This won't work.'
#endif
} //namespace Poly
