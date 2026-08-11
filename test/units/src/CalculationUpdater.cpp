#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for CalculationUpdater.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"         // for AccurateRip::V1
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "CalculationUpdater<>", "[calculationupdater] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using AccurateRipV1 = arcstk::AccurateRip::V1;
	using arcstk::CalculationUpdater;

	auto instance = CalculationUpdater<AccurateRipV1> {};


	SECTION ("Parametized construction is correct")
	{
		auto v1 = CalculationUpdater<AccurateRipV1> {};

		CHECK ( v1.algorithm_name() == "AccurateRip v1" );
	}

	// SECTION ("Copy construction is correct")
	// {
	// 	using UpdaterType = CalculationUpdater<AccurateRipV1>;
	// 	const UpdaterType copied { Copy<UpdaterType>::construct(instance) };
	//
	// 	// --
	//
	// 	CHECK ( copied.algorithm_name() == "AccurateRip v1" );
	// }

	SECTION ("Move construction is correct")
	{
		using UpdaterType = CalculationUpdater<AccurateRipV1>;
		const UpdaterType moved { Move<UpdaterType>::construct(instance) };

		// --

		CHECK ( moved.algorithm_name() == "AccurateRip v1" );
	}

	// SECTION ("Copy assignment is correct")
	// {
	// 	using UpdaterType = CalculationUpdater<AccurateRipV1>;
	// 	auto copied = UpdaterType {};
	// 	Copy<UpdaterType>::assign(copied, instance);
	//
	// 	// --
	//
	// 	CHECK ( copied.algorithm_name() == "AccurateRip v1" );
	// }

	SECTION ("Move assignment is correct")
	{
		using UpdaterType = CalculationUpdater<AccurateRipV1>;
		auto moved = UpdaterType {};
		Move<UpdaterType>::assign(moved, instance);

		// --

		CHECK ( moved.algorithm_name() == "AccurateRip v1" );
	}
}


TEST_CASE ( "CalculationUpdater<> default constructed instance",
		"[calculationupdater] [calc] [calculate]" )
{
	using AccurateRipV1 = arcstk::AccurateRip::V1;
	using arcstk::CalculationUpdater;
	using arcstk::Context;

	auto defaulted = CalculationUpdater<AccurateRipV1> {};

	SECTION ( "has default Context")
	{
		CHECK ( defaulted.settings().context() == Context::ALBUM );
	}
}


// TEST_CASE ( "CalculationUpdater property",
// 		"[calculationupdater] [calc] [calculate]" )
// {
// 	using AccurateRipV1 = arcstk::AccurateRip::V1;
// 	using arcstk::CalculationUpdater;
//
// 	auto defaulted = CalculationUpdater<AccurateRipV1> {};
//
//
// 	SECTION ("swap() is correct")
// 	{
// 		FAIL ( "swap() test is missing" );
// 	}
// }


TEST_CASE ( "CalculationUpdater<> functions",
		"[calculationupdater] [calc] [calculate]" )
{
	using AccurateRipV1 = arcstk::AccurateRip::V1;
	using arcstk::CalculationUpdater;
	using arcstk::Context;

	auto instance = CalculationUpdater<AccurateRipV1> {};


	SECTION ("algorithm_name() is correct")
	{
		CHECK ( instance.algorithm_name() == "AccurateRip v1" );
	}

	// SECTION ("update() is correct")
	// {
	// 	FAIL ("update test is missing");
	// }
	//
	// SECTION ("swap() is correct")
	// {
	// 	FAIL ("swap test is missing");
	// }
	//
	// SECTION ("do_algorithm() is correct")
	// {
	// 	FAIL ("do_algorithm test is missing");
	// }
	//
	// SECTION ("do_total_tracks() is correct")
	// {
	// 	FAIL ("do_total_tracks test is missing");
	// }
	//
	// SECTION ("do_offsets() is correct")
	// {
	// 	FAIL ("do_offsets test is missing");
	// }
	//
	// SECTION ("do_leadout() is correct")
	// {
	// 	FAIL ("do_leadout test is missing");
	// }
	//
	// SECTION ("do_init() is correct")
	// {
	// 	FAIL ("do_init test is missing");
	// }
	//
	// SECTION ("on_settings_changed() is correct")
	// {
	// 	FAIL ("on_settings_changed test is missing");
	// }
	//
	// SECTION ("on_completion() is correct")
	// {
	// 	FAIL ("on_completion test is missing");
	// }
	//
	// SECTION ("update_impl() is correct")
	// {
	// 	FAIL ("update_impl test is missing");
	// }
}

