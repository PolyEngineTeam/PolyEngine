#pragma once

namespace Poly
{
	class ENGINE_DLLEXPORT CoreConfig
	{
	public:

		// Rendering
		bool DebugNormalsFlag = false;
		bool WireframeRendering = false;
	};
	ENGINE_DLLEXPORT extern CoreConfig gCoreConfig;
}