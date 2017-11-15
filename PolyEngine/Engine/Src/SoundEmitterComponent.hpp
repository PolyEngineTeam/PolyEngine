#pragma once

#include "ComponentBase.hpp"
#include "SoundResource.hpp"

namespace Poly
{
	class SoundResource;

	class ENGINE_DLLEXPORT SoundEmitterComponent : public ComponentBase
	{
	public:
		SoundEmitterComponent();
		~SoundEmitterComponent();

		void PushSoundResource(String path, eResourceSource source, eSoundFileFormat format = eSoundFileFormat::OGG_VORBIS, size_t size = 0, size_t offset = 0) { Playlist.PushBack(ResourceManager<SoundResource>::Load(path, source, format, size, offset)); PlaylistChanged = true; }
		void PopSoundResource() { Playlist.PopFront(); PlaylistChanged = true; }
		const SoundResource* GetBuffer(size_t idx) { return Playlist[idx]; }
		size_t GetBufferCount() { return Playlist.GetSize(); }

		float Pitch = 1.0f;
		float Gain = 1.0f;
		float MaxDistance = 100.0f;
		float RolloffFactor = 1.0f;
		float RefferenceDistance = 100.0f;
		float MinGain = 1.0f;
		float MaxGain = 1.0f;
		float ConeInnerAngle = 360.0f;
		float ConeOuterAngle = 360.0f;
		float ConeOuterGain = 1.0f;
		float SecondsOffset = 0.0f;
		bool Looping = false;
		bool Paused = false;
		bool PlaylistChanged = false;
		bool StateChanged = true;

	private:
		Dynarray<SoundResource*> Playlist;
	};

} // namespace Poly