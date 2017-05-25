#pragma once

namespace Poly
{
	namespace ObjectLifetimeHelper
	{
		// Not really sure if this is needed
		template<class T>
		void DefaultCreate(T* ptr, typename std::enable_if<std::is_trivially_default_constructible<T>::value>::type* = 0)
		{
			::new (ptr)T; // TODO use memcpy here
		}

		template<class T>
		void DefaultCreate(T* ptr, typename std::enable_if<!std::is_trivially_default_constructible<T>::value>::type* = 0)
		{
			::new (ptr)T;
		}

		template<class T>
		void CopyCreate(T* ptr, const T& obj, typename std::enable_if<std::is_trivially_copy_constructible<T>::value>::type* = 0)
		{
			::new (ptr)T(obj); // TODO use memcpy here
		}

		template<class T>
		void CopyCreate(T* ptr, const T& obj, typename std::enable_if<!std::is_trivially_copy_constructible<T>::value>::type* = 0)
		{
			::new (ptr)T(obj);
		}

		template<class T>
		void MoveCreate(T* ptr, T&& obj, typename std::enable_if<std::is_trivially_move_constructible<T>::value>::type* = 0)
		{
			::new (ptr)T(std::move(obj)); // TODO use memcpy here
		}

		template<class T>
		void MoveCreate(T* ptr, T&& obj, typename std::enable_if<!std::is_trivially_move_constructible<T>::value>::type* = 0)
		{
			::new (ptr)T(std::move(obj));
		}

		template<class T>
		void Destroy(T* t, typename std::enable_if<std::is_trivially_destructible<T>::value>::type* = 0)
		{
			t->~T(); // calling destructor here is unnecessary
		}

		template<class T>
		void Destroy(T* t, typename std::enable_if<!std::is_trivially_destructible<T>::value>::type* = 0)
		{
			t->~T();
		}
	}
}