#pragma once

#include <pe/Defines.hpp>
#include <pe/core/BaseObject.hpp>

namespace pe::core::memory
{

	class CORE_DLLEXPORT BinaryBuffer final : public BaseObjectLiteralType<>
	{
	public:
		BinaryBuffer(size_t size);
		~BinaryBuffer();

		BinaryBuffer(const BinaryBuffer& rhs) { *this = rhs; }
		BinaryBuffer(BinaryBuffer&& rhs) { *this = std::move(rhs); }

		BinaryBuffer& operator=(const BinaryBuffer& rhs);
		BinaryBuffer& operator=(BinaryBuffer&& rhs);

		char* GetData() { return Data; }
		const char* GetData() const { return Data; }
		size_t GetSize() const { return Size; }

	private:
		char* Data;
		size_t Size;
	};

} // namespace Poly