#ifndef HGT_HEIGHT_ENTRY_HPP
#define HGT_HEIGHT_ENTRY_HPP

#include <string>
#include <array>

#include <SFML/Graphics/Color.hpp>

namespace hgt
{
	const unsigned int BITS = 16;
	const unsigned int HEIGHT_BITS = 12;

	using byte = unsigned char;
	using HeightArray = std::array<bool, HEIGHT_BITS>;

	// One height entry is the height of the southwest corner of a grid (10m x 10m area)
	// One height unit is 0.1m, giving a max height of 409.6m.
	class HeightEntry
	{
		public:
			// union for easy translation between integer and bits
			union Height
			{
				// v = initial value
				Height(unsigned int v = 0);

				unsigned int integer;							// max height is 409.6, so why an int? Well, floating point numbers are complicated bit-wise.
																// 409.6 is just 4096 / 10. So we can just use 4096 as the max, and let BZ do the hard work.

				std::array<byte, sizeof(unsigned int)> bytes;	// we only use the first 12 bits; the other 20 (8 * sizeof(unsigned int) - 12) are irrelevant
			};
			
			// v = initial height value
			HeightEntry(unsigned int v = 100);

			bool isCoplanarTriangles() const;
			bool isCoplanarSquares() const;

			unsigned int getHeightInt() const;
			HeightArray getHeightBits() const;

			void setCoplanarTriangles(bool v);
			void setCoplanarSquares(bool v);

			void setHeight(unsigned int v);
			void setHeight(const HeightArray& bits);
			
			// returns a string of bits representing the whole height entry
			std::string toString() const;

			// returns a greyscale Color scaled via the integer height
			sf::Color toColor() const;

		private:
			bool readBit(unsigned int b) const;
			void writeBit(unsigned int b, bool v);

									// bit #:
			bool coplanarTriangles;	// 15
			bool coplanarSquares;	// 14

			bool unused[2];			// 13 - 12

			Height height;			// 11 - 0
	};
}

#endif // HGT_HEIGHT_ENTRY_HPP
