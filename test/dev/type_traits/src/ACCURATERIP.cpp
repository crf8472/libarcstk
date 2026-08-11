#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for ACCURATERIP.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"          // for ACCURATERIP
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ACCURATERIP is non-abstract, final and non-empty",
		"[accuraterip] [calc] [accuraterip]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::ACCURATERIP> );
		CHECK ( ! std::is_abstract_v<arcstk::ACCURATERIP> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::ACCURATERIP> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::ACCURATERIP> );
	}
}


TEST_CASE ( "ACCURATERIP special members",
		"[accuraterip] [calc] [accuraterip]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::ACCURATERIP>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::ACCURATERIP>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::ACCURATERIP>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::ACCURATERIP>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::ACCURATERIP>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::ACCURATERIP> );
	}
}


TEST_CASE ( "ACCURATERIP is swappable, comparable, string convertible",
		"[accuraterip] [calc] [accuraterip]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::ACCURATERIP> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::ACCURATERIP> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::ACCURATERIP>::value );
	}
}

