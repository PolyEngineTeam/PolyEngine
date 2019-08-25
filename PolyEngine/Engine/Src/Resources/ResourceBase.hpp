#pragma once

#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>
#include <pe/core/memory/RefCountedBase.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	SILENCE_MSVC_WARNING(4275, "Exporting stl may cause incompatibility. We use same CRT so it's ok.")
	class ENGINE_DLLEXPORT ResourceLoadFailedException : public ::pe::core::BaseObject<>, public std::exception
	{
	public:
		ResourceLoadFailedException() {}
	};
	UNSILENCE_MSVC_WARNING()

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ResourceBase : public ::pe::core::memory::RefCountedBase, public RTTIBase
	{
	public:
		const core::storage::String& GetPath() const { return Path; }

		ResourceBase() = default;
		ResourceBase(const ResourceBase&) = delete;
		ResourceBase& operator=(const ResourceBase&) = delete;

	protected:
		virtual ~ResourceBase() {}

	private:
		core::storage::String Path;

		template<typename T> friend class ResourceManager;
	};
}