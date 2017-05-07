#pragma once

#include <exception>

#include <Defines.hpp>
#include <String.hpp>
#include <RefCountedBase.hpp>

namespace Poly
{
	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ResourceLoadFailedException : public std::exception
	{
	public:
		ResourceLoadFailedException() {}
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ResourceBase : public RefCountedBase
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