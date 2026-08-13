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


TEST_CASE ( "CalculationUpdater<> Old",
		"[calculationupdater] [calc] [calculate]" )
{
	using arcstk::AccurateRip::V1;
	using arcstk::AccurateRip::V1andV2;
	using arcstk::Algorithm;
	using arcstk::AudioSize;
	using arcstk::Calculation;
	using arcstk::Context;
	using arcstk::Points;
	using arcstk::Settings;
	using arcstk::ToC;
	using arcstk::UNIT;
	using arcstk::CalculationUpdater;
	using arcstk::checksum::type;
	using arcstk::make_toc;


	const auto toc = make_toc(
		// leadout
		253038,
		// offsets
		std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
			106333, 139495, 157863, 198495, 213368, 225320, 234103 }
	);

	const auto size      { AudioSize { 253038, UNIT::FRAMES } };

	auto calculation     { CalculationUpdater<V1andV2>(
			Settings { Context::ALBUM },
			toc.offsets(), size) };

	const auto algorithm { calculation.algorithm() };

	const auto result    { calculation.result() };

	//

	SECTION ("Is default constructible")
	{
		CHECK ( std::is_default_constructible<
				CalculationUpdater<V1andV2>>::value );
	}


	SECTION ("Parametized construction is as declared")
	{
		CHECK ( std::is_constructible<CalculationUpdater<V1andV2>,
				const Settings&,
				const Points&, const AudioSize&>::value
				);

		CHECK ( not std::is_trivially_constructible<CalculationUpdater<V1andV2>,
				const Settings&,
				const Points&, const AudioSize&>::value
				);

		CHECK ( not std::is_nothrow_constructible<CalculationUpdater<V1andV2>,
				const Settings&,
				const Points&, const AudioSize&>::value
				);
	}


	SECTION ("Parametized construction is correct")
	{
		CHECK ( calculation.algorithm() == algorithm );
		CHECK ( calculation.algorithm()->types() ==
				std::unordered_set<type> { type::ARCS1, type::ARCS2 } );

		CHECK ( calculation.samples_expected() == 148786344 );

		CHECK ( calculation.samples_processed() == 0 );

		CHECK ( calculation.samples_todo() == 148786344 );
		CHECK ( calculation.samples_todo() == calculation.samples_expected() );

		CHECK ( calculation.update_time_elapsed().count() == 0 );

		CHECK ( not calculation.complete() );

		//CHECK ( result.empty() );
	}


	/*
	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<Calculation<V1andV2>>::value );

		CHECK ( not std::is_nothrow_copy_constructible<Calculation<V1andV2>>::value );
	}


	SECTION ("Copy construction is correct")
	{
		auto c2 { calculation };

		// Algorithm instance is cloned when constructing c2
		CHECK ( c2.algorithm() != calculation.algorithm() );
		CHECK ( c2.algorithm() != algorithm );

		CHECK ( c2.algorithm()->types() ==
				std::unordered_set<type> { type::ARCS1, type::ARCS2 } );

		CHECK ( c2.samples_expected() == 148786344 );

		CHECK ( c2.samples_processed() == 0 );

		CHECK ( c2.samples_todo() == 148786344 );
		CHECK ( c2.samples_todo() == calculation.samples_expected() );

		CHECK ( c2.update_time_elapsed().count() == 0 );

		CHECK ( not c2.complete() );

		CHECK ( c2.result().empty() );
	}
	*/


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<
				CalculationUpdater<V1andV2>>::value );

		CHECK ( std::is_nothrow_move_constructible<
				CalculationUpdater<V1andV2>>::value );
	}


	SECTION ("Move construction is correct")
	{
		auto c3 { std::move(calculation) };

		// Algorithm instance is moved when constructing c2
		CHECK ( c3.algorithm() == algorithm );

		CHECK ( c3.algorithm()->types() ==
				std::unordered_set<type> { type::ARCS1, type::ARCS2 } );

		CHECK ( c3.samples_expected() == 148786344 );

		CHECK ( c3.samples_processed() == 0 );

		CHECK ( c3.samples_todo() == 148786344 );
		CHECK ( c3.samples_todo() == c3.samples_expected() );

		CHECK ( c3.update_time_elapsed().count() == 0 );

		CHECK ( not c3.complete() );

		//CHECK ( c3.result().empty() );
	}


	SECTION ("Instantiating a vector<Calculation> succeeds")
	{
		auto calculations = std::vector<CalculationUpdater<V1andV2>>();
		calculations.reserve(5);

		CHECK ( calculations.capacity() == 5 );
	}


	SECTION ("Construction with complete ToC succeeds")
	{
		using arcstk::checksum::type;

		const auto toc_1 = make_toc(
			// leadout
			253038,
			// offsets
			std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508,
				87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		);

		auto algorithmV1V2 { std::make_unique<V1andV2>() };

		const auto algo_types = algorithmV1V2->types();

		auto calc { CalculationUpdater<V1andV2>(
				Settings { Context::ALBUM },
				toc_1)
		};

		CHECK ( calc.samples_expected()  == 253038 * 588 );
		CHECK ( calc.samples_processed() == 0 );
		CHECK ( calc.samples_todo()      == 253038 * 588 );
		CHECK ( calc.update_time_elapsed().count() == 0 );
		CHECK ( not calc.complete() );
		//CHECK ( calc.result().empty() );

		CHECK ( calc.types() == algo_types );
	}


	SECTION ("Construction with incomplete ToC succeeds")
	{
		using arcstk::checksum::type;

		const auto toc_1 = make_toc(
			// offsets
			std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508,
				87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 }
			//// lengths
			//{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
			//	18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		);

		auto algorithmV1V2 { std::make_unique<V1andV2>() };

		auto c { CalculationUpdater<V1andV2>(
				Settings { Context::ALBUM },
				toc_1)
		};

		CHECK ( c.types() ==
				std::unordered_set<type>{ type::ARCS1, type::ARCS2 } );
	}
}


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

