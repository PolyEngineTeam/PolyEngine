#pragma once

#include <BaseObject.hpp>

namespace Poly 
{
	class World;
	class String;

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ISoundDevice : public BaseObject<> 
	{
	public:
		virtual void Init() = 0;
		virtual void Close() = 0;
		virtual void RenderWorld(World* world) = 0;

		virtual void SetDevice(const String& device) = 0;
		virtual const String& GetCurrentDevice() = 0;
		virtual const Dynarray<String>& GetAvailableDevices() = 0;
	};

}