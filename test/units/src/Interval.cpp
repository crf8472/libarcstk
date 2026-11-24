#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Interval<int32_t> (SampleRange).
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"           // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"        // for Copy, Move
#endif


TEST_CASE ( "Interval<>", "[interval][calc][calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Interval;

	auto instance = Interval<int> { 0, 100 };


	SECTION ( "Parametized construction is correct" )
	{
		CHECK ( instance.lower() ==   0 );
		CHECK ( instance.upper() == 100 );

		const auto i2 = Interval<int32_t> { -12, 123 };
		const auto i3 = Interval<int32_t> { -99, -200 };

		CHECK ( i2.lower() == -12 );
		CHECK ( i2.upper() == 123 );

		CHECK ( i3.lower() == -200 );
		CHECK ( i3.upper() ==  -99 );
	}

	SECTION ( "Copy construction is correct" )
	{
		using IntervalType = Interval<int>;

		const IntervalType copied { Copy<IntervalType>::construct(instance) };

		CHECK ( copied.lower() ==   0 );
		CHECK ( copied.upper() == 100 );
	}

	SECTION ( "Move construction is correct" )
	{
		using IntervalType = Interval<int>;
		const IntervalType moved { Move<IntervalType>::construct(instance) };

		CHECK ( moved.lower() ==   0 );
		CHECK ( moved.upper() == 100 );
	}

	SECTION ( "Copy assignment is correct" )
	{
		using IntervalType = Interval<int>;
		auto copied = IntervalType {};
		Copy<IntervalType>::assign(copied, instance);

		CHECK ( copied.lower() ==   0 );
		CHECK ( copied.upper() == 100 );
	}

	SECTION ( "Move assignment is correct" )
	{
		using IntervalType = Interval<int>;
		auto moved = IntervalType {};
		Move<IntervalType>::assign(moved, instance);

		CHECK ( moved.lower() ==   0 );
		CHECK ( moved.upper() == 100 );
	}
}


TEST_CASE ( "Interval<> default", "[interval][calc][calculate]" )
{
	const auto defaulted = arcstk::Interval<int32_t> {};

	SECTION ( "lower() and upper() are zero (provides [0,0])" )
	{
		CHECK ( defaulted.lower() == 0 );
		CHECK ( defaulted.upper() == 0 );
	}
}


TEST_CASE ( "Interval<int32_t> value", "[interval][calc][calculate]" )
{
	using arcstk::Interval;

	auto instance = Interval<int32_t> { -863, 915 };


	SECTION ( "swap() is correct" )
	{
		auto rhs = Interval<int32_t> { -2, -100 };

		using std::swap;

		swap(instance, rhs);

		CHECK ( instance.lower() == -100 );
		CHECK ( instance.upper() ==   -2 );
		CHECK ( rhs.lower()      == -863 );
		CHECK ( rhs.upper()      ==  915 );
	}

	SECTION ( "to_string() is correct" )
	{
		CHECK ( instance.to_string() == "[-863,915]" );
	}
}


TEST_CASE ( "Interval<int32_t> behavior", "[interval][calc][calculate]" )
{
	using arcstk::Interval;

	using std::int32_t;

	const auto instance = Interval<int32_t> { 96, 17 };


	SECTION ( "lower() is correct" )
	{
		CHECK ( instance.lower() == 17 );
	}

	SECTION ( "upper() is correct" )
	{
		CHECK ( instance.upper() == 96 );
	}

	SECTION ( "count() is correct" )
	{
		CHECK ( Interval<int32_t> { 96, 17 }.count() == 80 );
		CHECK ( Interval<int32_t> {  1,  1 }.count() ==  1 );
		CHECK ( Interval<int32_t> {  1,  2 }.count() ==  2 );
	}

	SECTION ( "contains() is FALSE for positive values not contained" )
	{
		CHECK_FALSE ( instance.contains(10) );
		CHECK_FALSE ( instance.contains(11) );
		CHECK_FALSE ( instance.contains(12) );
		CHECK_FALSE ( instance.contains(13) );
		CHECK_FALSE ( instance.contains(14) );
		CHECK_FALSE ( instance.contains(15) );
		CHECK_FALSE ( instance.contains(16) );
		// 17 -96
		CHECK_FALSE ( instance.contains(97) );
		CHECK_FALSE ( instance.contains(98) );
		CHECK_FALSE ( instance.contains(99) );
	}

	SECTION ( "contains() is TRUE for every positive contained value" )
	{
		for (auto i = int32_t { instance.lower()/*17*/ };
				i <= instance.upper()/*96*/; ++i)
		{
			CHECK ( instance.contains(i) );
		}
	}

	SECTION ( "contains() is correct for a partly negative interval" )
	{
		const auto i2 = Interval<int32_t> { -12, 123 };

		CHECK_FALSE ( i2.contains(-13) );
		CHECK       ( i2.contains(-12) );
		CHECK       ( i2.contains(-11) );
		CHECK       ( i2.contains(-10) );
		CHECK       ( i2.contains(122) );
		CHECK       ( i2.contains(123) );
		CHECK_FALSE ( i2.contains(124) );
	}

	SECTION ( "contains() is correct for a positiv interval of count() 1" )
	{
		const auto i = Interval<int32_t> { 1, 1 };

		REQUIRE ( 1 == i.count() );

		CHECK_FALSE ( i.contains(0) );
		CHECK       ( i.contains(1) );
		CHECK_FALSE ( i.contains(2) );
	}

	SECTION ( "equals() is correct" )
	{
		CHECK ( instance.equals(Interval<int32_t> { 96, 17 }) );
		CHECK ( instance.equals(Interval<int32_t> { 17, 96 }) );

		CHECK_FALSE ( instance.equals(Interval<int32_t> { 18, 96 }) );
		CHECK_FALSE ( instance.equals(Interval<int32_t> { 95, 17 }) );
	}
}

// vim:fdm=syntax

