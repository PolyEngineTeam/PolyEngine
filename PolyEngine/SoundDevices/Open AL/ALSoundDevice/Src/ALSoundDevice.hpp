#pragma once

#include <alc.h>
#include <al.h>

#include <ISoundDevice.hpp>
#include <SoundResource.hpp>
#include <Dynarray.hpp>
#include <EnumUtils.hpp>

namespace Poly
{
	//---------------------------------------------------------------------------------------------------
	class ALSoundDeviceInitializationException : public BaseObject<>, public std::exception
	{
	public:
		ALSoundDeviceInitializationException() {}
	};

	//---------------------------------------------------------------------------------------------------
	class SOUND_DEVICE_DLLEXPORT ALSoundDevice : public ISoundDevice
	{
	public:
		ALSoundDevice();
		~ALSoundDevice();

		void Init() override;
		void RenderWorld(World* world) override;

		void SetDevice(const String& device) override;
		String GetCurrentDevice() override;
		Dynarray<String> GetAvailableDevices() override;

		void PushSoundResource(SoundEmitterComponent* emitter, const SoundResource& res) override;
		void PopSoundResource(SoundEmitterComponent* emitter) override;
		int GetProcessedBuffersCount(SoundEmitterComponent* emitter) override;
		int GetQueuedBuffersCount(SoundEmitterComponent* emitter) override;

	private:
		ALCdevice* Device;
		ALCcontext* Context;

		const EnumArray<int, eSoundSampleFormat> FormatMap{ 
			{ eSoundSampleFormat::MONO8, AL_FORMAT_MONO8 }, 
			{ eSoundSampleFormat::MONO16, AL_FORMAT_MONO16 }, 
			{ eSoundSampleFormat::STEREO8, AL_FORMAT_STEREO8 }, 
			{ eSoundSampleFormat::STEREO16, AL_FORMAT_STEREO16 } };
	};
} // namespace Poly

  //---------------------------------------------------------------------------------------------------
extern "C"
{
	SOUND_DEVICE_DLLEXPORT Poly::ISoundDevice* __stdcall PolyCreateSoundDevice();
}