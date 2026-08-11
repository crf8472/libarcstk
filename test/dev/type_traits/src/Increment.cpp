#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Increment.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // for Increment
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Increment is non-abstract, final and non-empty",
		"[increment] [dbar] [dbar]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Increment> );
		CHECK ( ! std::is_abstract_v<arcstk::Increment> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Increment> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Increment> );
	}
}


TEST_CASE ( "Increment special members",
		"[increment] [dbar] [dbar]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::Increment>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::Increment>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::Increment>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::Increment>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::Increment>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Increment> );
	}
}


TEST_CASE ( "Increment is swappable, comparable, string convertible",
		"[increment] [dbar] [dbar]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::Increment> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::Increment> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::Increment>::value );
	}
}

