#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Counter<>.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"           // TO BE TESTED
#endif

#include <cstdint>                 // for int32_t
#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "Counter is non-abstract, final and non-empty",
		"[counter][calc][calculate]" )
{
	using std::int32_t;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::Counter<int32_t>> );
		CHECK ( ! std::is_abstract_v<arcstk::details::Counter<int32_t>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::Counter<int32_t>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::details::Counter<int32_t>> );
	}
}


TEST_CASE ( "Counter special members",
		"[counter][calc][calculate]" )
{
	using std::int32_t;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::details::Counter<int32_t>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::details::Counter<int32_t>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::details::Counter<int32_t>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::details::Counter<int32_t>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::details::Counter<int32_t>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::details::Counter<int32_t>> );
	}
}


TEST_CASE ( "Counter is swappable, comparable, string convertible",
		"[counter][calc][calculate]" )
{
	using std::int32_t;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::details::Counter<int32_t>> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::details::Counter<int32_t>> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::details::Counter<int32_t>>::value );
	}
}

