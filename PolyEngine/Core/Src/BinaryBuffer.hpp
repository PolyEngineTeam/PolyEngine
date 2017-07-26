#pragma once

#include "BaseObject.hpp"

namespace Poly
{

	class CORE_DLLEXPORT BinaryBuffer : public BaseObject<>
	{
	public:
		BinaryBuffer(size_t);
		~BinaryBuffer();

		void* GetBegin() const { return Begin; }
		size_t GetSize() const { return Size; }
		void AddData(void*, size_t);

	private:
		void* Begin;
		size_t Size;
	};

} // namespace Poly