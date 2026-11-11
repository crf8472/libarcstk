#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for CalculationSetUpdater<>.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for CalculationSetUpdater
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "CalculationSetUpdater is non-abstract, final and non-empty",
		"[calculationsetupdater][calc][calculate]" )
{
	using CalculationSetUpdater_t = arcstk::CalculationSetUpdater<int, int>;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<CalculationSetUpdater_t> );
		CHECK ( ! std::is_abstract_v<CalculationSetUpdater_t> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<CalculationSetUpdater_t> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<CalculationSetUpdater_t> );
	}
}


TEST_CASE ( "CalculationSetUpdater special members",
		"[calculationsetupdater][calc][calculate]" )
{
	using CalculationSetUpdater_t = arcstk::CalculationSetUpdater<int, int>;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<CalculationSetUpdater_t>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<CalculationSetUpdater_t>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<CalculationSetUpdater_t>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<CalculationSetUpdater_t>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<CalculationSetUpdater_t>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<CalculationSetUpdater_t> );
	}
}


TEST_CASE ( "CalculationSetUpdater is swappable",
		"[calculationsetupdater][calc][calculate]" )
{
	using CalculationSetUpdater_t = arcstk::CalculationSetUpdater<int, int>;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<CalculationSetUpdater_t> );
	}
}

