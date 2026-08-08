#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Functional test for perform_update().
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for perform_update()
#endif

#include <cstdint>                // for int32_t
#include <iterator>               // for begin, cbegin, cend, end
#include <memory>                 // for make_unique, unique_ptr
#include <numeric>                // for iota
#include <vector>                 // for vector

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"         // for AccurateRipV1V2
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // for AudioSize, UNIT
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type
#endif


TEST_CASE ( "perform_update", "[perform_update] [calc]" )
{
	using arcstk::AccurateRip::V1andV2;
	using arcstk::Algorithm;
	using arcstk::AudioSize;
	using arcstk::Checksums;
	using arcstk::ChecksumtypeSet;
	using arcstk::Context;
	using arcstk::Points;
	using arcstk::Settings;
	using arcstk::Updateable;
	using arcstk::UNIT;
	using arcstk::checksum::type;

	using arcstk::details::ind2am;
	using arcstk::details::Interval;
	using arcstk::details::TrackPartitioner;
	using arcstk::details::CalculationState;
	using arcstk::details::update::perform_update;

	using std::cbegin;
	using std::cend;
	using std::begin;
	using std::end;

	// This test simulates the calculation of an album

	// Partitioner

	/* use Bach, Organ Concertos, Simon Preston, DGG */
	const auto partitioner { TrackPartitioner {
		{ /* split points (track offsets) */
			{     33 * 588, UNIT::SAMPLES },
			{   5225 * 588, UNIT::SAMPLES },
			{   7390 * 588, UNIT::SAMPLES },
			{  23380 * 588, UNIT::SAMPLES },
			{  35608 * 588, UNIT::SAMPLES },
			{  49820 * 588, UNIT::SAMPLES },
			{  69508 * 588, UNIT::SAMPLES },
			{  87733 * 588, UNIT::SAMPLES },
			{ 106333 * 588, UNIT::SAMPLES },
			{ 139495 * 588, UNIT::SAMPLES },
			{ 157863 * 588, UNIT::SAMPLES },
			{ 198495 * 588, UNIT::SAMPLES },
			{ 213368 * 588, UNIT::SAMPLES },
			{ 225320 * 588, UNIT::SAMPLES },
			{ 234103 * 588, UNIT::SAMPLES }
		},
		{ 253038 * 588 /* 148786344 */, UNIT::SAMPLES },
		/* legal range w/ skips */ { 33 * 588 + 2939, 253038 * 588 - 2940 },
	}};

	REQUIRE ( partitioner.total_samples().samples() == 148786344 );
	REQUIRE ( partitioner.legal_range().lower()     == 22343 );
	REQUIRE ( partitioner.legal_range().upper()     == 148783404 );
	// TODO Verify split points

	// Algorithm

	auto algorithm { Updateable<V1andV2> { Context::ALBUM } };

	REQUIRE ( algorithm.context() == Context::ALBUM );
	REQUIRE ( algorithm.types()   ==
			ChecksumtypeSet { type::ARCS1, type::ARCS2 } );

	// State

	auto state = CalculationState {};

	// Result buffer

	Checksums buffer {};

	REQUIRE ( buffer.size() == 0 );

	// Input data

	auto dummy_data = std::vector<uint32_t>(148786344);
	std::iota(begin(dummy_data), end(dummy_data), 1);

	// for convenience
	const int32_t skipped_front { 19404 + 2939 }; // equivalent to legal lower


	SECTION ("Updating album w/ block_size 16777216 yields correct checksums")
	{
		// This simulates libarcsdec:readerwav

		const auto block_size = int { 16777216 }; // samples
		auto r = bool { true };

		r = perform_update(	cbegin(dummy_data) + 0 * block_size,
							cbegin(dummy_data) + 1 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == block_size );
		CHECK ( state.samples_processed() == block_size - skipped_front );
		CHECK ( buffer.size()             == 3 );

		CHECK ( buffer[ 0].get(type::ARCS1).first.value() == 0x0AF18BB6u );
		CHECK ( buffer[ 0].get(type::ARCS2).first.value() == 0x8FBB68BAu );

		CHECK ( buffer[ 1].get(type::ARCS1).first.value() == 0x60F64E9Au );
		CHECK ( buffer[ 1].get(type::ARCS2).first.value() == 0x8D040A9Au );

		CHECK ( buffer[ 2].get(type::ARCS1).first.value() == 0xBC5C57ECu );
		CHECK ( buffer[ 2].get(type::ARCS2).first.value() == 0x2A4FD377u );

		r = perform_update( cbegin(dummy_data) + 1 * block_size,
							cbegin(dummy_data) + 2 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 2 * block_size );
		CHECK ( state.samples_processed() == 2 * block_size - skipped_front );
		CHECK ( buffer.size()             == 5 );

		CHECK ( buffer[ 3].get(type::ARCS1).first.value() == 0xD394FC08u );
		CHECK ( buffer[ 3].get(type::ARCS2).first.value() == 0xCE55344Bu );

		CHECK ( buffer[ 4].get(type::ARCS1).first.value() == 0xD52E3008u );
		CHECK ( buffer[ 4].get(type::ARCS2).first.value() == 0x022C486Du );

		r = perform_update( cbegin(dummy_data) + 2 * block_size,
							cbegin(dummy_data) + 3 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 3 * block_size );
		CHECK ( state.samples_processed() == 3 * block_size - skipped_front );
		CHECK ( buffer.size()             == 6 );

		CHECK ( buffer[ 5].get(type::ARCS1).first.value() == 0x528B55D0u );
		CHECK ( buffer[ 5].get(type::ARCS2).first.value() == 0xC4778057u );

		r = perform_update( cbegin(dummy_data) + 3 * block_size,
							cbegin(dummy_data) + 4 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 4 * block_size );
		CHECK ( state.samples_processed() == 4 * block_size - skipped_front );
		CHECK ( buffer.size()             == 8 );

		CHECK ( buffer[ 6].get(type::ARCS1).first.value() == 0xB53625EAu );
		CHECK ( buffer[ 6].get(type::ARCS2).first.value() == 0x29DF16E5u );

		CHECK ( buffer[ 7].get(type::ARCS1).first.value() == 0x55480A90u );
		CHECK ( buffer[ 7].get(type::ARCS2).first.value() == 0x390C2F05u );

		r = perform_update( cbegin(dummy_data) + 4 * block_size,
							cbegin(dummy_data) + 5 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 5 * block_size );
		CHECK ( state.samples_processed() == 5 * block_size - skipped_front );
		CHECK ( buffer.size()             == 9 );

		CHECK ( buffer[ 8].get(type::ARCS1).first.value() == 0x53262404u );
		CHECK ( buffer[ 8].get(type::ARCS2).first.value() == 0xA8B5ADDDu );

		r = perform_update( cbegin(dummy_data) + 5 * block_size,
							cbegin(dummy_data) + 6 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 6 * block_size );
		CHECK ( state.samples_processed() == 6 * block_size - skipped_front );
		CHECK ( buffer.size()             == 10 );

		CHECK ( buffer[ 9].get(type::ARCS1).first.value() == 0x33A23980u );
		CHECK ( buffer[ 9].get(type::ARCS2).first.value() == 0x4D9350B0u );

		r = perform_update( cbegin(dummy_data) + 6 * block_size,
							cbegin(dummy_data) + 7 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 7 * block_size );
		CHECK ( state.samples_processed() == 7 * block_size - skipped_front );
		CHECK ( buffer.size()             == 11 );

		CHECK ( buffer[10].get(type::ARCS1).first.value() == 0xB66906B0u );
		CHECK ( buffer[10].get(type::ARCS2).first.value() == 0x49D26578u );

		r = perform_update( cbegin(dummy_data) + 7 * block_size,
							cbegin(dummy_data) + 8 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 8 * block_size );
		CHECK ( state.samples_processed() == 8 * block_size - skipped_front );
		CHECK ( buffer.size()             == 13 );

		CHECK ( buffer[11].get(type::ARCS1).first.value() == 0x2BE3B232u );
		CHECK ( buffer[11].get(type::ARCS2).first.value() == 0x355C7E28u );

		CHECK ( buffer[12].get(type::ARCS1).first.value() == 0x5D229B60u );
		CHECK ( buffer[12].get(type::ARCS2).first.value() == 0x970C0A35u );

		r = perform_update( cbegin(dummy_data) + 8 * block_size,
							cbegin(dummy_data) + 9 * block_size,
							partitioner, algorithm, state, buffer);

		CHECK ( r );

		// After the last partition, current_offset() will be 1 index ahead
		// as before. However, the block is smaller than block_size.

		CHECK ( state.current_offset()    ==
				ind2am(partitioner.legal_range().upper()) );

		CHECK ( state.samples_processed() ==
				ind2am(partitioner.legal_range().upper()) - skipped_front );

		CHECK ( buffer.size()             == 15 );

		CHECK ( buffer[13].get(type::ARCS1).first.value() == 0x3EF9CE06u );
		CHECK ( buffer[13].get(type::ARCS2).first.value() == 0x8348C62Fu );

		CHECK ( buffer[14].get(type::ARCS1).first.value() == 0x9F4BF9D9u );
		CHECK ( buffer[14].get(type::ARCS2).first.value() == 0xCE22774Eu );
	}

	// TODO Simulate the same album but with block_size 4096 like ffmpeg does
}

