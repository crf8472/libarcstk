#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Functional test for perform_update().
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"           // for perform_update(), Context, Updateable
#endif                             //     TrackPartitioner, CalculationState
                                   //     ind2am

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"          // for AccurateRip::V1andV2, Algorithm
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"            // for AudioSize, UNIT
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"            // for checksum::type, Checksum, Checksums
#endif                             //     ChecksumtypeSet

#ifndef LIBARCSTK_REFDATA_HPP_
#include "ref_data.hpp"            // for standard_data
#endif

#include <array>                   // for array
#include <cstddef>                 // for size_t, ptrdiff_t
#include <cstdint>                 // for int32_t
#include <iterator>                // for begin, cbegin, cend, end
#include <vector>                  // for vector


SCENARIO ( "Checksums of an album", "[perform_update][calc][details]" )
{
	using arcstk::AccurateRip::V1andV2;
	using arcstk::Algorithm;
	using arcstk::AudioSize;
	using arcstk::Checksum;
	using arcstk::Checksums;
	using arcstk::ChecksumtypeSet;
	using arcstk::Context;
	using arcstk::Updateable;
	using arcstk::UNIT;
	using arcstk::checksum::type;

	using arcstk::details::ind2am;
	using arcstk::details::TrackPartitioner;
	using arcstk::details::CalculationState;

	using std::cbegin;
	using std::cend;
	using std::begin;
	using std::end;


	auto algorithm { Updateable<V1andV2> { Context::ALBUM } };

	REQUIRE ( algorithm.context() == Context::ALBUM );
	REQUIRE ( algorithm.types()   ==
				ChecksumtypeSet { type::ARCS1, type::ARCS2 } );


	auto buffer = Checksums {}; // result buffer

	REQUIRE ( buffer.size() == 0 );


	auto state = CalculationState {};

	REQUIRE ( state.current_offset()    == 0 );
	REQUIRE ( state.samples_processed() == 0 );
	REQUIRE ( state.tracks_processed()  == 0 );

	GIVEN ( "album with 15 tracks" ) // simulates the calculation of an album
	{
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
			{ 253038 * 588, UNIT::SAMPLES },
			/* legal range w/ skips */ { 33 * 588 + 2939, 253038 * 588 - 2940 },
		}};

		constexpr std::int32_t TOTAL_SAMPLES { 253038 * 588 };

		constexpr std::int32_t SKIPPED_FRONT {     33 * 588 + 2939 };

		REQUIRE ( partitioner.total_samples().samples() == TOTAL_SAMPLES );
		REQUIRE ( partitioner.legal_range().lower()     == SKIPPED_FRONT );
		REQUIRE ( partitioner.legal_range().upper()     == 148783404 );
		// TODO Verify split points


		// reading:

		constexpr std::size_t BLOCK_SIZE { 16777216 }; // samples per block

		constexpr std::size_t BLOCKS { 9 }; // total blocks in block_size


		// accessors:

		// get length from one FRAME to another in SAMPLES
		const auto samples = [](const int from, const int to) -> std::int32_t
		{
			return AudioSize { to - from, UNIT::FRAMES }.samples();
		};

		// get ARCSv1 from Checksums
		const auto arcs1 = [](const Checksums& c, const std::size_t i)
			-> Checksum::value_type
		{
			return c[i].get(type::ARCS1).first.value();
		};

		// get ARCSv2 from Checksums
		const auto arcs2 = [](const Checksums& c, const std::size_t i)
			-> Checksum::value_type
		{
			return c[i].get(type::ARCS2).first.value();
		};

		// track length
		const auto length = [](const Checksums& c, const std::size_t i)
			-> std::int32_t
		{
			return c[i].length().samples();
		};


		// Input data

		const auto dummy_data =
			arcstk::testing::data::standard_data(TOTAL_SAMPLES);


		WHEN ( "Input is read completely" )
		{
			auto flags = std::array<bool,         BLOCKS> {}; // result flags
			auto pos   = std::array<std::int32_t, BLOCKS> {}; // offset position
			auto total = std::array<std::int32_t, BLOCKS> {}; // sample count

			auto start_offs = std::ptrdiff_t { 0 }; // block start offset
			auto stop_offs  = std::ptrdiff_t { 0 }; // block end offset

			// Read entire data in 9 blocks of specified size
			for (std::size_t i = 0; i < BLOCKS; ++i)
			{
				start_offs = static_cast<std::ptrdiff_t>( i      * BLOCK_SIZE);
				stop_offs  = static_cast<std::ptrdiff_t>((i + 1) * BLOCK_SIZE);

				/*
				NOLINTBEGIN (cppcoreguidelines-pro-bounds-constant-array-index)
				*/

				flags[i] = arcstk::details::update::perform_update(
						cbegin(dummy_data) + start_offs,
						cbegin(dummy_data) + stop_offs,
						partitioner, algorithm, state, buffer);

				pos[i]   = state.current_offset();
				total[i] = state.samples_processed();

				// NOLINTEND
			}


			THEN ( "checksums, lengths and state attributes are correct" )
			{
				// verify results

				// ARCSv1 values are correct
				CHECK ( arcs1(buffer,  0) == 0x0AF18BB6u );
				CHECK ( arcs1(buffer,  1) == 0x60F64E9Au );
				CHECK ( arcs1(buffer,  2) == 0xBC5C57ECu );
				CHECK ( arcs1(buffer,  3) == 0xD394FC08u );
				CHECK ( arcs1(buffer,  4) == 0xD52E3008u );
				CHECK ( arcs1(buffer,  5) == 0x528B55D0u );
				CHECK ( arcs1(buffer,  6) == 0xB53625EAu );
				CHECK ( arcs1(buffer,  7) == 0x55480A90u );
				CHECK ( arcs1(buffer,  8) == 0x53262404u );
				CHECK ( arcs1(buffer,  9) == 0x33A23980u );
				CHECK ( arcs1(buffer, 10) == 0xB66906B0u );
				CHECK ( arcs1(buffer, 11) == 0x2BE3B232u );
				CHECK ( arcs1(buffer, 12) == 0x5D229B60u );
				CHECK ( arcs1(buffer, 13) == 0x3EF9CE06u );
				CHECK ( arcs1(buffer, 14) == 0x9F4BF9D9u );

				// ARCSv2 values are correct
				CHECK ( arcs2(buffer,  0) == 0x8FBB68BAu );
				CHECK ( arcs2(buffer,  1) == 0x8D040A9Au );
				CHECK ( arcs2(buffer,  2) == 0x2A4FD377u );
				CHECK ( arcs2(buffer,  3) == 0xCE55344Bu );
				CHECK ( arcs2(buffer,  4) == 0x022C486Du );
				CHECK ( arcs2(buffer,  5) == 0xC4778057u );
				CHECK ( arcs2(buffer,  6) == 0x29DF16E5u );
				CHECK ( arcs2(buffer,  7) == 0x390C2F05u );
				CHECK ( arcs2(buffer,  8) == 0xA8B5ADDDu );
				CHECK ( arcs2(buffer,  9) == 0x4D9350B0u );
				CHECK ( arcs2(buffer, 10) == 0x49D26578u );
				CHECK ( arcs2(buffer, 11) == 0x355C7E28u );
				CHECK ( arcs2(buffer, 12) == 0x970C0A35u );
				CHECK ( arcs2(buffer, 13) == 0x8348C62Fu );
				CHECK ( arcs2(buffer, 14) == 0xCE22774Eu );

				// track lengths
				CHECK ( length(buffer,  0) == samples(    33,   5225) );
				CHECK ( length(buffer,  1) == samples(  5225,   7390) );
				CHECK ( length(buffer,  2) == samples(  7390,  23380) );
				CHECK ( length(buffer,  3) == samples( 23380,  35608) );
				CHECK ( length(buffer,  4) == samples( 35608,  49820) );
				CHECK ( length(buffer,  5) == samples( 49820,  69508) );
				CHECK ( length(buffer,  6) == samples( 69508,  87733) );
				CHECK ( length(buffer,  7) == samples( 87733, 106333) );
				CHECK ( length(buffer,  8) == samples(106333, 139495) );
				CHECK ( length(buffer,  9) == samples(139495, 157863) );
				CHECK ( length(buffer, 10) == samples(157863, 198495) );
				CHECK ( length(buffer, 11) == samples(198495, 213368) );
				CHECK ( length(buffer, 12) == samples(213368, 225320) );
				CHECK ( length(buffer, 13) == samples(225320, 234103) );
				CHECK ( length(buffer, 14) == samples(234103, 253038 - 5) + 1 );

				// total tracks
				CHECK ( state.tracks_processed() == 15 );
				CHECK ( buffer.size()            == 15 );

				// verify state attributes

				// state: result flags
				CHECK_FALSE ( flags[0] );
				CHECK_FALSE ( flags[1] );
				CHECK_FALSE ( flags[2] );
				CHECK_FALSE ( flags[3] );
				CHECK_FALSE ( flags[4] );
				CHECK_FALSE ( flags[5] );
				CHECK_FALSE ( flags[6] );
				CHECK_FALSE ( flags[7] );
				CHECK       ( flags[8] );

				// state: input offsets after blocks
				CHECK ( pos[0] == 1 * BLOCK_SIZE );
				CHECK ( pos[1] == 2 * BLOCK_SIZE );
				CHECK ( pos[2] == 3 * BLOCK_SIZE );
				CHECK ( pos[3] == 4 * BLOCK_SIZE );
				CHECK ( pos[4] == 5 * BLOCK_SIZE );
				CHECK ( pos[5] == 6 * BLOCK_SIZE );
				CHECK ( pos[6] == 7 * BLOCK_SIZE );
				CHECK ( pos[7] == 8 * BLOCK_SIZE );
				CHECK ( pos[8] == 1 + partitioner.legal_range().upper() );

				// state: input samples after blocks
				CHECK ( total[0] == 1 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[1] == 2 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[2] == 3 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[3] == 4 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[4] == 5 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[5] == 6 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[6] == 7 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[7] == 8 * BLOCK_SIZE - SKIPPED_FRONT );
				CHECK ( total[8] ==
						1 + partitioner.legal_range().upper() - SKIPPED_FRONT );
			}
		} // WHEN
	} // GIVEN

	// TODO Simulate the same album but with block_size 4096 like ffmpeg does
}

// vim:fdm=syntax

