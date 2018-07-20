#pragma once

#include <catch.hpp>
#include <Defines.hpp>

#include <KeyFrameAnimationSystem.hpp>
#include <Collections/StringBuilder.hpp>
#include <AnimKeys.hpp>

using namespace Poly;

TEST_CASE("KeyFrameAnimationSystem", "[KeyFrameAnimationSystem]")
{
	String path = "asd.txt";
	OrderedMap<String, AnimKeys> keys = KeyFrameAnimationSystem::LoadAnimTracks(path);
	StringBuilder sb;
	for (auto a : keys.Keys())
	{
		sb.Append(a);
	}
	gConsole.LogDebug("Keys: {}", sb.StealString());
	//gConsole.LogDebug("Values: {}", keys.Values());
}

