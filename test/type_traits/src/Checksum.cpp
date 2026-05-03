#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits for Checksum.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for Checksum
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Checksum is swappable, comparable, string convertible",
		"[checksum] [calc] [calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::Checksum> );
		CHECK ( std::is_swappable_v<arcstk::Checksum> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::Checksum> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::Checksum>::value );
	}
}

