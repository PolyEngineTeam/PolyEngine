#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"
#include "Vector.hpp"

namespace Poly {

	class CORE_DLLEXPORT AARect : public BaseObject<>
	{
	public:
		AARect(const Vector& position, const Vector& size);

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const AARect& color);

		Vector GetCenter() const { return Pos + (Size * 0.5f); }
		const Vector& GetMin() const { return Pos; }
		Vector GetMax() const { return Pos + Size; }
		
		const Vector& GetSize() const { return Size; }

		void SetPosition(const Vector& pos) { Pos = pos; }
		void SetSize(const Vector& size) { Size = size; }

	private:
		Vector Pos;
		Vector Size;
	};
}