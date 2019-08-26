#pragma once

#include <pe/Defines.hpp>

namespace pe::core::memory
{
	//wondering(vuko): proposed names: Object (Object::Create, Object::Destroy), ???
	namespace ObjectLifetimeHelper //todo(vuko/muniu): better naming + documentation
	{
		//note: functions are parametrized by trivial-constructibleness (wew, what a mouthful), so future logging/debugging/profiling can tell a difference

		// Not really sure if this is needed
		template<class T>
		void DefaultCreate(T* ptr, typename std::enable_if<std::is_trivially_default_constructible<T>::value>::type* = 0)
		{
			::new(ptr) T;
		}

		//wondering(vuko): proposed names: CreateAt, ???
		template<class T>
		void DefaultCreate(T* ptr, typename std::enable_if<!std::is_trivially_default_constructible<T>::value>::type* = 0)
		{
			::new(ptr) T;
		}

		//wondering(vuko): proposed names: CopyCreateAt, ???
		template<class T>
		void CopyCreate(T* ptr, const T& obj, typename std::enable_if<std::is_copy_constructible<T>::value && std::is_trivially_copy_constructible<T>::value>::type* = 0)
		{
			::new(ptr) T(obj);
		}

		template<class T>
		void CopyCreate(T* ptr, const T& obj, typename std::enable_if<std::is_copy_constructible<T>::value && !std::is_trivially_copy_constructible<T>::value>::type* = 0)
		{
			::new(ptr) T(obj);
		}

		//wondering(vuko): proposed names: MoveCreateAt, MoveCreating, ???
		template<class T>
		void MoveCreate(T* ptr, T&& obj, typename std::enable_if<std::is_move_constructible<T>::value && std::is_trivially_move_constructible<T>::value>::type* = 0)
		{
			//wondering(vuko): since `move` is in the name, we could take the source object by
			//                 lvalue-ref, which would give us a nicer call syntax:
			//                 `MoveCreate(dest, source)` instead of `MoveCreate(dest, std::move(source))`
			::new(ptr) T(std::move(obj));
		}

		template<class T>
		void MoveCreate(T* ptr, T&& obj, typename std::enable_if<std::is_move_constructible<T>::value && !std::is_trivially_move_constructible<T>::value>::type* = 0)
		{
			::new(ptr) T(std::move(obj));
		}

		//wondering(vuko): missing CopyInto, MoveInto, ForwardCreate

		//wondering(vuko): `Destroy(T&)` overload?
		template<class T>
		void Destroy(T* t, typename std::enable_if<std::is_trivially_destructible<T>::value>::type* = 0)
		{
			t->~T();
		}

		template<class T>
		void Destroy(T* t, typename std::enable_if<!std::is_trivially_destructible<T>::value>::type* = 0)
		{
			t->~T();
		}

		template<typename T>
		T* DefaultAllocateAndCreate(T* memory = nullptr, typename std::enable_if<std::is_default_constructible<T>::value>::type* = 0)
		{
			if (memory != nullptr)
			{
				ObjectLifetimeHelper::DefaultCreate<T>((T*)memory);
				return memory;
			}
			else
				return new T;
		}

		template<typename T>
		T* DefaultAllocateAndCreate(T* memory = nullptr, typename std::enable_if<!std::is_default_constructible<T>::value>::type* = 0)
		{
			ASSERTE(false, "Type is not default constructible!");
			return nullptr;
		}
	}
}
