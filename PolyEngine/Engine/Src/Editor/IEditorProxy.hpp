#pragma once

#include <Core.hpp>

namespace Poly
{
	class IEditorProxy : BaseObject<>
	{
	public:
		IEditorProxy() {}

		virtual const Poly::String& GetAssetsPathConfigPath() {}
	};
} // namespace Poly