#pragma once

#include "Defines.hpp"

namespace Poly {

	inline void* default_alloc(size_t size) { return malloc(size); }
	inline void* default_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
	inline void default_free(void* ptr) { free(ptr); }
}