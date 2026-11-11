#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Increment<>.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"                // TO BE TESTED
#endif

#include <cstdint>                 // for int32_t
#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "Increment is non-abstract, final and non-empty",
		"[increment][dbar][details]" )
{
	using std::int32_t;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::Increment<int32_t, false>> );
		CHECK ( ! std::is_abstract_v<arcstk::details::Increment<int32_t, false>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::Increment<int32_t, false>> );
	}

	SECTION ("has no non-static members")
	{
		CHECK ( std::is_empty_v<arcstk::details::Increment<int32_t, false>> );
	}
}


TEST_CASE ( "Increment special members",
		"[increment][dbar][details]" )
{
	using std::int32_t;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::details::Increment<int32_t, false>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::details::Increment<int32_t, false>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::details::Increment<int32_t, false>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::details::Increment<int32_t, false>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::details::Increment<int32_t, false>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::details::Increment<int32_t, false>> );
	}
}


TEST_CASE ( "Increment is swappable", "[increment][dbar][details]" )
{
	using std::int32_t;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::details::Increment<int32_t, false>> );
	}
}

