#pragma once

#include <pe/Defines.hpp>

namespace pe::core::storage
{
	template<class _Ty,class _Pr = std::less<typename std::vector<_Ty>::value_type> >
	class PriorityQueue final : public std::priority_queue<_Ty,std::vector<_Ty>,_Pr>
	{
	public:
		
		PriorityQueue()
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>()
		{
		}

		explicit PriorityQueue(const _Pr& _Pred)
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>(_Pred)
		{
		}


		template<class _InIt>
		PriorityQueue(_InIt _First, _InIt _Last)
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>(_First,_Last)
		{	
		}

		template<class _InIt>
		PriorityQueue(_InIt _First, _InIt _Last, const _Pr& _Pred)
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>(_First,_Last,_Pred)
		{	
		}

	
		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<_Ty>, _Alloc>>>
			explicit PriorityQueue(const _Alloc& _Al)
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>(_Al)
		{	
		}

		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<_Ty>, _Alloc>>>
			PriorityQueue(const _Pr& _Pred, const _Alloc& _Al)
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>(_Pred,_Al)
		{	
		}

		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<_Ty>, _Alloc>>>
			PriorityQueue(const std::priority_queue<_Ty, std::vector<_Ty>, _Pr>& _Right, const _Alloc& _Al)
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>(_Right,_Al)
		{	
		}

	
		template<class _Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<_Ty>, _Alloc>>>
			PriorityQueue(std::priority_queue<_Ty, std::vector<_Ty>, _Pr>&& _Right, const _Alloc& _Al)
			: std::priority_queue<_Ty, std::vector<_Ty>, _Pr>(_Right,_Al)
		{	
		}


		void clear()
		{
			this->c.clear();
		}
	};
} 

