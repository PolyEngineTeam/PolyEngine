#pragma once

namespace Poly
{
	/// <summary>This is class where all core configuration variables are placed</summary>
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