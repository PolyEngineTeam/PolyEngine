#pragma once
#include <pe/Defines.hpp>
#include <pe/core/storage/String.hpp>


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
		void ENGINE_DLLEXPORT SetDevice(const core::storage::String&);
		const ENGINE_DLLEXPORT core::storage::String& GetDeviceName();
		const ENGINE_DLLEXPORT std::vector<core::storage::String>& GetAvailableDevices();

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
		void ENGINE_DLLEXPORT SetEmitterSource(Scene*, Entity*, const core::storage::String&, eResourceSource);
		/*void ENGINE_DLLEXPORT QueueEmitterSource(Scene*, Entity*, const core::storage::String&);
		void ENGINE_DLLEXPORT DequeueEmitterSource(Scene*, Entity*, const core::storage::String&);
		void ENGINE_DLLEXPORT SetEmitterDistanceModel(Scene*, Entity*, void(*fun)(SoundEmitterComponent, SoundListenerComponent));
		*/
		// Listener
		void ENGINE_DLLEXPORT ActivateListener(Scene*, Entity*);
		void ENGINE_DLLEXPORT DeactivateListener(Scene*, Entity*);
		void ENGINE_DLLEXPORT SetListenerPosition(core::math::Vector);
		bool ENGINE_DLLEXPORT IsEmmiterActive(Scene*, Entity*);
	}
} // namespace Poly
