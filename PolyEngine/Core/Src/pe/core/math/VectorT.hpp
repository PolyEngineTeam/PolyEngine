#pragma once

#include <pe/Defines.hpp>

namespace Poly
{
	/// Class representing 3D float vector.
	template<typename T, size_t S>
	class VectorT final : public BaseObjectLiteralType<>
	{
		STATIC_ASSERTE(S <= 4 && S >= 2, "Vector size connnot be greater than 4 and smaller than 2");
		STATIC_ASSERTE(std::is_floating_point_v<T> || std::is_integral_v<T>, "Only floating point and integer types are supported.");

		template <typename K, size_t C> static constexpr bool isVecEq_v = std::is_same_v<K, T> && S == C;
		template <typename K, size_t C> static constexpr bool isVecGe_v = std::is_same_v<K, T> && S >= C;

	public:
		using ValueType = T;
		static constexpr size_t VectorSize = S;

		/// Creates zero vector.
		explicit constexpr VectorT() { std::fill(Data.begin(), Data.end(), (T)0); }

		/// Creates vector from float values.
		template<typename U = T>
		explicit constexpr VectorT(T x, T y,
			typename std::enable_if<isVecEq_v<U, 2>>::type* = 0)
		{
			Data = { {x, y} };
		}

		template<typename U = T>
		explicit constexpr VectorT(T x, T y, T z,
			typename std::enable_if<isVecEq_v<U, 3>>::type* = 0)
		{
			Data = { {x, y, z} };
		}

		template<typename U = T>
		explicit constexpr VectorT(T x, T y, T z, T w,
			typename std::enable_if<isVecEq_v<U, 4>>::type* = 0)
		{
			Data = { {x, y, z, w} };
		}

		template<typename U = T>
		T& GetX(typename std::enable_if<isVecGe_v<U, 1>>::type* = 0) { return Data[0]; }

		template<typename U = T>
		const T& GetX(typename std::enable_if<isVecGe_v<U, 1>>::type* = 0) const { return Data[0]; }

		template<typename U = T>
		T& GetY(typename std::enable_if<isVecGe_v<U, 2>>::type* = 0) { return Data[1]; }

		template<typename U = T>
		const T& GetY(typename std::enable_if<isVecGe_v<U, 2>>::type* = 0) const { return Data[1]; }

		template<typename U = T>
		T& GetZ(typename std::enable_if<isVecGe_v<U, 3>>::type* = 0) { return Data[2]; }

		template<typename U = T>
		const T& GetZ(typename std::enable_if<isVecGe_v<U, 3>>::type* = 0) const { return Data[2]; }

		template<typename U = T>
		T& GetW(typename std::enable_if<isVecGe_v<U, 4>>::type* = 0) { return Data[3]; }

		template<typename U = T>
		const T& GetW(typename std::enable_if<isVecGe_v<U, 4>>::type* = 0) const { return Data[3]; }

		size_t GetSize() const { return S; }

		// This structure allows to access vector elements by index or name.
		std::array<T, S> Data;
	};
}