#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"
#include "Vector.hpp"

namespace Poly {

	class CORE_DLLEXPORT AABox : public BaseObject<>
	{
	public:
		AABox(const Vector& position, const Vector& size);

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const AABox& color);

		Vector GetCenter() const { return Pos + (Size * 0.5f); }
		const Vector& GetMin() const { return Pos; }
		Vector GetMax() const { return Pos + Size; }
		
		const Vector& GetSize() const { return Size; }

		void SetPosition(const Vector& pos) { Pos = pos; }
		void SetSize(const Vector& size) { Size = size; }

		//////////////////////////////
		/// Checks whether this AABox contains a given point.
		/// @param point to be checked
		/// @see IsCollidingWith()
		//////////////////////////////
		inline bool Contains(const Vector& point) const 
		{ return point.X >= Pos.X && point.X <= (Pos.X + Size.X) 
			&& point.Y >= Pos.Y && point.Y <= (Pos.Y + Size.Y) 
			&& point.Z >= Pos.Z && point.Z <= (Pos.Z + Size.Z); }

		//////////////////////////////
		/// Checks whether a given AABox is colliding with this AABox.
		/// @param otherRect
		/// @see Contains()
		//////////////////////////////
		inline bool Intersects(const AABox* otherRect) const
		{	ASSERTE(otherRect != nullptr, "AABox::Intersects() called with otherRect == nullptr.");
			return (abs(Pos.X - otherRect->Pos.X) * 2 < (Size.X + otherRect->Size.X))
			&& (abs(Pos.Y - otherRect->Pos.Y) * 2 < (Size.Y + otherRect->Size.Y))
			&& (abs(Pos.Z - otherRect->Pos.Z) * 2 < (Size.Z + otherRect->Size.Z)); }

		//////////////////////////////
		/// Calculates the intersection of 2 AABoxs.
		/// @param otherRect to calculate intersection with
		/// @return AABox intersection
		/// @see Intersects()
		//////////////////////////////
		AABox GetIntersection(const AABox* otherRect) const;

	private:
		Vector Pos;
		Vector Size;
	};
}