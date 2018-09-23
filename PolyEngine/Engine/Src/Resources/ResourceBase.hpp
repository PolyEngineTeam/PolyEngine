#pragma once

#include <Defines.hpp>
#include <Collections/String.hpp>
#include <Memory/RefCountedBase.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	SILENCE_MSVC_WARNING(4275, "Exporting stl may cause incompatibility. We use same CRT so it's ok.")
	class ENGINE_DLLEXPORT ResourceLoadFailedException : public BaseObject<>, public std::exception
	{
	public:
		ResourceLoadFailedException() {}
	};
	UNSILENCE_MSVC_WARNING()

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ResourceBase : public RefCountedBase, public RTTIBase
	{
	public:
		const String& GetPath() const { return Path; }

		ResourceBase() = default;
		ResourceBase(const ResourceBase&) = delete;
		ResourceBase& operator=(const ResourceBase&) = delete;

	protected:
		virtual ~ResourceBase() {}

	private:
		String Path;

		template<typename T> friend class ResourceManager;
	};
}