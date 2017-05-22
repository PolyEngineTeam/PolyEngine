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

		//////////////////////////////
		/// Checks whether this AARect contains a given point.
		/// @param point to be checked
		/// @see IsCollidingWith()
		//////////////////////////////
		inline bool Contains(const Vector& point) const 
		{ return point.X >= Pos.X && point.X <= (Pos.X + Size.X) 
			&& point.Y >= Pos.Y && point.Y <= (Pos.Y + Size.Y) 
			&& point.Z >= Pos.Z && point.Z <= (Pos.Z + Size.Z); }

		//////////////////////////////
		/// Checks whether a given AARect is colliding with this AARect.
		/// @param otherRect
		/// @param intersection (optional) will be set to the intersection of the two tested AARects
		/// @see Contains()
		//////////////////////////////
		bool IsCollidingWith(const AARect* otherRect, AARect* intersection = nullptr) const;

	private:
		Vector Pos;
		Vector Size;
	};
}