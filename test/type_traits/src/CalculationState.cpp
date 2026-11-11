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
		"[calculationstate][calc][calculate][details]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::CalculationState> );
		CHECK ( ! std::is_abstract_v<arcstk::details::CalculationState> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::CalculationState> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::details::CalculationState> );
	}
}


TEST_CASE ( "CalculationState special members",
		"[calculationstate][calc][calculate][details]" )
{
	using CalculationState_t = arcstk::details::CalculationState;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<CalculationState_t>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<CalculationState_t>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<CalculationState_t>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<CalculationState_t>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<CalculationState_t>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<CalculationState_t> );
	}
}


TEST_CASE ( "CalculationState is swappable",
		"[calculationstate][calc][calculate][details]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::details::CalculationState> );
	}
}

