#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Interval.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Interval<>", "[interval] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::details::Interval;

	auto instance = Interval<int> { 0, 100 };


	SECTION ("Parametized construction is correct")
	{
		CHECK ( instance.lower() ==   0 );
		CHECK ( instance.upper() == 100 );
	}

	SECTION ("Copy construction is correct")
	{
		using IntervalType = Interval<int>;

		const IntervalType copied { Copy<IntervalType>::construct(instance) };

		// --

		CHECK ( copied.lower() ==   0 );
		CHECK ( copied.upper() == 100 );
	}

	SECTION ("Move construction is correct")
	{
		using IntervalType = Interval<int>;
		const IntervalType moved { Move<IntervalType>::construct(instance) };

		// --

		CHECK ( moved.lower() ==   0 );
		CHECK ( moved.upper() == 100 );
	}

	SECTION ("Copy assignment is correct")
	{
		using IntervalType = Interval<int>;
		auto copied = IntervalType {};
		Copy<IntervalType>::assign(copied, instance);

		// --

		CHECK ( copied.lower() ==   0 );
		CHECK ( copied.upper() == 100 );
	}

	SECTION ("Move assignment is correct")
	{
		using IntervalType = Interval<int>;
		auto moved = IntervalType {};
		Move<IntervalType>::assign(moved, instance);

		// --

		CHECK ( moved.lower() ==   0 );
		CHECK ( moved.upper() == 100 );
	}
}


TEST_CASE ( "Interval<> default constructed instance",
		"[interval] [calc] [calculate]" )
{
	const auto defaulted = arcstk::details::Interval<int>{};


	SECTION ( "lower and upper were default initialized")
	{
		CHECK ( defaulted.lower() == 0 );
		CHECK ( defaulted.upper() == 0 );
	}
}


TEST_CASE ( "Interval<> property", "[interval] [calc] [calculate]" )
{
	using arcstk::details::Interval;

	auto instance = Interval<int> { -863, 915 };


	// SECTION ("swap() is correct")
	// {
	// 	FAIL ( "swap() test is missing" );
	// }

	SECTION ("to_string() is correct")
	{
		CHECK ( instance.to_string() == "[-863,915]" );
	}
}


TEST_CASE ( "Interval<> functions", "[interval] [calc] [calculate]" )
{
	using arcstk::details::Interval;

	auto instance = Interval<int> { 17, 96 };


	SECTION ("lower() is correct")
	{
		CHECK ( instance.lower() == 17 );
	}

	SECTION ("upper() is correct")
	{
		CHECK ( instance.upper() == 96 );
	}

	SECTION ("contains() is correct")
	{
		CHECK ( ! instance.contains(10) );
		CHECK ( ! instance.contains(11) );
		CHECK ( ! instance.contains(12) );
		CHECK ( ! instance.contains(13) );
		CHECK ( ! instance.contains(14) );
		CHECK ( ! instance.contains(15) );
		CHECK ( ! instance.contains(16) );

		for (auto i = int { instance.lower() }; i <= instance.upper(); ++i)
		{
			CHECK ( instance.contains(i) );
		}

		CHECK ( ! instance.contains(97) );
		CHECK ( ! instance.contains(98) );
		CHECK ( ! instance.contains(99) );
	}
}

