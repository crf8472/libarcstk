#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for classes in module calculate
 */

#include <chrono>
#include <numeric>      // for iota

#ifndef __LIBARCSTK_ALGORITHMS_HPP__
#include "algorithms.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#include "calculate_impl.hpp"
#endif

#include <vector> // for vector


// Algorithm


TEST_CASE ( "Algorithm", "[calculate] [algorithm]" )
{

}


// Context


TEST_CASE ( "Context", "[calculate] [context]" )
{
	using arcstk::Context;

	SECTION ( "Constants behave as expected" )
	{
		CHECK ((Context::FIRST_TRACK | Context::NONE) == Context::FIRST_TRACK);
		CHECK ((Context::LAST_TRACK  | Context::NONE) == Context::LAST_TRACK);
		CHECK ((Context::ALBUM       | Context::NONE) == Context::ALBUM);

		CHECK ((Context::FIRST_TRACK | Context::LAST_TRACK)  == Context::ALBUM);
		CHECK ((Context::FIRST_TRACK | Context::ALBUM)       == Context::ALBUM);

		CHECK ((Context::LAST_TRACK  | Context::FIRST_TRACK) == Context::ALBUM);
		CHECK ((Context::LAST_TRACK  | Context::ALBUM)       == Context::ALBUM);

		CHECK ((Context::ALBUM       | Context::LAST_TRACK)  == Context::ALBUM);
		CHECK ((Context::ALBUM       | Context::FIRST_TRACK) == Context::ALBUM);
	}

	SECTION ( "any() is correct" )
	{
		CHECK ( not any(Context::NONE) );

		CHECK ( any(Context::FIRST_TRACK) );
		CHECK ( any(Context::LAST_TRACK) );
		CHECK ( any(Context::ALBUM) );
	}
}


// Calculation


TEST_CASE ( "Calculation", "[calculate] [calculation]" )
{
	using arcstk::AccurateRipV1V2;
	using arcstk::AudioSize;
	using arcstk::Algorithm;
	using arcstk::Calculation;
	using arcstk::checksum::type;
	using arcstk::Context;
	using arcstk::make_calculation;
	using arcstk::make_toc;
	using arcstk::Points;
	using arcstk::Settings;
	using arcstk::ToC;
	using arcstk::UNIT;


	const auto toc = make_toc(
		// leadout
		253038,
		// offsets
		std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
			106333, 139495, 157863, 198495, 213368, 225320, 234103 }
	);

	const auto size { AudioSize { 253038, UNIT::FRAMES } };

	auto calculation { Calculation(Context::ALBUM,
			std::make_unique<AccurateRipV1V2>(),
			size, toc->offsets()) };

	const auto algorithm { calculation.algorithm() };

	const auto result { calculation.result() };

	//

	SECTION ("Default construction is as declared")
	{
		CHECK ( not std::is_default_constructible<Calculation>::value );
		CHECK ( not std::is_trivially_default_constructible<Calculation>::value );
		CHECK ( not std::is_nothrow_default_constructible<Calculation>::value );
	}


	SECTION ("Parametized construction is as declared")
	{
		CHECK ( std::is_constructible<Calculation,
				const Settings&, std::unique_ptr<Algorithm>, const AudioSize&,
				const Points&>::value
				);

		CHECK ( not std::is_trivially_constructible<Calculation,
				const Settings&, std::unique_ptr<Algorithm>, const AudioSize&,
				const Points&>::value
				);

		CHECK ( not std::is_nothrow_constructible<Calculation,
				const Settings&, std::unique_ptr<Algorithm>, const AudioSize&,
				const Points&>::value
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

		CHECK ( result.empty() );
	}


	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<Calculation>::value );

		CHECK ( not std::is_nothrow_copy_constructible<Calculation>::value );
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


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<Calculation>::value );

		CHECK ( std::is_nothrow_move_constructible<Calculation>::value );
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

		CHECK ( c3.result().empty() );
	}


	SECTION ("Instantiating a vector<Calculation> succeeds")
	{
		auto calculations = std::vector<Calculation>();
		calculations.reserve(5);

		CHECK ( calculations.capacity() == 5 );
	}

	// 	const auto size_too_big = AudioSize { // bigger than allowed MAX
	// 		CDDA::MAX_OFFSET + 1, UNIT::FRAMES };
	//
	// 	const auto size_too_small = AudioSize { // smaller than allowed MIN
	// 		CDDA::MIN_TRACK_LEN_FRAMES - 1, UNIT::FRAMES };


	SECTION ("make_calculation() with complete ToC succeeds")
	{
		using arcstk::checksum::type;

		const auto toc_1 = make_toc(
			// leadout
			253038,
			// offsets
			std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508,
				87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		);

		auto algorithmV1V2 { std::make_unique<AccurateRipV1V2>() };

		const auto algo_types = algorithmV1V2->types();

		auto calc { make_calculation(std::move(algorithmV1V2), *toc_1) };

		CHECK ( calc->samples_expected()  == 253038 * 588 );
		CHECK ( calc->samples_processed() == 0 );
		CHECK ( calc->samples_todo()      == 253038 * 588 );
		CHECK ( calc->update_time_elapsed().count() == 0 );
		CHECK ( not calc->complete() );
		CHECK ( calc->result().empty() );

		CHECK ( calc->types() == algo_types );
	}


	SECTION ("make_calculation() with incomplete ToC succeeds")
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

		auto algorithmV1V2 { std::make_unique<AccurateRipV1V2>() };

		auto c { make_calculation(std::move(algorithmV1V2), *toc_1) };

		CHECK ( c->types() ==
				std::unordered_set<type>{ type::ARCS1, type::ARCS2 } );
	}
}

