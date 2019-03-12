#pragma once

#include <Defines.hpp>
#include <Memory/IterablePoolAllocator.hpp>
#include <RTTI/RTTI.hpp>

#if defined(_ENGINE)
#    define EXPORT_TEMPLATE
#else
#    define EXPORT_TEMPLATE extern
#endif

namespace Poly
{

	namespace Impl
	{
		/// <summary>Component ID generator.
		/// Class template that allows for creating groups of types.
		/// In such group every type is uniquely identified by static int value
		/// Use different specializations to create separate sets of identifiers.
		/// </summary>

		/// <para>Example:</para>
		/// <code>
		/// using engine_components_group = ComponentIDGenerator<struct EngineComponents>;
		/// using world_engine_components_group = ComponentIDGenerator<struct EngineWorldComponents>;
		/// </code>

		/// <para>
		/// A component identified by this generator and used across dll linking
		/// (f.ex. created in Engine and being used in Game)
		/// MUST(!) define a REGISTER_COMPONENT macro under it's declaration
		/// (i.e. under component class in .hpp file).
		/// Otherwise the two compilation units (f.ex. Engine and Game)
		/// won't know about each other templates (i.e. about unique ids for components).
		/// Definitions are kept in ComponentIDGeneratorImpl.hpp file, which is included once
		/// in ECS/ComponentBase.hpp. This allows compiler to create a definition
		/// of ComponentIDGenerator::GetComponentTypeID<U>() type (where U is a type of the component)
		/// where the component is used for the first time (f.ex. created).
		/// https://itk.org/Wiki/Proposals:Explicit_Instantiation#DLL_Symbol_Resolution_on_Windows
		/// </para>
		class ENGINE_DLLEXPORT ComponentIDGenerator
		{
			static size_t GenerateID() noexcept;

		public:
			///<summary>
			/// Returns an unique identifier for the given type.
			///</summary>
			template<typename U>
			static size_t GetComponentTypeID() noexcept;
		};
	}

	using ComponentsIDGroup = Impl::ComponentIDGenerator;

	//------------------------------------------------------------------------------
	/// <summary>Returns statically set component type ID.</summary>
	/// <tparam name="T">Type of requested component.</tparam>
	/// <returns>Associated ID.</returns>
	template<typename T> size_t GetComponentID() noexcept
	{
		return ComponentsIDGroup::GetComponentTypeID<T>();
	}

	class ENGINE_DLLEXPORT ComponentManager : public BaseObject<>
	{
	public:
		static ComponentManager& Get();

		ComponentManager() = default;
		~ComponentManager() override
		{
			IDToCreatorMap.clear();
		}

		ComponentManager(const ComponentManager&) = delete;
		ComponentManager(ComponentManager&&) = delete;
		ComponentManager operator=(const ComponentManager&) = delete;
		ComponentManager operator=(ComponentManager&&) = delete;

		template<typename T>
		void RegisterComponent()
		{
			size_t id = ::Poly::GetComponentID<T>();
			RTTI::TypeInfo typeinfo = RTTI::TypeInfo::Get<T>();
			TypeToIDMap.insert({ typeinfo, id });
			IDToTypeMap.insert({ id, typeinfo});
			std::function<IterablePoolAllocatorBase*(size_t)> allocator = [](size_t count) { return static_cast<IterablePoolAllocatorBase*>(new IterablePoolAllocator<T>(count)); };
			IDToCreatorMap.insert({ id, allocator });
		}

		Optional<size_t> GetComponentID(const RTTI::TypeInfo& typeinfo) const;
		RTTI::TypeInfo GetComponentType(size_t id) const;
		Dynarray<std::pair<RTTI::TypeInfo, size_t>> GetComponentTypesList() const;

		IterablePoolAllocatorBase* CreateAllocator(size_t id, size_t componentCount) const;

		void Clear();
	private:
		std::map<RTTI::TypeInfo, size_t> TypeToIDMap;
		std::map < size_t, RTTI::TypeInfo> IDToTypeMap;
		std::map < size_t, std::function<IterablePoolAllocatorBase*(size_t)>> IDToCreatorMap;
	};
}

template <typename T>
struct AutoRegisterComponent {
	AutoRegisterComponent() {
#if defined(_WIN32)
		STATIC_ASSERTE(false, "Component not defined in rtti system!");
#else
		ASSERTE(false, "Component not defined in rtti system!");
#endif
	}
};

#define REGISTER_COMPONENT(GROUP, COMPONENT) \
	EXPORT_TEMPLATE template size_t ENGINE_DLLEXPORT GROUP::GetComponentTypeID<COMPONENT>() noexcept;

#define RTTI_DECLARE_COMPONENT(TYPE) \
	public: \
	size_t GetComponentID() const override { return ::Poly::GetComponentID<TYPE>(); }	\
	RTTI_DECLARE_TYPE_DERIVED(TYPE, ::Poly::ComponentBase)

#define RTTI_DEFINE_COMPONENT(TYPE) \
	template <> struct AutoRegisterComponent<TYPE> { AutoRegisterComponent() \
	{ ::Poly::ComponentManager::Get().RegisterComponent<TYPE>(); } }; \
	static const AutoRegisterComponent<TYPE> RTTI_CAT(autoRegisterComponent, __COUNTER__); \
	RTTI_DEFINE_TYPE(TYPE)
