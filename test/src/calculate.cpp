#include "catch2/catch.hpp"

#include <cstdint>
#include <fstream>
#include <memory>

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif


/**
 * \file calculate.cpp Fixtures for classes in module calculate
 */


// Calculation


TEST_CASE ( "Calculation construction", "[calculate] [calculation]" )
{
	using arcstk::details::TOCBuilder;


	SECTION ( "Construct with only context (default type)" )
	{
		// Implicitly tested in section
		// 'Calculation configuration/Changing the type...' below

		// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
		std::unique_ptr<arcstk::TOC> toc1 = TOCBuilder::build(
				3, /* track count */
				{ 12, 433, 924 }, /* offsets */
				1233 /* leadout */
		);

		auto ctx = arcstk::make_context(std::string(/* no filename */), *toc1);

		CHECK ( ctx->track_count() == 3 );
		CHECK ( ctx->offset(0) ==  12 );
		CHECK ( ctx->offset(1) == 433 );
		CHECK ( ctx->offset(2) == 924 );
		CHECK ( ctx->audio_size().leadout_frame() == 1233 );

		CHECK ( ctx->is_multi_track() );
		CHECK ( ctx->skips_front() );
		CHECK ( ctx->skips_back() );
		CHECK ( 2939 == ctx->num_skip_front() );
		CHECK ( 2940 == ctx->num_skip_back() );


		// Construct Calculation

		arcstk::Calculation calc(arcstk::checksum::type::ARCS1, std::move(ctx));

		CHECK ( calc.context().audio_size().leadout_frame() == 1233 );
		CHECK ( calc.context().audio_size().pcm_byte_count() == 2900016 );
		CHECK ( calc.context().audio_size().sample_count() == 725004 );

		CHECK ( calc.context().is_multi_track() );
		CHECK ( calc.context().skips_front() );
		CHECK ( calc.context().skips_back() );
		CHECK ( 2939 == calc.context().num_skip_front() );
		CHECK ( 2940 == calc.context().num_skip_back() );

		CHECK ( calc.type() == arcstk::checksum::type::ARCS1 );
		CHECK ( not calc.complete() );
	}

	SECTION ( "Construct with type and context" )
	{
		// Create a context

		using arcstk::details::TOCBuilder;

		// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
		std::unique_ptr<arcstk::TOC> toc1 = TOCBuilder::build(
				3, /* track count */
				{ 12, 433, 924 }, /* offsets */
				1233 /* leadout */
		);

		auto ctx = arcstk::make_context(std::string(/* no filename */), *toc1);

		CHECK ( ctx->track_count() == 3 );
		CHECK ( ctx->offset(0) ==  12 );
		CHECK ( ctx->offset(1) == 433 );
		CHECK ( ctx->offset(2) == 924 );
		CHECK ( ctx->audio_size().leadout_frame() == 1233 );

		CHECK ( ctx->is_multi_track() );
		CHECK ( ctx->skips_front() );
		CHECK ( ctx->skips_back() );
		CHECK ( 2939 == ctx->num_skip_front() );
		CHECK ( 2940 == ctx->num_skip_back() );


		// Construct Calculation

		arcstk::Calculation calc(arcstk::checksum::type::ARCS1, std::move(ctx));

		CHECK ( calc.context().audio_size().leadout_frame() == 1233 );
		CHECK ( calc.context().audio_size().pcm_byte_count() == 2900016 );
		CHECK ( calc.context().audio_size().sample_count() == 725004 );

		CHECK ( calc.context().is_multi_track() );
		CHECK ( calc.context().skips_front() );
		CHECK ( calc.context().skips_back() );
		CHECK ( 2939 == calc.context().num_skip_front() );
		CHECK ( 2940 == calc.context().num_skip_back() );

		CHECK ( calc.type() == arcstk::checksum::type::ARCS1 );
		CHECK ( not calc.complete() );
	}
}


TEST_CASE ( "Calculation configuration", "[calculate] [calculation]" )
{
	arcstk::Calculation calc(arcstk::make_context("foo", false, false));

	arcstk::AudioSize audiosize;
	audiosize.set_sample_count(196608); // fits calculation-test-01.bin
	calc.update_audiosize(audiosize);

	CHECK ( calc.context().audio_size().pcm_byte_count() == 786432 );
	CHECK ( calc.context().audio_size().sample_count() == 196608 );
	CHECK ( calc.context().audio_size().leadout_frame() == 334 );
	CHECK ( calc.context().filename() == "foo" );
	CHECK ( not calc.context().is_multi_track() );
	CHECK ( not calc.context().skips_front() );
	CHECK ( not calc.context().skips_back() );
	CHECK ( 0 == calc.context().num_skip_front() );
	CHECK ( 0 == calc.context().num_skip_back() );

	CHECK ( calc.type() == arcstk::checksum::type::ARCS2 );
	CHECK ( not calc.complete() );


	SECTION ( "Changing the context updates Calculation correctly" )
	{
		using arcstk::details::TOCBuilder;

		// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
		std::unique_ptr<arcstk::TOC> toc1 = TOCBuilder::build(
				3, /* track count */
				{ 12, 433, 924 }, /* offsets */
				1233 /* leadout */
		);

		auto ctx = arcstk::make_context(std::string(/* no filename */), *toc1);

		CHECK ( ctx->track_count() == 3 );
		CHECK ( ctx->offset(0) ==  12 );
		CHECK ( ctx->offset(1) == 433 );
		CHECK ( ctx->offset(2) == 924 );
		CHECK ( ctx->audio_size().leadout_frame() == 1233 );

		CHECK ( ctx->is_multi_track() );
		CHECK ( ctx->skips_front() );
		CHECK ( ctx->skips_back() );
		CHECK ( 2939 == ctx->num_skip_front() );
		CHECK ( 2940 == ctx->num_skip_back() );


		// Set the different context as the new context at check values again

		calc.set_context(std::move(ctx));

		CHECK ( calc.context().audio_size().leadout_frame() == 1233 );
		CHECK ( calc.context().audio_size().pcm_byte_count() == 2900016 );
		CHECK ( calc.context().audio_size().sample_count() == 725004 );

		// changed: multi_track + skipping
		CHECK ( calc.context().is_multi_track() );
		CHECK ( calc.context().skips_front() );
		CHECK ( calc.context().skips_back() );
		CHECK ( 2939 == calc.context().num_skip_front() );
		CHECK ( 2940 == calc.context().num_skip_back() );
		CHECK ( not calc.complete() );
	}
}


TEST_CASE ( "Calculation::update() with aligned blocks in singletrack/v1+2",
		"[calculate] [calculation]" )
{
	// Initialize Calculation

	arcstk::Calculation calc(arcstk::make_context("foo", false, false));

	arcstk::AudioSize audiosize;
	audiosize.set_sample_count(196608);
	calc.update_audiosize(audiosize);

	CHECK ( calc.context().audio_size().pcm_byte_count() == 786432 );
	CHECK ( calc.context().audio_size().sample_count() == 196608 );
	CHECK ( calc.context().audio_size().leadout_frame() == 334 );
	CHECK ( calc.context().filename() == "foo" );
	CHECK ( not calc.context().is_multi_track() );
	CHECK ( not calc.context().skips_front() );
	CHECK ( not calc.context().skips_back() );
	CHECK ( calc.type() == arcstk::checksum::type::ARCS2 );
	CHECK ( not calc.complete() );

	// Initialize Buffer

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
		CHECK ( not calc.complete() );

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

	CHECK ( calc.complete() );

	auto checksums = calc.result();

	CHECK ( checksums.size() == 1 );


	SECTION ( "Calculation::update() calculates aligned blocks correctly" )
	{
		// Only track with correct ARCSs

		using type = arcstk::checksum::type;

		auto track0 = checksums[0];

		CHECK ( track0.size() == 2 );
		CHECK ( 0xD15BB487 == (*track0.find(type::ARCS2)).value() );
		CHECK ( 0x8FE8D29B == (*track0.find(type::ARCS1)).value() );
	}
}


TEST_CASE ( "Calculation::update() with non-aligned blocks in singletrack/v1+2",
		"[calculate] [calculation]" )
{
	// Initialize Calculation

	arcstk::Calculation calc(arcstk::make_context("bar", false, false));

	arcstk::AudioSize audiosize;
	audiosize.set_sample_count(196608);
	calc.update_audiosize(audiosize);

	CHECK ( calc.context().audio_size().pcm_byte_count() == 786432 );
	CHECK ( calc.context().audio_size().sample_count() == 196608 );
	CHECK ( calc.context().audio_size().leadout_frame() == 334 );
	CHECK ( calc.context().filename() == "bar" );
	CHECK ( not calc.context().is_multi_track() );
	CHECK ( not calc.context().skips_front() );
	CHECK ( not calc.context().skips_back() );
	CHECK ( not calc.complete() );


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

		CHECK ( not calc.complete() );
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

	CHECK ( calc.complete() );

	auto checksums = calc.result();

	CHECK ( checksums.size() == 1 );


	SECTION ( "Calculation::update() calculates non-aligned blocks correctly" )
	{
		using type = arcstk::checksum::type;

		// Only track with correct ARCSs

		auto track0 = checksums[0];

		CHECK ( track0.size() == 2 );
		CHECK ( 0xD15BB487 == (*track0.find(type::ARCS2)).value() );
		CHECK ( 0x8FE8D29B == (*track0.find(type::ARCS1)).value() );
	}
}


TEST_CASE ( "Calculation::update() with aligned blocks in multitrack",
		"[calculate] [calculation]" )
{
	// Initialize Buffer and Calculation

	using arcstk::details::TOCBuilder;

	// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
	std::unique_ptr<arcstk::TOC> toc1 =
		TOCBuilder::build(3, { 12, 433, 924 }, 1233);

	auto ctx = arcstk::make_context(std::string(), *toc1);

	CHECK ( ctx->track_count() == 3 );
	CHECK ( ctx->offset(0) ==  12 );
	CHECK ( ctx->offset(1) == 433 );
	CHECK ( ctx->offset(2) == 924 );
	CHECK ( ctx->audio_size().leadout_frame() == 1233 );

	CHECK ( ctx->is_multi_track() );
	CHECK ( ctx->skips_front() );
	CHECK ( ctx->skips_back() );


	arcstk::Calculation calc(std::move(ctx));

	CHECK ( calc.context().audio_size().leadout_frame() == 1233 );
	CHECK ( calc.context().audio_size().pcm_byte_count() == 2900016 );
	CHECK ( calc.context().audio_size().sample_count() == 725004 );

	CHECK ( calc.context().is_multi_track() );
	CHECK ( calc.context().skips_front() );
	CHECK ( calc.context().skips_back() );
	CHECK ( 2939 == calc.context().num_skip_front() );
	CHECK ( 2940 == calc.context().num_skip_back() );
	CHECK ( arcstk::checksum::type::ARCS2 == calc.type() );
	CHECK ( not calc.complete() );


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
		CHECK ( not calc.complete() );

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

	CHECK ( calc.complete() );

	auto checksums = calc.result();

	CHECK ( checksums.size() == 3 );


	SECTION ( "Calculation::update() calculates aligned blocks correctly" )
	{
		using type = arcstk::checksum::type;

		auto track1 = checksums[0];

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (*track1.find(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (*track1.find(type::ARCS1)).value());

		auto track2 = checksums[1];

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (*track2.find(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (*track2.find(type::ARCS1)).value());

		auto track3 = checksums[2];

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (*track3.find(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (*track3.find(type::ARCS1)).value());
	}
}


TEST_CASE ( "Calculation::update() with non-aligned blocks in multitrack",
		"[calculate] [calculation]" )
{
	// Initialize Calculation

	using arcstk::details::TOCBuilder;

	// Emulate 3 tracks in 1233 frames with offsets 12, 433, 924
	std::unique_ptr<arcstk::TOC> toc = TOCBuilder::build(3, { 12, 433, 924 }, 1233);

	auto ctx = arcstk::make_context(std::string(), *toc);

	CHECK ( ctx->track_count() == 3 );
	CHECK ( ctx->offset(0) == 12 );
	CHECK ( ctx->offset(1) == 433 );
	CHECK ( ctx->offset(2) == 924 );
	CHECK ( ctx->audio_size().leadout_frame() == 1233 );

	CHECK ( ctx->is_multi_track() );
	CHECK ( ctx->skips_front() );
	CHECK ( ctx->skips_back() );


	arcstk::Calculation calc(std::move(ctx));

	CHECK ( calc.context().audio_size().leadout_frame() == 1233 );
	CHECK ( calc.context().audio_size().pcm_byte_count() == 2900016 );
	CHECK ( calc.context().audio_size().sample_count() == 725004 );

	CHECK ( calc.context().is_multi_track() );
	CHECK ( calc.context().skips_front() );
	CHECK ( calc.context().skips_back() );
	CHECK ( 2939 == calc.context().num_skip_front() );
	CHECK ( 2940 == calc.context().num_skip_back() );
	CHECK ( arcstk::checksum::type::ARCS2 == calc.type() );
	CHECK ( not calc.complete() );


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

		CHECK ( not calc.complete() );
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

	CHECK ( calc.complete() );

	auto checksums = calc.result();

	CHECK ( checksums.size() == 3 );


	SECTION ( "Calculation::update() calculates non-aligned blocks correctly" )
	{
		using type = arcstk::checksum::type;

		auto track1 = checksums[0];

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (*track1.find(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (*track1.find(type::ARCS1)).value());

		auto track2 = checksums[1];

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (*track2.find(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (*track2.find(type::ARCS1)).value());

		auto track3 = checksums[2];

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (*track3.find(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (*track3.find(type::ARCS1)).value());
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

	arcstk::Calculation calc(arcstk::make_context("bar", false, false));

	arcstk::AudioSize audiosize;
	audiosize.set_sample_count(196608);
	calc.update_audiosize(audiosize);

	CHECK ( calc.context().audio_size().pcm_byte_count() == 786432 );
	CHECK ( calc.context().audio_size().sample_count() == 196608 );
	CHECK ( calc.context().audio_size().leadout_frame() == 334 );
	CHECK ( calc.context().filename() == "bar" );
	CHECK ( not calc.context().is_multi_track() );
	CHECK ( not calc.context().skips_front() );
	CHECK ( not calc.context().skips_back() );
	CHECK ( not calc.complete() );


	// Initialize Buffer

	std::vector<uint32_t> buffer(80000); // samples

	//calc.update(buffer);
}


// CalcContext


TEST_CASE ( "SingleCalcContext construction without parameters",
		"[calculate] [calccontext]" )
{
	// Construct a CalcContext without any parameters

	auto sctx = arcstk::make_context("", false, false);


	//SECTION ( "Construction without parameters is correct" )
	//{
		auto empty_default_arid = arcstk::make_empty_arid();

		CHECK ( sctx->id() == *empty_default_arid );

		CHECK ( sctx->filename() == std::string() );

		CHECK ( sctx->track_count() == 1 );

		CHECK ( sctx->offset(0) == 0 );
		CHECK ( sctx->offset(1) == 0 );
		CHECK ( sctx->offset(99) == 0 );

		CHECK ( sctx->length(0) == 0 );
		CHECK ( sctx->length(1) == 0 );
		CHECK ( sctx->length(99) == 0 );

		CHECK ( sctx->audio_size().leadout_frame()  == 0 );
		CHECK ( sctx->audio_size().sample_count()   == 0 );
		CHECK ( sctx->audio_size().pcm_byte_count() == 0 );

		CHECK ( not sctx->is_multi_track() );
		CHECK ( not sctx->skips_front() );
		CHECK ( not sctx->skips_back() );

		CHECK ( sctx->first_relevant_sample(0)   == 0 );
		CHECK ( sctx->first_relevant_sample(1)   == 0 );
		CHECK ( sctx->first_relevant_sample(99)  == 0 );
		CHECK ( sctx->first_relevant_sample(255) == 0 );

		CHECK ( sctx->last_relevant_sample()    == static_cast<uint32_t>(-1) );

		CHECK ( sctx->last_relevant_sample(0)   == static_cast<uint32_t>(-1) );
		CHECK ( sctx->last_relevant_sample(1)   == static_cast<uint32_t>(-1) );
		CHECK ( sctx->last_relevant_sample(99)  == static_cast<uint32_t>(-1) );
		CHECK ( sctx->last_relevant_sample(255) == static_cast<uint32_t>(-1) );

		CHECK ( sctx->track(0) == 1 );
		CHECK ( sctx->track(static_cast<uint32_t>(-1)) == 1);
	//}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) > 0, TOC with leadout",
	"[calculate] [calccontext]" )
{
	using arcstk::details::TOCBuilder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	std::unique_ptr<arcstk::TOC> toc = TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	auto mctx = arcstk::make_context("", *toc);


	SECTION ("pcm_byte_count() and filename()")
	{
		CHECK ( mctx->audio_size().pcm_byte_count() == 595145376 );
		CHECK ( mctx->filename() == std::string() );
	}

	SECTION ("leadout_frame() and sample_count()")
	{
		CHECK ( mctx->audio_size().leadout_frame()  == 253038 );
		CHECK ( mctx->audio_size().sample_count()   == 148786344 );
	}

	SECTION ("track_count(), offset(), length()" )
	{
		CHECK ( mctx->track_count()    == 15 );

		CHECK ( mctx->offset(0)  ==     33 );
		CHECK ( mctx->offset(1)  ==   5225 );
		CHECK ( mctx->offset(2)  ==   7390 );
		CHECK ( mctx->offset(3)  ==  23380 );
		CHECK ( mctx->offset(4)  ==  35608 );
		CHECK ( mctx->offset(5)  ==  49820 );
		CHECK ( mctx->offset(6)  ==  69508 );
		CHECK ( mctx->offset(7)  ==  87733 );
		CHECK ( mctx->offset(8)  == 106333 );
		CHECK ( mctx->offset(9)  == 139495 );
		CHECK ( mctx->offset(10) == 157863 );
		CHECK ( mctx->offset(11) == 198495 );
		CHECK ( mctx->offset(12) == 213368 );
		CHECK ( mctx->offset(13) == 225320 );
		CHECK ( mctx->offset(14) == 234103 );
		CHECK ( mctx->offset(15) ==      0 );

		// The lengths parsed from the CUEsheet differ from the lengths
		// computed by CalcContext. The cause is that for CalcContext the length
		// of track i is the difference offset(i+1) - offset(i). This accepts
		// the gaps as part of the track and appends each gap to the end of
		// the track. Libcue on the other hand seems to just ignore the gaps and
		// subtract them from the actual length.

		CHECK ( mctx->length(0)  ==  5192 );
		CHECK ( mctx->length(1)  ==  2165 );
		CHECK ( mctx->length(2)  == 15990 ); // TOC: 15885
		CHECK ( mctx->length(3)  == 12228 );
		CHECK ( mctx->length(4)  == 14212 ); // TOC: 13925
		CHECK ( mctx->length(5)  == 19688 ); // TOC: 19513
		CHECK ( mctx->length(6)  == 18225 ); // TOC: 18155
		CHECK ( mctx->length(7)  == 18600 ); // TOC: 18325
		CHECK ( mctx->length(8)  == 33162 ); // TOC: 33075
		CHECK ( mctx->length(9)  == 18368 );
		CHECK ( mctx->length(10) == 40632 ); // TOC: 40152
		CHECK ( mctx->length(11) == 14873 ); // TOC: 14798
		CHECK ( mctx->length(12) == 11952 );
		CHECK ( mctx->length(13) ==  8783 ); // TOC: 8463
		CHECK ( mctx->length(14) == 18935 ); // TOC: 18935
		CHECK ( mctx->length(15) ==     0 );
	}

	SECTION ("id(), skips_front(), skips_back() and is_multi_track()")
	{
		CHECK ( mctx->id() ==
				arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );
	}

	SECTION ("first_relevant_sample()")
	{
		CHECK ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->first_relevant_sample(1)  ==     22343 ); // skipping
		CHECK ( mctx->first_relevant_sample(2)  ==   3072300 );
		CHECK ( mctx->first_relevant_sample(3)  ==   4345320 );
		CHECK ( mctx->first_relevant_sample(4)  ==  13747440 );
		CHECK ( mctx->first_relevant_sample(5)  ==  20937504 );
		CHECK ( mctx->first_relevant_sample(6)  ==  29294160 );
		CHECK ( mctx->first_relevant_sample(7)  ==  40870704 );
		CHECK ( mctx->first_relevant_sample(8)  ==  51587004 );
		CHECK ( mctx->first_relevant_sample(9)  ==  62523804 );
		CHECK ( mctx->first_relevant_sample(10) ==  82023060 );
		CHECK ( mctx->first_relevant_sample(11) ==  92823444 );
		CHECK ( mctx->first_relevant_sample(12) == 116715060 );
		CHECK ( mctx->first_relevant_sample(13) == 125460384 );
		CHECK ( mctx->first_relevant_sample(14) == 132488160 );
		CHECK ( mctx->first_relevant_sample(15) == 137652564 );
		CHECK ( mctx->first_relevant_sample(16) ==         0 ); // not a track
		CHECK ( mctx->first_relevant_sample(99) ==         0 ); // not a track
	}

	SECTION ("last_relevant_sample()")
	{
		CHECK ( mctx->last_relevant_sample() == 148783403 );

		CHECK ( mctx->last_relevant_sample(0)  ==     19403 ); // not a track

		CHECK ( mctx->last_relevant_sample(1)  ==   3072299 );
		CHECK ( mctx->last_relevant_sample(2)  ==   4345319 );
		CHECK ( mctx->last_relevant_sample(3)  ==  13747439 );
		CHECK ( mctx->last_relevant_sample(4)  ==  20937503 );
		CHECK ( mctx->last_relevant_sample(5)  ==  29294159 );
		CHECK ( mctx->last_relevant_sample(6)  ==  40870703 );
		CHECK ( mctx->last_relevant_sample(7)  ==  51587003 );
		CHECK ( mctx->last_relevant_sample(8)  ==  62523803 );
		CHECK ( mctx->last_relevant_sample(9)  ==  82023059 );
		CHECK ( mctx->last_relevant_sample(10) ==  92823443 );
		CHECK ( mctx->last_relevant_sample(11) == 116715059 );
		CHECK ( mctx->last_relevant_sample(12) == 125460383 );
		CHECK ( mctx->last_relevant_sample(13) == 132488159 );
		CHECK ( mctx->last_relevant_sample(14) == 137652563 );
		CHECK ( mctx->last_relevant_sample(15) == 148783403 ); // skipping

		CHECK ( mctx->last_relevant_sample(16) == 148783403 ); // not a track
		CHECK ( mctx->last_relevant_sample(99) == 148783403 ); // not a track
	}

	SECTION ("track()")
	{
		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
		CHECK ( mctx->track(1) == 0);

		CHECK ( mctx->track(19402) == 0);
		CHECK ( mctx->track(19403) == 0);
		CHECK ( mctx->track(19404) == 1);

		CHECK ( mctx->track(3072298) == 1);
		CHECK ( mctx->track(3072299) == 1);
		CHECK ( mctx->track(3072300) == 2);

		CHECK ( mctx->track(4345318) == 2);
		CHECK ( mctx->track(4345319) == 2);
		CHECK ( mctx->track(4345320) == 3);

		CHECK ( mctx->track(13747438) == 3);
		CHECK ( mctx->track(13747439) == 3);
		CHECK ( mctx->track(13747440) == 4);

		CHECK ( mctx->track(20937502) == 4);
		CHECK ( mctx->track(20937503) == 4);
		CHECK ( mctx->track(20937504) == 5);

		CHECK ( mctx->track(29294158) == 5);
		CHECK ( mctx->track(29294159) == 5);
		CHECK ( mctx->track(29294160) == 6);

		CHECK ( mctx->track(40870702) == 6);
		CHECK ( mctx->track(40870703) == 6);
		CHECK ( mctx->track(40870704) == 7);

		CHECK ( mctx->track(51587002) == 7);
		CHECK ( mctx->track(51587003) == 7);
		CHECK ( mctx->track(51587004) == 8);

		CHECK ( mctx->track(62523802) == 8);
		CHECK ( mctx->track(62523803) == 8);
		CHECK ( mctx->track(62523804) == 9);

		CHECK ( mctx->track(82023058) == 9);
		CHECK ( mctx->track(82023059) == 9);
		CHECK ( mctx->track(82023060) == 10);

		CHECK ( mctx->track(92823442) == 10);
		CHECK ( mctx->track(92823443) == 10);
		CHECK ( mctx->track(92823444) == 11);

		CHECK ( mctx->track(116715058) == 11);
		CHECK ( mctx->track(116715059) == 11);
		CHECK ( mctx->track(116715060) == 12);

		CHECK ( mctx->track(125460382) == 12);
		CHECK ( mctx->track(125460383) == 12);
		CHECK ( mctx->track(125460384) == 13);

		CHECK ( mctx->track(132488158) == 13);
		CHECK ( mctx->track(132488159) == 13);
		CHECK ( mctx->track(132488160) == 14);

		CHECK ( mctx->track(137652562) == 14);
		CHECK ( mctx->track(137652563) == 14);
		CHECK ( mctx->track(137652564) == 15);

		CHECK ( mctx->track(148783403) == 15);
		CHECK ( mctx->track(148783404) > mctx->track_count() );
	}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) > 0, TOC with lenghts",
	"[calculate] [calccontext]" )
{
	using arcstk::details::TOCBuilder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	std::unique_ptr<arcstk::TOC> toc = TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// lengths
		{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075, 18368,
			40152, 14798, 11952, 8463, 18935 }
	);

	auto mctx = arcstk::make_context(std::string(), *toc);


	// NOTE that this TOC and the TOC from the previous test case are
	// completely equivalent, but now we specify the lengths instead of the
	// leadout! The CHECKments are exactly the same.


	SECTION ("pcm_byte_count() and filename()")
	{
		CHECK ( mctx->audio_size().pcm_byte_count() == 595145376 );
		CHECK ( mctx->filename() == std::string() );
	}

	SECTION ("leadout_frame() and sample_count()")
	{
		CHECK ( mctx->audio_size().leadout_frame()  == 253038 );
		CHECK ( mctx->audio_size().sample_count()   == 148786344 );
	}

	SECTION ("track_count(), offset(), length()" )
	{
		CHECK ( mctx->track_count() == 15 );

		CHECK ( mctx->offset(0)  ==     33 );
		CHECK ( mctx->offset(1)  ==   5225 );
		CHECK ( mctx->offset(2)  ==   7390 );
		CHECK ( mctx->offset(3)  ==  23380 );
		CHECK ( mctx->offset(4)  ==  35608 );
		CHECK ( mctx->offset(5)  ==  49820 );
		CHECK ( mctx->offset(6)  ==  69508 );
		CHECK ( mctx->offset(7)  ==  87733 );
		CHECK ( mctx->offset(8)  == 106333 );
		CHECK ( mctx->offset(9)  == 139495 );
		CHECK ( mctx->offset(10) == 157863 );
		CHECK ( mctx->offset(11) == 198495 );
		CHECK ( mctx->offset(12) == 213368 );
		CHECK ( mctx->offset(13) == 225320 );
		CHECK ( mctx->offset(14) == 234103 );
		CHECK ( mctx->offset(15) ==      0 );

		// The lengths parsed from the CUEsheet differ from the lengths
		// computed by CalcContext. The cause is that for CalcContext the length
		// of track i is the difference offset(i+1) - offset(i). This accepts
		// the gaps as part of the track and appends each gap to the end of
		// the track. Libcue on the other hand seems to just ignore the gaps and
		// subtract them from the actual length.

		CHECK ( mctx->length(0)  ==  5192 );
		CHECK ( mctx->length(1)  ==  2165 );
		CHECK ( mctx->length(2)  == 15990 ); // TOC: 15885
		CHECK ( mctx->length(3)  == 12228 );
		CHECK ( mctx->length(4)  == 14212 ); // TOC: 13925
		CHECK ( mctx->length(5)  == 19688 ); // TOC: 19513
		CHECK ( mctx->length(6)  == 18225 ); // TOC: 18155
		CHECK ( mctx->length(7)  == 18600 ); // TOC: 18325
		CHECK ( mctx->length(8)  == 33162 ); // TOC: 33075
		CHECK ( mctx->length(9)  == 18368 );
		CHECK ( mctx->length(10) == 40632 ); // TOC: 40152
		CHECK ( mctx->length(11) == 14873 ); // TOC: 14798
		CHECK ( mctx->length(12) == 11952 );
		CHECK ( mctx->length(13) ==  8783 ); // TOC: 8463
		CHECK ( mctx->length(14) == 18935 ); // TOC: 18935
		CHECK ( mctx->length(15) ==     0 );
	}

	SECTION ("id(), skips_front(), skips_back() and is_multi_track()")
	{
		CHECK ( mctx->id() ==
				arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );
	}

	SECTION ("first_relevant_sample()")
	{
		CHECK ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->first_relevant_sample(1)  ==     22343 ); // skipping
		CHECK ( mctx->first_relevant_sample(2)  ==   3072300 );
		CHECK ( mctx->first_relevant_sample(3)  ==   4345320 );
		CHECK ( mctx->first_relevant_sample(4)  ==  13747440 );
		CHECK ( mctx->first_relevant_sample(5)  ==  20937504 );
		CHECK ( mctx->first_relevant_sample(6)  ==  29294160 );
		CHECK ( mctx->first_relevant_sample(7)  ==  40870704 );
		CHECK ( mctx->first_relevant_sample(8)  ==  51587004 );
		CHECK ( mctx->first_relevant_sample(9)  ==  62523804 );
		CHECK ( mctx->first_relevant_sample(10) ==  82023060 );
		CHECK ( mctx->first_relevant_sample(11) ==  92823444 );
		CHECK ( mctx->first_relevant_sample(12) == 116715060 );
		CHECK ( mctx->first_relevant_sample(13) == 125460384 );
		CHECK ( mctx->first_relevant_sample(14) == 132488160 );
		CHECK ( mctx->first_relevant_sample(15) == 137652564 );
		CHECK ( mctx->first_relevant_sample(16) ==         0 ); // not a track
		CHECK ( mctx->first_relevant_sample(99) ==         0 ); // not a track
	}

	SECTION ("last_relevant_sample()")
	{
		CHECK ( mctx->last_relevant_sample() == 148783403 );

		CHECK ( mctx->last_relevant_sample(0)  ==     19403 ); // not a track

		CHECK ( mctx->last_relevant_sample(1)  ==   3072299 );
		CHECK ( mctx->last_relevant_sample(2)  ==   4345319 );
		CHECK ( mctx->last_relevant_sample(3)  ==  13747439 );
		CHECK ( mctx->last_relevant_sample(4)  ==  20937503 );
		CHECK ( mctx->last_relevant_sample(5)  ==  29294159 );
		CHECK ( mctx->last_relevant_sample(6)  ==  40870703 );
		CHECK ( mctx->last_relevant_sample(7)  ==  51587003 );
		CHECK ( mctx->last_relevant_sample(8)  ==  62523803 );
		CHECK ( mctx->last_relevant_sample(9)  ==  82023059 );
		CHECK ( mctx->last_relevant_sample(10) ==  92823443 );
		CHECK ( mctx->last_relevant_sample(11) == 116715059 );
		CHECK ( mctx->last_relevant_sample(12) == 125460383 );
		CHECK ( mctx->last_relevant_sample(13) == 132488159 );
		CHECK ( mctx->last_relevant_sample(14) == 137652563 );
		CHECK ( mctx->last_relevant_sample(15) == 148783403 ); // skipping

		CHECK ( mctx->last_relevant_sample(16) == 148783403 ); // not a track
		CHECK ( mctx->last_relevant_sample(99) == 148783403 ); // not a track
	}

	SECTION ("track()")
	{
		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
		CHECK ( mctx->track(1) == 0);

		CHECK ( mctx->track(19402) == 0);
		CHECK ( mctx->track(19403) == 0);
		CHECK ( mctx->track(19404) == 1);

		CHECK ( mctx->track(3072298) == 1);
		CHECK ( mctx->track(3072299) == 1);
		CHECK ( mctx->track(3072300) == 2);

		CHECK ( mctx->track(4345318) == 2);
		CHECK ( mctx->track(4345319) == 2);
		CHECK ( mctx->track(4345320) == 3);

		CHECK ( mctx->track(13747438) == 3);
		CHECK ( mctx->track(13747439) == 3);
		CHECK ( mctx->track(13747440) == 4);

		CHECK ( mctx->track(20937502) == 4);
		CHECK ( mctx->track(20937503) == 4);
		CHECK ( mctx->track(20937504) == 5);

		CHECK ( mctx->track(29294158) == 5);
		CHECK ( mctx->track(29294159) == 5);
		CHECK ( mctx->track(29294160) == 6);

		CHECK ( mctx->track(40870702) == 6);
		CHECK ( mctx->track(40870703) == 6);
		CHECK ( mctx->track(40870704) == 7);

		CHECK ( mctx->track(51587002) == 7);
		CHECK ( mctx->track(51587003) == 7);
		CHECK ( mctx->track(51587004) == 8);

		CHECK ( mctx->track(62523802) == 8);
		CHECK ( mctx->track(62523803) == 8);
		CHECK ( mctx->track(62523804) == 9);

		CHECK ( mctx->track(82023058) == 9);
		CHECK ( mctx->track(82023059) == 9);
		CHECK ( mctx->track(82023060) == 10);

		CHECK ( mctx->track(92823442) == 10);
		CHECK ( mctx->track(92823443) == 10);
		CHECK ( mctx->track(92823444) == 11);

		CHECK ( mctx->track(116715058) == 11);
		CHECK ( mctx->track(116715059) == 11);
		CHECK ( mctx->track(116715060) == 12);

		CHECK ( mctx->track(125460382) == 12);
		CHECK ( mctx->track(125460383) == 12);
		CHECK ( mctx->track(125460384) == 13);

		CHECK ( mctx->track(132488158) == 13);
		CHECK ( mctx->track(132488159) == 13);
		CHECK ( mctx->track(132488160) == 14);

		CHECK ( mctx->track(137652562) == 14);
		CHECK ( mctx->track(137652563) == 14);
		CHECK ( mctx->track(137652564) == 15);

		CHECK ( mctx->track(148783403) == 15);
		CHECK ( mctx->track(148783404) > mctx->track_count() );
	}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) == 0, TOC with leadout",
		"[calculate] [calccontext]" )
{
	using arcstk::details::TOCBuilder;

	// Bent: Programmed to Love

	std::unique_ptr<arcstk::TOC> toc = TOCBuilder::build(
		// track count
		18,
		// offsets
		{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
			174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
		// leadout
		332075
	);


	CHECK ( toc->track_count() == 18 );

	CHECK ( toc->offset( 1) ==      0 );
	CHECK ( toc->offset( 2) ==  29042 );
	CHECK ( toc->offset( 3) ==  53880 );
	CHECK ( toc->offset( 4) ==  58227 );
	CHECK ( toc->offset( 5) ==  84420 );
	CHECK ( toc->offset( 6) ==  94192 );
	CHECK ( toc->offset( 7) == 119165 );
	CHECK ( toc->offset( 8) == 123030 );
	CHECK ( toc->offset( 9) == 147500 );
	CHECK ( toc->offset(10) == 148267 );
	CHECK ( toc->offset(11) == 174602 );
	CHECK ( toc->offset(12) == 208125 );
	CHECK ( toc->offset(13) == 212705 );
	CHECK ( toc->offset(14) == 239890 );
	CHECK ( toc->offset(15) == 268705 );
	CHECK ( toc->offset(16) == 272055 );
	CHECK ( toc->offset(17) == 291720 );
	CHECK ( toc->offset(18) == 319992 );

	auto mctx = arcstk::make_context(std::string(), *toc);


	SECTION ("pcm_byte_count() and filename()")
	{
		// TODO
	}

	SECTION ("leadout_frame() and sample_count()")
	{
		// TODO
	}

	SECTION ("track_count(), offset(), length()" )
	{
		CHECK ( mctx->track_count() == 18 );

		// TODO
	}

	SECTION ("id(), skips_front(), skips_back() and is_multi_track()")
	{
		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );
	}

	SECTION ("first_relevant_sample()")
	{
		CHECK ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->first_relevant_sample(1)  ==      2939 ); // skipping!
		CHECK ( mctx->first_relevant_sample(2)  ==  17076696 );
		CHECK ( mctx->first_relevant_sample(3)  ==  31681440 );
		CHECK ( mctx->first_relevant_sample(4)  ==  34237476 );
		CHECK ( mctx->first_relevant_sample(5)  ==  49638960 );
		CHECK ( mctx->first_relevant_sample(6)  ==  55384896 );
		CHECK ( mctx->first_relevant_sample(7)  ==  70069020 );
		CHECK ( mctx->first_relevant_sample(8)  ==  72341640 );
		CHECK ( mctx->first_relevant_sample(9)  ==  86730000 );
		CHECK ( mctx->first_relevant_sample(10) ==  87180996 );
		CHECK ( mctx->first_relevant_sample(11) == 102665976 );
		CHECK ( mctx->first_relevant_sample(12) == 122377500 );
		CHECK ( mctx->first_relevant_sample(13) == 125070540 );
		CHECK ( mctx->first_relevant_sample(14) == 141055320 );
		CHECK ( mctx->first_relevant_sample(15) == 157998540 );
		CHECK ( mctx->first_relevant_sample(16) == 159968340 );
		CHECK ( mctx->first_relevant_sample(17) == 171531360 );
		CHECK ( mctx->first_relevant_sample(18) == 188155296 );

		CHECK ( mctx->first_relevant_sample(19) == 0 ); // not a track
	}

	SECTION ("last_relevant_sample()")
	{
		CHECK ( mctx->last_relevant_sample() == 195257159 );

		CHECK ( mctx->last_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->last_relevant_sample(1)  ==  17076695 );
		CHECK ( mctx->last_relevant_sample(2)  ==  31681439 );
		CHECK ( mctx->last_relevant_sample(3)  ==  34237475 );
		CHECK ( mctx->last_relevant_sample(4)  ==  49638959 );
		CHECK ( mctx->last_relevant_sample(5)  ==  55384895 );
		CHECK ( mctx->last_relevant_sample(6)  ==  70069019 );
		CHECK ( mctx->last_relevant_sample(7)  ==  72341639 );
		CHECK ( mctx->last_relevant_sample(8)  ==  86729999 );
		CHECK ( mctx->last_relevant_sample(9)  ==  87180995 );
		CHECK ( mctx->last_relevant_sample(10) == 102665975 );
		CHECK ( mctx->last_relevant_sample(11) == 122377499 );
		CHECK ( mctx->last_relevant_sample(12) == 125070539 );
		CHECK ( mctx->last_relevant_sample(13) == 141055319 );
		CHECK ( mctx->last_relevant_sample(14) == 157998539 );
		CHECK ( mctx->last_relevant_sample(15) == 159968339 );
		CHECK ( mctx->last_relevant_sample(16) == 171531359 );
		CHECK ( mctx->last_relevant_sample(17) == 188155295 );
		CHECK ( mctx->last_relevant_sample(18) == 195257159 ); // skipping

		CHECK ( mctx->last_relevant_sample(19) == 195257159 ); // not a track
	}


	// TODO
	//SECTION ("track()")
	//{
	//}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) == 0, TOC with lenghts",
		"[calculate] [calccontext]" )
{
	using arcstk::details::TOCBuilder;

	// Bent: Programmed to Love

	std::unique_ptr<arcstk::TOC> toc = TOCBuilder::build(
		// track count
		18,
		// offsets
		{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
			174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
		// lengths
		{ 29042, 24673, 4347, 26035, 9772, 24973, 3865, 24325, 767, 26335,
			33523, 4580, 27185, 28737, 3350, 19665, 28272, -1}
	);

	// TODO
}


TEST_CASE ( "MultitrackCalcContext copying", "[calculate] [calccontext]" )
{
	using arcstk::details::TOCBuilder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	std::unique_ptr<arcstk::TOC> toc = TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	auto mctx = arcstk::make_context(std::string(), *toc);

	CHECK ( mctx->audio_size().pcm_byte_count() == 595145376 );
	CHECK ( mctx->filename() == std::string() );

	CHECK ( mctx->track_count() == 15 );

	CHECK ( mctx->offset(0)  ==     33 );
	CHECK ( mctx->offset(1)  ==   5225 );
	CHECK ( mctx->offset(2)  ==   7390 );
	CHECK ( mctx->offset(3)  ==  23380 );
	CHECK ( mctx->offset(4)  ==  35608 );
	CHECK ( mctx->offset(5)  ==  49820 );
	CHECK ( mctx->offset(6)  ==  69508 );
	CHECK ( mctx->offset(7)  ==  87733 );
	CHECK ( mctx->offset(8)  == 106333 );
	CHECK ( mctx->offset(9)  == 139495 );
	CHECK ( mctx->offset(10) == 157863 );
	CHECK ( mctx->offset(11) == 198495 );
	CHECK ( mctx->offset(12) == 213368 );
	CHECK ( mctx->offset(13) == 225320 );
	CHECK ( mctx->offset(14) == 234103 );
	CHECK ( mctx->offset(15) ==      0 );

	// The lengths parsed from the CUEsheet differ from the lengths
	// computed by CalcContext. The cause is that for CalcContext the length
	// of track i is the difference offset(i+1) - offset(i). This accepts
	// the gaps as part of the track and appends each gap to the end of
	// the track. Libcue on the other hand seems to just ignore the gaps and
	// subtract them from the actual length.

	CHECK ( mctx->length(0)  ==  5192 );
	CHECK ( mctx->length(1)  ==  2165 );
	CHECK ( mctx->length(2)  == 15990 ); // TOC: 15885
	CHECK ( mctx->length(3)  == 12228 );
	CHECK ( mctx->length(4)  == 14212 ); // TOC: 13925
	CHECK ( mctx->length(5)  == 19688 ); // TOC: 19513
	CHECK ( mctx->length(6)  == 18225 ); // TOC: 18155
	CHECK ( mctx->length(7)  == 18600 ); // TOC: 18325
	CHECK ( mctx->length(8)  == 33162 ); // TOC: 33075
	CHECK ( mctx->length(9)  == 18368 );
	CHECK ( mctx->length(10) == 40632 ); // TOC: 40152
	CHECK ( mctx->length(11) == 14873 ); // TOC: 14798
	CHECK ( mctx->length(12) == 11952 );
	CHECK ( mctx->length(13) ==  8783 ); // TOC: 8463
	CHECK ( mctx->length(14) == 18935 ); // TOC: 18935
	CHECK ( mctx->length(15) ==     0 );


	SECTION ( "clone()" )
	{
		auto ctx_copy = mctx->clone();

		// TODO Implement and test equality/inequality before this
		//CHECK ( ctx_copy == ctx0 );
		//CHECK ( not(ctx_copy != ctx0 ) );

		CHECK ( ctx_copy->audio_size().leadout_frame()  == 253038 );
		CHECK ( ctx_copy->audio_size().sample_count()   == 148786344 );
		CHECK ( ctx_copy->audio_size().pcm_byte_count() == 595145376 );

		CHECK ( ctx_copy->track_count() == 15 );

		CHECK ( ctx_copy->offset(0)  ==     33 );
		CHECK ( ctx_copy->offset(1)  ==   5225 );
		CHECK ( ctx_copy->offset(2)  ==   7390 );
		CHECK ( ctx_copy->offset(3)  ==  23380 );
		CHECK ( ctx_copy->offset(4)  ==  35608 );
		CHECK ( ctx_copy->offset(5)  ==  49820 );
		CHECK ( ctx_copy->offset(6)  ==  69508 );
		CHECK ( ctx_copy->offset(7)  ==  87733 );
		CHECK ( ctx_copy->offset(8)  == 106333 );
		CHECK ( ctx_copy->offset(9)  == 139495 );
		CHECK ( ctx_copy->offset(10) == 157863 );
		CHECK ( ctx_copy->offset(11) == 198495 );
		CHECK ( ctx_copy->offset(12) == 213368 );
		CHECK ( ctx_copy->offset(13) == 225320 );
		CHECK ( ctx_copy->offset(14) == 234103 );
		CHECK ( ctx_copy->offset(15) ==      0 );

		CHECK ( ctx_copy->length(0)  ==  5192 );
		CHECK ( ctx_copy->length(1)  ==  2165 );
		CHECK ( ctx_copy->length(2)  == 15990 );
		CHECK ( ctx_copy->length(3)  == 12228 );
		CHECK ( ctx_copy->length(4)  == 14212 );
		CHECK ( ctx_copy->length(5)  == 19688 );
		CHECK ( ctx_copy->length(6)  == 18225 );
		CHECK ( ctx_copy->length(7)  == 18600 );
		CHECK ( ctx_copy->length(8)  == 33162 );
		CHECK ( ctx_copy->length(9)  == 18368 );
		CHECK ( ctx_copy->length(10) == 40632 );
		CHECK ( ctx_copy->length(11) == 14873 );
		CHECK ( ctx_copy->length(12) == 11952 );
		CHECK ( ctx_copy->length(13) ==  8783 );
		CHECK ( ctx_copy->length(14) == 18935 );
		CHECK ( ctx_copy->length(15) ==     0 );

		CHECK ( ctx_copy->filename() == std::string() );
		CHECK ( ctx_copy->id() ==
				arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		CHECK ( ctx_copy->skips_front() );
		CHECK ( ctx_copy->skips_back() );
		CHECK ( ctx_copy->is_multi_track() );


		CHECK ( ctx_copy->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( ctx_copy->first_relevant_sample(1)  ==     22343 ); // skipping
		CHECK ( ctx_copy->first_relevant_sample(2)  ==   3072300 );
		CHECK ( ctx_copy->first_relevant_sample(3)  ==   4345320 );
		CHECK ( ctx_copy->first_relevant_sample(4)  ==  13747440 );
		CHECK ( ctx_copy->first_relevant_sample(5)  ==  20937504 );
		CHECK ( ctx_copy->first_relevant_sample(6)  ==  29294160 );
		CHECK ( ctx_copy->first_relevant_sample(7)  ==  40870704 );
		CHECK ( ctx_copy->first_relevant_sample(8)  ==  51587004 );
		CHECK ( ctx_copy->first_relevant_sample(9)  ==  62523804 );
		CHECK ( ctx_copy->first_relevant_sample(10) ==  82023060 );
		CHECK ( ctx_copy->first_relevant_sample(11) ==  92823444 );
		CHECK ( ctx_copy->first_relevant_sample(12) == 116715060 );
		CHECK ( ctx_copy->first_relevant_sample(13) == 125460384 );
		CHECK ( ctx_copy->first_relevant_sample(14) == 132488160 );
		CHECK ( ctx_copy->first_relevant_sample(15) == 137652564 );
		CHECK ( ctx_copy->first_relevant_sample(16) ==         0 ); // not a track
		CHECK ( ctx_copy->first_relevant_sample(99) ==         0 ); // not a track

		CHECK ( ctx_copy->last_relevant_sample()    == 148783403 );

		CHECK ( ctx_copy->last_relevant_sample(0)   ==     19403 ); // not a track

		CHECK ( ctx_copy->last_relevant_sample(1)   ==   3072299 );
		CHECK ( ctx_copy->last_relevant_sample(2)   ==   4345319 );
		CHECK ( ctx_copy->last_relevant_sample(3)   ==  13747439 );
		CHECK ( ctx_copy->last_relevant_sample(4)   ==  20937503 );
		CHECK ( ctx_copy->last_relevant_sample(5)   ==  29294159 );
		CHECK ( ctx_copy->last_relevant_sample(6)   ==  40870703 );
		CHECK ( ctx_copy->last_relevant_sample(7)   ==  51587003 );
		CHECK ( ctx_copy->last_relevant_sample(8)   ==  62523803 );
		CHECK ( ctx_copy->last_relevant_sample(9)   ==  82023059 );
		CHECK ( ctx_copy->last_relevant_sample(10)  ==  92823443 );
		CHECK ( ctx_copy->last_relevant_sample(11)  == 116715059 );
		CHECK ( ctx_copy->last_relevant_sample(12)  == 125460383 );
		CHECK ( ctx_copy->last_relevant_sample(13)  == 132488159 );
		CHECK ( ctx_copy->last_relevant_sample(14)  == 137652563 );
		CHECK ( ctx_copy->last_relevant_sample(15)  == 148783403 ); // skipping

		CHECK ( ctx_copy->last_relevant_sample(16)  == 148783403 ); // not a track
		CHECK ( ctx_copy->last_relevant_sample(99)  == 148783403 ); // not a track

		CHECK ( ctx_copy->track(0) == 0);
		CHECK ( ctx_copy->track(1) == 0);

		CHECK ( ctx_copy->track(19402) == 0);
		CHECK ( ctx_copy->track(19403) == 0);
		CHECK ( ctx_copy->track(19404) == 1);

		CHECK ( ctx_copy->track(3072298) == 1);
		CHECK ( ctx_copy->track(3072299) == 1);
		CHECK ( ctx_copy->track(3072300) == 2);

		CHECK ( ctx_copy->track(4345318) == 2);
		CHECK ( ctx_copy->track(4345319) == 2);
		CHECK ( ctx_copy->track(4345320) == 3);

		CHECK ( ctx_copy->track(13747438) == 3);
		CHECK ( ctx_copy->track(13747439) == 3);
		CHECK ( ctx_copy->track(13747440) == 4);

		CHECK ( ctx_copy->track(20937502) == 4);
		CHECK ( ctx_copy->track(20937503) == 4);
		CHECK ( ctx_copy->track(20937504) == 5);

		CHECK ( ctx_copy->track(29294158) == 5);
		CHECK ( ctx_copy->track(29294159) == 5);
		CHECK ( ctx_copy->track(29294160) == 6);

		CHECK ( ctx_copy->track(40870702) == 6);
		CHECK ( ctx_copy->track(40870703) == 6);
		CHECK ( ctx_copy->track(40870704) == 7);

		CHECK ( ctx_copy->track(51587002) == 7);
		CHECK ( ctx_copy->track(51587003) == 7);
		CHECK ( ctx_copy->track(51587004) == 8);

		CHECK ( ctx_copy->track(62523802) == 8);
		CHECK ( ctx_copy->track(62523803) == 8);
		CHECK ( ctx_copy->track(62523804) == 9);

		CHECK ( ctx_copy->track(82023058) == 9);
		CHECK ( ctx_copy->track(82023059) == 9);
		CHECK ( ctx_copy->track(82023060) == 10);

		CHECK ( ctx_copy->track(92823442) == 10);
		CHECK ( ctx_copy->track(92823443) == 10);
		CHECK ( ctx_copy->track(92823444) == 11);

		CHECK ( ctx_copy->track(116715058) == 11);
		CHECK ( ctx_copy->track(116715059) == 11);
		CHECK ( ctx_copy->track(116715060) == 12);

		CHECK ( ctx_copy->track(125460382) == 12);
		CHECK ( ctx_copy->track(125460383) == 12);
		CHECK ( ctx_copy->track(125460384) == 13);

		CHECK ( ctx_copy->track(132488158) == 13);
		CHECK ( ctx_copy->track(132488159) == 13);
		CHECK ( ctx_copy->track(132488160) == 14);

		CHECK ( ctx_copy->track(137652562) == 14);
		CHECK ( ctx_copy->track(137652563) == 14);
		CHECK ( ctx_copy->track(137652564) == 15);

		CHECK ( ctx_copy->track(148783403) == 15);
		CHECK ( ctx_copy->track(148783404) > ctx_copy->track_count() );
	}

}


// Interval


TEST_CASE ( "Interval" "[calculate] [interval]" )
{
	arcstk::v_1_0_0::details::Interval i(10, 20);

	CHECK ( i.contains(10) );
	CHECK ( i.contains(11) );
	CHECK ( i.contains(19) );
	CHECK ( i.contains(20) );
}

