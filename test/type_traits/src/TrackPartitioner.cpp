#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for TrackPartitioner.
 */

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"           // for TrackPartitioner
#endif

#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "TrackPartitioner is non-abstract, final and non-empty",
		"[trackpartitioner][calc][calculate]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::TrackPartitioner> );
		CHECK ( ! std::is_abstract_v<arcstk::details::TrackPartitioner> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::TrackPartitioner> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::details::TrackPartitioner> );
	}
}


TEST_CASE ( "TrackPartitioner special members",
		"[trackpartitioner][calc][calculate]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::details::TrackPartitioner>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::details::TrackPartitioner>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::details::TrackPartitioner>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::details::TrackPartitioner>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::details::TrackPartitioner>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::details::TrackPartitioner> );
	}
}


TEST_CASE ( "TrackPartitioner is swappable, comparable, string convertible",
		"[trackpartitioner][calc][calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::details::TrackPartitioner> );
	}

	SECTION ("has no comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( ! is_comparable_v<arcstk::details::TrackPartitioner> );
	}

	SECTION ("has no string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( ! has_tostring_functionality<arcstk::details::TrackPartitioner>::value );
	}
}

