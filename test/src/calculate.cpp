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

// AudioSize


TEST_CASE ( "AudioSize", "[calculate] [audiosize]" )
{
	using arcstk::AudioSize;
	using UNIT = arcstk::AudioSize::UNIT;

	AudioSize empty_size{};

	REQUIRE ( empty_size.zero() );

	//

	AudioSize size1;

	REQUIRE ( size1.zero() );

	size1.set_total_frames(253038);

	//

	AudioSize size2; // equals size1
					 //
	REQUIRE ( size2.zero() );

	size2.set_total_frames(253038);

	//

	AudioSize size3(253038,    UNIT::FRAMES);  // equal to size1 and size2
	AudioSize size4(148786344, UNIT::SAMPLES); // equal to size1 and size2
	AudioSize size5(595145376, UNIT::BYTES);   // equal to size1 and size2

	//

	AudioSize different_size; // not equal to size1-5

	REQUIRE ( different_size.zero() );

	different_size.set_total_frames(14827);

	//

	SECTION ("Default construction is as declared")
	{
		CHECK ( std::is_default_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_default_constructible<AudioSize>::value );
	}

	SECTION ("Parametized construction is as declared")
	{
		CHECK ( std::is_constructible<AudioSize,
				const int32_t, const AudioSize::UNIT>::value );

		CHECK ( std::is_nothrow_constructible<AudioSize,
				const int32_t, const AudioSize::UNIT>::value );
	}

	SECTION ("Parametized construction is correct")
	{
		CHECK ( empty_size.zero() );
		CHECK ( 0 == empty_size.total_pcm_bytes() );
		CHECK ( 0 == empty_size.leadout_frame() );

		// constructed with frames
		CHECK ( size1.leadout_frame()   ==    253038 );
		CHECK ( size1.total_frames()    ==    253038 );
		CHECK ( size1.total_samples()   == 148786344 );
		CHECK ( size1.total_pcm_bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size2.leadout_frame()   ==    253038 );
		CHECK ( size2.total_frames()    ==    253038 );
		CHECK ( size2.total_samples()   == 148786344 );
		CHECK ( size2.total_pcm_bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size3.leadout_frame()   ==    253038 );
		CHECK ( size3.total_frames()    ==    253038 );
		CHECK ( size3.total_samples()   == 148786344 );
		CHECK ( size3.total_pcm_bytes() == 595145376 );

		// constructed with samples
		CHECK ( size4.leadout_frame()   ==    253038 );
		CHECK ( size4.total_frames()    ==    253038 );
		CHECK ( size4.total_samples()   == 148786344 );
		CHECK ( size4.total_pcm_bytes() == 595145376 );

		// constructed with bytes
		CHECK ( size5.leadout_frame()   ==    253038 );
		CHECK ( size5.total_frames()    ==    253038 );
		CHECK ( size5.total_samples()   == 148786344 );
		CHECK ( size5.total_pcm_bytes() == 595145376 );

		// different size, constructed with frames
		CHECK ( different_size.leadout_frame()   ==    14827 );
		CHECK ( different_size.total_frames()    ==    14827 );
		CHECK ( different_size.total_samples()   ==  8718276 );
		CHECK ( different_size.total_pcm_bytes() == 34873104 );
	}

	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_copy_constructible<AudioSize>::value );
	}

	// TODO Copy construction correct?

	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_move_constructible<AudioSize>::value );
	}

	// TODO Move construction correct?

	SECTION ("Maximum values are correct")
	{
		CHECK (     449999 == empty_size.max(UNIT::FRAMES)  );
		CHECK (  264599412 == empty_size.max(UNIT::SAMPLES) );
		CHECK ( 1058397648 == empty_size.max(UNIT::BYTES)   );
	}

	SECTION ("Equality operator is correct")
	{
		CHECK ( size1 == size1 );
		CHECK ( size2 == size2 );
		CHECK ( size3 == size3 );
		CHECK ( size4 == size4 );
		CHECK ( size5 == size5 );

		CHECK ( size1 == size2 );
		CHECK ( size2 == size1 );

		CHECK ( size2 == size3 );
		CHECK ( size3 == size2 );

		CHECK ( size3 == size4 );
		CHECK ( size4 == size3 );

		CHECK ( size4 == size5 );
		CHECK ( size5 == size4 );

		CHECK ( size1 == size3 );
		CHECK ( size1 == size4 );
		CHECK ( size1 == size5 );
		CHECK ( size2 == size4 );
		CHECK ( size2 == size5 );
		CHECK ( size3 == size5 );

		CHECK ( different_size != size1 );
		CHECK ( different_size != size2 );
		CHECK ( different_size != size4 );
		CHECK ( different_size != size5 );
	}

	SECTION ("Less-than operator is correct")
	{
		CHECK ( different_size < size1 );
		CHECK ( different_size < size2 );
		CHECK ( different_size < size3 );
		CHECK ( different_size < size4 );
		CHECK ( different_size < size5 );

		CHECK ( !(different_size > size1) );
		CHECK ( !(different_size > size2) );
		CHECK ( !(different_size > size3) );
		CHECK ( !(different_size > size4) );
		CHECK ( !(different_size > size5) );

		CHECK ( empty_size < different_size );
	}

	SECTION ("Swap operation is correct")
	{
		using std::swap;

		swap(size2, different_size);

		CHECK (  14827 == size2.total_frames() );
		CHECK ( 253038 == different_size.total_frames() );

		swap(empty_size, size3);

		CHECK (      0 == size3.total_frames() );
		CHECK ( 253038 == empty_size.total_frames() );
	}
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


// CalculationStateImpl


TEST_CASE ( "CalculationStateImpl", "[calculate] [calculationstateimpl]" )
{
	using arcstk::Algorithm;
	using arcstk::AccurateRipV1V2;
	using arcstk::details::CalculationStateImpl;

	using std::begin;
	using std::end;

	auto algorithm { std::make_unique<AccurateRipV1V2>() };
	auto impl1 { CalculationStateImpl { algorithm.get() } };

	{
		auto dummy_data = std::vector<uint32_t>(1000000);
		std::iota(begin(dummy_data), end(dummy_data), 1);

		const auto start_time { std::chrono::steady_clock::now() };
		impl1.update(begin(dummy_data), end(dummy_data));
		const auto time_elapsed {
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start_time)
		};

		impl1.increment_proc_time_elapsed(time_elapsed);
	}


	SECTION ("Construction is correct")
	{
		auto impl { CalculationStateImpl { algorithm.get() } };

		CHECK ( impl.algorithm()         == algorithm.get() );
		CHECK ( impl.samples_processed() == 0 );
	}


	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<CalculationStateImpl>::value );

		CHECK ( not
			std::is_nothrow_copy_constructible<CalculationStateImpl>::value );
	}


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<CalculationStateImpl>::value );

		CHECK ( std::is_nothrow_move_constructible<CalculationStateImpl>::value );
	}


	SECTION ("Copy construction is correct")
	{
		auto impl2 { impl1 };

		CHECK ( impl2.algorithm()         == algorithm.get() );
		CHECK ( impl2.samples_processed() == 1000000 );
		CHECK ( impl2.proc_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("Move construction is correct")
	{
		auto impl3 { std::move(impl1) };

		CHECK ( impl3.algorithm()         == algorithm.get() );
		CHECK ( impl3.samples_processed() == 1000000 );
		CHECK ( impl3.proc_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("update() counts the amount of samples processed")
	{
		CHECK ( impl1.samples_processed() == 1000000 );
	}


	SECTION ("increment_proc_time_elapsed() updates time counter")
	{
		CHECK ( impl1.proc_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("current_subtotal() returns the subtotals")
	{
		auto checksums { impl1.current_subtotal() };

		CHECK ( checksums.size() == 2 );
	}
}


// Calculation


TEST_CASE ( "Calculation", "[calculate] [calculation]" )
{
	using arcstk::Calculation;
	using arcstk::Context;
	using arcstk::Algorithm;
	using arcstk::TOC;
	using arcstk::AudioSize;
	using arcstk::make_calculation;
	using arcstk::AccurateRipV1V2;
	using arcstk::checksum::type;
	using arcstk::Settings;


	const auto toc = arcstk::details::TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	const auto size { AudioSize { 253038, AudioSize::UNIT::FRAMES } };

	auto calculation { Calculation(Context::ALBUM,
			std::make_unique<AccurateRipV1V2>(),
			size, arcstk::toc::get_offsets(toc)) };

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
				const std::vector<int32_t>&>::value
				);

		CHECK ( not std::is_trivially_constructible<Calculation,
				const Settings&, std::unique_ptr<Algorithm>, const AudioSize&,
				const std::vector<int32_t>&>::value
				);

		CHECK ( not std::is_nothrow_constructible<Calculation,
				const Settings&, std::unique_ptr<Algorithm>, const AudioSize&,
				const std::vector<int32_t>&>::value
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

		CHECK ( calculation.proc_time_elapsed().count() == 0 );

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

		CHECK ( c2.proc_time_elapsed().count() == 0 );

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

		CHECK ( c3.proc_time_elapsed().count() == 0 );

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
	// 		CDDA::MAX_OFFSET + 1, AudioSize::UNIT::FRAMES };
	//
	// 	const auto size_too_small = AudioSize { // smaller than allowed MIN
	// 		CDDA::MIN_TRACK_LEN_FRAMES - 1, AudioSize::UNIT::FRAMES };


	SECTION ("make_calculation() with complete TOC succeeds")
	{
		using arcstk::checksum::type;

		const auto toc_1 = arcstk::details::TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		auto algorithmV1V2 { std::make_unique<AccurateRipV1V2>() };

		const auto algo_types = algorithmV1V2->types();

		auto calc { make_calculation(std::move(algorithmV1V2), *toc_1) };

		CHECK ( calc->samples_expected()  == 253038 * 588 );
		CHECK ( calc->samples_processed() == 0 );
		CHECK ( calc->samples_todo()      == 253038 * 588 );
		CHECK ( calc->proc_time_elapsed().count() == 0 );
		CHECK ( not calc->complete() );
		CHECK ( calc->result().empty() );

		CHECK ( calc->types() == algo_types );
	}


	SECTION ("make_calculation() with incomplete TOC succeeds")
	{
		using arcstk::checksum::type;

		const auto toc_1 = arcstk::details::TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		);

		auto algorithmV1V2 { std::make_unique<AccurateRipV1V2>() };

		auto c { make_calculation(std::move(algorithmV1V2), *toc_1) };

		CHECK ( c->types() ==
				std::unordered_set<type>{ type::ARCS1, type::ARCS2 } );
	}
}

