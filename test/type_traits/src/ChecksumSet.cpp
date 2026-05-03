#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits for ChecksumSet.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for ChecksumSet
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ChecksumSet is swappable, comparable, string convertible",
		"[checksumset] [calc] [calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::ChecksumSet> );
		CHECK ( std::is_swappable_v<arcstk::ChecksumSet> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::ChecksumSet> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::ChecksumSet>::value );
	}
}

