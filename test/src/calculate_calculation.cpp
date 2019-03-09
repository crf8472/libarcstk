#include "catch2/catch.hpp"

#include <cstdint>
#include <fstream>
#include <memory>

#ifndef __LIBARCS_CALCULATE_HPP__
#include "calculate.hpp"
#endif


/**
 * \file calculate_calculation.cpp Fixtures for class \ref Calculation
 */

TEST_CASE ( "Calculation construction", "[calculate] [calculation]" )
{

// Commented out: constructors will be removed from public API
//
//	SECTION ( "Construct without arguments" )
//	{
//		arcs::Calculation calc; // equivalent to: singletrack + arcsv2
//
//		REQUIRE ( not calc.context().is_multi_track() );
//		REQUIRE ( not calc.context().skips_front() );
//		REQUIRE ( not calc.context().skips_back() );
//		REQUIRE ( 0 == calc.context().num_skip_front() );
//		REQUIRE ( 0 == calc.context().num_skip_back() );
//		REQUIRE ( calc.type() == arcs::checksum::type::ARCS2 );
//	}
//
//	SECTION ( "Construct with type" )
//	{
//		arcs::Calculation calc1(arcs::checksum::type::ARCS1);
//
//		REQUIRE ( not calc1.context().is_multi_track() );
//		REQUIRE ( not calc1.context().skips_front() );
//		REQUIRE ( not calc1.context().skips_back() );
//		REQUIRE ( calc1.type() == arcs::checksum::type::ARCS1 );
//
//
//		arcs::Calculation calc2(arcs::checksum::type::ARCS2);
//
//		REQUIRE ( not calc2.context().is_multi_track() );
//		REQUIRE ( not calc2.context().skips_front() );
//		REQUIRE ( not calc2.context().skips_back() );
//		REQUIRE ( calc2.type() == arcs::checksum::type::ARCS2 );
//	}

	SECTION ( "Construct with only context (default type)" )
	{
		// Implicitly tested in section
		// 'Calculation configuration/Changing the type...' below

		arcs::details::TOCBuilder builder;

		// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
		std::unique_ptr<arcs::TOC> toc1 = builder.build(
				3, /* track count */
				{ 12, 433, 924 }, /* offsets */
				1233 /* leadout */);

		auto ctx = arcs::make_context(std::string(/* no filename */), *toc1);

		REQUIRE ( ctx->track_count() == 3 );
		REQUIRE ( ctx->offset(0) ==  12 );
		REQUIRE ( ctx->offset(1) == 433 );
		REQUIRE ( ctx->offset(2) == 924 );
		REQUIRE ( ctx->audio_size().leadout_frame() == 1233 );

		REQUIRE ( ctx->is_multi_track() );
		REQUIRE ( ctx->skips_front() );
		REQUIRE ( ctx->skips_back() );
		REQUIRE ( 2939 == ctx->num_skip_front() );
		REQUIRE ( 2940 == ctx->num_skip_back() );


		// Construct Calculation

		arcs::Calculation calc(arcs::checksum::type::ARCS1, std::move(ctx));

		REQUIRE ( calc.context().audio_size().leadout_frame() == 1233 );
		REQUIRE ( calc.context().audio_size().pcm_byte_count() == 2900016 );
		REQUIRE ( calc.context().audio_size().sample_count() == 725004 );

		REQUIRE ( calc.context().is_multi_track() );
		REQUIRE ( calc.context().skips_front() );
		REQUIRE ( calc.context().skips_back() );
		REQUIRE ( 2939 == calc.context().num_skip_front() );
		REQUIRE ( 2940 == calc.context().num_skip_back() );

		REQUIRE ( calc.type() == arcs::checksum::type::ARCS1 );
		REQUIRE ( not calc.complete() );
	}

	SECTION ( "Construct with type and context" )
	{
		// Create a context

		arcs::details::TOCBuilder builder;

		// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
		std::unique_ptr<arcs::TOC> toc1 = builder.build(
				3, /* track count */
				{ 12, 433, 924 }, /* offsets */
				1233 /* leadout */);

		auto ctx = arcs::make_context(std::string(/* no filename */), *toc1);

		REQUIRE ( ctx->track_count() == 3 );
		REQUIRE ( ctx->offset(0) ==  12 );
		REQUIRE ( ctx->offset(1) == 433 );
		REQUIRE ( ctx->offset(2) == 924 );
		REQUIRE ( ctx->audio_size().leadout_frame() == 1233 );

		REQUIRE ( ctx->is_multi_track() );
		REQUIRE ( ctx->skips_front() );
		REQUIRE ( ctx->skips_back() );
		REQUIRE ( 2939 == ctx->num_skip_front() );
		REQUIRE ( 2940 == ctx->num_skip_back() );


		// Construct Calculation

		arcs::Calculation calc(arcs::checksum::type::ARCS1, std::move(ctx));

		REQUIRE ( calc.context().audio_size().leadout_frame() == 1233 );
		REQUIRE ( calc.context().audio_size().pcm_byte_count() == 2900016 );
		REQUIRE ( calc.context().audio_size().sample_count() == 725004 );

		REQUIRE ( calc.context().is_multi_track() );
		REQUIRE ( calc.context().skips_front() );
		REQUIRE ( calc.context().skips_back() );
		REQUIRE ( 2939 == calc.context().num_skip_front() );
		REQUIRE ( 2940 == calc.context().num_skip_back() );

		REQUIRE ( calc.type() == arcs::checksum::type::ARCS1 );
		REQUIRE ( not calc.complete() );
	}
}


TEST_CASE ( "Calculation configuration", "[calculate] [calculation]" )
{
	arcs::Calculation calc(arcs::make_context("foo", false, false));

	arcs::AudioSize audiosize;
	audiosize.set_sample_count(196608); // fits calculation-test-01.bin
	calc.update_audiosize(audiosize);

	REQUIRE ( calc.context().audio_size().pcm_byte_count() == 786432 );
	REQUIRE ( calc.context().audio_size().sample_count() == 196608 );
	REQUIRE ( calc.context().audio_size().leadout_frame() == 334 );
	REQUIRE ( calc.context().filename() == "foo" );
	REQUIRE ( not calc.context().is_multi_track() );
	REQUIRE ( not calc.context().skips_front() );
	REQUIRE ( not calc.context().skips_back() );
	REQUIRE ( 0 == calc.context().num_skip_front() );
	REQUIRE ( 0 == calc.context().num_skip_back() );

	REQUIRE ( calc.type() == arcs::checksum::type::ARCS2 );
	REQUIRE ( not calc.complete() );


	SECTION ( "Changing the context updates Calculation correctly" )
	{
		// Change the type to a non-default type

		//calc.set_type(arcs::checksum::type::ARCS1);

		//REQUIRE ( calc.type() == arcs::checksum::type::ARCS1 );


		// Create a completely different context

		arcs::details::TOCBuilder builder;

		// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
		std::unique_ptr<arcs::TOC> toc1 = builder.build(
				3, /* track count */
				{ 12, 433, 924 }, /* offsets */
				1233 /* leadout */);

		auto ctx = arcs::make_context(std::string(/* no filename */), *toc1);

		REQUIRE ( ctx->track_count() == 3 );
		REQUIRE ( ctx->offset(0) ==  12 );
		REQUIRE ( ctx->offset(1) == 433 );
		REQUIRE ( ctx->offset(2) == 924 );
		REQUIRE ( ctx->audio_size().leadout_frame() == 1233 );

		REQUIRE ( ctx->is_multi_track() );
		REQUIRE ( ctx->skips_front() );
		REQUIRE ( ctx->skips_back() );
		REQUIRE ( 2939 == ctx->num_skip_front() );
		REQUIRE ( 2940 == ctx->num_skip_back() );


		// Set the different context as the new context at check values again

		calc.set_context(std::move(ctx));

		REQUIRE ( calc.context().audio_size().leadout_frame() == 1233 );
		REQUIRE ( calc.context().audio_size().pcm_byte_count() == 2900016 );
		REQUIRE ( calc.context().audio_size().sample_count() == 725004 );

		// changed: multi_track + skipping
		REQUIRE ( calc.context().is_multi_track() );
		REQUIRE ( calc.context().skips_front() );
		REQUIRE ( calc.context().skips_back() );
		REQUIRE ( 2939 == calc.context().num_skip_front() );
		REQUIRE ( 2940 == calc.context().num_skip_back() );
		REQUIRE ( not calc.complete() );

		// type: unchanged
		//REQUIRE ( calc.type() == arcs::checksum::type::ARCS1 );
	}


//	SECTION ( "Changing the type updates Calculation correctly" )
//	{
//		arcs::details::TOCBuilder builder;
//
//		// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
//		std::unique_ptr<arcs::TOC> toc1 = builder.build(
//				3, /* track count */
//				{ 12, 433, 924 }, /* offsets */
//				1233 /* leadout */);
//
//		auto ctx = arcs::make_context(std::string(/* no filename */), *toc1);
//
//		REQUIRE ( ctx->track_count() == 3 );
//		REQUIRE ( ctx->offset(0) ==  12 );
//		REQUIRE ( ctx->offset(1) == 433 );
//		REQUIRE ( ctx->offset(2) == 924 );
//		REQUIRE ( ctx->audio_size().leadout_frame() == 1233 );
//
//		REQUIRE ( ctx->is_multi_track() );
//		REQUIRE ( ctx->skips_front() );
//		REQUIRE ( ctx->skips_back() );
//		REQUIRE ( 2939 == ctx->num_skip_front() );
//		REQUIRE ( 2940 == ctx->num_skip_back() );
//
//
//		// Construct a Calculation with this context
//
//		arcs::Calculation calc1(std::move(ctx));
//
//		REQUIRE ( calc1.context().audio_size().leadout_frame() == 1233 );
//		REQUIRE ( calc1.context().audio_size().pcm_byte_count() == 2900016 );
//		REQUIRE ( calc1.context().audio_size().sample_count() == 725004 );
//
//		REQUIRE ( calc1.context().is_multi_track() );
//		REQUIRE ( calc1.context().skips_front() );
//		REQUIRE ( calc1.context().skips_back() );
//		REQUIRE ( 2939 == calc1.context().num_skip_front() );
//		REQUIRE ( 2940 == calc1.context().num_skip_back() );
//
//		REQUIRE ( calc1.type() == arcs::checksum::type::ARCS2 );
//
//
//		// Now change the type
//
//		calc1.set_type(arcs::checksum::type::ARCS1);
//
//		// Context must be equivalent, only type must have changed
//
//		REQUIRE ( calc1.context().audio_size().leadout_frame() == 1233 );
//		REQUIRE ( calc1.context().audio_size().pcm_byte_count() == 2900016 );
//		REQUIRE ( calc1.context().audio_size().sample_count() == 725004 );
//
//		REQUIRE ( calc1.context().is_multi_track() );
//		REQUIRE ( calc1.context().skips_front() );
//		REQUIRE ( calc1.context().skips_back() );
//		REQUIRE ( 2939 == calc1.context().num_skip_front() );
//		REQUIRE ( 2940 == calc1.context().num_skip_back() );
//
//		REQUIRE ( calc1.type() == arcs::checksum::type::ARCS1 );
//	}
}


TEST_CASE ( "Calculation::update() with aligned blocks in singletrack/v1+2",
		"[calculate] [calculation]" )
{
	// Initialize Calculation

	arcs::Calculation calc(arcs::make_context("foo", false, false));

	arcs::AudioSize audiosize;
	audiosize.set_sample_count(196608);
	calc.update_audiosize(audiosize);

	REQUIRE ( calc.context().audio_size().pcm_byte_count() == 786432 );
	REQUIRE ( calc.context().audio_size().sample_count() == 196608 );
	REQUIRE ( calc.context().audio_size().leadout_frame() == 334 );
	REQUIRE ( calc.context().filename() == "foo" );
	REQUIRE ( not calc.context().is_multi_track() );
	REQUIRE ( not calc.context().skips_front() );
	REQUIRE ( not calc.context().skips_back() );
	REQUIRE ( calc.type() == arcs::checksum::type::ARCS2 );
	REQUIRE ( not calc.complete() );

	// Initialize Buffer

	//arcs::SampleBlock buffer(65536); // samples
	std::vector<uint32_t> buffer(65536); // samples
	// This forms 3 blocks with 65536 samples each


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

	for (int i = 0; i < 3; ++i)
	{
		REQUIRE ( not calc.complete() );

		try
		{
			in.read(reinterpret_cast<char*>(&buffer[0]), 262144);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error while reading from file calculation-test-01.bin");
		}

		try
		{
			calc.update(buffer.begin(), buffer.end());
		} catch (...)
		{
			in.close();
			FAIL ("Error while updating buffer");
		}
	}

	in.close();

	REQUIRE ( calc.complete() );

	auto checksums = calc.result();

	REQUIRE ( checksums.size() == 1 );


	SECTION ( "Calculation::update() calculates aligned blocks correctly" )
	{
		// Only track with correct ARCSs

		using type = arcs::checksum::type;

		auto track0 = checksums[0];

		REQUIRE ( track0.size() == 2 );
		REQUIRE ( 0xD15BB487 == (*track0.find(type::ARCS2)).value() );
		REQUIRE ( 0x8FE8D29B == (*track0.find(type::ARCS1)).value() );
	}
}


TEST_CASE ( "Calculation::update() with non-aligned blocks in singletrack/v1+2",
		"[calculate] [calculation]" )
{
	// Initialize Calculation

	arcs::Calculation calc(arcs::make_context("bar", false, false));

	arcs::AudioSize audiosize;
	audiosize.set_sample_count(196608);
	calc.update_audiosize(audiosize);

	REQUIRE ( calc.context().audio_size().pcm_byte_count() == 786432 );
	REQUIRE ( calc.context().audio_size().sample_count() == 196608 );
	REQUIRE ( calc.context().audio_size().leadout_frame() == 334 );
	REQUIRE ( calc.context().filename() == "bar" );
	REQUIRE ( not calc.context().is_multi_track() );
	REQUIRE ( not calc.context().skips_front() );
	REQUIRE ( not calc.context().skips_back() );
	REQUIRE ( not calc.complete() );


	// Initialize Buffer

	std::vector<uint32_t> buffer(80000); // samples
	// This forms 3 blocks: two with 80000 samples and one with 36608 samples


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

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error while reading from file calculation-test-01.bin");
		}

		try
		{
			calc.update(buffer.begin(), buffer.end());
		} catch (...)
		{
			in.close();
			FAIL ("Error while updating buffer");
		}

		REQUIRE ( not calc.complete() );
	}
	try // last block is smaller
	{
		//buffer.set_size(36608);
		buffer.resize(36608);
		in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

	} catch (const std::ifstream::failure& f)
	{
		in.close();
		FAIL ("Error on last block from file calculation-test-01.bin");
	}

	in.close();

	calc.update(buffer.begin(), buffer.end());

	REQUIRE ( calc.complete() );

	auto checksums = calc.result();

	REQUIRE ( checksums.size() == 1 );


	SECTION ( "Calculation::update() calculates non-aligned blocks correctly" )
	{
		using type = arcs::checksum::type;

		// Only track with correct ARCSs

		auto track0 = checksums[0];

		REQUIRE ( track0.size() == 2 );
		REQUIRE ( 0xD15BB487 == (*track0.find(type::ARCS2)).value() );
		REQUIRE ( 0x8FE8D29B == (*track0.find(type::ARCS1)).value() );
	}
}


TEST_CASE ( "Calculation::update() with aligned blocks in multitrack",
		"[calculate] [calculation]" )
{
	// Initialize Buffer and Calculation

	arcs::details::TOCBuilder builder;

	// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
	std::unique_ptr<arcs::TOC> toc1 = builder.build(3, { 12, 433, 924 }, 1233);

	auto ctx = arcs::make_context(std::string(), *toc1);

	REQUIRE ( ctx->track_count() == 3 );
	REQUIRE ( ctx->offset(0) ==  12 );
	REQUIRE ( ctx->offset(1) == 433 );
	REQUIRE ( ctx->offset(2) == 924 );
	REQUIRE ( ctx->audio_size().leadout_frame() == 1233 );

	REQUIRE ( ctx->is_multi_track() );
	REQUIRE ( ctx->skips_front() );
	REQUIRE ( ctx->skips_back() );


	arcs::Calculation calc(std::move(ctx));

	REQUIRE ( calc.context().audio_size().leadout_frame() == 1233 );
	REQUIRE ( calc.context().audio_size().pcm_byte_count() == 2900016 );
	REQUIRE ( calc.context().audio_size().sample_count() == 725004 );

	REQUIRE ( calc.context().is_multi_track() );
	REQUIRE ( calc.context().skips_front() );
	REQUIRE ( calc.context().skips_back() );
	REQUIRE ( 2939 == calc.context().num_skip_front() );
	REQUIRE ( 2940 == calc.context().num_skip_back() );
	REQUIRE ( arcs::checksum::type::ARCS2 == calc.type() );
	REQUIRE ( not calc.complete() );


	// Initialize Buffer

	std::vector<uint32_t> buffer(181251); // samples
	// Gives 4 blocks with 181251 samples (==725004 bytes) each
	// ==725004 samples total, 2900016 bytes total


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
		REQUIRE ( not calc.complete() );

		try
		{
			in.read(reinterpret_cast<char*>(&buffer[0]), 725004);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error while reading from file calculation-test-02.bin");
		}

		try
		{
			calc.update(buffer.begin(), buffer.end());
		} catch (...)
		{
			in.close();
			FAIL ("Error while updating buffer");
		}
	}

	in.close();

	REQUIRE ( calc.complete() );

	auto checksums = calc.result();

	REQUIRE ( checksums.size() == 3 );


	SECTION ( "Calculation::update() calculates aligned blocks correctly" )
	{
		using type = arcs::checksum::type;

		auto track1 = checksums[0];

		REQUIRE ( track1.size() == 2 );
		REQUIRE ( 0x0DF230F0 == (*track1.find(type::ARCS2)).value());
		REQUIRE ( 0x7C7BFAF4 == (*track1.find(type::ARCS1)).value());

		auto track2 = checksums[1];

		REQUIRE ( track2.size() == 2 );
		REQUIRE ( 0x34C681C3 == (*track2.find(type::ARCS2)).value());
		REQUIRE ( 0x5989C533 == (*track2.find(type::ARCS1)).value());

		auto track3 = checksums[2];

		REQUIRE ( track3.size() == 2 );
		REQUIRE ( 0xB845A497 == (*track3.find(type::ARCS2)).value());
		REQUIRE ( 0xDD95CE6C == (*track3.find(type::ARCS1)).value());
	}
}


TEST_CASE ( "Calculation::update() with non-aligned blocks in multitrack",
		"[calculate] [calculation]" )
{
	// Initialize Calculation

	arcs::details::TOCBuilder builder;

	// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
	std::unique_ptr<arcs::TOC> toc = builder.build(3, { 12, 433, 924 }, 1233);

	auto ctx = arcs::make_context(std::string(), *toc);

	REQUIRE ( ctx->track_count() == 3 );
	REQUIRE ( ctx->offset(0) == 12 );
	REQUIRE ( ctx->offset(1) == 433 );
	REQUIRE ( ctx->offset(2) == 924 );
	REQUIRE ( ctx->audio_size().leadout_frame() == 1233 );

	REQUIRE ( ctx->is_multi_track() );
	REQUIRE ( ctx->skips_front() );
	REQUIRE ( ctx->skips_back() );


	arcs::Calculation calc(std::move(ctx));

	REQUIRE ( calc.context().audio_size().leadout_frame() == 1233 );
	REQUIRE ( calc.context().audio_size().pcm_byte_count() == 2900016 );
	REQUIRE ( calc.context().audio_size().sample_count() == 725004 );

	REQUIRE ( calc.context().is_multi_track() );
	REQUIRE ( calc.context().skips_front() );
	REQUIRE ( calc.context().skips_back() );
	REQUIRE ( 2939 == calc.context().num_skip_front() );
	REQUIRE ( 2940 == calc.context().num_skip_back() );
	REQUIRE ( arcs::checksum::type::ARCS2 == calc.type() );
	REQUIRE ( not calc.complete() );


	// Initialize Buffer

	std::vector<uint32_t> buffer(241584); // samples
	// Gives 3 blocks with 241584 samples each (== 966336 bytes)
	// and 1 block with 252 samples (== 1008 bytes)
	// ==725004 samples total, 2900016 bytes total


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

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error while reading from file calculation-test-02.bin");
		}

		try
		{
			calc.update(buffer.begin(), buffer.end());
		} catch (...)
		{
			in.close();
			FAIL ("Error while updating buffer");
		}

		REQUIRE ( not calc.complete() );
	}
	try // last block is smaller
	{
		//buffer.set_size(252);
		buffer.resize(252);
		in.read(reinterpret_cast<char*>(&buffer[0]), 1008);
	} catch (const std::ifstream::failure& f)
	{
		in.close();
		FAIL ("Error on last block from file calculation-test-02.bin");
	}

	in.close();

	calc.update(buffer.begin(), buffer.end());

	REQUIRE ( calc.complete() );

	auto checksums = calc.result();

	REQUIRE ( checksums.size() == 3 );


	SECTION ( "Calculation::update() calculates non-aligned blocks correctly" )
	{
		using type = arcs::checksum::type;

		auto track1 = checksums[0];

		REQUIRE ( track1.size() == 2 );
		REQUIRE ( 0x0DF230F0 == (*track1.find(type::ARCS2)).value());
		REQUIRE ( 0x7C7BFAF4 == (*track1.find(type::ARCS1)).value());

		auto track2 = checksums[1];

		REQUIRE ( track2.size() == 2 );
		REQUIRE ( 0x34C681C3 == (*track2.find(type::ARCS2)).value());
		REQUIRE ( 0x5989C533 == (*track2.find(type::ARCS1)).value());

		auto track3 = checksums[2];

		REQUIRE ( track3.size() == 2 );
		REQUIRE ( 0xB845A497 == (*track3.find(type::ARCS2)).value());
		REQUIRE ( 0xDD95CE6C == (*track3.find(type::ARCS1)).value());
	}
}


TEST_CASE ( "Calculation::update() accepts vector<uint32_t>", "" )
{
	// Load example samples

	std::ifstream in;
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		in.open("samplesequence-test-01.bin",
				std::ifstream::in | std::ifstream::binary);
	} catch (const std::ifstream::failure& f)
	{
		FAIL ("Could not open test data file calculation-test-01.bin");
	}

	std::vector<uint8_t> bytes(
			(std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>()
	);

	in.close();

	arcs::Calculation calc(arcs::make_context("bar", false, false));

	arcs::AudioSize audiosize;
	audiosize.set_sample_count(196608);
	calc.update_audiosize(audiosize);

	REQUIRE ( calc.context().audio_size().pcm_byte_count() == 786432 );
	REQUIRE ( calc.context().audio_size().sample_count() == 196608 );
	REQUIRE ( calc.context().audio_size().leadout_frame() == 334 );
	REQUIRE ( calc.context().filename() == "bar" );
	REQUIRE ( not calc.context().is_multi_track() );
	REQUIRE ( not calc.context().skips_front() );
	REQUIRE ( not calc.context().skips_back() );
	REQUIRE ( not calc.complete() );


	// Initialize Buffer

	std::vector<uint32_t> buffer(80000); // samples

	//calc.update(buffer);
}

