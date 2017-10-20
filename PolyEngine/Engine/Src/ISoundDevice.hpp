#pragma once

#include <BaseObject.hpp>

namespace Poly 
{
	class World;

	//------------------------------------------------------------------------------
	class SOUND_DEVICE_DLLEXPORT ISoundListenerDeviceProxy : public BaseObject<>
	{
	};

	//------------------------------------------------------------------------------
	class SOUND_DEVICE_DLLEXPORT ISoundEmitterDeviceProxy : public BaseObject<>
	{
	};

	//------------------------------------------------------------------------------
	class SOUND_DEVICE_DLLEXPORT ISoundDevice : public BaseObject<> 
	{
	public:
		virtual void RenderWorld(World* world) = 0;
		virtual void Init() = 0;

		virtual std::unique_ptr<ISoundEmitterDeviceProxy> CreateEmitter() = 0;
		virtual std::unique_ptr<ISoundListenerDeviceProxy> CreateListener() = 0;
	};

}