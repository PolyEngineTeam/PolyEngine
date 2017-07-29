#include "Level.hpp"
#include "FileIO.hpp"

using namespace SGJ;

Level::Level(const Poly::String& path)
{
	Poly::String tileMap = Poly::LoadTextFileRelative(Poly::eResourceSource::GAME, path);

	Poly::Dynarray<Poly::String> rows = tileMap.Split('\n');
	for (Poly::String row : rows)
	{
		for (Poly::String tile : row.Split(','))
		{
			Tiles.PushBack(static_cast<eTileType>(std::stoi(tile.GetCStr())));
		}	
	}
		
}
