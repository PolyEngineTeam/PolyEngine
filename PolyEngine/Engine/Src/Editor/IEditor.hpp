#pragma once

#include <Core.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT IEditor
	{
	public:
		virtual const Poly::String& GetAssetsPathConfigPath() const = 0;
		virtual void Init() = 0;
		virtual void Deinit() = 0;
	};
} // namespace Poly