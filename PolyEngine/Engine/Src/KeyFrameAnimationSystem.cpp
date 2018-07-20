#include "EnginePCH.hpp"
#include <KeyFrameAnimationSystem.hpp>
#include <AnimKeys.hpp>
#include <Collections/String.hpp>

using namespace Poly;

void KeyFrameAnimationSystem::KeyFrameAnimationUpdatePhase(Scene* scene)
{
	//TODO implement
}

OrderedMap<String, AnimKeys> KeyFrameAnimationSystem::LoadAnimTracks(String path)
{
	gConsole.LogInfo("GameManagerSystem::LoadAnimTrack");
	// FIXME change eResourceSource
	String animSrc = LoadTextFileRelative(eResourceSource::GAME, path);
	// gConsole.LogInfo("GameManagerSystem::CreateAnimTrack AnimSrc: {}", animSrc);

	OrderedMap<String, AnimKeys> keys;

	String name;
	Dynarray<Vector> positions;
	Dynarray<Vector> scales;
	Dynarray<Quaternion> rotations;

	bool hasAnimationKeyRotation = false;
	bool hasAnimationKeyScale = false;
	bool hasAnimationKeyPosition = false;
	int rowCounterRotation = 0;
	int rowCounterScale = 0;
	int rowCounterPosition = 0;

	animSrc.Replace("\r\n", "\n");
	animSrc.GetTrimmed();
	Dynarray<String> rows = animSrc.Split('\n');
	for (String row : rows)
	{
		if (positions.GetSize() > 0 && scales.GetSize() > 0 && rotations.GetSize() > 0 && !hasAnimationKeyPosition && !hasAnimationKeyRotation && !hasAnimationKeyScale)
		{
			gConsole.LogDebug("Saving transform animation for mesh {}", name);
			AnimKeys animKeys = AnimKeys(positions, rotations, scales);
			keys.Insert(name, animKeys);
			positions.Clear();
			rotations.Clear();
			scales.Clear();
			hasAnimationKeyRotation = false;
			hasAnimationKeyScale = false;
			hasAnimationKeyPosition = false;
			rowCounterRotation = 0;
			rowCounterScale = 0;
			rowCounterPosition = 0;
			continue;
		}
		if (row.Contains("{") && row.Contains("}"))
		{
			size_t startIdx = row.Find('{');
			size_t endIdx = row.Find('}');
			name = row.Substring(startIdx + 1, endIdx);
			gConsole.LogDebug("Mesh name: {}", name);
			continue;
		}
		if (row.IsEmpty())
		{
			continue;
		}

		if (row.Contains("Rotation"))
		{
			hasAnimationKeyRotation = true;
		}

		if (row.Contains("Scale"))
		{
			hasAnimationKeyScale = true;
		}

		if (row.Contains("Position"))
		{
			hasAnimationKeyPosition = true;
		}

		if (row.Contains("}"))
		{
			hasAnimationKeyRotation = false;
			hasAnimationKeyScale = false;
			hasAnimationKeyPosition = false;
		}

		if (hasAnimationKeyRotation)
		{
			if (rowCounterRotation == 1)
			{
				gConsole.LogInfo("GameManagerSystem::CreateAnimTrack TODO: read if rotation or track ID?, row: {}", row);
			}

			if (rowCounterRotation == 2)
			{
				gConsole.LogInfo("GameManagerSystem::CreateAnimTrack TODO: number of keyframes, row: {}", row);
			}

			if (rowCounterRotation > 2)
			{
				rotations.PushBack(AnimTrack_ReadQuternion4FromRow(row));
			}

			rowCounterRotation++;
		}

		if (hasAnimationKeyScale)
		{
			if (rowCounterScale == 1)
			{
				gConsole.LogInfo("GameManagerSystem::CreateAnimTrack TODO: read if rotation or track ID?, row: {}", row);
			}

			if (rowCounterScale == 2)
			{
				gConsole.LogInfo("GameManagerSystem::CreateAnimTrack TODO: number of keyframes, row: {}", row);
			}

			if (rowCounterScale > 2)
			{
				scales.PushBack(AnimTrack_ReadVector3FromRow(row));
			}

			rowCounterScale++;
		}

		if (hasAnimationKeyPosition)
		{
			if (rowCounterPosition == 1)
			{
				gConsole.LogInfo("GameManagerSystem::CreateAnimTrack TODO: read if rotation or track ID?, row: {}", row);
			}

			if (rowCounterPosition == 2)
			{
				gConsole.LogInfo("GameManagerSystem::CreateAnimTrack TODO: number of keyframes, row: {}", row);
			}

			if (rowCounterPosition > 2)
			{
				positions.PushBack(AnimTrack_ReadVector3FromRow(row));
			}

			rowCounterPosition++;
		}
		
	}

	gConsole.LogInfo("GameManagerSystem::CreateAnimTrack Print loaded rotations:");
	for (size_t i = 0; i < rotations.GetSize(); ++i)
	{
		gConsole.LogInfo("GameManagerSystem::CreateAnimTrack rotation[{}]: {}", i, rotations[i]);
	}

	gConsole.LogInfo("GameManagerSystem::CreateAnimTrack Print loaded scales:");
	for (size_t i = 0; i < scales.GetSize(); ++i)
	{
		gConsole.LogInfo("GameManagerSystem::CreateAnimTrack scale[{}]: {}", i, scales[i]);
	}

	gConsole.LogInfo("GameManagerSystem::CreateAnimTrack Print loaded positions:");
	for (size_t i = 0; i < positions.GetSize(); ++i)
	{
		gConsole.LogInfo("GameManagerSystem::CreateAnimTrack position[{}]: {}", i, positions[i]);
	}

	// ASSERTE(false, "Stop at AnimTrack loading for easier debugging ;)");

	return keys;
}

Quaternion KeyFrameAnimationSystem::AnimTrack_ReadQuternion4FromRow(String row)
{
	// 0;4;0.000000, 0.000000, 0.000000, 0.000000;;,
	gConsole.LogDebug("Rotation row: {}", row);
	Dynarray<String> tokens = row.Split(';'); // values are at token with index 2 
	Dynarray<String> channels = tokens[2].Split(',');
	float x = (float)std::atof(channels[0].GetCStr());
	float y = (float)std::atof(channels[1].GetCStr());
	float z = (float)std::atof(channels[2].GetCStr());
	float w = (float)std::atof(channels[3].GetCStr());
	return Quaternion(x, y, z, w);
}

Vector KeyFrameAnimationSystem::AnimTrack_ReadVector3FromRow(String row)
{
	// 0;4;0.000000, 0.000000, 0.000000, 0.000000;;,	
	Dynarray<String> tokens = row.Split(';'); // values are at token with index 2 
	Dynarray<String> channels = tokens[2].Split(',');
	float x = (float)std::atof(channels[0].GetCStr());
	float y = (float)std::atof(channels[1].GetCStr());
	float z = (float)std::atof(channels[2].GetCStr());
	return Vector(x, y, z);
}