#pragma once

#include "Defines.hpp"
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	template<typename PrimaryComponent, typename... SecondaryComponents>
	struct IteratorProxy;

	class IEntityIteratorPolicy : public BaseObject<>
	{
		public:
			virtual bool operator==(const IEntityIteratorPolicy&) const = 0;
			virtual bool operator!=(const IEntityIteratorPolicy&) const = 0;
			virtual ComponentBase* Get() const = 0;
			virtual void Increment() = 0;
			virtual bool IsValid() const = 0;
	};

	template<typename PrimaryComponent, typename... SecondaryComponents>
	class ComponentIterator : public BaseObject<>,
							public std::iterator<std::forward_iterator_tag, std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type...>>	
	{
		public:
			explicit ComponentIterator(std::unique_ptr<IEntityIteratorPolicy> iter) : Iter(std::move(iter))
			{
				UpdateIterator();
			}

			bool operator==(const ComponentIterator& rhs) const { return *GetIteratorPolicy() == *rhs.GetIteratorPolicy(); } 
			bool operator!=(const ComponentIterator& rhs) const { return !(*this == rhs); }

			const std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > operator*() const 
			{
				ComponentBase* base = GetIteratorPolicy()->Get();
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
			IEntityIteratorPolicy* GetIteratorPolicy() const
			{
				return Iter.get();
			}

		private:
			void Increment()
			{
				ComponentBase* base = GetIteratorPolicy()->Get();
				PrimaryComponent* primary = base->GetSibling<PrimaryComponent>();
				ASSERTE(primary, "Primary component is nullptr!");

				GetIteratorPolicy()->Increment();
				UpdateIterator();
			}
			
			void UpdateIterator()
			{
				ComponentBase* component = GetIteratorPolicy()->Get();
				while( GetIteratorPolicy()->IsValid() && !HasSiblings<PrimaryComponent, SecondaryComponents...>(component))
				{
					GetIteratorPolicy()->Increment();
					component = GetIteratorPolicy()->Get();
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

			std::unique_ptr<IEntityIteratorPolicy> Iter;
	};
}