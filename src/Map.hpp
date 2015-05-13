#ifndef HGT_MAP_HPP
#define HGT_MAP_HPP

#include <vector>
#include <memory>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "HeightZone.hpp"

namespace hgt
{
	// this class represents a whole BZ height map, containing a number of zones
	// (size / 1280) * (size / 1280)
	class Map : public sf::Drawable
	{
		public:
			static const unsigned int INITIAL_HEIGHT = 100;

			// s = size of map in meters
			// initialHeight = initial elevation of grids
			Map();
			~Map() = default;

			void update();

			bool write(const std::string& file) const;
			bool load(const std::string& file);

			sf::FloatRect getBounds() const;

			HeightEntry& getEntry(int x, int y);

			// returns the BZ map size in meters for the X and Y
			unsigned int getSize() const;

			// returns the number of zones in X and Y
			unsigned int getNumOfZones() const;

		private:
			sf::Vector2u screenToMap(const sf::Vector2i& vec) const;
			sf::Vector2u screenToZone(const sf::Vector2i& vec) const;
			sf::Vector2f zoneAndMapToScreen(const sf::Vector2u& map, const sf::Vector2u& zone) const;
			sf::Vector2u zoneNumToMap(unsigned int z) const;
			sf::Vector2u indexToZone(unsigned int i) const;

			HeightEntry& getEntry(int x, int y, bool track);

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
			void fillVertices();

			static const unsigned int SIZE_DIV = 1280;

			unsigned int size;
			unsigned int zoneNum;

			std::unique_ptr<sf::VertexArray> vertices;
			std::vector<unsigned int> changedVerts;

			std::vector<std::unique_ptr<HeightZone>> zones;
	};
}

#endif	// HGT_MAP_HPP