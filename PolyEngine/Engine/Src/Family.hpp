#pragma once

#if defined(_ENGINE)
#    define EXPIMP_TEMPLATE
#else
#    define EXPIMP_TEMPLATE extern
#endif

#define COMPONENT_INS(FAMILY, COMPONENT) \
	EXPIMP_TEMPLATE template size_t ENGINE_DLLEXPORT FAMILY::type_id<COMPONENT>();

namespace Poly
{
	/// <summary>Identifier generator.
	/// Class template that allows for creating groups of types.
	/// In such group every type is uniquely identified by static int value
	/// Use different specializations to create separate sets of identifiers.
	/// </summary>

	/// <para>Example:</para>
	/// <code>
	/// using engine_components_family = Family<struct EngineComponents>;
	/// using world_engine_components_family = Family<struct EngineWorldComponents>;
	/// </code>

	/// <para>
	/// A component identified by this generator and used across dll linking
	/// (f.ex. created in Engine and being used in Game)
	/// MUST(!) define a COMPONENT_INS macro under it's declaration
	/// (i.e. under component class in .hpp file).
	/// Otherwise the two compilation units (f.ex. Engine and Game)
	/// won't know about each other templates (i.e. about unique ids for components)
	/// Also in .cpp files where components are being used/created
	/// a include of "Family.inc" file is needed.
	/// https://itk.org/Wiki/Proposals:Explicit_Instantiation#DLL_Symbol_Resolution_on_Windows
	/// </para>

	/// <para>
	/// Definitions are kept in Family.inc file. That means that this '.inc' file must
	/// be included in every .cpp file where template of Family with specific component type
	/// is used for the first time. This allows compiler to create a definition
	/// of Family::type_id<U>() type (where U is typeof the component).
	/// </para>
	template<typename T>
	class Family
	{
		static size_t identifier();

	public:
		///<summary>
		/// Returns an unique identifier for the given type.
		///</summary>
		template<typename U>
		static size_t type_id();
	};

	using ComponentsFamily = Family<struct ComponentsFamilyType>;
	using WorldComponentsFamily = Family<struct WorldComponentsFamilyType>;
}