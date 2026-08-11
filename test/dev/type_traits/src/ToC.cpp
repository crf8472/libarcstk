#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for ToC.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"          // for ToC
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ToC is non-abstract, final and non-empty",
		"[toc] [metadata] [metadata]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::ToC> );
		CHECK ( ! std::is_abstract_v<arcstk::ToC> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::ToC> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::ToC> );
	}
}


TEST_CASE ( "ToC special members",
		"[toc] [metadata] [metadata]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::ToC>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::ToC>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::ToC>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::ToC>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::ToC>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::ToC> );
	}
}


TEST_CASE ( "ToC is swappable, comparable, string convertible",
		"[toc] [metadata] [metadata]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::ToC> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::ToC> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::ToC>::value );
	}
}

