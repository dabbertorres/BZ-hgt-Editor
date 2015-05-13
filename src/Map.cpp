#include "Map.hpp"

#include <fstream>

namespace hgt
{
	Map::Map()
	:	size(0),
		zoneNum(0),
		vertices(std::make_unique<sf::VertexArray>())
	{}

	void Map::update()
	{
		for(auto& v : changedVerts)
		{
			sf::Vertex& vertex = (*vertices)[v];

			unsigned int x = static_cast<unsigned int>(vertex.position.x);
			unsigned int y = static_cast<unsigned int>(vertex.position.y);

			vertex.color = getEntry(x, y, false).toColor();
		}

		changedVerts.clear();
	}

	bool Map::write(const std::string& file) const
	{
		std::ofstream fout(file, std::ios::binary);

		if(fout.bad())
		{
			return false;
		}

		for(auto& z : zones)
		{
			z->write(fout);
		}

		return true;
	}

	bool Map::load(const std::string& file)
	{
		std::ifstream fin(file, std::ios::binary);

		if(fin.bad())
		{
			return false;
		}

		zones.clear();

		DataArray data;
		while(fin.read(reinterpret_cast<char*>(data.data()), sizeof(data)))
		{
			zones.emplace_back(std::make_unique<HeightZone>());
			zones.back()->load(data);
		}

		zoneNum = static_cast<unsigned int>(std::sqrt(zones.size()));
		size = zoneNum * SIZE_DIV;

		fillVertices();

		return true;
	}

	sf::FloatRect Map::getBounds() const
	{
		return vertices->getBounds();
	}

	HeightEntry& Map::getEntry(int x, int y)
	{
		return getEntry(x, y, true);
	}

	unsigned int Map::getSize() const
	{
		return size;
	}

	unsigned int Map::getNumOfZones() const
	{
		return zoneNum;
	}

	sf::Vector2u Map::screenToMap(const sf::Vector2i& vec) const
	{
		return {vec.x / ZONE_SIZE, zoneNum - 1 - vec.y / ZONE_SIZE};
	}

	sf::Vector2u Map::screenToZone(const sf::Vector2i& vec) const
	{
		return {vec.x % ZONE_SIZE, ZONE_SIZE - 1 - vec.y % ZONE_SIZE};
	}

	sf::Vector2f Map::zoneAndMapToScreen(const sf::Vector2u& map, const sf::Vector2u& zone) const
	{
		return {static_cast<float>(map.x * ZONE_SIZE + zone.x), static_cast<float>((zoneNum - 1 - map.y) * ZONE_SIZE + ZONE_SIZE - 1 - zone.y)};
	}

	sf::Vector2u Map::zoneNumToMap(unsigned int z) const
	{
		return {z % zoneNum, z / zoneNum};
	}

	sf::Vector2u Map::indexToZone(unsigned int i) const
	{
		return {i % ZONE_SIZE, ZONE_SIZE - 1 - (i / ZONE_SIZE)};
	}

	HeightEntry& Map::getEntry(int x, int y, bool track)
	{
		if(track)
		{
			// convert map and zone coordinates to index
			unsigned int idx = y * zoneNum * ZONE_SIZE + x;

			// since this vertex is being accessed, it might be changed.
			// cache its index to check later
			changedVerts.push_back(idx);
		}

		sf::Vector2u map = screenToMap({x, y});

		sf::Vector2u zone = screenToZone({x, y});

		return zones[map.y * zoneNum + map.x]->get(zone.x, zone.y);
	}

	void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(*vertices, states);
	}

	void Map::fillVertices()
	{
		vertices->clear();
		vertices->setPrimitiveType(sf::PrimitiveType::Points);
		vertices->resize(zoneNum * ZONE_SIZE * zoneNum * ZONE_SIZE);

		const unsigned int DIM = ZONE_SIZE * zoneNum;

		for(unsigned int y = 0; y < DIM; y++)
		{
			for(unsigned int x = 0; x < DIM; x++)
			{
				sf::Vertex& vert = (*vertices)[y * DIM + x];

				vert.position = {static_cast<float>(x), static_cast<float>(y)};
				vert.color = getEntry(x, y, false).toColor();
			}
		}
	}
}