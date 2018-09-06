#pragma once

#include "Defines.hpp"
#include "BaseObject.hpp"
#include "Math/Vector2f.hpp"

namespace Poly
{
	class CORE_DLLEXPORT AARect : public BaseObject<>
	{
	public:

		/// <summary>Constructor from position and size.</summary>
		/// <param name="position">Position of the min point of the box.</param>
		/// <param name="size">Size (extent) of the box in each of the directions.</param>
		AARect(const Vector2f& position, const Vector2f& size);

		/// <summary>Calculates center of the box.</summary>
		/// <returns>Center of the box.</returns>
		Vector2f GetCenter() const { return Pos + (Size * 0.5f); }

		/// <summary>Returns the min point of the box. (posiiton)</summary>
		/// <returns>Min point of the box.</returns>
		const Vector2f& GetMin() const { return Pos; }

		/// <summary>Returns the max point of the box.</summary>
		/// <returns>Max point of the box.</returns>
		Vector2f GetMax() const { return Pos + Size; }

		/// <summary>Returns Size of the box in all three dimensions.</summary>
		/// <returns>Size of the box. (extent)</returns>
		const Vector2f& GetSize() const { return Size; }

		/// <summary>Sets position of the box.</summary>
		/// <param name="pos">New position</param>
		void SetPosition(const Vector2f& pos) { Pos = pos; }

		/// <summary>Sets size of the box.</summary>
		/// <param name="size">New size</param>
		void SetSize(const Vector2f& size) { Size = size; }

		/// <summary>Checks whether this AARect contains a given point.</summary>
		/// <param name="point">Point to be checked.</param>
		/// <see cref="AARect.IsCollidingWith()"/>
		inline bool Contains(const Vector2f& point) const
		{
			return point.X >= Pos.X && point.X <= (Pos.X + Size.X)
				&& point.Y >= Pos.Y && point.Y <= (Pos.Y + Size.Y);
		}

		/// <summary>Checks whether a given AARect is interecting with this AARect.</summary>
		/// <param name="rhs">Other box.</param>
		/// <see cref="AARect.Contains()"/>
		inline bool Intersects(const AARect& rhs) const
		{
			return (abs(Pos.X - rhs.Pos.X) * 2 < (Size.X + rhs.Size.X))
				&& (abs(Pos.Y - rhs.Pos.Y) * 2 < (Size.Y + rhs.Size.Y));
		}

		/// <summary>Calculates the intersection volume of 2 AARectes.</summary>
		/// <param name="rhs">Other box to calculate intersection with.</param>
		/// <returns>Box representing the intersection volume.</returns>
		/// <see cref="AARect.Intersects()"/>
		AARect GetIntersectionVolume(const AARect& rhs) const;

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const AARect& color);
	private:
		Vector2f Pos;
		Vector2f Size;
	};
}