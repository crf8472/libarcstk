#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for implementation details in module calculate
 */

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"                // for Points
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#include "calculate_impl.hpp"           // for CalculationStateImpl
#endif
#ifndef __LIBARCSTK_ALGORITHMS_HPP__
#include "algorithms.hpp"
#endif

#include <numeric>  // for iota

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
		impl1.track_finished();

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


	SECTION ("update() counts the amount of samples processed correctly")
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


TEST_CASE ( "perform_update", "[calculate]" )
{
	using arcstk::Algorithm;
	using arcstk::AccurateRipV1V2;
	using arcstk::AudioSize;
	using arcstk::Checksums;
	using arcstk::Context;
	using arcstk::Points;
	using arcstk::Settings;
	using arcstk::UNIT;
	using arcstk::checksum::type;

	using arcstk::details::ind2am;
	using arcstk::details::Interval;
	using arcstk::details::TrackPartitioner;
	using arcstk::details::CalculationStateImpl;
	using arcstk::details::perform_update;

	using std::cbegin;
	using std::cend;
	using std::begin;
	using std::end;

	// This test simulates the calculation of an album

	const auto s { Settings { Context::ALBUM } };

	auto algorithm { std::make_unique<AccurateRipV1V2>() };
	algorithm->set_settings(&s);

	auto state { CalculationStateImpl { algorithm.get() } };

	REQUIRE ( state.algorithm() == algorithm.get() );

	/* use Bach, Organ Concertos, Simon Preston, DGG */
	const auto partitioner { TrackPartitioner {
		/* total samples */ 253038 * 588 /* 148786344 */,
		{ /* split points (track offsets) */
			AudioSize {     33 * 588, UNIT::SAMPLES },
			AudioSize {   5225 * 588, UNIT::SAMPLES },
			AudioSize {   7390 * 588, UNIT::SAMPLES },
			AudioSize {  23380 * 588, UNIT::SAMPLES },
			AudioSize {  35608 * 588, UNIT::SAMPLES },
			AudioSize {  49820 * 588, UNIT::SAMPLES },
			AudioSize {  69508 * 588, UNIT::SAMPLES },
			AudioSize {  87733 * 588, UNIT::SAMPLES },
			AudioSize { 106333 * 588, UNIT::SAMPLES },
			AudioSize { 139495 * 588, UNIT::SAMPLES },
			AudioSize { 157863 * 588, UNIT::SAMPLES },
			AudioSize { 198495 * 588, UNIT::SAMPLES },
			AudioSize { 213368 * 588, UNIT::SAMPLES },
			AudioSize { 225320 * 588, UNIT::SAMPLES },
			AudioSize { 234103 * 588, UNIT::SAMPLES }
		},
		/* legal range w/ skips */ { 33 * 588 + 2939, 253038 * 588 - 2940 },
	}};

	REQUIRE ( partitioner.total_samples()       == 148786344 );
	REQUIRE ( partitioner.legal_range().lower() == 22343 );
	REQUIRE ( partitioner.legal_range().upper() == 148783404 );
	// TODO Verify split points

	Checksums buffer { Checksums{}  };

	REQUIRE ( buffer.size() == 0 );

	// for convenience
	const int32_t skipped_front { 19404 + 2939 }; // equivalent to legal lower

	auto dummy_data = std::vector<uint32_t>(148786344 );
	std::iota(begin(dummy_data), end(dummy_data), 1);


	SECTION ("Updating album w/ block_size 16777216 yields correct checksums")
	{
		// This simulates libarcsdec:readerwav

		const auto block_size = int { 16777216 }; // samples
		auto r = bool { true };

		r = perform_update(	cbegin(dummy_data) + 0 * block_size,
							cbegin(dummy_data) + 1 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == block_size );
		CHECK ( state.samples_processed() == block_size - skipped_front );
		CHECK ( buffer.size()             == 3 );

		CHECK ( buffer[ 0].get(type::ARCS1) == 0x0AF18BB6u );
		CHECK ( buffer[ 0].get(type::ARCS2) == 0x8FBB68BAu );

		CHECK ( buffer[ 1].get(type::ARCS1) == 0x60F64E9Au );
		CHECK ( buffer[ 1].get(type::ARCS2) == 0x8D040A9Au );

		CHECK ( buffer[ 2].get(type::ARCS1) == 0xBC5C57ECu );
		CHECK ( buffer[ 2].get(type::ARCS2) == 0x2A4FD377u );

		r = perform_update( cbegin(dummy_data) + 1 * block_size,
							cbegin(dummy_data) + 2 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 2 * block_size );
		CHECK ( state.samples_processed() == 2 * block_size - skipped_front );
		CHECK ( buffer.size()             == 5 );

		CHECK ( buffer[ 3].get(type::ARCS1) == 0xD394FC08u );
		CHECK ( buffer[ 3].get(type::ARCS2) == 0xCE55344Bu );

		CHECK ( buffer[ 4].get(type::ARCS1) == 0xD52E3008u );
		CHECK ( buffer[ 4].get(type::ARCS2) == 0x022C486Du );

		r = perform_update( cbegin(dummy_data) + 2 * block_size,
							cbegin(dummy_data) + 3 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 3 * block_size );
		CHECK ( state.samples_processed() == 3 * block_size - skipped_front );
		CHECK ( buffer.size()             == 6 );

		CHECK ( buffer[ 5].get(type::ARCS1) == 0x528B55D0u );
		CHECK ( buffer[ 5].get(type::ARCS2) == 0xC4778057u );

		r = perform_update( cbegin(dummy_data) + 3 * block_size,
							cbegin(dummy_data) + 4 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 4 * block_size );
		CHECK ( state.samples_processed() == 4 * block_size - skipped_front );
		CHECK ( buffer.size()             == 8 );

		CHECK ( buffer[ 6].get(type::ARCS1) == 0xB53625EAu );
		CHECK ( buffer[ 6].get(type::ARCS2) == 0x29DF16E5u );

		CHECK ( buffer[ 7].get(type::ARCS1) == 0x55480A90u );
		CHECK ( buffer[ 7].get(type::ARCS2) == 0x390C2F05u );

		r = perform_update( cbegin(dummy_data) + 4 * block_size,
							cbegin(dummy_data) + 5 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 5 * block_size );
		CHECK ( state.samples_processed() == 5 * block_size - skipped_front );
		CHECK ( buffer.size()             == 9 );

		CHECK ( buffer[ 8].get(type::ARCS1) == 0x53262404u );
		CHECK ( buffer[ 8].get(type::ARCS2) == 0xA8B5ADDDu );

		r = perform_update( cbegin(dummy_data) + 5 * block_size,
							cbegin(dummy_data) + 6 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 6 * block_size );
		CHECK ( state.samples_processed() == 6 * block_size - skipped_front );
		CHECK ( buffer.size()             == 10 );

		CHECK ( buffer[ 9].get(type::ARCS1) == 0x33A23980u );
		CHECK ( buffer[ 9].get(type::ARCS2) == 0x4D9350B0u );

		r = perform_update( cbegin(dummy_data) + 6 * block_size,
							cbegin(dummy_data) + 7 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 7 * block_size );
		CHECK ( state.samples_processed() == 7 * block_size - skipped_front );
		CHECK ( buffer.size()             == 11 );

		CHECK ( buffer[10].get(type::ARCS1) == 0xB66906B0u );
		CHECK ( buffer[10].get(type::ARCS2) == 0x49D26578u );

		r = perform_update( cbegin(dummy_data) + 7 * block_size,
							cbegin(dummy_data) + 8 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 8 * block_size );
		CHECK ( state.samples_processed() == 8 * block_size - skipped_front );
		CHECK ( buffer.size()             == 13 );

		CHECK ( buffer[11].get(type::ARCS1) == 0x2BE3B232u );
		CHECK ( buffer[11].get(type::ARCS2) == 0x355C7E28u );

		CHECK ( buffer[12].get(type::ARCS1) == 0x5D229B60u );
		CHECK ( buffer[12].get(type::ARCS2) == 0x970C0A35u );

		r = perform_update( cbegin(dummy_data) + 8 * block_size,
							cbegin(dummy_data) + 9 * block_size,
							partitioner, state, buffer);

		CHECK ( r );

		// After the last partition, current_offset() will be 1 index ahead
		// as before. However, the block is smaller than block_size.

		CHECK ( state.current_offset()    ==
				ind2am(partitioner.legal_range().upper()) );

		CHECK ( state.samples_processed() ==
				ind2am(partitioner.legal_range().upper()) - skipped_front );

		CHECK ( buffer.size()             == 15 );

		CHECK ( buffer[13].get(type::ARCS1) == 0x3EF9CE06u );
		CHECK ( buffer[13].get(type::ARCS2) == 0x8348C62Fu );

		CHECK ( buffer[14].get(type::ARCS1) == 0x9F4BF9D9u );
		CHECK ( buffer[14].get(type::ARCS2) == 0xCE22774Eu );
	}

	// TODO Simulate the same album but with block_size 4096 like ffmpeg does
}

