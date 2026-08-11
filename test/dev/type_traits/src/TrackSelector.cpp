#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for TrackSelector.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"          // for TrackSelector
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "TrackSelector is non-abstract, final and non-empty",
		"[trackselector] [verify] [verify]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::TrackSelector> );
		CHECK ( ! std::is_abstract_v<arcstk::TrackSelector> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::TrackSelector> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::TrackSelector> );
	}
}


TEST_CASE ( "TrackSelector special members",
		"[trackselector] [verify] [verify]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::TrackSelector>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::TrackSelector>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::TrackSelector>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::TrackSelector>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::TrackSelector>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::TrackSelector> );
	}
}


TEST_CASE ( "TrackSelector is swappable, comparable, string convertible",
		"[trackselector] [verify] [verify]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::TrackSelector> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::TrackSelector> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::TrackSelector>::value );
	}
}

