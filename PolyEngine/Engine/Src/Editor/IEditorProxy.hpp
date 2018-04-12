#pragma once

#include <Core.hpp>

namespace Poly
{
	class ENGINE_DLLEXPORT IEditorProxy : BaseObject<>
	{
	public:
		IEditorProxy() {}

		virtual const Poly::String& GetAssetsPathConfigPath() const { return String::EMPTY; }
	};
} // namespace Poly