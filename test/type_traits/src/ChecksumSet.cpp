#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for ChecksumSet.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"          // for ChecksumSet
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ChecksumSet is non-abstract, final and non-empty",
		"[checksumset] [calc] [checksum]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::ChecksumSet> );
		CHECK ( ! std::is_abstract_v<arcstk::ChecksumSet> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::ChecksumSet> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::ChecksumSet> );
	}
}


TEST_CASE ( "ChecksumSet special members",
		"[checksumset] [calc] [checksum]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::ChecksumSet>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::ChecksumSet>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::ChecksumSet>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::ChecksumSet>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::ChecksumSet>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::ChecksumSet> );
	}
}


TEST_CASE ( "ChecksumSet is swappable, comparable, string convertible",
		"[checksumset] [calc] [checksum]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::ChecksumSet> );
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

