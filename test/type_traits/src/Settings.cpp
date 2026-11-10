#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Settings.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for Settings
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Settings is non-abstract, final and non-empty",
		"[settings] [calc] [calculate]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Settings> );
		CHECK ( ! std::is_abstract_v<arcstk::Settings> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Settings> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Settings> );
	}
}


TEST_CASE ( "Settings special members",
		"[settings] [calc] [calculate]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::Settings>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::Settings>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::Settings>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::Settings>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::Settings>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Settings> );
	}
}


TEST_CASE ( "Settings is swappable, comparable, string convertible",
		"[settings] [calc] [calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::Settings> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::Settings> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::Settings>::value );
	}
}

