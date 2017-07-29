#include "LevelResource.hpp"
#include "FileIO.hpp"

using namespace SGJ;

LevelResource::LevelResource(const Poly::String& path)
{
	Poly::String tileMap = Poly::LoadTextFileRelative(Poly::eResourceSource::GAME, path);

	for (Poly::String row : tileMap.Split('\n'))
		for (Poly::String tile : row.Split(','))
			Tiles.PushBack(static_cast<eTileType>(std::stoi(tile.GetCStr())));
}
