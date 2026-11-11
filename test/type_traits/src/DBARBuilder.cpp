#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for DBARBuilder.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // for DBARBuilder
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "DBARBuilder is non-abstract, final and non-empty",
		"[dbarbuilder][dbar]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::DBARBuilder> );
		CHECK ( ! std::is_abstract_v<arcstk::DBARBuilder> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::DBARBuilder> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::DBARBuilder> );
	}
}


TEST_CASE ( "DBARBuilder special members",
		"[dbarbuilder][dbar]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::DBARBuilder>);
	}

	SECTION ( "is not copy-constructable")
	{
		CHECK ( ! std::is_copy_constructible_v<arcstk::DBARBuilder>);
	}

	SECTION ( "is not copy-assignable")
	{
		CHECK ( ! std::is_copy_assignable_v<arcstk::DBARBuilder>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::DBARBuilder>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::DBARBuilder>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::DBARBuilder> );
	}
}


TEST_CASE ( "DBARBuilder is swappable", "[dbarbuilder][dbar]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::DBARBuilder> );
	}
}

