#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for bytes.hpp.
 */

#ifndef LIBARCSTK_BYTES_HPP_
#include "bytes.hpp"     // TO BE TESTED
#endif


TEST_CASE ( "Combine samples", "[combine]" )
{
	using arcstk::details::combine;

	SECTION ( "combine" )
	{
		using arcstk::details::cpp17::endian;

		if constexpr(endian::native == endian::little)
		{
			// swapped
			CHECK ( combine<int32_t>(10, 01) == 0b10100000000000000001 ); // 655361

			// not swapped
			CHECK ( combine<int32_t>( 1, 10) ==    0b10000000000001010 ); //  65546
			CHECK ( combine<int32_t>( 2,  9) ==   0b100000000000001001 ); // 131081
		}
	}
}

