#pragma once

// core
#include <Dynarray.hpp>
#include <BaseObject.hpp>
#include <UniqueID.hpp>

namespace Poly 
{
	class World;
	class String;

	class SoundResource;
	class SoundEmitterComponent;

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ISoundBufferProxy : public BaseObject<>
	{
	public:
		ISoundBufferProxy() {}
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ISoundEmitterDataProxy : public BaseObject<>
	{
	public:
		ISoundEmitterDataProxy() {}
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ISoundListenerDataProxy : public BaseObject<>
	{
	public:
		ISoundListenerDataProxy() {}
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ISoundDevice : public BaseObject<> 
	{
	public:
		virtual void Init() = 0;
		virtual void RenderWorld(World* world) = 0;

		virtual void SetDevice(const String& device) = 0;
		virtual String GetCurrentDevice() = 0;
		virtual Dynarray<String> GetAvailableDevices() = 0;

		virtual void PushSoundResource(SoundEmitterComponent* emitter, const SoundResource& res) = 0;
		virtual void PopSoundResource(SoundEmitterComponent* emitter) = 0;
		virtual int GetProcessedBuffersCount(SoundEmitterComponent* emitter) = 0;
		virtual int GetQueuedBuffersCount(SoundEmitterComponent* emitter) = 0;
	};
} // namespace Poly