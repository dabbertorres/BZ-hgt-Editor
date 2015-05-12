#include "Map.hpp"

#include <fstream>

namespace hgt
{
	Map::Map()
	:	size(0),
		zoneNum(0),
		vertices(std::make_unique<sf::VertexArray>())
	{}

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

		vertices->clear();
		vertices->setPrimitiveType(sf::PrimitiveType::Points);
		vertices->resize(zoneNum * ZONE_SIZE * zoneNum * ZONE_SIZE);

		for(unsigned int z = 0; z < zones.size(); z++)
		{
			fillVertices(z);
		}

		return true;
	}

	sf::FloatRect Map::getBounds() const
	{
		return vertices->getBounds();
	}

	HeightEntry& Map::getEntry(unsigned int x, unsigned int y)
	{
		// convert screen coordinates to map coordinates
		unsigned int mc = x / ZONE_SIZE;
		unsigned int mr = zoneNum - 1 - y / ZONE_SIZE;

		// convert screen coordinates to zone coordinates
		unsigned int zc = x % ZONE_SIZE;
		unsigned int zr = ZONE_SIZE - 1 - y % ZONE_SIZE;

		return zones[mr * zoneNum + mc]->get(zc, zr);
	}

	unsigned int Map::getSize() const
	{
		return size;
	}

	unsigned int Map::getNumOfZones() const
	{
		return zoneNum;
	}

	void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(*vertices, states);
	}

	void Map::fillVertices(unsigned int z)
	{
		for(unsigned int i = 0; i < ZONE_SIZE * ZONE_SIZE; i++)
		{
			unsigned int idx = i + z * ZONE_SIZE * ZONE_SIZE;

			// map coordinates
			unsigned int mx = z % zoneNum;
			unsigned int my = z / zoneNum;

			// zone coordinates
			unsigned int zx = i % ZONE_SIZE;
			unsigned int zy = ZONE_SIZE - 1 - (i / ZONE_SIZE);

			// window position
			float x = static_cast<float>(mx * ZONE_SIZE + zx);
			float y = static_cast<float>((zoneNum - 1 - my) * ZONE_SIZE + (ZONE_SIZE - 1 - zy));

			(*vertices)[idx].position = {x, y};
			(*vertices)[idx].color = zones[z]->get(zx, zy).toColor();
		}
	}
}