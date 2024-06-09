#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for classes in module calculate2
 */

#include <fstream>                // for ifstream, operator|, ios_base::failure

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#include "calculate_impl.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif


// AudioSize


TEST_CASE ( "AudioSize", "[calculate] [audiosize]" )
{
	using arcstk::AudioSize;
	using UNIT = arcstk::AudioSize::UNIT;

	AudioSize empty_size{};

	AudioSize size1;
	size1.set_total_frames(253038);

	AudioSize size2; // equals size1
	size2.set_total_frames(253038);

	AudioSize size3(253038,    UNIT::FRAMES);  // equal to size1 and size2
	AudioSize size4(148786344, UNIT::SAMPLES); // equal to size1 and size2
	AudioSize size5(595145376, UNIT::BYTES);   // equal to size1 and size2

	AudioSize different_size; // not equal to size1-5

	REQUIRE ( different_size.zero() );

	different_size.set_total_frames(14827);


	SECTION ("Construction is correct")
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


// CalcContext implementations


TEST_CASE ( "SingletrackCalcContext", "[calculate] [calccontext]" )
{
	using arcstk::make_context;
	using arcstk::make_empty_arid;

	const auto empty_default_arid = make_empty_arid();

	SECTION ( "Construction without parameters is correct" )
	{
		auto sctx { make_context(false, false) };

		CHECK ( sctx->id() == *empty_default_arid );

		CHECK ( sctx->filename() == std::string() );

		CHECK ( sctx->total_tracks() == 1 );

		CHECK ( sctx->offset(0) == 0 );
		CHECK ( sctx->offset(1) == 0 );
		CHECK ( sctx->offset(99) == 0 );

		CHECK ( sctx->length(0) == 0 );
		CHECK ( sctx->length(1) == 0 );
		CHECK ( sctx->length(99) == 0 );

		CHECK ( sctx->audio_size().leadout_frame()  == 0 );
		CHECK ( sctx->audio_size().total_samples()  == 0 );
		CHECK ( sctx->audio_size().total_pcm_bytes() == 0 );

		CHECK ( not sctx->is_multi_track() );
		CHECK ( not sctx->skips_front() );
		CHECK ( not sctx->skips_back() );

		CHECK ( sctx->first_relevant_sample(0)   == 0 );
		CHECK ( sctx->first_relevant_sample(1)   == 0 );
		CHECK ( sctx->first_relevant_sample(99)  == 0 );
		CHECK ( sctx->first_relevant_sample(255) == 0 );

		CHECK ( sctx->last_relevant_sample()    < 0 );

		CHECK ( sctx->last_relevant_sample(0)   < 0 );
		CHECK ( sctx->last_relevant_sample(1)   < 0 );
		CHECK ( sctx->last_relevant_sample(99)  < 0 );
		CHECK ( sctx->last_relevant_sample(255) < 0 );

		CHECK ( sctx->track(0) == 1 );
		CHECK ( sctx->track(std::numeric_limits<int32_t>::max()) == 1);
	}

	// TODO Construction with parameters? (bool, bool, audiofilename)
}


TEST_CASE ( "MultitrackCalcContext", "[calculate] [calccontext] [multitrack]" )
{
	using arcstk::details::TOCBuilder;
	using arcstk::make_context;


	SECTION ("Complete TOC with leadout, offset(1) > 0")
	{
		// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

		auto toc = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		CHECK_THROWS ( toc->offset( 0)    );
		CHECK ( toc->offset( 1) ==     33 );
		CHECK ( toc->offset( 2) ==   5225 );
		CHECK ( toc->offset( 3) ==   7390 );
		CHECK ( toc->offset( 4) ==  23380 );
		CHECK ( toc->offset( 5) ==  35608 );
		CHECK ( toc->offset( 6) ==  49820 );
		CHECK ( toc->offset( 7) ==  69508 );
		CHECK ( toc->offset( 8) ==  87733 );
		CHECK ( toc->offset( 9) == 106333 );
		CHECK ( toc->offset(10) == 139495 );
		CHECK ( toc->offset(11) == 157863 );
		CHECK ( toc->offset(12) == 198495 );
		CHECK ( toc->offset(13) == 213368 );
		CHECK ( toc->offset(14) == 225320 );
		CHECK ( toc->offset(15) == 234103 );
		CHECK_THROWS ( toc->offset(16)    );

		CHECK_THROWS ( toc->parsed_length( 0));
		CHECK ( toc->parsed_length( 1) ==  0 );
		CHECK ( toc->parsed_length(15) ==  0 );
		CHECK_THROWS ( toc->parsed_length(16));

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(15) == "" );
		CHECK_THROWS ( toc->filename(16)       );

		CHECK ( toc->leadout() == 253038 );
		CHECK ( toc->total_tracks() == 15 );
		CHECK ( toc->complete() );

		auto mctx = make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		CHECK ( mctx->id() == arid );

		CHECK ( audiosize.leadout_frame()  ==    253038 );
		CHECK ( audiosize.total_samples()  == 148786344 );
		CHECK ( audiosize.total_pcm_bytes() == 595145376 );

		CHECK ( mctx->total_tracks() == 15 );

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
		CHECK ( mctx->offset(15) ==      0 ); // not a track

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
		CHECK ( mctx->length(15) ==     0 ); // not a track

		CHECK ( mctx->id() ==
				arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

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
		CHECK ( mctx->track(148783404) > mctx->total_tracks() );
	}


	SECTION ("Complete TOC with lengths, offset(1) > 0")
	{
		// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

		auto toc = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075, 18368,
				40152, 14798, 11952, 8463, 18935 }
		);

		CHECK_THROWS ( toc->offset( 0)    );
		CHECK ( toc->offset( 1) ==     33 );
		CHECK ( toc->offset( 2) ==   5225 );
		CHECK ( toc->offset( 3) ==   7390 );
		CHECK ( toc->offset( 4) ==  23380 );
		CHECK ( toc->offset( 5) ==  35608 );
		CHECK ( toc->offset( 6) ==  49820 );
		CHECK ( toc->offset( 7) ==  69508 );
		CHECK ( toc->offset( 8) ==  87733 );
		CHECK ( toc->offset( 9) == 106333 );
		CHECK ( toc->offset(10) == 139495 );
		CHECK ( toc->offset(11) == 157863 );
		CHECK ( toc->offset(12) == 198495 );
		CHECK ( toc->offset(13) == 213368 );
		CHECK ( toc->offset(14) == 225320 );
		CHECK ( toc->offset(15) == 234103 );
		CHECK_THROWS ( toc->offset(16)    );

		CHECK_THROWS ( toc->parsed_length( 0)    );
		CHECK ( toc->parsed_length( 1) ==   5192 );
		CHECK ( toc->parsed_length( 2) ==   2165 );
		CHECK ( toc->parsed_length( 3) ==  15885 );
		CHECK ( toc->parsed_length( 4) ==  12228 );
		CHECK ( toc->parsed_length( 5) ==  13925 );
		CHECK ( toc->parsed_length( 6) ==  19513 );
		CHECK ( toc->parsed_length( 7) ==  18155 );
		CHECK ( toc->parsed_length( 8) ==  18325 );
		CHECK ( toc->parsed_length( 9) ==  33075 );
		CHECK ( toc->parsed_length(10) ==  18368 );
		CHECK ( toc->parsed_length(11) ==  40152 );
		CHECK ( toc->parsed_length(12) ==  14798 );
		CHECK ( toc->parsed_length(13) ==  11952 );
		CHECK ( toc->parsed_length(14) ==   8463 );
		CHECK ( toc->parsed_length(15) ==  18935 );
		CHECK_THROWS ( toc->parsed_length(16)    );

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(15) == "" );
		CHECK_THROWS ( toc->filename(16)       );

		CHECK ( toc->leadout() == 253038 );
		CHECK ( toc->total_tracks() == 15 );
		CHECK ( toc->complete() );

		auto mctx = make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		CHECK ( mctx->id() == arid );

		CHECK ( audiosize.leadout_frame()  ==    253038 );
		CHECK ( audiosize.total_samples()  == 148786344 );
		CHECK ( audiosize.total_pcm_bytes() == 595145376 );

		CHECK ( mctx->total_tracks() == 15 );

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
		CHECK ( mctx->offset(15) ==      0 ); // not a track

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

		CHECK ( mctx->last_relevant_sample()   == 148783403 );

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
		CHECK ( mctx->track(148783404) > mctx->total_tracks() );
	}


	SECTION ("Complete TOC with leadout, offset(1) == 0")
	{
		// Bent: Programmed to Love

		auto toc = TOCBuilder::build(
			// track count
			18,
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
				174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
			// leadout
			332075
		);

		CHECK_THROWS ( toc->offset( 0)    );
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
		CHECK_THROWS ( toc->offset(19)    );

		CHECK_THROWS ( toc->parsed_length( 0)      );
		CHECK        ( toc->parsed_length( 1) == 0 );
		CHECK        ( toc->parsed_length(18) == 0 );
		CHECK_THROWS ( toc->parsed_length(19)      );

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(18) == "" );
		CHECK_THROWS ( toc->filename(19)       );

		CHECK ( toc->leadout() == 332075 );
		CHECK ( toc->total_tracks() == 18 );
		CHECK ( toc->complete() );


		auto mctx = make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(18, 0x00307c78, 0x0281351d, 0x27114b12);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		CHECK ( mctx->id() == arid );

		CHECK ( audiosize.leadout_frame()  ==    332075 );
		CHECK ( audiosize.total_samples()  == 195260100 );
		CHECK ( audiosize.total_pcm_bytes() == 781040400 );

		CHECK ( mctx->total_tracks() == 18 );

		CHECK ( mctx->offset( 0) ==      0 );
		CHECK ( mctx->offset( 1) ==  29042 );
		CHECK ( mctx->offset( 2) ==  53880 );
		CHECK ( mctx->offset( 3) ==  58227 );
		CHECK ( mctx->offset( 4) ==  84420 );
		CHECK ( mctx->offset( 5) ==  94192 );
		CHECK ( mctx->offset( 6) == 119165 );
		CHECK ( mctx->offset( 7) == 123030 );
		CHECK ( mctx->offset( 8) == 147500 );
		CHECK ( mctx->offset( 9) == 148267 );
		CHECK ( mctx->offset(10) == 174602 );
		CHECK ( mctx->offset(11) == 208125 );
		CHECK ( mctx->offset(12) == 212705 );
		CHECK ( mctx->offset(13) == 239890 );
		CHECK ( mctx->offset(14) == 268705 );
		CHECK ( mctx->offset(15) == 272055 );
		CHECK ( mctx->offset(16) == 291720 );
		CHECK ( mctx->offset(17) == 319992 );

		CHECK ( mctx->length( 0) == 29042 );
		CHECK ( mctx->length( 1) == 24838 );
		CHECK ( mctx->length( 2) ==  4347 );
		CHECK ( mctx->length( 3) == 26193 );
		CHECK ( mctx->length( 4) ==  9772 );
		CHECK ( mctx->length( 5) == 24973 );
		CHECK ( mctx->length( 6) ==  3865 );
		CHECK ( mctx->length( 7) == 24470 );
		CHECK ( mctx->length( 8) ==   767 );
		CHECK ( mctx->length( 9) == 26335 );
		CHECK ( mctx->length(10) == 33523 );
		CHECK ( mctx->length(11) ==  4580 );
		CHECK ( mctx->length(12) == 27185 );
		CHECK ( mctx->length(13) == 28815 );
		CHECK ( mctx->length(14) ==  3350 );
		CHECK ( mctx->length(15) == 19665 );
		CHECK ( mctx->length(16) == 28272 );
		CHECK ( mctx->length(17) == 12083 );

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

		CHECK ( mctx->last_relevant_sample()   == 195257159 );

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

		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
		CHECK ( mctx->track(1) == 1);
		CHECK ( mctx->track(2) == 1);

		CHECK ( mctx->track(17076694) == 1);
		CHECK ( mctx->track(17076695) == 1);
		CHECK ( mctx->track(17076696) == 2);

		CHECK ( mctx->track(31681438) == 2);
		CHECK ( mctx->track(31681439) == 2);
		CHECK ( mctx->track(31681440) == 3);

		CHECK ( mctx->track(34237474) == 3);
		CHECK ( mctx->track(34237475) == 3);
		CHECK ( mctx->track(34237476) == 4);

		CHECK ( mctx->track(49638958) == 4);
		CHECK ( mctx->track(49638959) == 4);
		CHECK ( mctx->track(49638960) == 5);

		CHECK ( mctx->track(55384894) == 5);
		CHECK ( mctx->track(55384895) == 5);
		CHECK ( mctx->track(55384896) == 6);

		CHECK ( mctx->track(70069018) == 6);
		CHECK ( mctx->track(70069019) == 6);
		CHECK ( mctx->track(70069020) == 7);

		CHECK ( mctx->track(72341638) == 7);
		CHECK ( mctx->track(72341639) == 7);
		CHECK ( mctx->track(72341640) == 8);

		CHECK ( mctx->track(86729998) == 8);
		CHECK ( mctx->track(86729999) == 8);
		CHECK ( mctx->track(86730000) == 9);

		CHECK ( mctx->track(87180994) == 9);
		CHECK ( mctx->track(87180995) == 9);
		CHECK ( mctx->track(87180996) == 10);

		CHECK ( mctx->track(102665974) == 10);
		CHECK ( mctx->track(102665975) == 10);
		CHECK ( mctx->track(102665976) == 11);

		CHECK ( mctx->track(122377498) == 11);
		CHECK ( mctx->track(122377499) == 11);
		CHECK ( mctx->track(122377500) == 12);

		CHECK ( mctx->track(125070538) == 12);
		CHECK ( mctx->track(125070539) == 12);
		CHECK ( mctx->track(125070540) == 13);

		CHECK ( mctx->track(141055318) == 13);
		CHECK ( mctx->track(141055319) == 13);
		CHECK ( mctx->track(141055320) == 14);

		CHECK ( mctx->track(157998538) == 14);
		CHECK ( mctx->track(157998539) == 14);
		CHECK ( mctx->track(157998540) == 15);

		CHECK ( mctx->track(159968338) == 15);
		CHECK ( mctx->track(159968339) == 15);
		CHECK ( mctx->track(159968340) == 16);

		CHECK ( mctx->track(171531358) == 16);
		CHECK ( mctx->track(171531359) == 16);
		CHECK ( mctx->track(171531360) == 17);

		CHECK ( mctx->track(188155294) == 17);
		CHECK ( mctx->track(188155295) == 17);
		CHECK ( mctx->track(188155296) == 18);

		CHECK ( mctx->track(195257158) == 18);
		CHECK ( mctx->track(195257159) == 18);
		CHECK ( mctx->track(195257160) > mctx->total_tracks());
	}


	SECTION ("Complete TOC with lengths, offset(1) == 0")
	{
		// Bent: Programmed to Love

		auto toc = TOCBuilder::build(
			// track count
			18,
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
				174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
			// lengths
			{ 29042, 24673, 4347, 26035, 9772, 24973, 3865, 24325, 767, 26335,
				33523, 4580, 27185, 28737, 3350, 19665, 28272, -1}
		);

		CHECK ( toc->total_tracks() == 18 );

		CHECK_THROWS ( toc->offset( 0)    );
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
		CHECK_THROWS ( toc->offset(19)    );

		CHECK_THROWS ( toc->parsed_length( 0)   );
		CHECK ( toc->parsed_length( 1) == 29042 );
		CHECK ( toc->parsed_length( 2) == 24673 );
		CHECK ( toc->parsed_length( 3) ==  4347 );
		CHECK ( toc->parsed_length( 4) == 26035 );
		CHECK ( toc->parsed_length( 5) ==  9772 );
		CHECK ( toc->parsed_length( 6) == 24973 );
		CHECK ( toc->parsed_length( 7) ==  3865 );
		CHECK ( toc->parsed_length( 8) == 24325 );
		CHECK ( toc->parsed_length( 9) ==   767 );
		CHECK ( toc->parsed_length(10) == 26335 );
		CHECK ( toc->parsed_length(11) == 33523 );
		CHECK ( toc->parsed_length(12) ==  4580 );
		CHECK ( toc->parsed_length(13) == 27185 );
		CHECK ( toc->parsed_length(14) == 28737 );
		CHECK ( toc->parsed_length(15) ==  3350 );
		CHECK ( toc->parsed_length(16) == 19665 );
		CHECK ( toc->parsed_length(17) == 28272 );
		CHECK ( toc->parsed_length(18) ==     0 ); //normalized
		CHECK_THROWS ( toc->parsed_length(19)   );

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(18) == "" );
		CHECK_THROWS ( toc->filename(19)       );

		CHECK ( toc->total_tracks() == 18 );
		CHECK ( not toc->complete() );
		CHECK ( toc->leadout() == 0 ); // unknown due to last length unknown

		auto mctx = make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(18, 0x00307c78, 0x0281351d, 0x27114b12);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		//CHECK ( mctx->id() == arid );  // undefined

		CHECK ( audiosize.leadout_frame()  == 0 );
		CHECK ( audiosize.total_samples()  == 0 );
		CHECK ( audiosize.total_pcm_bytes() == 0 );

		CHECK ( mctx->total_tracks() == 18 );

		CHECK ( mctx->offset( 0) ==      0 );
		CHECK ( mctx->offset( 1) ==  29042 );
		CHECK ( mctx->offset( 2) ==  53880 );
		CHECK ( mctx->offset( 3) ==  58227 );
		CHECK ( mctx->offset( 4) ==  84420 );
		CHECK ( mctx->offset( 5) ==  94192 );
		CHECK ( mctx->offset( 6) == 119165 );
		CHECK ( mctx->offset( 7) == 123030 );
		CHECK ( mctx->offset( 8) == 147500 );
		CHECK ( mctx->offset( 9) == 148267 );
		CHECK ( mctx->offset(10) == 174602 );
		CHECK ( mctx->offset(11) == 208125 );
		CHECK ( mctx->offset(12) == 212705 );
		CHECK ( mctx->offset(13) == 239890 );
		CHECK ( mctx->offset(14) == 268705 );
		CHECK ( mctx->offset(15) == 272055 );
		CHECK ( mctx->offset(16) == 291720 );
		CHECK ( mctx->offset(17) == 319992 );

		CHECK ( mctx->length( 0) == 29042 );
		CHECK ( mctx->length( 1) == 24838 );
		CHECK ( mctx->length( 2) ==  4347 );
		CHECK ( mctx->length( 3) == 26193 );
		CHECK ( mctx->length( 4) ==  9772 );
		CHECK ( mctx->length( 5) == 24973 );
		CHECK ( mctx->length( 6) ==  3865 );
		CHECK ( mctx->length( 7) == 24470 );
		CHECK ( mctx->length( 8) ==   767 );
		CHECK ( mctx->length( 9) == 26335 );
		CHECK ( mctx->length(10) == 33523 );
		CHECK ( mctx->length(11) ==  4580 );
		CHECK ( mctx->length(12) == 27185 );
		CHECK ( mctx->length(13) == 28815 );
		CHECK ( mctx->length(14) ==  3350 );
		CHECK ( mctx->length(15) == 19665 );
		CHECK ( mctx->length(16) == 28272 );
		CHECK ( mctx->length(17) ==     0 );

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

		//CHECK ( mctx->last_relevant_sample()   == ); // undefined

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
		//CHECK ( mctx->last_relevant_sample(18) ==         0 ); // undefined

		//CHECK ( mctx->last_relevant_sample(19) == 195257159 ); // undefined

		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
// FIXME Would be better to yield those values instead of 0
//		CHECK ( mctx->track(1) == 1);
//		CHECK ( mctx->track(2) == 1);
//
//		CHECK ( mctx->track(17076694) == 1);
//		CHECK ( mctx->track(17076695) == 1);
//		CHECK ( mctx->track(17076696) == 2);
//
//		CHECK ( mctx->track(31681438) == 2);
//		CHECK ( mctx->track(31681439) == 2);
//		CHECK ( mctx->track(31681440) == 3);
//
//		CHECK ( mctx->track(34237474) == 3);
//		CHECK ( mctx->track(34237475) == 3);
//		CHECK ( mctx->track(34237476) == 4);
//
//		CHECK ( mctx->track(49638958) == 4);
//		CHECK ( mctx->track(49638959) == 4);
//		CHECK ( mctx->track(49638960) == 5);
//
//		CHECK ( mctx->track(55384894) == 5);
//		CHECK ( mctx->track(55384895) == 5);
//		CHECK ( mctx->track(55384896) == 6);
//
//		CHECK ( mctx->track(70069018) == 6);
//		CHECK ( mctx->track(70069019) == 6);
//		CHECK ( mctx->track(70069020) == 7);
//
//		CHECK ( mctx->track(72341638) == 7);
//		CHECK ( mctx->track(72341639) == 7);
//		CHECK ( mctx->track(72341640) == 8);
//
//		CHECK ( mctx->track(86729998) == 8);
//		CHECK ( mctx->track(86729999) == 8);
//		CHECK ( mctx->track(86730000) == 9);
//
//		CHECK ( mctx->track(87180994) == 9);
//		CHECK ( mctx->track(87180995) == 9);
//		CHECK ( mctx->track(87180996) == 10);
//
//		CHECK ( mctx->track(102665974) == 10);
//		CHECK ( mctx->track(102665975) == 10);
//		CHECK ( mctx->track(102665976) == 11);
//
//		CHECK ( mctx->track(122377498) == 11);
//		CHECK ( mctx->track(122377499) == 11);
//		CHECK ( mctx->track(122377500) == 12);
//
//		CHECK ( mctx->track(125070538) == 12);
//		CHECK ( mctx->track(125070539) == 12);
//		CHECK ( mctx->track(125070540) == 13);
//
//		CHECK ( mctx->track(141055318) == 13);
//		CHECK ( mctx->track(141055319) == 13);
//		CHECK ( mctx->track(141055320) == 14);
//
//		CHECK ( mctx->track(157998538) == 14);
//		CHECK ( mctx->track(157998539) == 14);
//		CHECK ( mctx->track(157998540) == 15);
//
//		CHECK ( mctx->track(159968338) == 15);
//		CHECK ( mctx->track(159968339) == 15);
//		CHECK ( mctx->track(159968340) == 16);
//
//		CHECK ( mctx->track(171531358) == 16);
//		CHECK ( mctx->track(171531359) == 16);
//		CHECK ( mctx->track(171531360) == 17);
//
//		CHECK ( mctx->track(188155294) == 17);
//		CHECK ( mctx->track(188155295) == 17);
//		CHECK ( mctx->track(188155296) == 18);
//
//		CHECK ( mctx->track(195257158) == 18);
//		CHECK ( mctx->track(195257159) == 18);
//		CHECK ( mctx->track(195257160) > mctx->total_tracks());
	}
}


TEST_CASE ("MultitrackCalcContext::clone()",
		"[calculate] [calccontext] [multitrack]" )
{
	using arcstk::details::TOCBuilder;
	using arcstk::make_context;

	SECTION ( "clone()" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

		auto toc = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		auto mctx = make_context(toc);

		CHECK ( mctx->audio_size().total_pcm_bytes() == 595145376 );
		CHECK ( mctx->filename() == std::string() );

		CHECK ( mctx->total_tracks() == 15 );

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


		auto ctx_copy = mctx->clone();

		// TODO Implement and test equality/inequality before this
		//CHECK ( ctx_copy == ctx0 );
		//CHECK ( not(ctx_copy != ctx0 ) );

		CHECK ( ctx_copy->audio_size().leadout_frame()  == 253038 );
		CHECK ( ctx_copy->audio_size().total_samples()  == 148786344 );
		CHECK ( ctx_copy->audio_size().total_pcm_bytes() == 595145376 );

		CHECK ( ctx_copy->total_tracks() == 15 );

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
		CHECK ( ctx_copy->track(148783404) > ctx_copy->total_tracks() );
	}
}


TEST_CASE ("MultitrackCalcContext Equality",
		"[calculate] [calccontext] [multitrack]" )
{
	using arcstk::details::MultitrackCalcContext;
	using arcstk::details::TOCBuilder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	auto mctx1 = MultitrackCalcContext(TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	));

	// same as mctx 1
	auto mctx2 = MultitrackCalcContext(TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	));

	// different from mctx 1 and 2
	auto mctx3 = MultitrackCalcContext(TOCBuilder::build(
		// track count
		16,
		// offsets
		{ 0, 2500, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
		139495, 157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	));

	CHECK ( mctx1 == mctx1 );
	CHECK ( mctx2 == mctx2 );
	CHECK ( mctx3 == mctx3 );

	CHECK ( mctx1 == mctx2 );
	CHECK ( mctx1 != mctx3 );
	CHECK ( mctx2 != mctx3 );
}

