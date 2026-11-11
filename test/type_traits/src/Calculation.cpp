#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Calculation.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for Calculation
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Calculation is non-abstract, final and non-empty",
		"[calculation][calc][calculate]" )
{
	SECTION ("is an abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Calculation> );
		CHECK ( std::is_abstract_v<arcstk::Calculation> );
	}

	SECTION ("is not final")
	{
		CHECK ( ! std::is_final_v<arcstk::Calculation> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Calculation> );
	}
}


TEST_CASE ( "Calculation special members", "[calculation][calc][calculate]" )
{
	SECTION ( "is not default constructable" )
	{
		CHECK ( ! std::is_default_constructible_v<arcstk::Calculation>);
	}

	SECTION ( "is destructible" )
	{
		CHECK ( std::is_destructible_v<arcstk::Calculation> );
	}

	SECTION ( "is not copy-constructable")
	{
		CHECK ( ! std::is_copy_constructible_v<arcstk::Calculation>);
	}

	SECTION ( "is not copy-assignable")
	{
		CHECK ( ! std::is_copy_assignable_v<arcstk::Calculation>);
	}

	SECTION ( "is not move-constructable")
	{
		CHECK ( ! std::is_move_constructible_v<arcstk::Calculation>);
	}

	SECTION ( "is not move-assignable")
	{
		CHECK ( ! std::is_nothrow_move_assignable_v<arcstk::Calculation>);
	}
}

