#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Result.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"          // for Result
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Result is non-abstract, final and non-empty",
		"[result] [verify] [verify]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Result> );
		CHECK ( ! std::is_abstract_v<arcstk::Result> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Result> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Result> );
	}
}


TEST_CASE ( "Result special members",
		"[result] [verify] [verify]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::Result>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::Result>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::Result>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::Result>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::Result>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Result> );
	}
}


TEST_CASE ( "Result is swappable, comparable, string convertible",
		"[result] [verify] [verify]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::Result> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::Result> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::Result>::value );
	}
}

