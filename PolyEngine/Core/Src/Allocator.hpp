#pragma once

#include "Defines.hpp"

namespace Poly {

	//FIXME _aligned_malloc is Visual Studio only
	inline void* DefaultAlloc(size_t size) { return _aligned_malloc(size, 16); }
	inline void* DefaultRealloc(void* ptr, size_t size) { return _aligned_realloc(ptr, size, 16); }
	inline void DefaultFree(void* ptr) { _aligned_free(ptr); }
}