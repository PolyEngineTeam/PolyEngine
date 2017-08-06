#include "Level.hpp"
#include "FileIO.hpp"

using namespace SGJ;

Level::Level(const Poly::String& path)
{
	Poly::String tileMap = Poly::LoadTextFileRelative(Poly::eResourceSource::GAME, path);

	Poly::Dynarray<Poly::String> rows = tileMap.Split('\n');
	Height = rows.GetSize();
	for (Poly::String row : rows)
	{
		Poly::String line = "";

		Width = 0;
		for (Poly::String tile : row.Split(','))
		{
			Width++;
			Tiles.PushBack(static_cast<eTileType>(std::stoi(tile.GetCStr())));
		}
	}
		
}
