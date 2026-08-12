#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for AlgorithmTypes.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "AlgorithmTypes<>", "[algorithmtypes] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::AccurateRip::V1;
	using arcstk::AccurateRip::V2;
	using arcstk::AlgorithmTypes;

	auto instance = AlgorithmTypes<V1, V2> {};

	REQUIRE ( instance.count == 2 );


	SECTION ("Copy construction is correct")
	{
		using MyTypes = AlgorithmTypes<V1, V2>;

		const MyTypes copied { Copy<MyTypes>::construct(instance) };

		// --

        CHECK ( copied.count == 2 );
	}

	SECTION ("Move construction is correct")
	{
		using MyTypes = AlgorithmTypes<V1, V2>;

		const MyTypes moved { Move<MyTypes>::construct(instance) };

		// --

        CHECK ( moved.count == 2 );
	}

	SECTION ("Copy assignment is correct")
	{
		using MyTypes = AlgorithmTypes<V1, V2>;

		auto copied = MyTypes {};
		Copy<MyTypes>::assign(copied, instance);

		// --

        CHECK ( copied.count == 2 );
	}

	SECTION ("Move assignment is correct")
	{
		using MyTypes = AlgorithmTypes<V1, V2>;

		auto moved = MyTypes {};
		Move<MyTypes>::assign(moved, instance);

		// --

        CHECK ( moved.count == 2 );
	}
}


TEST_CASE ( "AlgorithmTypes<> default constructed instance",
		"[algorithmtypes] [calc] [calculate]" )
{
	using arcstk::AccurateRip::V1;
	using arcstk::AccurateRip::V2;
	using arcstk::AlgorithmTypes;

	const auto defaulted_1 = AlgorithmTypes<V2> {};
	const auto defaulted_2 = AlgorithmTypes<V1, V2> {};

	SECTION ( "contains correct number of types" )
	{
		CHECK ( defaulted_1.count == 1 );
		CHECK ( defaulted_2.count == 2 );
	}
}


// TEST_CASE ( "AlgorithmTypes<> property", "[algorithmtypes] [calc] [calculate]" )
// {
// 	using arcstk::AccurateRip::V1;
// 	using arcstk::AccurateRip::V2;
// 	using arcstk::AlgorithmTypes;
//
// 	auto instance = AlgorithmTypes<V1, V2> {};
//
//
// 	// SECTION ("Equality operator == is correct")
// 	// {
// 	// 	FAIL ( "Equality operator test is missing" );
// 	// }
// 	//
// 	// SECTION ("Stream-in operator << is correct")
// 	// {
// 	// 	FAIL ( "Stream-in operator << test is missing" );
// 	// }
//
// 	// SECTION ("to_string() is correct")
// 	// {
// 	// 	FAIL ( "to_string() test is missing" );
// 	// }
// }


// TEST_CASE ( "AlgorithmTypes<> functions",
// 		"[algorithmtypes] [calc] [calculate]" )
// {
// 	using arcstk::AccurateRip::V1;
// 	using arcstk::AccurateRip::V2;
// 	using arcstk::AlgorithmTypes;
//
// 	using Types = AlgorithmTypes<V1, V2> {};
//
// 	SECTION ("configure() is correct")
// 	{
// 		Types::configure(...);
// 	}
//
// 	SECTION ("typed_calculationset_for() is correct")
// 	{
// 		using arcstk::CalculationSet;
// 		using arcstk::Context;
//
// 		using B = ...;
// 		using E = ...;
//
// 		auto set = std::unique_ptr<CalculationSet> {
// 			Types::typed_calculationset_for<B, E>( { Context::LAST_TRACK }) };
// 	}
//
// 	SECTION ("calculationset_for() is correct")
// 	{
// 		using arcstk::Context;
//
// 		using B = ...;
// 		using E = ...;
//
// 		auto set = Types::calculationset_for<B, E>({ Context::LAST_TRACK });
// 	}
// }

