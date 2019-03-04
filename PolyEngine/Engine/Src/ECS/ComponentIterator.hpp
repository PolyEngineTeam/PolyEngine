#pragma once

#include "Defines.hpp"

namespace Poly
{
	template<typename T>
	class Dynarray;
	class ComponentBase;
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
			explicit ComponentIterator(std::unique_ptr<IEntityIteratorHelper> iter) : Iter(std::move(iter))
			{
				bCacheValid = false;
			}
			bool operator==(const  ComponentIterator& rhs) const { return GetIteratorHelper()->get() == rhs.GetIteratorHelper()->get(); } //they never  trigger underlying overload, pointer comparison ongoing
			bool operator!=(const  ComponentIterator& rhs) const { return !(GetIteratorHelper()->get() == rhs.GetIteratorHelper()->get()); }

			const std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... >& operator*() const //canot be const if  change cache
			{
				return GetCache();
			}
			const std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... >& operator->() const
			{
				return GetCache();
			}

			ComponentIterator& operator++() { Increment(); return *this; }
			ComponentIterator operator++(int) { ComponentIterator ret(Iter); Increment(); return ret; } //test for double incrementing etc

		protected:
			IEntityIteratorHelper* GetIteratorHelper() const
			{
				return Iter.get();
			}

		private:
			void Increment()
			{
				//ASSERTE(Iter.get()->isValid(), "Next Iterator is not valid!");	//currently not working as we never set it to nullptr?
				GetIteratorHelper()->increment();	
				bCacheValid = false;
			}

			void UpdateCache() const 
			{
				Entity* ent = GetIteratorHelper()->get();
				PrimaryComponent* primary = ent->GetComponent<PrimaryComponent>();
				ASSERTE(primary, "Primary component is nullptr!");

				if constexpr (!Trait::IsVariadicEmpty<SecondaryComponents...>::value)
				{
					Cache = std::make_tuple(primary, primary->template GetSibling<SecondaryComponents>()...);
				}
				else
				{
					Cache = std::make_tuple(primary);
				}

				bCacheValid = true;
			}

			const std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... >& GetCache() const
			{
				if (!bCacheValid)
					UpdateCache();
				return Cache;
			}

			friend struct IteratorProxy<PrimaryComponent, SecondaryComponents...>;

			mutable bool bCacheValid;
			std::unique_ptr<IEntityIteratorHelper> Iter;
			mutable std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type... > Cache;
	};
}