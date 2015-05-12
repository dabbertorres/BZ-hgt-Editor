#include "HeightZone.hpp"

namespace hgt
{
	HeightEntry& HeightZone::get(unsigned int c, unsigned int r)
	{
		return heights.at(r * ZONE_SIZE + c);
	}

	void HeightZone::write(std::ofstream& fout) const
	{
		for(auto& e : heights)
		{
			bitField16 data = 0;

			data += e.isCoplanarTriangles() ? 1 << 15 : 0;
			data += e.isCoplanarSquares() ? 1 << 14 : 0;
			// skip bits 13 - 12

			HeightArray bits = e.getHeightBits();
			for(unsigned int i = 0; i < HEIGHT_BITS; i++)
			{
				data += bits[i] ? 1 << i : 0;
			}

			fout.write(reinterpret_cast<const char*>(&data), sizeof(data));
		}
	}

	void HeightZone::load(DataArray& data)
	{
		for(unsigned int i = 0; i < ZONE_SIZE * ZONE_SIZE; i++)
		{
			HeightEntry& e = heights[i];
			bitField16& d = data[i];
			
			e.setCoplanarTriangles((d & 1 << 15) != 0);
			e.setCoplanarTriangles((d & 1 << 14) != 0);
			// skip bits 13 - 12

			HeightArray bits;

			for(unsigned int i = 0; i < HEIGHT_BITS; i++)
			{
				bits[i] = (d & 1 << i) != 0;
			}

			e.setHeight(bits);

			sf::Color col = e.toColor();
		}
	}
}

