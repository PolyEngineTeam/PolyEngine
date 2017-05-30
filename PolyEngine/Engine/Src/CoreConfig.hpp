#pragma once

namespace Poly
{
	class ENGINE_DLLEXPORT CoreConfig
	{
	public:

		// Rendering
		bool DebugNormalsFlag = false;
		bool WireframeRendering = false;
		bool DisplayFPS = true;
	};
	ENGINE_DLLEXPORT extern CoreConfig gCoreConfig;
}