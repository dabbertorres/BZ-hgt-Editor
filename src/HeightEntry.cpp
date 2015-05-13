#include "HeightEntry.hpp"

#include <cmath>

namespace hgt
{
	HeightEntry::Height::Height(int v)
	:	integer(v)
	{}

	HeightEntry::HeightEntry(int v)
	:	coplanarTriangles(false),
		coplanarSquares(false),
		height(v)
	{
		unused[0] = false;
		unused[1] = false;
	}

	bool HeightEntry::isCoplanarTriangles() const
	{
		return coplanarTriangles;
	}

	bool HeightEntry::isCoplanarSquares() const
	{
		return coplanarSquares;
	}

	int HeightEntry::getHeightInt() const
	{
		return height.integer;
	}

	HeightArray HeightEntry::getHeightBits() const
	{
		HeightArray bits;

		for(unsigned int i = 0; i < HEIGHT_BITS; i++)
		{
			bits[i] = readBit(i);
		}

		return bits;
	}

	void HeightEntry::setCoplanarTriangles(bool v)
	{
		coplanarTriangles = v;
	}

	void HeightEntry::setCoplanarSquares(bool v)
	{
		coplanarSquares = v;
	}

	void HeightEntry::setHeight(int v)
	{
		height.integer = v;
	}

	void HeightEntry::setHeight(const std::array<bool, HEIGHT_BITS>& bits)
	{
		for(unsigned int i = 0; i < HEIGHT_BITS; i++)
		{
			writeBit(i, bits[i]);
		}
	}

	std::string HeightEntry::toString() const
	{
		std::string str = "";

		str += coplanarTriangles ? '1' : '0';
		str += coplanarSquares ? '1' : '0';
		str += unused[0] ? '1' : '0';
		str += unused[1] ? '1' : '0';

		for(int i = HEIGHT_BITS - 1; i >= 0; i--)
		{
			str += readBit(i) ? '1' : '0';
		}

		return str;
	}

	sf::Color HeightEntry::toColor() const
	{
		double heightScale = static_cast<double>(height.integer) / hgt::HEIGHT_MAX;

		double bScale = 0;
		double gScale = 0;
		double rScale = 0;

		// calculate colors
		if(heightScale <= 0.5)
		{
			rScale = 0;
			gScale = heightScale / 0.5;
			bScale = 1.0 - gScale;
		}
		else
		{
			bScale = 0;
			gScale = (1.0 - heightScale) / 0.5;
			rScale = 1.0 - gScale;
		}

		// bound colors
		if(bScale < 0)
		{
			bScale = 0;
		}

		if(gScale < 0)
		{
			gScale = 0;
		}

		if(rScale < 0)
		{
			rScale = 0;

		}

		// 255 = max value of a component of color
		byte r = static_cast<byte>(255 * rScale);
		byte g = static_cast<byte>(255 * gScale);
		byte b = static_cast<byte>(255 * bScale);

		return {r, g, b, 255};
	}

	bool HeightEntry::readBit(unsigned int b) const
	{
		return (height.bytes[b / 8] & (1 << (b % 8))) != 0;
	}

	void HeightEntry::writeBit(unsigned int b, bool v)
	{
		if(v)
		{
			if(!readBit(b))
			{
				height.bytes[b / 8] += 1 << (b % 8);
			}
		}
		else
		{
			if(readBit(b))
			{
				height.bytes[b / 8] -= 1 << (b % 8);
			}
		}
	}
}
