#pragma once

#include "Defines.hpp"
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	template<typename PrimaryComponent, typename... SecondaryComponents>
	struct IteratorProxy;

	class ENGINE_DLLEXPORT IComponentIteratorImpl : public BaseObject<>
	{
		public:
			virtual bool operator==(const IComponentIteratorImpl&) const = 0;
			virtual bool operator!=(const IComponentIteratorImpl&) const = 0;
			virtual ComponentBase* Get() const = 0;
			virtual void Increment() = 0;
			virtual bool IsValid() const = 0;
	};

	template<typename PrimaryComponent, typename... SecondaryComponents>
	class ComponentIterator : public BaseObject<>,
							public std::iterator<std::forward_iterator_tag, std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type...>>	
	{
		public:
			explicit ComponentIterator(std::unique_ptr<IComponentIteratorImpl> iter) : Iter(std::move(iter))
			{
				UpdateIterator();
			}

			bool operator==(const ComponentIterator& rhs) const { return *GetIteratorImpl() == *rhs.GetIteratorImpl(); } 
			bool operator!=(const ComponentIterator& rhs) const { return !(*this == rhs); }

			const std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > operator*() const 
			{
				ComponentBase* base = GetIteratorImpl()->Get();
				PrimaryComponent* primary = base->GetSibling<PrimaryComponent>();
				return std::make_tuple(primary, primary->template GetSibling<SecondaryComponents>()...);
			}
			const std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > operator->() const
			{
				return **this;
			}

			ComponentIterator& operator++() { Increment(); return *this; }
			ComponentIterator operator++(int) { ComponentIterator ret(Iter); Increment(); return ret; } 

		protected:
			IComponentIteratorImpl* GetIteratorImpl() const
			{
				return Iter.get();
			}

		private:
			void Increment()
			{
				ComponentBase* base = GetIteratorImpl()->Get();
				PrimaryComponent* primary = base->GetSibling<PrimaryComponent>();
				ASSERTE(primary, "Primary component is nullptr!");

				GetIteratorImpl()->Increment();
				UpdateIterator();
			}
			
			void UpdateIterator()
			{
				ComponentBase* component = GetIteratorImpl()->Get();
				while( GetIteratorImpl()->IsValid() && !HasSiblings<PrimaryComponent, SecondaryComponents...>(component))
				{
					GetIteratorImpl()->Increment();
					component = GetIteratorImpl()->Get();
				}
			}

			template<int zero = 0>
			bool HasSiblings(const ComponentBase* component) const { return true; }
			
			template<typename Component, typename... Rest>
			bool HasSiblings(const ComponentBase* component) const
			{
				return component->template GetSibling<Component>() && HasSiblings<Rest...>(component);
			}

			friend struct IteratorProxy<PrimaryComponent, SecondaryComponents...>;

			std::unique_ptr<IComponentIteratorImpl> Iter;
	};
}