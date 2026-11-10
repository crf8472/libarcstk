#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for DBARTriplet.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // for DBARTriplet
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "DBARTriplet is non-abstract, final and non-empty",
		"[dbartriplet] [dbar] [dbar]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::DBARTriplet> );
		CHECK ( ! std::is_abstract_v<arcstk::DBARTriplet> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::DBARTriplet> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::DBARTriplet> );
	}
}


TEST_CASE ( "DBARTriplet special members",
		"[dbartriplet] [dbar] [dbar]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::DBARTriplet>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::DBARTriplet>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::DBARTriplet>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::DBARTriplet>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::DBARTriplet>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::DBARTriplet> );
	}
}


TEST_CASE ( "DBARTriplet is swappable, comparable, string convertible",
		"[dbartriplet] [dbar] [dbar]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::DBARTriplet> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::DBARTriplet> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::DBARTriplet>::value );
	}
}

