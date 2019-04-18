#pragma once

#include "Defines.hpp"
#include <ECS/Entity.hpp>

namespace Poly
{
	template<typename T>
	class Dynarray;
	class ComponentBase;
	template<typename PrimaryComponent, typename... SecondaryComponents>
	struct IteratorProxy;

	class IEntityIteratorPolicy : public BaseObject<>
	{
		public:
			virtual bool operator==(const IEntityIteratorPolicy&) const = 0;
			virtual bool operator!=(const IEntityIteratorPolicy&) const = 0;
			virtual Entity* Get() const = 0;
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
				//UpdateIterator();
			}

			bool operator==(const  ComponentIterator& rhs) const { return GetIteratorPolicy()->Get() == rhs.GetIteratorPolicy()->Get(); } 
			bool operator!=(const  ComponentIterator& rhs) const { return !(GetIteratorPolicy()->Get() == rhs.GetIteratorPolicy()->Get()); }

			const std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > operator*() const 
			{
				Entity* ent = GetIteratorPolicy()->Get();
				PrimaryComponent* primary = ent->GetComponent<PrimaryComponent>();
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
				Entity* ent = GetIteratorPolicy()->Get();
				PrimaryComponent* primary = ent->GetComponent<PrimaryComponent>();
				ASSERTE(primary, "Primary component is nullptr!");

				GetIteratorPolicy()->Increment();
				UpdateIterator();
			}
			
			void UpdateIterator()
			{
				Entity* ent = GetIteratorPolicy()->Get();
				while ( GetIteratorPolicy()->IsValid() && !HasComponents<PrimaryComponent, SecondaryComponents...>(ent) )
				{
					GetIteratorPolicy()->Increment();
					ent = GetIteratorPolicy()->Get();					
				}
			}

			template<int zero = 0>
			bool HasComponents(const Entity* entity) const { return true; }

			template<typename Component, typename... Rest>
			bool HasComponents(const Entity* entity) const 
			{
				return entity->template HasComponent<Component>() && HasComponents<Rest...>(entity);
			}

			friend struct IteratorProxy<PrimaryComponent, SecondaryComponents...>;

			std::unique_ptr<IEntityIteratorPolicy> Iter;
	};
}