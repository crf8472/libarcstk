#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for CalculationSetUpdater.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for CalculationSetUpdater
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "CalculationSetUpdater is non-abstract, final and non-empty",
		"[calculationsetupdater] [calc] [calculate]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::CalculationSetUpdater> );
		CHECK ( ! std::is_abstract_v<arcstk::CalculationSetUpdater> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::CalculationSetUpdater> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::CalculationSetUpdater> );
	}
}


TEST_CASE ( "CalculationSetUpdater special members",
		"[calculationsetupdater] [calc] [calculate]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::CalculationSetUpdater>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::CalculationSetUpdater>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::CalculationSetUpdater>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::CalculationSetUpdater>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::CalculationSetUpdater>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::CalculationSetUpdater> );
	}
}


TEST_CASE ( "CalculationSetUpdater is swappable, comparable, string convertible",
		"[calculationsetupdater] [calc] [calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::CalculationSetUpdater> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::CalculationSetUpdater> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::CalculationSetUpdater>::value );
	}
}

