#pragma once

// core
#include <Dynarray.hpp>
#include <BaseObject.hpp>

namespace Poly 
{
	class ENGINE_DLLEXPORT ISoundDataHolderProxy : public BaseObject<>
	{
	public:
		ISoundDataHolderProxy() {}
	};

	class World;
	class String;

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ISoundDevice : public BaseObject<> 
	{
	public:
		virtual void Init() = 0;
		virtual void RenderWorld(World* world) = 0;

		virtual void SetDevice(const String& device) = 0;
		virtual String GetCurrentDevice() = 0;
		virtual Dynarray<String> GetAvailableDevices() = 0;
	};
} // namespace Poly