#pragma once

#include <pe/Defines.hpp>

namespace pe::core::storage
{
	template<class Ty,class Pr = std::less<typename std::vector<Ty>::value_type> >
	class PriorityQueue final : public std::priority_queue<Ty,std::vector<Ty>,Pr>
	{
	public:
		
		PriorityQueue()
			: std::priority_queue<Ty, std::vector<Ty>, Pr>()
		{
		}

		PriorityQueue(const std::vector<Ty>& _Cont)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(Pr(), _Cont)
		{	
		}

		PriorityQueue(const Pr& Pred, const std::vector<Ty>& _Cont)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(Pred,_Cont)
		{	
		}


		explicit PriorityQueue(const Pr& Pred)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(Pred)
		{
		}


		template<class InIt>
		PriorityQueue(InIt _First, InIt _Last)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(_First,_Last)
		{	
		}

		template<class InIt>
		PriorityQueue(InIt _First, InIt _Last, const Pr& Pred)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(_First,_Last,Pred)
		{	
		}

	
		template<class Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<Ty>, Alloc>>>
			explicit PriorityQueue(const Alloc& _Al)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(_Al)
		{	
		}

		template<class Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<Ty>, Alloc>>>
			PriorityQueue(const Pr& Pred, const Alloc& _Al)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(Pred,_Al)
		{	
		}

		template<class Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<Ty>, Alloc>>>
			PriorityQueue(const std::priority_queue<Ty, std::vector<Ty>, Pr>& _Right, const Alloc& _Al)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(_Right,_Al)
		{	
		}

	
		template<class Alloc,
			class = std::enable_if_t<std::uses_allocator_v<std::vector<Ty>, Alloc>>>
			PriorityQueue(std::priority_queue<Ty, std::vector<Ty>, Pr>&& _Right, const Alloc& _Al)
			: std::priority_queue<Ty, std::vector<Ty>, Pr>(_Right,_Al)
		{	
		}


		void clear()
		{
			this->c.clear();
		}
	};
} 

