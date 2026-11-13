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
// 	SECTION ( "Parametized construction is correct" )
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ( "Copy construction is correct" )
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
// 	SECTION ( "Move construction is correct" )
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
// 	SECTION ( "Copy assignment is correct" )
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
// 	SECTION ( "Move assignment is correct" )
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
// 	SECTION ( "is empty()" )
// 	{
// 		CHECK ( defaulted.empty() );
// 	}
//
// 	SECTION ( "converts to FALSE" )
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
// 	SECTION ( "Equality operator == is correct" )
// 	{
// 		FAIL ( "Equality operator test is missing" );
// 	}
//
// 	SECTION ( "Stream-in operator << is correct" )
// 	{
// 		FAIL ( "Stream-in operator << test is missing" );
// 	}
//
// 	SECTION ( "operator bool() is correct" )
// 	{
// 		FAIL ( "operator bool() test is missing" );
// 	}
//
// 	SECTION ( "swap() is correct" )
// 	{
// 		FAIL ( "swap() test is missing" );
// 	}
//
// 	SECTION ( "to_string() is correct" )
// 	{
// 		FAIL ( "to_string() test is missing" );
// 	}
//
// 	SECTION ( "clone() is correct" )
// 	{
// 		FAIL ( "clone() test is missing" );
// 	}
//
// 	SECTION ( "size() is correct" )
// 	{
// 		FAIL ( "size() test is missing" );
// 	}
//
// 	SECTION ( "empty() is correct" )
// 	{
// 		FAIL ( "empty() test is missing" );
// 	}
// }


TEST_CASE ( "Increment functions", "[increment][dbar][details]" )
{
	using arcstk::details::Increment;

	auto i = int { 467 };
	auto j = int {  17 };
	auto k = int {   1 };
	auto l = int {   0 };
	auto m = int {  -1 };
	auto n = int {  -2 };

	SECTION ( "Increment<T, false>::operator() works" )
	{
		const auto instance_inc = Increment<std::int32_t, false> {};
		//auto i = int { 0 };

		instance_inc(i);
		instance_inc(j);
		instance_inc(k);
		instance_inc(l);
		instance_inc(m);
		instance_inc(n);

		CHECK ( i == 468 );
		CHECK ( j ==  18 );
		CHECK ( k ==   2 );
		CHECK ( l ==   1 );
		CHECK ( m ==   0 );
		CHECK ( n ==  -1 );
	}

	SECTION ( "Increment<T, true>::operator() works" )
	{
		const auto instance_dec = Increment<std::int32_t, true> {};

		instance_dec(i);
		instance_dec(j);
		instance_dec(k);
		instance_dec(l);
		instance_dec(m);
		instance_dec(n);

		CHECK ( i == 466 );
		CHECK ( j ==  16 );
		CHECK ( k ==   0 );
		CHECK ( l ==  -1 );
		CHECK ( m ==  -2 );
		CHECK ( n ==  -3 );
	}
}

