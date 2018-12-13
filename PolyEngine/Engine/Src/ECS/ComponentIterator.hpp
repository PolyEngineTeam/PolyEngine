#pragma once

#include "Defines.hpp"

namespace Poly
{
	template<typename T>
	class Dynarray;
	class ComponentBase;
	class Scene;
	template<typename PrimaryComponent, typename... SecondaryComponents>
	struct IteratorProxy;

	class IEntityIteratorHelper : public BaseObject<>
	{
		public:
			virtual bool operator==(const IEntityIteratorHelper&) const = 0;
			virtual bool operator!=(const IEntityIteratorHelper&) const = 0;
			virtual Entity* get() const = 0;
			virtual void increment() = 0;
			virtual bool isValid() const = 0;
	};

	template<typename PrimaryComponent, typename... SecondaryComponents>
	class ComponentIterator : public BaseObject<>,
							public std::iterator<std::forward_iterator_tag, std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type...>>	
	{
		public:
			explicit ComponentIterator(std::unique_ptr<IEntityIteratorHelper> iter)
			{
				Iter(std::move(iter));
				UpdateCache();
			}
			bool operator==(const  ComponentIterator& rhs) const { return Iter.get() == rhs.Get(); }
			bool operator!=(const  ComponentIterator& rhs) const { return !(Iter.get() == rhs.Get()); }

			std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > operator*() const //canot be const if  change cache
			{
				if (!bCacheValid)
					UpdateCache();
				// assert for all cmp (has all comp we require (primary,secondary))
				return Cache; //extract
			}
			std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > operator->() const
			{
				if (bCacheValid)
					return Cache;
				UpdateCache();
				return Cache;
			}

			ComponentIterator& operator++() { Increment(); return *this; }
			ComponentIterator operator++(int) { ComponentIterator ret(Iter); Increment(); return ret; } //test for double incrementing etc

			IEntityIteratorHelper* Get() const
			{
				return Iter.get();
			}
		private:
			void Increment()
			{
				Iter.increment();
				bCacheValid = false;
			}

			void UpdateCache() const
			{
				Entity* ent = Iter.get()->get();
				PrimaryComponent* primary = ent->GetComponent<PrimaryComponent>();
				Cache(std::make_tuple(primary, primary->template GetSibling<SecondaryComponents>()...)); //have to make proper unwrapping of this tuple
				bCacheValid = true;
			}

			friend struct IteratorProxy<PrimaryComponent, SecondaryComponents...>;

			mutable bool bCacheValid;
			std::unique_ptr<IEntityIteratorHelper> Iter;
			mutable std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > Cache; //cast what helper returned to tuples 
	};
	/// Iterator proxy
	template<typename PrimaryComponent, typename... SecondaryComponents>
	struct IteratorProxy : BaseObject<>
	{
		IteratorProxy(Scene* s) : S(s) {}
		ComponentIterator<PrimaryComponent, SecondaryComponents...> Begin()
		{
			return ComponentIterator<PrimaryComponent, SecondaryComponents...>(S->MakeSceneComponentIteratorHelper<PrimaryComponent, SecondaryComponents...>());
		}
		ComponentIterator<PrimaryComponent, SecondaryComponents...> End() //better pass scene and move this method inside component (13.12. need to be sure about h
		{
			return ComponentIterator<PrimaryComponent, SecondaryComponents...>(S->MakeSceneComponentIteratorHelper<PrimaryComponent, SecondaryComponents...>());
		}
		auto begin() { return Begin(); }
		auto end() { return End(); }
		Scene* const S;
	};
}