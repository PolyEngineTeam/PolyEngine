#pragma once

#include <pe/Defines.hpp>

namespace pe::core::storage
{
	template<class _Ty,class _Container = std::vector<_Ty>,class _Pr = std::less<typename _Container::value_type> >
	class PriorityQueue final : public std::priority_queue<_Ty,_Container,_Pr>
	{
	public:
		
		PriorityQueue()
			: std::priority_queue<_Ty, _Container, _Pr>()
		{
		}

		explicit PriorityQueue(const _Pr& _Pred)
			: std::priority_queue<_Ty, _Container, _Pr>(_Pred)
		{
		}

		PriorityQueue(const _Pr& _Pred, const _Container& _Cont)
			: std::priority_queue<_Ty, _Container, _Pr>(_Pred,_Cont)
		{
		}

		template<class _InIt>
		PriorityQueue(_InIt _First, _InIt _Last)
			: std::priority_queue<_Ty, _Container, _Pr>(_First,_Last)
		{	
		}

		template<class _InIt>
		PriorityQueue(_InIt _First, _InIt _Last, const _Pr& _Pred)
			: std::priority_queue<_Ty, _Container, _Pr>(_First,_Last,_Pred)
		{	
		}

		template<class _InIt>
		PriorityQueue(_InIt _First, _InIt _Last, const _Pr& _Pred, const _Container& _Cont)
			: std::priority_queue<_Ty, _Container, _Pr>(_First,_Last,_Pred,_Cont)
		{	
		}

		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<_Container, _Alloc>>>
			explicit PriorityQueue(const _Alloc& _Al)
			: std::priority_queue<_Ty, _Container, _Pr>(_All)
		{	
		}

		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<_Container, _Alloc>>>
			PriorityQueue(const _Pr& _Pred, const _Alloc& _Al)
			: std::priority_queue<_Ty, _Container, _Pr>(_Pred,_Al)
		{	
		}

		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<_Container, _Alloc>>>
			PriorityQueue(const _Pr& _Pred, const _Container& _Cont, const _Alloc& _Al)
			: std::priority_queue<_Ty, _Container, _Pr>(_Pred,_Cont,_Al)
		{	
		}

		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<_Container, _Alloc>>>
			PriorityQueue(const priority_queue& _Right, const _Alloc& _Al)
			: std::priority_queue<_Ty, _Container, _Pr>(_Right,_Al)
		{	
		}

		PriorityQueue(const _Pr& _Pred, _Container&& _Cont)
			: std::priority_queue<_Ty, _Container, _Pr>(_Pred,_Cont)
		{
		}

		template<class _InIt>
		PriorityQueue(_InIt _First, _InIt _Last, const _Pr& _Pred, _Container&& _Cont)
			: std::priority_queue<_Ty, _Container, _Pr>(_First,_Last,_Pred,_Cont)
		{	
		}
		
		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<_Container, _Alloc>>>
			PriorityQueue(const _Pr& _Pred, _Container&& _Cont, const _Alloc& _Al)
			: std::priority_queue<_Ty, _Container, _Pr>(_Pred,_Cont,_Al)
		{	
		}

		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<_Container, _Alloc>>>
			PriorityQueue(priority_queue&& _Right, const _Alloc& _Al)
			: std::priority_queue<_Ty, _Container, _Pr>(_Right,_Al)
		{	
		}


		void clear()
		{
			c.clear();
		}
	};
} 

