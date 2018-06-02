#pragma once

namespace Poly
{
	class World;
	class SoundResource;
	class Entity;
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
		void ENGINE_DLLEXPORT PlayEmitter(World*, Entity*);
		void ENGINE_DLLEXPORT ReplayEmitter(World*, Entity*);
		void ENGINE_DLLEXPORT PauseEmitter(World*, Entity*);
		void ENGINE_DLLEXPORT StopEmitter(World*, Entity*);
		void ENGINE_DLLEXPORT LoopEmitter(World*, Entity*);
		void ENGINE_DLLEXPORT UnLoopEmitter(World*, Entity*);
		void ENGINE_DLLEXPORT SetEmitterGain(World*, Entity*, float);
		void ENGINE_DLLEXPORT SetEmitterFrequency(World*, Entity*, float);
		void ENGINE_DLLEXPORT SetEmitterOffsetInSeconds(World*, Entity*, float);
		void ENGINE_DLLEXPORT SetEmitterOffsetInSamples(World*, Entity*, size_t);
		void ENGINE_DLLEXPORT SetEmitterOffsetInBytes(World*, Entity*, size_t);
		void ENGINE_DLLEXPORT SetEmitterSource(World*, Entity*, const String&, eResourceSource);
		/*void ENGINE_DLLEXPORT QueueEmitterSource(World*, Entity*, const String&);
		void ENGINE_DLLEXPORT DequeueEmitterSource(World*, Entity*, const String&);
		void ENGINE_DLLEXPORT SetEmitterDistanceModel(World*, Entity*, void(*fun)(SoundEmitterComponent, SoundListenerComponent));
		*/
		// Listener
		void ENGINE_DLLEXPORT ActivateListener(World*, Entity*);
		void ENGINE_DLLEXPORT DeactivateListener(World*, Entity*);
		void ENGINE_DLLEXPORT SetListenerGain(World*, Entity*, float);
		bool ENGINE_DLLEXPORT IsEmmiterActive(World*, Entity*);
	}
} // namespace Poly
