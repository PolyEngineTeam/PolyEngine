#pragma once
#include <any>
#include <typeindex>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

namespace Poly
{
    constexpr auto List = [](auto ...xs) {
        return [=](auto access) { return access(xs...); };
    };

    template <typename F, typename X, typename ... XS>
    void _runList(F f, X x, XS... xs)
    {
        f(x);
        _runList(f, xs...);
    }

    template <typename F, typename X>
        void _runList(F f, X x) { f(x); }

    constexpr auto runList = [](auto list, auto f) {
        list([f](auto... xs){ _runList(f, xs...); });
    };
    namespace RTTI2
    {
        template <typename T> struct RTTIinfo {
            constexpr static auto info = List();
        };

        using AttrMap = std::unordered_map<std::type_index, std::any>;

        template <typename T> struct SimpleAttribute
        {
            using container = std::vector<T>;
            static void add(AttrMap& m, T x)
            {
                auto i = m.find(typeid(T));
                if(i == m.end())
                    i = m.emplace(typeid(T), container{}).first;
                std::any_cast<container>(&i->second)->emplace_back(x);
            }
        };

        template <typename T> struct DerivedAttribute
        {
            using container = std::vector<std::shared_ptr<T>>;
            template <typename TT> static void add(AttrMap& m, TT x)
            {
                static_assert(std::is_base_of<T,TT>::value, "invalid type");
                auto i = m.find(typeid(T));
                if(i == m.end())
                    i = m.emplace(typeid(T), container{}).first;
				std::any_cast<container>(&i->second)->emplace_back(std::shared_ptr<T>{
					new TT{ x },
						[](T* p) { delete static_cast<TT*>(p); }
				});
            }
        };

        template <typename T> struct UniqueAttribute
        {
            using container = T;
            static void add(AttrMap& m, T x)
            {
				if (!m.try_emplace(typeid(T), x).second) {}
                    //throw std::runtime_error("inserting second UniqueAttribute");
            }
        };

        template <typename T> struct AttrType {
            using type = SimpleAttribute<T>;
        };

		struct classname { const char* name; };
        template <> struct AttrType<classname> {
            using type = UniqueAttribute<classname>;
        };

        struct TypeInfo;

        struct TypeManager {
            static TypeManager& get()
            {
                static TypeManager instance{};
                return instance;
            }
            template <typename T> const TypeInfo& registerOrGetType();
            std::unordered_map<std::string, std::shared_ptr<TypeInfo>> byName;
            std::unordered_map<std::type_index, std::shared_ptr<TypeInfo>> byId;
			TypeManager(const TypeManager& rhs) = delete;
			TypeManager& operator=(const TypeManager& rhs) = delete;
            private:
            TypeManager() = default;
        };

		namespace { struct baseClassBase {}; }
		template <> struct AttrType<baseClassBase> { using type = DerivedAttribute<baseClassBase>; };
        /* this is the only attribute that will not be included in TypeInfo
         * it is empty anyway; insted it will be used to store pointers to
         * store base class pointers */
		template <typename T> struct baseclass : baseClassBase { using type = T; };
		template <typename T> struct AttrType<baseclass<T>> { using type = DerivedAttribute<baseClassBase>; };

        struct TypeInfo {
            std::type_index id;
            std::type_index listId;
            std::vector<std::reference_wrapper<const TypeInfo>> bases; //maybe weak_ptr
            AttrMap attrs;
            std::function<void*()> construct;
            std::function<void(void*)> destroy;
            TypeInfo()
				: id{typeid(void)}
                , listId{typeid(void)}
                , bases{}
                , attrs{}
                , construct{nullptr}
                //, serialize{nullptr}
                {}
            template <typename T> void init()
            {
                runList(RTTIinfo<T>::info, [this](auto x){
                    using xT = decltype(x);
                    if constexpr (std::is_base_of<baseClassBase, xT>::value) {
                        using type = typename xT::type;
                        bases.emplace_back(TypeManager::get().registerOrGetType<type>());
                    } else {
                        AttrType<xT>::type::add(attrs, x);
                    }
                });
                id = std::type_index{typeid(T)};
                listId = std::type_index{typeid(RTTIinfo<T>::info)};
				if constexpr (std::is_default_constructible<T>::value)
					construct = [](){ return new T; };
				if constexpr (std::is_destructible<T>::value)
					destroy = [](void* p) { delete (T*)p; };
            }
            template <typename T> const typename AttrType<T>::type::container * const get() const
            { 
                auto i = attrs.find(typeid(T));
                return i != attrs.end() ? std::any_cast<typename AttrType<T>::type::container>(&i->second) : nullptr;
            }
        };
        template <typename T> const TypeInfo& TypeManager::registerOrGetType()
        {
            const static TypeInfo& ti = [this](){
                if(byId.count(typeid(T))) {
                    TypeInfo& ti = *byId.at(typeid(T));
                    // not sure if this is possible to happen
                    if(ti.listId != std::type_index{typeid(RTTIinfo<T>::info)})
                        throw std::runtime_error("attempting to register list with different list");
                    return ti;
                }
                auto p = std::make_shared<TypeInfo>();
                p->init<T>();
                //insert into byName only if name exists
                runList(RTTIinfo<T>::info, [&, this](auto x) {
                    if constexpr (std::is_same<classname, decltype(x)>::value) {
                        if(byName.count(x.name)) {
                            std::string s = "different class with name ";
                            s += x.name;
                            s += " is alredy registered";
                            throw std::runtime_error(s);
                        }
                        byName.insert_or_assign(x.name, p);
                    }
                });
                byId.insert_or_assign(p->id, p);
				return *p;
            }();
            return ti;
        }

        template <typename T> bool isSame(const TypeInfo& ti)
        {
            return std::type_index{typeid(T)} == ti.id;
        }
        
        template <typename T> bool isDerived(const TypeInfo& ti)
        {
            return std::any_of(ti.bases.begin(), ti.bases.end(), [&](const TypeInfo& base) {
                return base.id == std::type_index{typeid(T)} || isDerived<T>(base); //dfs
            });
        }

        template <typename T> bool isDerivedOrSame(const TypeInfo& ti)
        {
            return isSame<T>(ti) || isDerived<T>(ti);
        }

        struct RTTIBase {
            const TypeInfo& typeInfo;
			RTTIBase() = delete;
			RTTIBase(const TypeInfo& ti) : typeInfo(ti) {}
            template <typename T> bool isOfType() { return isDerivedOrSame<T>(typeInfo); }
			~RTTIBase() {}
        };
		
		//for some reason this doesn't work, maybe someone will enlighten me why
		template <typename T> struct RTTIBaseT : public virtual RTTIBase {
			RTTIBaseT() : RTTIBase(TypeManager::get().registerOrGetType<T>()) {}
		};
    }
}
