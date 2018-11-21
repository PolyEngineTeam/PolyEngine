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
	};

	template<typename PrimaryComponent, typename... SecondaryComponents>
	class ComponentIterator : public BaseObject<>,
							public std::iterator<std::forward_iterator_tag, std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type...>>	
	{
		public:
			explicit ComponentIterator(std::unique_ptr<IEntityIteratorHelper> iter)
			{
				Iter(std::move(iter));
				FillCache();
			}
			bool operator==(const  ComponentIterator& rhs) const { return Iter.get() == rhs.Get(); }
			bool operator!=(const  ComponentIterator& rhs) const { return Iter.get() != rhs.Get(); } //cannot find the correct overload

			std::tuple<typename std::add_pointer<PrimaryComponent>::type, Dynarray<ComponentBase*> > operator*() //canot be const if  change cache
			{
				if (bCacheValid)
					return Cache;
				FillCache();
				return Cache;
			}
			std::tuple<typename std::add_pointer<PrimaryComponent>::type, Dynarray<ComponentBase*> > operator->()
			{
				if (bCacheValid)
					return Cache;
				FillCache();
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

			void FillCache()
			{
				Entity* ent = Iter.get()->get();
				PrimaryComponent* primary = ent->GetComponent<PrimaryComponent>();
				Cache(std::make_tuple(primary, primary->template GetSibling<SecondaryComponents>()...));
				bCacheValid = true;
			}

			friend struct IteratorProxy<PrimaryComponent, SecondaryComponents...>;

			bool bCacheValid;
			std::unique_ptr<IEntityIteratorHelper> Iter;
			std::tuple<typename std::add_pointer<PrimaryComponent>::type, Dynarray<ComponentBase*> > Cache;
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
		ComponentIterator<PrimaryComponent, SecondaryComponents...> End()
		{
			return ComponentIterator<PrimaryComponent, SecondaryComponents...>(S->MakeSceneComponentIteratorHelper<PrimaryComponent, SecondaryComponents...>());
		}
		auto begin() { return Begin(); }
		auto end() { return End(); }
		Scene* const S;
	};
}