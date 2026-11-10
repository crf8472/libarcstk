#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for IteratorElement<>.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"                // TO BE TESTED
#endif

#include <cstdint>                 // for int32_t
#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "IteratorElement is non-abstract, final and non-empty",
		"[iteratorelement][dbar][details]" )
{
	using std::int32_t;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::IteratorElement<int32_t>> );
		CHECK ( ! std::is_abstract_v<arcstk::IteratorElement<int32_t>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::IteratorElement<int32_t>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::IteratorElement<int32_t>> );
	}
}


TEST_CASE ( "IteratorElement special members",
		"[iteratorelement][dbar][details]" )
{
	using std::int32_t;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::IteratorElement<int32_t>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::IteratorElement<int32_t>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::IteratorElement<int32_t>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::IteratorElement<int32_t>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::IteratorElement<int32_t>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::IteratorElement<int32_t>> );
	}
}


TEST_CASE ( "IteratorElement is swappable, comparable, string convertible",
		"[iteratorelement][dbar][details]" )
{
	using std::int32_t;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::IteratorElement<int32_t>> );
	}

	SECTION ("has no comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( ! is_comparable_v<arcstk::IteratorElement<int32_t>> );
	}

	SECTION ("has no string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( ! has_tostring_functionality<arcstk::IteratorElement<int32_t>>::value );
	}
}

