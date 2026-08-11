#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for CalculationUpdater.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for CalculationUpdater
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "CalculationUpdater is non-abstract, final and non-empty",
		"[calculationupdater] [calc] [calculate]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::CalculationUpdater> );
		CHECK ( ! std::is_abstract_v<arcstk::CalculationUpdater> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::CalculationUpdater> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::CalculationUpdater> );
	}
}


TEST_CASE ( "CalculationUpdater special members",
		"[calculationupdater] [calc] [calculate]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::CalculationUpdater>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::CalculationUpdater>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::CalculationUpdater>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::CalculationUpdater>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::CalculationUpdater>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::CalculationUpdater> );
	}
}


TEST_CASE ( "CalculationUpdater is swappable, comparable, string convertible",
		"[calculationupdater] [calc] [calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::CalculationUpdater> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::CalculationUpdater> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::CalculationUpdater>::value );
	}
}

