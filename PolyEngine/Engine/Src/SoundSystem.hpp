#pragma once

namespace Poly
{
	class World;
	class SoundResource;
	class UniqueID;
	class SoundEmitterComponent;
	class SoundListenerComponent;
	enum class eResourceSource;

	namespace SoundSystem
	{
		void ENGINE_DLLEXPORT SoundPhase(World*);

		// Device
		void ENGINE_DLLEXPORT SetDevice(const String&);
		const ENGINE_DLLEXPORT String& GetDeviceName();
		const ENGINE_DLLEXPORT Dynarray<String>& GetAvailableDevices();

		// Context
		void ENGINE_DLLEXPORT SetWorldCurrent(World*);

		// Emitter
		void ENGINE_DLLEXPORT PlayEmitter(World*, const UniqueID&);
		void ENGINE_DLLEXPORT ReplayEmitter(World*, const UniqueID&);
		void ENGINE_DLLEXPORT PauseEmitter(World*, const UniqueID&);
		void ENGINE_DLLEXPORT StopEmitter(World*, const UniqueID&);
		void ENGINE_DLLEXPORT LoopEmitter(World*, const UniqueID&);
		void ENGINE_DLLEXPORT UnLoopEmitter(World*, const UniqueID&);
		void ENGINE_DLLEXPORT SetEmitterGain(World*, const UniqueID&, float);
		void ENGINE_DLLEXPORT SetEmitterFrequency(World*, const UniqueID&, float);
		void ENGINE_DLLEXPORT SetEmitterOffsetInSeconds(World*, const UniqueID&, float);
		void ENGINE_DLLEXPORT SetEmitterOffsetInSamples(World*, const UniqueID&, size_t);
		void ENGINE_DLLEXPORT SetEmitterOffsetInBytes(World*, const UniqueID&, size_t);
		void ENGINE_DLLEXPORT SetEmitterSource(World*, const UniqueID&, const String&, eResourceSource);
		/*void ENGINE_DLLEXPORT QueueEmitterSource(World*, const UniqueID&, const String&);
		void ENGINE_DLLEXPORT DequeueEmitterSource(World*, const UniqueID&, const String&);
		void ENGINE_DLLEXPORT SetEmitterDistanceModel(World*, const UniqueID&, void(*fun)(SoundEmitterComponent, SoundListenerComponent));
		*/
		// Listener
		void ENGINE_DLLEXPORT ActivateListener(World*, const UniqueID&);
		void ENGINE_DLLEXPORT DeactivateListener(World*, const UniqueID&);
		void ENGINE_DLLEXPORT SetListenerGain(World*, const UniqueID&, float);
	}
} // namespace Poly
