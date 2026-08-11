#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for istream_wrapper.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // for istream_wrapper
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "istream_wrapper is non-abstract, final and non-empty",
		"[istream_wrapper] [dbar] [dbar]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::istream_wrapper> );
		CHECK ( ! std::is_abstract_v<arcstk::istream_wrapper> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::istream_wrapper> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::istream_wrapper> );
	}
}


TEST_CASE ( "istream_wrapper special members",
		"[istream_wrapper] [dbar] [dbar]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::istream_wrapper>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::istream_wrapper>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::istream_wrapper>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::istream_wrapper>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::istream_wrapper>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::istream_wrapper> );
	}
}


TEST_CASE ( "istream_wrapper is swappable, comparable, string convertible",
		"[istream_wrapper] [dbar] [dbar]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::istream_wrapper> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::istream_wrapper> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::istream_wrapper>::value );
	}
}

