#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Counter.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Counter<>", "[counter] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::details::Counter;

	auto instance = Counter<int> { 8472 };


	SECTION ("Parametized construction is correct")
	{
		CHECK ( instance.value() == 8472 );
	}

	SECTION ("Copy construction is correct")
	{
		using CounterType = Counter<int>;

		CounterType copied { Copy<CounterType>::construct(instance) };

		// --

        CHECK ( copied.value() == 8472 );
	}

	SECTION ("Move construction is correct")
	{
		using CounterType = Counter<int>;

		CounterType moved { Move<CounterType>::construct(instance) };

		// --

        CHECK ( moved.value() == 8472 );
	}

	SECTION ("Copy assignment is correct")
	{
		using CounterType = Counter<int>;

		auto copied = CounterType {};
		Copy<CounterType>::assign(copied, instance);

		// --

        CHECK ( copied.value() == 8472 );
	}

	SECTION ("Move assignment is correct")
	{
		using CounterType = Counter<int>;

		auto moved = CounterType {};
		Move<CounterType>::assign(moved, instance);

		// --

        CHECK ( moved.value() == 8472 );
	}
}


TEST_CASE ( "Counter<> default constructed instance",
		"[counter] [calc] [calculate]" )
{
	using arcstk::details::Counter;

	const auto defaulted = Counter<int>{};

	SECTION ( "is 0")
	{
		CHECK ( defaulted.value() == 0 );
	}

	// SECTION ( "converts to FALSE")
	// {
	// 	CHECK ( !defaulted );
	// }
}


// TEST_CASE ( "Counter<> property", "[counter] [calc] [calculate]" )
// {
// 	using arcstk::details::Counter;
//
// 	const auto instance = Counter<int>{};
//
//
// 	SECTION ("Equality operator == is correct")
// 	{
// 		FAIL ( "Equality operator test is missing" );
// 	}
//
// 	SECTION ("Stream-in operator << is correct")
// 	{
// 		FAIL ( "Stream-in operator << test is missing" );
// 	}
//
// 	SECTION ("operator bool() is correct")
// 	{
// 		FAIL ( "operator bool() test is missing" );
// 	}
//
// 	SECTION ("swap() is correct")
// 	{
// 		FAIL ( "swap() test is missing" );
// 	}
//
// 	SECTION ("to_string() is correct")
// 	{
// 		FAIL ( "to_string() test is missing" );
// 	}
//
// 	SECTION ("clone() is correct")
// 	{
// 		FAIL ( "clone() test is missing" );
// 	}
//
// 	SECTION ("size() is correct")
// 	{
// 		FAIL ( "size() test is missing" );
// 	}
//
// 	SECTION ("empty() is correct")
// 	{
// 		FAIL ( "empty() test is missing" );
// 	}
// }


TEST_CASE ( "Counter<> functions", "[counter] [calc] [calculate]" )
{
	using arcstk::details::Counter;

	auto instance = Counter<int> { 479 };
	// https://github.com/catchorg/Catch2/issues/2910

	REQUIRE ( instance.value() == 479 );


	SECTION ("value() is correct")
	{
		CHECK ( instance.value() == 479 );
	}

	SECTION ("increment() is correct")
	{
		instance.increment(28496);

		CHECK ( instance.value() == 479 + 28496 );
	}

	SECTION ("repeated increment() is correct")
	{
		auto counter = Counter<int> {};

		REQUIRE ( counter.value() == 0 );

		// ***

		counter.increment(5);
		counter.increment(28);
		counter.increment(10191);

		CHECK ( counter.value() == 5 + 28 + 10191 );

	}

	SECTION ("reset() is correct")
	{
		instance.reset();

		CHECK ( instance.value() == 0 );

		// ***

		auto counter = Counter<int> {};

		REQUIRE ( counter.value() == 0 );

		// --

		counter.increment(6);
		counter.increment(782);

		REQUIRE ( counter.value() == 788 );

		counter.reset();

		CHECK ( counter.value() == 0 );
	}
}

