#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for CalculationState.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for CalculationState
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "CalculationState is non-abstract, final and non-empty",
		"[calculationstate] [calc] [calculate]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::CalculationState> );
		CHECK ( ! std::is_abstract_v<arcstk::CalculationState> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::CalculationState> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::CalculationState> );
	}
}


TEST_CASE ( "CalculationState special members",
		"[calculationstate] [calc] [calculate]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::CalculationState>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::CalculationState>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::CalculationState>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::CalculationState>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::CalculationState>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::CalculationState> );
	}
}


TEST_CASE ( "CalculationState is swappable, comparable, string convertible",
		"[calculationstate] [calc] [calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::CalculationState> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::CalculationState> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::CalculationState>::value );
	}
}

