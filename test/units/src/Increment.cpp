#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Increment<>.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"                // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"        // for Copy, Move
#endif

#include <cstdint>                 // for int32_t

// TEST_CASE ( "Increment", "[increment][dbar][details]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::Increment;
//
// 	auto instance = Increment {};
//
//
// 	SECTION ("Parametized construction is correct")
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ("Copy construction is correct")
// 	{
// 		const Increment copied { Copy<Increment>::construct(instance) };
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy construction test is missing" );
// 	}
//
// 	SECTION ("Move construction is correct")
// 	{
// 		const Increment moved { Move<Increment>::construct(instance) };
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move construction test is missing" );
// 	}
//
// 	SECTION ("Copy assignment is correct")
// 	{
// 		auto copied = Increment {};
// 		Copy<Increment>::assign(copied, instance);
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy assignment test is missing" );
// 	}
//
// 	SECTION ("Move assignment is correct")
// 	{
// 		auto moved = Increment {};
// 		Move<Increment>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move assignment test is missing" );
// 	}
// }


// TEST_CASE ( "Increment default constructed instance",
// 		"[increment][dbar][details]" )
// {
// 	const auto defaulted = arcstk::Increment{};
//
// 	SECTION ( "is empty()")
// 	{
// 		CHECK ( defaulted.empty() );
// 	}
//
// 	SECTION ( "converts to FALSE")
// 	{
// 		CHECK ( !defaulted );
// 	}
// }


// TEST_CASE ( "Increment property", "[increment][dbar][details]" )
// {
// 	using arcstk::Increment;
//
// 	auto instance = Increment {};
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


TEST_CASE ( "Increment functions", "[increment][dbar][details]" )
{
	using arcstk::details::Increment;

	SECTION ( "Increment<T, true>::operator() works" )
	{
		const auto instance_inc = Increment<std::int32_t, true> {};

		CHECK ( instance_inc(467) == 468 );
		CHECK ( instance_inc(17)  ==  18 );
		CHECK ( instance_inc(1)   ==   2 );
		CHECK ( instance_inc(0)   ==   1 );
		CHECK ( instance_inc(-1)  ==   0 );
		CHECK ( instance_inc(-2)  ==  -1 );
	}

	SECTION ( "Increment<T, false>::operator() works" )
	{
		const auto instance_dec = Increment<std::int32_t, false> {};

		CHECK ( instance_dec(467) == 466 );
		CHECK ( instance_dec(17)  ==  16 );
		CHECK ( instance_dec(1)   ==   0 );
		CHECK ( instance_dec(0)   ==  -1 );
		CHECK ( instance_dec(-1)  ==  -2 );
		CHECK ( instance_dec(-2)  ==  -3 );
	}
}

