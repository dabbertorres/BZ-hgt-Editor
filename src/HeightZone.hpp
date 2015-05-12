#ifndef HGT_HEIGHT_BLOCK_HPP
#define HGT_HEIGHT_BLOCK_HPP

#include "HeightEntry.hpp"

#include <fstream>
#include <array>

namespace hgt
{
	class HeightZone;

	const unsigned int ZONE_SIZE = 128;
	using bitField16 = unsigned short;
	using DataArray = std::array<bitField16, ZONE_SIZE * ZONE_SIZE>;

	// this class represents a 128x128 "zone" in a BZ map.
	// index 0 starts in the bottom left corner, and increases
	// to the right, and up
	class HeightZone
	{
		public:
			// e = elevation of grids in this zone
			HeightZone() = default;

			// r = row
			// c = column
			HeightEntry& get(unsigned int c, unsigned int r);

			void write(std::ofstream& fout) const;
			void load(DataArray& data);

		private:
			std::array<HeightEntry, ZONE_SIZE * ZONE_SIZE> heights;
	};
}

#endif // HGT_HEIGHT_BLOCK_HPP
