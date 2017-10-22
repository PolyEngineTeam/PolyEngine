#pragma once

#include <BaseObject.hpp>

namespace Poly 
{
	class World;

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ISoundDevice : public BaseObject<> 
	{
	public:
		virtual void RenderWorld(World* world) = 0;
		virtual void Init() = 0;
	};

}