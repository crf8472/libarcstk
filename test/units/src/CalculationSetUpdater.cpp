#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for CalculationSetUpdater.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


// TEST_CASE ( "CalculationSetUpdater",
// 		"[calculationsetupdater] [calc] [calculate]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::AccurateRip::V1;
// 	using arcstk::AccurateRip::V2;
// 	using arcstk::CalculationSetUpdater;
//
// 	auto instance = CalculationSetUpdater<V1,V2> {};
//
//
// 	// SECTION ("Parametized construction is correct")
// 	// {
// 	// 	FAIL ( "Parametized construction test is missing" );
// 	// }
//
// 	SECTION ("Copy construction is correct")
// 	{
// 		using MyType = CalculationSetUpdater<V1,V2>;
//
// 		const MyType copied { Copy<MyType>::construct(instance) };
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 	}
//
// 	SECTION ("Move construction is correct")
// 	{
// 		using MyType = CalculationSetUpdater<V1,V2>;
//
// 		const MyType moved { Move<MyType>::construct(instance) };
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 	}
//
// 	SECTION ("Copy assignment is correct")
// 	{
// 		using MyType = CalculationSetUpdater<V1,V2>;
//
// 		auto copied = MyType {};
// 		Copy<MyType>::assign(copied, instance);
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 	}
//
// 	SECTION ("Move assignment is correct")
// 	{
// 		using MyType = CalculationSetUpdater<V1,V2>;
//
// 		auto moved = MyType {};
// 		Move<MyType>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 	}
// }


TEST_CASE ( "CalculationSetUpdater default constructed instance",
		"[calculationsetupdater] [calc] [calculate]" )
{
	using arcstk::csample_t;
	using ci_t = std::vector<csample_t>::const_iterator;
	using arcstk::CalculationSetUpdater;

	const auto defaulted = CalculationSetUpdater<ci_t,ci_t> {};

	SECTION ( "is empty()")
	{
		CHECK ( ! defaulted.empty() ); // V1andV2 is default
		CHECK ( defaulted.size() == 1 );
	}
}


// TEST_CASE ( "CalculationSetUpdater property",
// 		"[calculationsetupdater] [calc] [calculate]" )
// {
// 	using arcstk::AccurateRip::V1;
// 	using arcstk::AccurateRip::V2;
// 	using arcstk::CalculationSetUpdater;
//
// 	auto instance = CalculationSetUpdater<V1,V2> {};
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


// TEST_CASE ( "CalculationSetUpdater functions",
// 		"[calculationsetupdater] [calc] [calculate]" )
// {
// 	using arcstk::AccurateRip::V1;
// 	using arcstk::AccurateRip::V2;
// 	using arcstk::CalculationSetUpdater;
//
// 	auto instance = CalculationSetUpdater<V1,V2> {};
//
// 	SECTION ("CalculationSetUpdater() is correct")
// 	{
// 		FAIL ("CalculationSetUpdater() test is missing");
// 	}
//
// 	SECTION ("add() is correct")
// 	{
// 		FAIL ("add() test is missing");
// 	}
//
// 	SECTION ("update() is correct")
// 	{
// 		FAIL ("update() test is missing");
// 	}
//
// 	SECTION ("do_init() is correct")
// 	{
// 		FAIL ("do_init() test is missing");
// 	}
//
// 	SECTION ("do_total_tracks() is correct")
// 	{
// 		FAIL ("do_total_tracks() test is missing");
// 	}
//
// 	SECTION ("do_offsets() is correct")
// 	{
// 		FAIL ("do_offsets() test is missing");
// 	}
//
// 	SECTION ("do_leadout() is correct")
// 	{
// 		FAIL ("do_leadout() test is missing");
// 	}
//
// 	SECTION ("do_update() is correct")
// 	{
// 		FAIL ("do_update() test is missing");
// 	}
//
// 	SECTION ("do_result() is correct")
// 	{
// 		FAIL ("do_result() test is missing");
// 	}
//
// 	SECTION ("do_size() is correct")
// 	{
// 		FAIL ("do_size() test is missing");
// 	}
//
// 	SECTION ("do_empty() is correct")
// 	{
// 		FAIL ("do_empty() test is missing");
// 	}
//
// 	SECTION ("do_complete() is correct")
// 	{
// 		FAIL ("do_complete() test is missing");
// 	}
//
// 	SECTION ("merge_results() is correct")
// 	{
// 		FAIL ("merge_results() test is missing");
// 	}
//
// }


// This test is broken due to false partitioning
// TEST_CASE ( "CalculationSet", "[calculationset] [calc]" )
// {
// 	using arcstk::AlgorithmTypes;
// 	using arcstk::AccurateRip::V1andV2;
// 	using arcstk::AudioSize;
// 	using arcstk::UNIT;
// 	using arcstk::checksum::type;
//
// 	using std::cbegin;
// 	using std::cend;
//
// 	std::vector<uint32_t> samples { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
//
// 	using start_type = decltype( samples.cbegin() );
// 	using stop_type  = decltype( samples.cend() );
//
// 	auto calc_set = AlgorithmTypes<V1andV2>::typed_calculationset_for<
// 		start_type, stop_type>({});
//
// 	REQUIRE ( calc_set.size() == 1 );
// 	REQUIRE ( ! calc_set.complete() );
// 	REQUIRE ( calc_set.result().empty() );
//
// 	calc_set.init({ { 0, UNIT::FRAMES } }, { 10, UNIT::SAMPLES });
//
// 	calc_set.update(cbegin(samples),     cbegin(samples) + 4);
// 	calc_set.update(cbegin(samples) + 5, cbegin(samples) + 9);
//
// 	const auto checksums = calc_set.result();
// 	const auto& track = checksums[0];
// 	const auto types = track.types();
// 	const auto [ checksum1, exists1 ] = track.get(type::ARCS1);
// 	const auto [ checksum2, exists2 ] = track.get(type::ARCS2);
//
// 	SECTION ("Result of an instantiated CalculationSet is as expected")
// 	{
// 		CHECK ( not checksums.empty() );
// 		CHECK ( checksums.size() == 1 );
//
// 		CHECK ( track.length() == AudioSize { 0, UNIT::SAMPLES } );
// 		CHECK ( ! track.contains( type::ARCS1 ) );
// 		CHECK ( ! track.contains( type::ARCS2 ) );
//
// 		CHECK ( ! exists1 );
// 		CHECK ( checksum1.zero() );
//
// 		CHECK ( ! exists2 );
// 		CHECK ( checksum2.zero() );
// 	}
// }

