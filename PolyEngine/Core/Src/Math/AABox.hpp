#pragma once

#include <Defines.hpp>
#include <Math/BasicMath.hpp>
#include <Math/Vector.hpp>
#include <Math/Matrix.hpp>

namespace Poly {

	/// <summary>Class representing axis aligned box.</summary>
	class CORE_DLLEXPORT AABox : public BaseObject<>
	{
	public:

		/// <summary>Constructor from position and size.</summary>
		/// <param name="position">Position of the min point of the box.</param>
		/// <param name="size">Size (extent) of the box in each of the directions.</param>
		AABox(const Vector& position, const Vector& size);
		AABox() = default;

		/// <summary>Calculates center of the box.</summary>
		/// <returns>Center of the box.</returns>
		Vector GetCenter() const { return Pos + (Size * 0.5f); }
		
		/// <summary>Returns the min point of the box. (posiiton)</summary>
		/// <returns>Min point of the box.</returns>
		const Vector& GetMin() const { return Pos; }

		/// <summary>Returns the max point of the box.</summary>
		/// <returns>Max point of the box.</returns>
		Vector GetMax() const { return Pos + Size; }
		
		/// <summary>Returns Size of the box in all three dimensions.</summary>
		/// <returns>Size of the box. (extent)</returns>
		const Vector& GetSize() const { return Size; }

		/// <summary>Sets position of the box.</summary>
		/// <param name="pos">New position</param>
		void SetPosition(const Vector& pos) { Pos = pos; }

		/// <summary>Sets size of the box.</summary>
		/// <param name="size">New size</param>
		void SetSize(const Vector& size) { Size = size; }

		/// <summary>Checks whether this AABox contains a given point.</summary>
		/// <param name="point">Point to be checked.</param>
		/// <see cref="AABox.IsCollidingWith()"/>
		inline bool Contains(const Vector& point) const 
		{ 
			return point.X >= Pos.X && point.X <= (Pos.X + Size.X) 
			&& point.Y >= Pos.Y && point.Y <= (Pos.Y + Size.Y) 
			&& point.Z >= Pos.Z && point.Z <= (Pos.Z + Size.Z);
		}

		/// <summary>Checks whether a given AABox is interecting with this AABox.</summary>
		/// <param name="rhs">Other box.</param>
		/// <see cref="AABox.Contains()"/>
		inline bool Intersects(const AABox& rhs) const
		{	
			return (abs(Pos.X - rhs.Pos.X) * 2 < (Size.X + rhs.Size.X))
			&& (abs(Pos.Y - rhs.Pos.Y) * 2 < (Size.Y + rhs.Size.Y))
			&& (abs(Pos.Z - rhs.Pos.Z) * 2 < (Size.Z + rhs.Size.Z));
		}

		std::array<Vector, 8> GetVertices() const;

		/// <summary>Calculates the intersection volume of 2 AABoxes.</summary>
		/// <param name="rhs">Other box to calculate intersection with.</param>
		/// <returns>Box representing the intersection volume.</returns>
		/// <see cref="AABox.Intersects()"/>
		AABox GetIntersectionVolume(const AABox& rhs) const;

		AABox GetTransformed(const Matrix& transform) const;

		AABox& Expand(const AABox& rhs);

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const AABox& color);
	private:
		Vector Pos;
		Vector Size;
	};
}