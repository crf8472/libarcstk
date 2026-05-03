#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits for ToC.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // for ToC
#endif

#include <type_traits>            // for is_copy_constructible

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ToC is non-abstract, final and non-empty",
		"[toc] [meta] [metadata]" )
{
	SECTION ("is non-abstract final class")
	{
		CHECK ( std::is_class_v<arcstk::ToC> );
		CHECK ( ! std::is_abstract_v<arcstk::ToC> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::ToC> );
	}

	SECTION ("HAS non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::ToC> );
	}
}


TEST_CASE ( "ToC special members", "[toc] [meta] [metadata]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::ToC>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::ToC> );
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
		CHECK ( std::is_move_constructible_v<arcstk::ToC>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::ToC>);
		CHECK ( std::is_move_assignable_v<arcstk::ToC>);
	}
}


TEST_CASE ( "ToC default construction", "[toc] [meta] [metadata]" )
{
	SECTION ( "Default constructed instance is empty")
	{
		auto toc = arcstk::ToC{};

		CHECK ( toc.empty() );
		CHECK ( !toc );
	}
}


TEST_CASE ( "ToC is swappable, comparable, string convertible",
		"[toc] [meta] [metadata]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::ToC> );
		CHECK ( std::is_swappable_v<arcstk::ToC> );
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

