#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Update tests for accuraterip.hpp.
 */

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#define LIBARCSTK_ALGORITHMS_HPP_ // allow accuraterip.hpp
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"        // TO BE TESTED
#endif

#include <fstream>                // for ifstream
#include <unordered_set>          // for unordered_set
#include <vector>                 // for vector

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type
#endif
#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"         // for make_arid
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // for AudioSize
#endif
#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"            // for csample_t
#endif

#include "ref_data.hpp"           // for standard_data


TEST_CASE ( "Updating ARCS v1+v2", "[arcsalgorithm] [calc]" )
{
	using arcstk::AudioSize;
	using arcstk::checksum::type;
	using arcstk::csample_t;
	using arcstk::Updateable;

	// fits calculation-test-01.bin
	//auto audiosize = AudioSize { 196608, UNIT::SAMPLES };

	SECTION ( "Updating ARCS 1 singletrack & aligned blocks is correct" )
	{
		auto algo = Updateable<arcstk::accuraterip::algorithm::Version1>{};

		REQUIRE ( algo.types() == std::unordered_set<type>{ type::ARCS1 } );

		// Initialize Buffer

		std::vector<csample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("data/calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				algo.update(buffer.begin(), buffer.end(), 80000);
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		algo.update(buffer.cbegin(), buffer.cend(), 36608);
		algo.finalize_track(1, AudioSize{ 36608, arcstk::UNIT::SAMPLES });

		auto checksums { algo.track(1) };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 1 /* types */ );
		CHECK ( (checksums.get(type::ARCS1).first.value()) == 0x8FE8D29B );
	}


	SECTION ( "Updating ARCS 2 singletrack & aligned blocks is correct" )
	{
		auto state = arcstk::accuraterip::details::UpdateableSubtotals<
			type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{ type::ARCS2 } );

		// Initialize Buffer

		std::vector<csample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("data/calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end(), 80000);
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		state.update(buffer.begin(), buffer.end(), 36608);

		// Only track with correct ARCSs

		CHECK ( state.value<type::ARCS2>() == 0xD15BB487 );
	}


	SECTION ( "Updating ARCS v1+2 singletrack & aligned blocks is correct" )
	{
		auto state = arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1,
			 type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{
				type::ARCS1, type::ARCS2 } );

		// Initialize Buffer

		std::vector<csample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("data/calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end(), 80000);
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		state.update(buffer.begin(), buffer.end(), 36608);

		// Only track with correct ARCSs

		CHECK ( state.value<type::ARCS1>() == 0x8FE8D29B );
		CHECK ( state.value<type::ARCS2>() == 0xD15BB487 );
	}


	SECTION ( "Updating ARCS v1+2 singletrack & non-aligned blocks is correct" )
	{
		auto state =
			arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1,
			type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{
				type::ARCS1, type::ARCS2 } );

		// Initialize Buffer

		std::vector<csample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("data/calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end(), 80000);
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		state.update(buffer.begin(), buffer.end(), 36608);

		// Only track with correct ARCSs

		CHECK ( state.value<type::ARCS1>() == 0x8FE8D29B );
		CHECK ( state.value<type::ARCS2>() == 0xD15BB487 );
	}
}


TEST_CASE ( "first<>() is correct", "[arcsalgorithm] [calc]" )
{
	using arcstk::accuraterip::details::UpdateableSubtotals;
	using arcstk::accuraterip::details::first;
	using arcstk::checksum::type;

	using arcstk::testing::data::standard_data;

	using std::cbegin;
	using std::cend;

	const auto sdata = standard_data(3052896);

	auto st12 = UpdateableSubtotals<type::ARCS1,type::ARCS2> {};
	st12.cache(cbegin(sdata), cbegin(sdata) + 2940);
	st12.finalize();

	const auto st = st12.subtotals();

	SECTION ( "for 0" )
	{
		CHECK ( first<type::ARCS1>(0, st) == 0 );
	}

	SECTION ( "from 1 to 2940" )
	{
		CHECK ( first<type::ARCS1>(1, st) == 1 * 1 );
		CHECK ( first<type::ARCS1>(2, st) == 1 * 1 + 2 * 2 );
		CHECK ( first<type::ARCS1>(3, st) == 1 * 1 + 2 * 2 + 3 * 3 );
		// ...
		CHECK ( first<type::ARCS1>(2940, st) == 4180082994 );
	}

	SECTION ( "for illegal values (> 2940)" )
	{
		CHECK ( first<type::ARCS1>(2941, st) == 0 );
		CHECK ( first<type::ARCS1>(2942, st) == 0 );
		CHECK ( first<type::ARCS1>(2943, st) == 0 );
		//CHECK ( first<type::ARCS1>(6000, st) == 0 );
	}
}


TEST_CASE ( "last<>() is correct", "[arcsalgorithm] [calc]" )
{
	using arcstk::accuraterip::details::UpdateableSubtotals;
	using arcstk::accuraterip::details::last;
	using arcstk::checksum::type;

	using arcstk::testing::data::standard_data;

	using std::cbegin;
	using std::cend;

	const auto sdata = standard_data(3052896);

	auto st12 = UpdateableSubtotals<type::ARCS1,type::ARCS2> {};
	st12.cache(cbegin(sdata), cbegin(sdata) + 2940);
	//st12.cache(cbegin(sdata) + (3052896 - 2940), cend(sdata));
	st12.finalize();

	const auto st = st12.subtotals();

	SECTION ( "for 0" )
	{
		CHECK ( last<type::ARCS1>(0, st) == 0 );
	}

	// SECTION ( "from 1 to 2940" )
	// {
	// 	CHECK ( st.subtotals_v1[5879] == (3052896 * 3052896 & 0xFFFFFFFF) );
	// 	CHECK ( last<type::ARCS1>(1, st) == (3052896 * 3052896 & 0xFFFFFFFF) );
	// 	CHECK ( last<type::ARCS1>(2, st) == 1 * 1 + 2 * 2 );
	// 	CHECK ( last<type::ARCS1>(3, st) == 1 * 1 + 2 * 2 + 3 * 3 );
	// 	// ...
	// 	CHECK ( last<type::ARCS1>(2940, st) == 4180082994 );
	// }

	// SECTION ( "for illegal values (> 2940)" )
	// {
	// 	CHECK ( last<type::ARCS1>(2941, st) == 0 );
	// 	CHECK ( last<type::ARCS1>(2942, st) == 0 );
	// 	CHECK ( last<type::ARCS1>(2943, st) == 0 );
	// 	CHECK ( last<type::ARCS1>(6000, st) == 0 );
	// }
}


// TEST_CASE ( "cs_sum() is correct", "[arcsalgorithm] [calc]" )
// {
// 	using arcstk::accuraterip::details::UpdateableSubtotals;
// 	using arcstk::accuraterip::details::cs_sum;
// 	using arcstk::checksum::type;
//
// 	using arcstk::testing::data::standard_data;
//
// 	using std::cbegin;
// 	using std::cend;
//
// 	const auto sdata = standard_data(3052896);
//
// 	auto st12 = UpdateableSubtotals<type::ARCS1,type::ARCS2> {};
// 	st12.cache(cbegin(sdata), cbegin(sdata) + 2940);
// 	st12.finalize();
//
// 	const auto st = st12.subtotals();
//
// 	SECTION ( "for 0" )
// 	{
// 		CHECK ( cs_sum(0, st) == 0 );
// 	}
//
// 	SECTION ( "from 1 to 2940" )
// 	{
// 		CHECK ( cs_sum(1, st) == 1  );
// 		CHECK ( cs_sum(2, st) == 1 + 2 );
// 		CHECK ( cs_sum(3, st) == 1 + 2 + 3 );
// 		// ...
// 		//CHECK ( cs_sum(2940, st) == 4323270 );
// 	}
//
// 	SECTION ( "for illegal values (> 5880)" )
// 	{
// 		CHECK ( cs_sum(2941, st) == 0 );
// 		CHECK ( cs_sum(2942, st) == 0 );
// 		CHECK ( cs_sum(2943, st) == 0 );
// 		CHECK ( cs_sum(6000, st) == 0 );
// 	}
// }


TEST_CASE ( "Updating ARCS v1+v2 with drive offset", "[arcsalgorithm] [calc]" )
{
	using arcstk::AudioSize;
	using arcstk::checksum::type;
	using arcstk::csample_t;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Updateable;

	using arcstk::testing::data::standard_data;

	using std::cbegin;
	using std::cend;

	const auto sdata = standard_data(3052896);
	// length of Bach, Organ Concertos, Track 1

	REQUIRE ( sdata[      0] ==       1 );
	REQUIRE ( sdata[3052895] == 3052896 );

	REQUIRE ( *cbegin(sdata) == 1 );
	REQUIRE ( *cbegin(sdata) + 2939 - 1 == 2939 );

	// 3049957
	REQUIRE ( *(cbegin(sdata) + (3052896 - 2940)) == 3052896 - 2940 + 1 );
	REQUIRE ( *(cend(sdata) - 1) == 3052896 );

	// for convenience: extract ARCSv1 from ChecksumSet
	const auto checksum_value = [](const ChecksumSet& s)
		-> Checksum::value_type
	{
		return s.get(type::ARCS1).first.value();
	};

	SECTION ("Trying to shift by k < -2939 throws")
	{
		auto v1 = Updateable<arcstk::accuraterip::algorithm::Version1>{};
		v1.update(cbegin(sdata), cend(sdata), 3052896);
		v1.finalize_track(1, AudioSize { 3052896, arcstk::UNIT::SAMPLES });

		CHECK_THROWS ( checksum_value(v1.track(1, -2940)) );
	}

	SECTION ("Trying to shift by k > 2940 throws")
	{
		auto v1 = Updateable<arcstk::accuraterip::algorithm::Version1>{};
		v1.update(cbegin(sdata), cend(sdata), 3052896);
		v1.finalize_track(1, AudioSize { 3052896, arcstk::UNIT::SAMPLES });

		CHECK_THROWS ( checksum_value(v1.track(1, 2941)) );
	}

	SECTION ("Shift inner track by k > 0 with ARCSv1")
	{
		const auto track2 = standard_data(3000, 3052897);

		REQUIRE ( track2[0] == 3052897 );
		REQUIRE ( track2[1] == 3052898 );
		REQUIRE ( track2[2] == 3052899 );
		REQUIRE ( track2.size() == 3000 );

		auto v1 = Updateable<arcstk::accuraterip::algorithm::Version1>{};

		// v1.start_track()
		v1.update(cbegin(sdata), cend(sdata), 3052896);
		v1.finalize_track(1, AudioSize { 3052896, arcstk::UNIT::SAMPLES });

		// v1.start_track()
		v1.update(cbegin(track2), cend(track2), 3000);
		v1.finalize_track(2, AudioSize { 3000, arcstk::UNIT::SAMPLES });

		REQUIRE ( checksum_value(v1.track(1)) == 0x2434B590 );

		// Reproduce checksum manually:

		auto cs = uint32_t { 1 };
		auto arcs_v1_track = uint32_t { 0 };
		for (uint64_t i = 1; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_track += (i * cs & 0xFFFFFFFF);
		}

		REQUIRE ( arcs_v1_track == 0x2434B590 ); /* Checksum is correct */


		/* Reproduce shifted checksum arithmetically */

		// first, calculate simple sum of remaining part (4 to 3052896)
		auto sum = uint32_t { 0 };
		for (uint32_t i = 4; i <= 3052896; ++i)
		{
			sum += i & 0xFFFFFFFF;
		}

		REQUIRE ( sum == 49003690 );

		const auto shifted_checksum = (0x2434B590
			- (1 * 1) - (2 * 2) - (3 * 3)) - 3 * sum
			+ (3052894 * 3052897u & 0xFFFFFFFF)
			+ (3052895 * 3052898u & 0xFFFFFFFF)
			+ (3052896 * 3052899u & 0xFFFFFFFF);

		REQUIRE ( shifted_checksum == 0x2CF7EBA0 );


		/* Reproduce shifted checksum manually */

		auto arcs_v1_3_track = uint32_t { 0 };
		cs = 4;
		for (uint64_t i = 1; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_3_track += (i * cs & 0xFFFFFFFF);
		}

		REQUIRE ( arcs_v1_3_track == 0x2CF7EBA0 );


		/* Verify that both shifting methods lead to equivalent results */

		REQUIRE ( arcs_v1_3_track == shifted_checksum ); // == 754445216


		auto arcs_v1_2940_track = uint32_t { 0 };
		cs = 2941;
		for (uint64_t i = 1; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_2940_track += (i * cs & 0xFFFFFFFF);
		}

		REQUIRE ( arcs_v1_2940_track == 0xAF7FAAD0 );

		// More k > 0 ...
		CHECK ( checksum_value(v1.track(1,    1)) == 0x27207240 );
		CHECK ( checksum_value(v1.track(1,    2)) == 0x2A0C2EF0 );
		CHECK ( checksum_value(v1.track(1,    3)) == 0x2CF7EBA0 ); //
		CHECK ( checksum_value(v1.track(1,    4)) == 0x2FE3A850 );
		CHECK ( checksum_value(v1.track(1,    5)) == 0x32CF6500 );
		// ...
		CHECK ( checksum_value(v1.track(1, 2936)) == 0xA3D0B810 );
		CHECK ( checksum_value(v1.track(1, 2937)) == 0xA6BC74C0 );
		CHECK ( checksum_value(v1.track(1, 2938)) == 0xA9A83170 );
		CHECK ( checksum_value(v1.track(1, 2939)) == 0xAC93EE20 );
		CHECK ( checksum_value(v1.track(1, 2940)) == 0xAF7FAAD0 ); //
	}

	SECTION ("Shift last track by k > 0 with ARCSv1")
	{
		auto v1 = Updateable<arcstk::accuraterip::algorithm::Version1>{};

		REQUIRE ( v1.total_tracks() == 0 );

		// Consider sdata as a last track without the trailing 2940 samples
		v1.start_track(1, AudioSize { 3052896 - 2940, arcstk::UNIT::SAMPLES });
		v1.update(cbegin(sdata), cbegin(sdata) + (3052896 - 2940),
				3052896 - 2940);
		v1.post_range(cbegin(sdata) + (3052896 - 2940), cend(sdata));
		v1.finalize_track(1,
				AudioSize { 3052896 - 2940, arcstk::UNIT::SAMPLES });

		REQUIRE ( v1.total_tracks() == 1 );
		REQUIRE ( checksum_value(v1.track(1)) == 0x050E83EE );

		/* Reproduce checksum manually */

		auto arcs_v1_track = uint32_t { 0 };
		auto cs = uint32_t { 1 };
		for (uint64_t i = 1; i <= 3052896 - 2940; ++i, ++cs)
		{
			arcs_v1_track += (i * cs & 0xFFFFFFFF);
		}

		REQUIRE ( arcs_v1_track == 0x050E83EE ); /* Checksum is correct */


		/* Reproduce shifted checksum arithmetically */

		// shift with k = 3 ("forward by 3"):

		auto shifted_checksum = arcs_v1_track - (1 * 1) - (2 * 2) - (3 * 3);
		// shift the remaining part

		// first, calculate simple sum of remaining part (4 - 3052896)
		auto sum = uint32_t { 0 };
		for (uint32_t i = 4; i <= 3052896 - 2940; ++i)
		{
			sum += i & 0xFFFFFFFF;
		}

		REQUIRE ( sum == 3962711668 );

		shifted_checksum = shifted_checksum - 3 * sum;
		// Now, 1*4 is the start while 3052893 * 3052896 is the end

		// add next 3 samples
		// those are the 2940-last, 2941-last and 2942-last samples
		shifted_checksum += (3049954 * 3049957u & 0xFFFFFFFF)
			+ (3049955 * 3049958u & 0xFFFFFFFF)
			+ (3049956 * 3049959u & 0xFFFFFFFF);

		REQUIRE ( shifted_checksum == 0xC9A50F5C );


		/* Reproduce shifted checksum manually */

		auto arcs_v1_3_track = uint32_t { 0 };
		cs = 4;
		for (uint64_t i = 1; i <= 3052896 - 2940; ++i, ++cs)
		{
			arcs_v1_3_track += (i * cs & 0xFFFFFFFF);
		}

		REQUIRE ( arcs_v1_3_track == 0xC9A50F5C );

		using arcstk::testing::data::arcs1_over_standard_data;
		REQUIRE ( arcs1_over_standard_data(4, 3052896 - 2940) == 0xC9A50F5C );


		auto arcs_v1_2940_track = uint32_t { 0 };
		cs = 2941;
		for (uint64_t i = 1; i <= 3052896 - 2940; ++i, ++cs)
		{
			arcs_v1_2940_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_2940_track == 0x955C4506 ); // 2940
		REQUIRE ( arcs1_over_standard_data(2941, 3052896 - 2940) == 0x955C4506 );


		//CHECK ( checksum_value(v1.track(1, 3)) == 0xC9A50F5C );

		// More k > 0 ...
		CHECK ( checksum_value(v1.track(1,    1)) == 0xF140B268 );
		CHECK ( checksum_value(v1.track(1,    2)) == 0xDD72E0E2 );
		CHECK ( checksum_value(v1.track(1,    3)) == 0xC9A50F5C ); //
		CHECK ( checksum_value(v1.track(1,    4)) == 0xB5D73DD6 );
		CHECK ( checksum_value(v1.track(1,    5)) == 0xA2096C50 );
		// ...
		CHECK ( checksum_value(v1.track(1, 2936)) == 0xE4938B1E );
		CHECK ( checksum_value(v1.track(1, 2937)) == 0xD0C5B998 );
		CHECK ( checksum_value(v1.track(1, 2938)) == 0xBCF7E812 );
		CHECK ( checksum_value(v1.track(1, 2939)) == 0xA92A168C );
		CHECK ( checksum_value(v1.track(1, 2940)) == 0x955C4506 ); //
	}

	SECTION ("Shift inner track by k < 0 with ARCSv1")
	{
		using arcstk::testing::data::arcs1_over_standard_data;

		auto v1 = Updateable<arcstk::accuraterip::algorithm::Version1>{};

		REQUIRE ( v1.total_tracks() == 0 );

		// Consider sdata as a last track without the trailing 2940 samples
		v1.start_track(1, AudioSize { 3052896, arcstk::UNIT::SAMPLES });
		v1.update(cbegin(sdata), cend(sdata), 3052896);
		v1.finalize_track(1, AudioSize { 3052896, arcstk::UNIT::SAMPLES });

		v1.start_track(2, AudioSize { 3052896, arcstk::UNIT::SAMPLES });
		v1.update(cbegin(sdata), cend(sdata), 3052896);
		v1.finalize_track(2, AudioSize { 3052896, arcstk::UNIT::SAMPLES });

		REQUIRE ( v1.total_tracks() == 2 );
		REQUIRE ( checksum_value(v1.track(2)) == 0x2434B590 );

		/* Reproduce shifted checksum arithmetically */

		auto sum = uint32_t { 0 };
		for (uint64_t i = 1; i <= (3052896 - 3); ++i)
		{
			sum += i & 0xFFFFFFFF;
		}

		const auto arcs_v1_shifted = ((0x2434B590
			- (3052894 * 3052894u & 0xFFFFFFFF)
			- (3052895 * 3052895u & 0xFFFFFFFF)
			- (3052896 * 3052896u & 0xFFFFFFFF)) + 3 * sum)
			+ (1 * 3052894 & 0xFFFFFFFF)
			+ (2 * 3052895 & 0xFFFFFFFF)
			+ (3 * 3052896 & 0xFFFFFFFF);

		REQUIRE ( arcs_v1_shifted == 0x1C88FFC0 );

		/* Reproduce shifted checksum manually */

		auto arcs_v1_3_track = uint32_t { 0 };

		auto cs = uint32_t { 1 };
		for (uint64_t i = (1 + 3); i <= 3052896; ++i, ++cs)
		{
			arcs_v1_3_track += (i * cs & 0xFFFFFFFF);
		}

		REQUIRE ( arcs_v1_3_track == 0x1B717F84 );

		arcs_v1_3_track += (1 * 3052894 & 0xFFFFFFFF);
		arcs_v1_3_track += (2 * 3052895 & 0xFFFFFFFF);
		arcs_v1_3_track += (3 * 3052896 & 0xFFFFFFFF);

		REQUIRE ( arcs_v1_3_track == 0x1C88FFC0 );

		//

		// More k < 0 ...
		CHECK ( checksum_value(v1.track(1,    -1)) == 0x21ccd174 );
		CHECK ( checksum_value(v1.track(1,    -2)) == 0x1f64ed58 );
		CHECK ( checksum_value(v1.track(1,    -3)) == 0x1cfd093c );

		CHECK ( checksum_value(v1.track(2,    -1)) == 0x21778E40 );
		CHECK ( checksum_value(v1.track(2,    -2)) == 0x1EE8FC50 );
		CHECK ( checksum_value(v1.track(2,    -3)) == 0x1C88FFC0 ); //
		CHECK ( checksum_value(v1.track(2,    -4)) == 0x1A579890 );
		CHECK ( checksum_value(v1.track(2,    -5)) == 0x1854C6C0 );
		// ...
		CHECK ( checksum_value(v1.track(2, -2935)) == 0x39F40940 );
		CHECK ( checksum_value(v1.track(2, -2936)) == 0x4D497190 );
		CHECK ( checksum_value(v1.track(2, -2937)) == 0x60CD6F40 );
		CHECK ( checksum_value(v1.track(2, -2938)) == 0x74800250 );
		CHECK ( checksum_value(v1.track(2, -2939)) == 0x88612AC0 );
	}

	SECTION ("Shift first track by k < 0 with ARCSv1")
	{
		using arcstk::testing::data::arcs1_over_standard_data;

		auto v1 = Updateable<arcstk::accuraterip::algorithm::Version1>{};

		REQUIRE ( v1.total_tracks() == 0 );

		// Consider sdata as a last track without the trailing 2940 samples
		v1.start_track(1, AudioSize { 3052896, arcstk::UNIT::SAMPLES });
		v1.pre_range(cbegin(sdata), cbegin(sdata) + 2939);
		v1.update(cbegin(sdata) + 2939, cend(sdata), 3052896 - 2939);
		v1.finalize_track(1, AudioSize { 3052896, arcstk::UNIT::SAMPLES });

		REQUIRE ( v1.total_tracks() == 1 );
		REQUIRE ( checksum_value(v1.track(1)) == 0x2B91986E );

		/* Reproduce checksum manually */

		auto arcs_v1_track = uint32_t { 0 };
		auto cs = uint32_t { 2940 };
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_track == 0x2B91986E );

		/* Reproduce shifted checksums manually */

		auto arcs_v1_1_track = uint32_t { 0 };
		cs = uint32_t { 2939 }; // skip first 2938
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_1_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_1_track == 0x28E7C808 ); // -1

		auto arcs_v1_2_track = uint32_t { 0 };
		cs = uint32_t { 2938 };
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_2_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_2_track == 0x263DF7A2 ); // -2

		auto arcs_v1_3_track = uint32_t { 0 };
		cs = uint32_t { 2937 };
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_3_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_3_track == 0x2394273C ); // -3

		auto arcs_v1_4_track = uint32_t { 0 };
		cs = uint32_t { 2936 };
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_4_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_4_track == 0x20EA56D6 ); // -4

		auto arcs_v1_5_track = uint32_t { 0 };
		cs = uint32_t { 2935 };
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_5_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_5_track == 0x1E408670 ); // -5

		// More k < 0 ...
		CHECK ( checksum_value(v1.track(1,    -1)) == 0x28E7C808 ); //
		CHECK ( checksum_value(v1.track(1,    -2)) == 0x263DF7A2 ); //
		CHECK ( checksum_value(v1.track(1,    -3)) == 0x2394273C ); //
		CHECK ( checksum_value(v1.track(1,    -4)) == 0x20EA56D6 ); //
		CHECK ( checksum_value(v1.track(1,    -5)) == 0x1E408670 ); //
		// ...
		CHECK ( checksum_value(v1.track(1, -2935)) == 0xA2AD5704 );
		CHECK ( checksum_value(v1.track(1, -2936)) == 0xA003869E );
		CHECK ( checksum_value(v1.track(1, -2937)) == 0x9D59B638 );
		CHECK ( checksum_value(v1.track(1, -2938)) == 0x9AAFE5D2 ); //
		CHECK ( checksum_value(v1.track(1, -2939)) == 0x9806156C ); //

		auto arcs_v1_2938_track = uint32_t { 0 };
		cs = uint32_t { 2 };
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_2938_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_2938_track == 0x9AAFE5D2 ); // -2938

		auto arcs_v1_2939_track = uint32_t { 0 };
		cs = uint32_t { 1 };
		for (uint64_t i = 2940; i <= 3052896; ++i, ++cs)
		{
			arcs_v1_2939_track += (i * cs & 0xFFFFFFFF);
		}
		REQUIRE ( arcs_v1_2939_track == 0x9806156C ); // -2939
	}

	// SECTION ("Use ARCSv2 shifted by k == 3 on inner track")
	// {
	// 	auto algo_v2 = Updateable<arcstk::accuraterip::algorithm::Version2>{};
	// 	// TODO
	// }

	// SECTION ("Use ARCSv2 shifted by k == 3 on last track")
	// {
	// 	auto algo_v2 = Updateable<arcstk::accuraterip::algorithm::Version2>{};
	// 	// TODO
	// }

	// SECTION ("Use ARCSv2 shifted by k == -3 on inner track")
	// {
	// 	auto algo_v2 = Updateable<arcstk::accuraterip::algorithm::Version2>{};
	// 	// TODO
	// }

	// SECTION ("Use ARCSv2 shifted by k == -3 on first track")
	// {
	// 	auto algo_v2 = Updateable<arcstk::accuraterip::algorithm::Version2>{};
	// 	// TODO
	// }
}


TEST_CASE ( "Current AccurateRip Request URL", "[make_empty_arid] [id]" )
{
	using arcstk::ACCURATERIP;

	REQUIRE ( ACCURATERIP::default_request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

	REQUIRE ( ACCURATERIP::request_url_prefix() ==
				ACCURATERIP::default_request_url_prefix() );

	// Bent: "Programmed to Love"

	const auto id4 = arcstk::make_arid(
			arcstk::ToC{ arcstk::toc::construct(
				// leadout
				332075,
				// offsets
				{
					0, 29042, 53880, 58227, 84420, 94192, 119165, 123030,
					147500, 148267, 174602, 208125, 212705, 239890, 268705,
					272055, 291720, 319992
				}
			)}
	);


	SECTION ( "request_url_prefix() returns updated URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://foobar/test/success");

		CHECK ( ACCURATERIP::request_url_prefix() ==
				"https://foobar/test/success" );
	}

	SECTION ( "reset_request_url_prefix() resets request URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://some/test/success/");

		CHECK ( ACCURATERIP::request_url_prefix() ==
				"https://some/test/success/" );

		ACCURATERIP::reset_request_url_prefix();

		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );
	}

	SECTION ( "ARId::url() returns updated URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		CHECK ( id4.url().substr(0, 39) ==
				"http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://some/test/at/least/");

		CHECK ( id4.url().substr(0, 27) ==
				"https://some/test/at/least/" );
	}

	SECTION ( "ARId::prefix() returns updated URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		CHECK ( id4.prefix() == "http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://some/test/at/least/");

		CHECK ( id4.prefix() == "https://some/test/at/least/" );
	}

	ACCURATERIP::reset_request_url_prefix();
}


TEST_CASE ( "disc_id_1, disc_id_2, cddb_id", "[id]" )
{
	const auto offsets1 = std::vector<int32_t>
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 };
	const auto leadout1 = int32_t { 253038 };

	const auto offsets2 = std::vector<int32_t>
		{ 32, 96985, 166422 };
	const auto leadout2 = int32_t { 264957 };

	const auto offsets3 = std::vector<int32_t>
		{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 };
	const auto leadout3 = int32_t { 210143 };

	const auto offsets4 = std::vector<int32_t>
		{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500,
			148267, 174602, 208125, 212705, 239890, 268705, 272055, 291720,
			319992 };
	const auto leadout4 = int32_t { 332075 };

	const auto offsets5 = std::vector<int32_t> { 33 };
	const auto leadout5 = int32_t { 233484 };


	SECTION ( "disc_id_1() works" )
	{
		using arcstk::accuraterip::id::disc_id_1;

		CHECK ( 0x001B9178 == disc_id_1(offsets1, leadout1) );
		CHECK ( 0x0008100C == disc_id_1(offsets2, leadout2) );
		CHECK ( 0x001008A6 == disc_id_1(offsets3, leadout3) );
		CHECK ( 0x00307C78 == disc_id_1(offsets4, leadout4) );
		CHECK ( 0x0003902D == disc_id_1(offsets5, leadout5) );
	}

	SECTION ( "disc_id_1() is 0 for empty or zero input" )
	{
		using arcstk::accuraterip::id::disc_id_1;

		CHECK ( 0 == disc_id_1({ /*empty*/  },  0) );
		CHECK ( 0 == disc_id_1({ 0, 0, 0, 0 },  0) );
	}

	SECTION ( "disc_id_2() works" )
	{
		using arcstk::accuraterip::id::disc_id_2;

		CHECK ( 0x014BE24E == disc_id_2(offsets1, leadout1) );
		CHECK ( 0x001AC008 == disc_id_2(offsets2, leadout2) );
		CHECK ( 0x007469B8 == disc_id_2(offsets3, leadout3) );
		CHECK ( 0x0281351D == disc_id_2(offsets4, leadout4) );
		CHECK ( 0x00072039 == disc_id_2(offsets5, leadout5) );
	}

	SECTION ( "disc_id_2() works for empty or zero input" )
	{
		using arcstk::accuraterip::id::disc_id_2;

		CHECK ( 0 == disc_id_2({ /*empty*/  },  0) );
		CHECK ( 1 * 1 + 1 * 2 + 1 * 3 + 1 * 4 == disc_id_2({ 0, 0, 0, 0 },  0) );
	}

	SECTION ( "cddb_id() works" )
	{
		using arcstk::accuraterip::id::cddb_id;

		CHECK ( 0xB40d2d0f == cddb_id(offsets1, leadout1) );
		CHECK ( 0x190DCC03 == cddb_id(offsets2, leadout2) );
		CHECK ( 0x870AF109 == cddb_id(offsets3, leadout3) );
		CHECK ( 0x27114B12 == cddb_id(offsets4, leadout4) );
		CHECK ( 0x020C2901 == cddb_id(offsets5, leadout5) );
	}

	SECTION ( "cddb_id() works for empty or zero input" )
	{
		using arcstk::accuraterip::id::cddb_id;

		CHECK ( 0 == cddb_id({ /*empty*/  },  0) );
		CHECK ( 0x08000004 == cddb_id({ 0, 0, 0, 0 },  0) );
	}
}


TEST_CASE ( "construct_filename", "[id]" )
{
	using arcstk::accuraterip::id::construct_filename;

	SECTION ( "Constructing regular dBAR filenames works" )
	{
		CHECK ( construct_filename(10, 0x02C34FD0, 0x01F880CC, 0xBC55023F)
				== "dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( construct_filename(15, 0x001B9178, 0x014BE24E, 0xB40d2d0f)
				== "dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		CHECK ( construct_filename(3, 0x0008100C, 0x001AC008, 0x190DCC03)
				== "dBAR-003-0008100c-001ac008-190dcc03.bin" );

		CHECK ( construct_filename(9, 0x001008A6, 0x007469B8, 0x870AF109)
				== "dBAR-009-001008a6-007469b8-870af109.bin" );

		CHECK ( construct_filename(18, 0x00307C78, 0x0281351D, 0x27114B12)
				== "dBAR-018-00307c78-0281351d-27114b12.bin" );

		CHECK ( construct_filename(1, 0x0003902D, 0x00072039, 0x020C2901)
				== "dBAR-001-0003902d-00072039-020c2901.bin" );
	}
}


TEST_CASE ( "construct_url", "[id]" )
{
	using arcstk::accuraterip::id::construct_url;

	const auto url = std::string { "http://www.accuraterip.com/accuraterip/" };

	REQUIRE ( url == arcstk::ACCURATERIP::request_url_prefix());


	SECTION ( "Constructing regular AccurateRip URLs works" )
	{
		CHECK ( construct_url(10, 0x02C34FD0, 0x01F880CC, 0xBC55023F)
				== url + "0/d/f/" + "dBAR-010-02c34fd0-01f880cc-bc55023f.bin");

		CHECK ( construct_url(15, 0x001B9178, 0x014BE24E, 0xB40d2d0f)
				== url + "8/7/1/" + "dBAR-015-001b9178-014be24e-b40d2d0f.bin");

		CHECK ( construct_url(3, 0x0008100C, 0x001AC008, 0x190DCC03)
				== url + "c/0/0/" + "dBAR-003-0008100c-001ac008-190dcc03.bin");

		CHECK ( construct_url(9, 0x001008A6, 0x007469B8, 0x870AF109)
				== url + "6/a/8/" + "dBAR-009-001008a6-007469b8-870af109.bin");

		CHECK ( construct_url(18, 0x00307C78, 0x0281351D, 0x27114B12)
				== url + "8/7/c/" + "dBAR-018-00307c78-0281351d-27114b12.bin");

		CHECK ( construct_url(1, 0x0003902D, 0x00072039, 0x020C2901)
				== url + "d/2/0/" + "dBAR-001-0003902d-00072039-020c2901.bin");
	}
}


TEST_CASE ( "construct_id", "[id]" )
{
	using arcstk::accuraterip::id::construct_id;

	SECTION ( "Constructing regular AccurateRip ids works" )
	{
		CHECK ( construct_id(10, 0x02C34FD0, 0x01F880CC, 0xBC55023F)
				== "010-02c34fd0-01f880cc-bc55023f");

		CHECK ( construct_id(15, 0x001B9178, 0x014BE24E, 0xB40d2d0f)
				== "015-001b9178-014be24e-b40d2d0f");

		CHECK ( construct_id(3, 0x0008100C, 0x001AC008, 0x190DCC03)
				== "003-0008100c-001ac008-190dcc03");

		CHECK ( construct_id(9, 0x001008A6, 0x007469B8, 0x870AF109)
				== "009-001008a6-007469b8-870af109");

		CHECK ( construct_id(18, 0x00307C78, 0x0281351D, 0x27114B12)
				== "018-00307c78-0281351d-27114b12");

		CHECK ( construct_id(1, 0x0003902D, 0x00072039, 0x020C2901)
				== "001-0003902d-00072039-020c2901");
	}
}


//TEST_CASE ( "Updating ARCS v1+v2 with MultiTrackContext", "[update]" )
//{
/*
	using arcstk::details::TOCBuilder;
	using arcstk::make_context;

	auto toc { TOCBuilder::build(
		3, // track count
		{ 12, 433, 924 }, // offsets
		1233 // leadout
	)};

	auto mtcx { make_context(toc) };

	CHECK ( mtcx->total_tracks() == 3 );
	CHECK ( mtcx->offset(0) ==  12 );
	CHECK ( mtcx->offset(1) == 433 );
	CHECK ( mtcx->offset(2) == 924 );
	CHECK ( mtcx->audio_size().leadout_frame() == 1233 );
	CHECK ( mtcx->is_multi_track() );
	CHECK ( mtcx->skips_front() );
	CHECK ( mtcx->skips_back() );
	CHECK ( mtcx->num_skip_front() == 2939 );
	CHECK ( mtcx->num_skip_back()  == 2940 );
*/

/*
	SECTION ( "Correct ARCS1+2 with aligned blocks" )
	{
		arcstk::accuraterip::UpdateableSubtotals<type::ARCS1,type::ARCS2> state {};

		// Initialize Buffer

		std::vector<csample_t> buffer(181251); // samples

		// => forms 4 blocks with 181251 samples each
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("data/calculation-test-02.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-02.bin");
		}
		for (int i = 0; i < 4; ++i)
		{
			//CHECK ( not calculation.complete() );

			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 725004);
				// 725004 bytes == 181251 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-02.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end(), 181251);
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}
		}

		in.close();

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 3 );


		// Checks

		auto track1 { checksums[0] };

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (track1.get(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (track1.get(type::ARCS1)).value());

		auto track2 { checksums[1] };

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (track2.get(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (track2.get(type::ARCS1)).value());

		auto track3 { checksums[2] };

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (track3.get(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (track3.get(type::ARCS1)).value());
	}


	SECTION ( "Correct ARCS1+2 with non-aligned blocks" )
	{
		using arcstk::csample_t;

		// Initialize Buffer

		std::vector<csample_t> buffer(241584); // samples

		// => forms 3 blocks: 2 x 241584 samples and 1 x 252 samples
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("data/calculation-test-02.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-02.bin");
		}
		for (int i = 0; i < 3; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 966336);
				// 966336 bytes == 241584 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-02.bin");
			}

			try
			{
				calculation.update(buffer.begin(), buffer.end(), 241584);
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(252);
			in.read(reinterpret_cast<char*>(&buffer[0]), 1008);
		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-02.bin");
		}

		in.close();

		calculation.update(buffer.begin(), buffer.end(), 241584);

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 3 );


		// Checks

		auto track1 = checksums[0];

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (track1.get(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (track1.get(type::ARCS1)).value());

		auto track2 = checksums[1];

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (track2.get(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (track2.get(type::ARCS1)).value());

		auto track3 = checksums[2];

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (track3.get(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (track3.get(type::ARCS1)).value());
	}
*/
//}

