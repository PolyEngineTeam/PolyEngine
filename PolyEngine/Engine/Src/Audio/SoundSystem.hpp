#pragma once

namespace Poly
{
	class Scene;
	class SoundResource;
	class Entity;
	class SoundEmitterComponent;
	class SoundListenerComponent;
	enum class eResourceSource;

	namespace SoundSystem
	{
		void ENGINE_DLLEXPORT SoundPhase(Scene*);

		// Device
		void ENGINE_DLLEXPORT SetDevice(const String&);
		const ENGINE_DLLEXPORT String& GetDeviceName();
		const ENGINE_DLLEXPORT Dynarray<String>& GetAvailableDevices();

		// Context
		void ENGINE_DLLEXPORT SetWorldCurrent(Scene*);

		// Emitter
		void ENGINE_DLLEXPORT PlayEmitter(Scene*, Entity*);
		void ENGINE_DLLEXPORT ReplayEmitter(Scene*, Entity*);
		void ENGINE_DLLEXPORT PauseEmitter(Scene*, Entity*);
		void ENGINE_DLLEXPORT StopEmitter(Scene*, Entity*);
		void ENGINE_DLLEXPORT LoopEmitter(Scene*, Entity*);
		void ENGINE_DLLEXPORT UnLoopEmitter(Scene*, Entity*);
		void ENGINE_DLLEXPORT SetEmitterGain(Scene*, Entity*, float);
		void ENGINE_DLLEXPORT SetEmitterFrequency(Scene*, Entity*, float);
		void ENGINE_DLLEXPORT SetEmitterOffsetInSeconds(Scene*, Entity*, float);
		void ENGINE_DLLEXPORT SetEmitterOffsetInSamples(Scene*, Entity*, size_t);
		void ENGINE_DLLEXPORT SetEmitterOffsetInBytes(Scene*, Entity*, size_t);
		void ENGINE_DLLEXPORT SetEmitterSource(Scene*, Entity*, const String&, eResourceSource);
		/*void ENGINE_DLLEXPORT QueueEmitterSource(Scene*, Entity*, const String&);
		void ENGINE_DLLEXPORT DequeueEmitterSource(Scene*, Entity*, const String&);
		void ENGINE_DLLEXPORT SetEmitterDistanceModel(Scene*, Entity*, void(*fun)(SoundEmitterComponent, SoundListenerComponent));
		*/
		// Listener
		void ENGINE_DLLEXPORT ActivateListener(Scene*, Entity*);
		void ENGINE_DLLEXPORT DeactivateListener(Scene*, Entity*);
		void ENGINE_DLLEXPORT SetListenerGain(Scene*, Entity*, float);
		bool ENGINE_DLLEXPORT IsEmmiterActive(Scene*, Entity*);
	}
} // namespace Poly
