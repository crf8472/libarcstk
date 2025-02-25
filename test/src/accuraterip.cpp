#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for accuraterip.hpp.
 */

#ifndef __LIBARCSTK_ALGORITHMS_HPP__
#define __LIBARCSTK_ALGORITHMS_HPP__ // allow accuraterip.hpp
#endif
#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#include "accuraterip.hpp"        // TO BE TESTED
#endif

#ifndef __LIBARCSTK_SAMPLES_HPP__
#include "samples.hpp"            // for sample_t
#endif
#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"           // for checksum::type
#endif
#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"           // for AudioSize
#endif

#include <fstream>                // for ifstream
#include <unordered_set>          // for unordered_set
#include <vector>                 // for vector


TEST_CASE ( "AccurateRipCS", "[updatable]" )
{
	using arcstk::accuraterip::details::AccurateRipCS;
	using arcstk::checksum::type;

	AccurateRipCS<type::ARCS1,type::ARCS2> u1;
	AccurateRipCS<type::ARCS1,type::ARCS2> u2;

	std::vector<uint32_t> data { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

	using std::cbegin;
	using std::cend;

	u1.set_multiplier(1);
	u1.update(cbegin(data), cend(data));

	SECTION( "Swapping instances of AccurateRipCS<> works" )
	{
		CHECK ( u1.value().get(type::ARCS2).value() == 650 );
		CHECK ( u2.value().get(type::ARCS2).value() ==   0 );

		using std::swap;
		swap(u1, u2);

		CHECK ( u1.value().get(type::ARCS2).value() ==   0 );
		CHECK ( u2.value().get(type::ARCS2).value() == 650 );
	}
}


TEST_CASE ( "Updating ARCS v1+v2", "[arcsalgorithm] [calc]" )
{
	using arcstk::AudioSize;
	using arcstk::checksum::type;
	using arcstk::sample_t;
	namespace AccurateRip = arcstk::accuraterip::details;

	// fits calculation-test-01.bin
	//auto audiosize = AudioSize { 196608, UNIT::SAMPLES };

	SECTION ( "Updating ARCS 1 singletrack & aligned blocks is correct" )
	{
		auto algo = AccurateRip::Version1{};
		REQUIRE ( algo.types() == std::unordered_set<type>{ type::ARCS1 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
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
				algo.update(buffer.begin(), buffer.end());
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

		algo.update(buffer.begin(), buffer.end());
		algo.track_finished(1, AudioSize{});

		//CHECK ( calculation.complete() );

		auto checksums { algo.result() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 1 /* types */ );
		CHECK ( 0x8FE8D29B == (checksums.get(type::ARCS1)) );
	}


	SECTION ( "Updating ARCS 2 singletrack & aligned blocks is correct" )
	{
		auto state = AccurateRip::AccurateRipCS<type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{ type::ARCS2 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
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
				state.update(buffer.begin(), buffer.end());
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

		state.update(buffer.begin(), buffer.end());

		//CHECK ( calculation.complete() );

		auto checksums { state.value() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 1 /* types */ );
		CHECK ( 0xD15BB487 == (checksums.get(type::ARCS2)) );
	}


	SECTION ( "Updating ARCS v1+2 singletrack & aligned blocks is correct" )
	{
		auto state = AccurateRip::AccurateRipCS<type::ARCS1,type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{
				type::ARCS1, type::ARCS2 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
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
				state.update(buffer.begin(), buffer.end());
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

		state.update(buffer.begin(), buffer.end());

		//CHECK ( calculation.complete() );

		auto checksums { state.value() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 2 /* types */ );
		CHECK ( 0xD15BB487 == (checksums.get(type::ARCS2)) );
		CHECK ( 0x8FE8D29B == (checksums.get(type::ARCS1)) );
	}


	SECTION ( "Updating ARCS v1+2 singletrack & non-aligned blocks is correct" )
	{
		auto state = AccurateRip::AccurateRipCS<type::ARCS1,type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{
				type::ARCS1, type::ARCS2 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
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
				state.update(buffer.begin(), buffer.end());
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

		state.update(buffer.begin(), buffer.end());

		//CHECK ( calculation.complete() );

		auto checksums { state.value() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 2 );
		CHECK ( 0xD15BB487 == (checksums.get(type::ARCS2)).value() );
		CHECK ( 0x8FE8D29B == (checksums.get(type::ARCS1)).value() );
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
		arcstk::accuraterip::AccurateRipCS<type::ARCS1,type::ARCS2> state {};

		// Initialize Buffer

		std::vector<sample_t> buffer(181251); // samples

		// => forms 4 blocks with 181251 samples each
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-02.bin",
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
				state.update(buffer.begin(), buffer.end());
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
		using arcstk::sample_t;

		// Initialize Buffer

		std::vector<sample_t> buffer(241584); // samples

		// => forms 3 blocks: 2 x 241584 samples and 1 x 252 samples
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-02.bin",
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
				calculation.update(buffer.begin(), buffer.end());
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

		calculation.update(buffer.begin(), buffer.end());

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

