#pragma once

namespace Poly
{
	class ENGINE_DLLEXPORT CoreConfig
	{
	public:
		bool DebugNormalsFlag = false;
	};
	ENGINE_DLLEXPORT extern CoreConfig gCoreConfig;
}