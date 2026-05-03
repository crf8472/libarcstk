#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits for DBAR.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"               // for DBAR
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "DBAR is non-abstract, final and non-empty", "[dbar]" )
{
	SECTION ( "is non-abstract final class" )
	{
		CHECK ( std::is_class_v<arcstk::DBAR> );
		CHECK ( ! std::is_abstract_v<arcstk::DBAR> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::DBAR> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::DBAR> );
	}
}


TEST_CASE ( "DBAR special members", "[dbar]" )
{
	SECTION ( "is default constructable" )
	{
		// default constructability is required for IteratorElement
		CHECK ( std::is_default_constructible_v<arcstk::DBAR>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::DBAR> );
	}

	SECTION ( "is copy-constructable" )
	{
		CHECK ( std::is_copy_constructible_v<arcstk::DBAR>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::DBAR>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::DBAR>);
		CHECK ( std::is_move_constructible_v<arcstk::DBAR>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::DBAR>);
		CHECK ( std::is_move_assignable_v<arcstk::DBAR>);
	}
}


TEST_CASE ( "DBAR default construction", "[dbar]" )
{
	auto dbar = arcstk::DBAR{};

	SECTION ( "is empty")
	{
		CHECK ( dbar.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !dbar );
	}
}


TEST_CASE ( "DBAR is swappable, comparable, string convertible", "[dbar]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::DBAR> );
		CHECK ( std::is_swappable_v<arcstk::DBAR> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::DBAR> );
	}
}

