#include "catch2/catch.hpp"

#include <cstdint>
#include <memory>

#ifndef __LIBARCS_CALCULATE_HPP__
#include "calculate.hpp"
#endif


/**
 * \file calculate_calccontext.cpp Fixtures for class \ref CalcContext
 */


TEST_CASE ( "SingleCalcContext construction without parameters",
		"[calculate] [calccontext]" )
{
	// Construct a CalcContext without any parameters

	auto sctx = arcs::make_context("", false, false);


	//SECTION ( "Construction without parameters is correct" )
	//{
		arcs::details::ARIdBuilder id_builder;
		auto empty_default_arid = id_builder.build_empty_id();

		REQUIRE ( sctx->id() == *empty_default_arid );

		REQUIRE ( sctx->filename() == std::string() );

		REQUIRE ( sctx->track_count() == 1 );

		REQUIRE ( sctx->offset(0) == 0 );
		REQUIRE ( sctx->offset(1) == 0 );
		REQUIRE ( sctx->offset(99) == 0 );

		REQUIRE ( sctx->length(0) == 0 );
		REQUIRE ( sctx->length(1) == 0 );
		REQUIRE ( sctx->length(99) == 0 );

		REQUIRE ( sctx->audio_size().leadout_frame()  == 0 );
		REQUIRE ( sctx->audio_size().sample_count()   == 0 );
		REQUIRE ( sctx->audio_size().pcm_byte_count() == 0 );

		REQUIRE ( not sctx->is_multi_track() );
		REQUIRE ( not sctx->skips_front() );
		REQUIRE ( not sctx->skips_back() );

		REQUIRE ( sctx->first_relevant_sample(0)   == 0 );
		REQUIRE ( sctx->first_relevant_sample(1)   == 0 );
		REQUIRE ( sctx->first_relevant_sample(99)  == 0 );
		REQUIRE ( sctx->first_relevant_sample(255) == 0 );

		REQUIRE ( sctx->last_relevant_sample()    == static_cast<uint32_t>(-1) );

		REQUIRE ( sctx->last_relevant_sample(0)   == static_cast<uint32_t>(-1) );
		REQUIRE ( sctx->last_relevant_sample(1)   == static_cast<uint32_t>(-1) );
		REQUIRE ( sctx->last_relevant_sample(99)  == static_cast<uint32_t>(-1) );
		REQUIRE ( sctx->last_relevant_sample(255) == static_cast<uint32_t>(-1) );

		REQUIRE ( sctx->track(0) == 1 );
		REQUIRE ( sctx->track(static_cast<uint32_t>(-1)) == 1);
	//}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) > 0, TOC with leadout",
	"[calculate] [calccontext]" )
{
	arcs::details::TOCBuilder toc_builder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	std::unique_ptr<arcs::TOC> toc = toc_builder.build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	auto mctx = arcs::make_context("", *toc);


	SECTION ("pcm_byte_count() and filename()")
	{
		REQUIRE ( mctx->audio_size().pcm_byte_count() == 595145376 );
		REQUIRE ( mctx->filename() == std::string() );
	}

	SECTION ("leadout_frame() and sample_count()")
	{
		REQUIRE ( mctx->audio_size().leadout_frame()  == 253038 );
		REQUIRE ( mctx->audio_size().sample_count()   == 148786344 );
	}

	SECTION ("track_count(), offset(), length()" )
	{
		REQUIRE ( mctx->track_count()    == 15 );

		REQUIRE ( mctx->offset(0)  ==     33 );
		REQUIRE ( mctx->offset(1)  ==   5225 );
		REQUIRE ( mctx->offset(2)  ==   7390 );
		REQUIRE ( mctx->offset(3)  ==  23380 );
		REQUIRE ( mctx->offset(4)  ==  35608 );
		REQUIRE ( mctx->offset(5)  ==  49820 );
		REQUIRE ( mctx->offset(6)  ==  69508 );
		REQUIRE ( mctx->offset(7)  ==  87733 );
		REQUIRE ( mctx->offset(8)  == 106333 );
		REQUIRE ( mctx->offset(9)  == 139495 );
		REQUIRE ( mctx->offset(10) == 157863 );
		REQUIRE ( mctx->offset(11) == 198495 );
		REQUIRE ( mctx->offset(12) == 213368 );
		REQUIRE ( mctx->offset(13) == 225320 );
		REQUIRE ( mctx->offset(14) == 234103 );
		REQUIRE ( mctx->offset(15) ==      0 );

		// The lengths parsed from the CUEsheet differ from the lengths
		// computed by CalcContext. The cause is that for CalcContext the length
		// of track i is the difference offset(i+1) - offset(i). This accepts
		// the gaps as part of the track and appends each gap to the end of
		// the track. Libcue on the other hand seems to just ignore the gaps and
		// subtract them from the actual length.

		REQUIRE ( mctx->length(0)  ==  5192 );
		REQUIRE ( mctx->length(1)  ==  2165 );
		REQUIRE ( mctx->length(2)  == 15990 ); // TOC: 15885
		REQUIRE ( mctx->length(3)  == 12228 );
		REQUIRE ( mctx->length(4)  == 14212 ); // TOC: 13925
		REQUIRE ( mctx->length(5)  == 19688 ); // TOC: 19513
		REQUIRE ( mctx->length(6)  == 18225 ); // TOC: 18155
		REQUIRE ( mctx->length(7)  == 18600 ); // TOC: 18325
		REQUIRE ( mctx->length(8)  == 33162 ); // TOC: 33075
		REQUIRE ( mctx->length(9)  == 18368 );
		REQUIRE ( mctx->length(10) == 40632 ); // TOC: 40152
		REQUIRE ( mctx->length(11) == 14873 ); // TOC: 14798
		REQUIRE ( mctx->length(12) == 11952 );
		REQUIRE ( mctx->length(13) ==  8783 ); // TOC: 8463
		REQUIRE ( mctx->length(14) == 18935 ); // TOC: 18935
		REQUIRE ( mctx->length(15) ==     0 );
	}

	SECTION ("id(), skips_front(), skips_back() and is_multi_track()")
	{
		REQUIRE ( mctx->id() ==
				arcs::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		REQUIRE ( mctx->skips_front() );
		REQUIRE ( mctx->skips_back() );
		REQUIRE ( mctx->is_multi_track() );
	}

	SECTION ("first_relevant_sample()")
	{
		REQUIRE ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		REQUIRE ( mctx->first_relevant_sample(1)  ==     22343 ); // skipping
		REQUIRE ( mctx->first_relevant_sample(2)  ==   3072300 );
		REQUIRE ( mctx->first_relevant_sample(3)  ==   4345320 );
		REQUIRE ( mctx->first_relevant_sample(4)  ==  13747440 );
		REQUIRE ( mctx->first_relevant_sample(5)  ==  20937504 );
		REQUIRE ( mctx->first_relevant_sample(6)  ==  29294160 );
		REQUIRE ( mctx->first_relevant_sample(7)  ==  40870704 );
		REQUIRE ( mctx->first_relevant_sample(8)  ==  51587004 );
		REQUIRE ( mctx->first_relevant_sample(9)  ==  62523804 );
		REQUIRE ( mctx->first_relevant_sample(10) ==  82023060 );
		REQUIRE ( mctx->first_relevant_sample(11) ==  92823444 );
		REQUIRE ( mctx->first_relevant_sample(12) == 116715060 );
		REQUIRE ( mctx->first_relevant_sample(13) == 125460384 );
		REQUIRE ( mctx->first_relevant_sample(14) == 132488160 );
		REQUIRE ( mctx->first_relevant_sample(15) == 137652564 );
		REQUIRE ( mctx->first_relevant_sample(16) ==         0 ); // not a track
		REQUIRE ( mctx->first_relevant_sample(99) ==         0 ); // not a track
	}

	SECTION ("last_relevant_sample()")
	{
		REQUIRE ( mctx->last_relevant_sample() == 148783403 );

		REQUIRE ( mctx->last_relevant_sample(0)  ==     19403 ); // not a track

		REQUIRE ( mctx->last_relevant_sample(1)  ==   3072299 );
		REQUIRE ( mctx->last_relevant_sample(2)  ==   4345319 );
		REQUIRE ( mctx->last_relevant_sample(3)  ==  13747439 );
		REQUIRE ( mctx->last_relevant_sample(4)  ==  20937503 );
		REQUIRE ( mctx->last_relevant_sample(5)  ==  29294159 );
		REQUIRE ( mctx->last_relevant_sample(6)  ==  40870703 );
		REQUIRE ( mctx->last_relevant_sample(7)  ==  51587003 );
		REQUIRE ( mctx->last_relevant_sample(8)  ==  62523803 );
		REQUIRE ( mctx->last_relevant_sample(9)  ==  82023059 );
		REQUIRE ( mctx->last_relevant_sample(10) ==  92823443 );
		REQUIRE ( mctx->last_relevant_sample(11) == 116715059 );
		REQUIRE ( mctx->last_relevant_sample(12) == 125460383 );
		REQUIRE ( mctx->last_relevant_sample(13) == 132488159 );
		REQUIRE ( mctx->last_relevant_sample(14) == 137652563 );
		REQUIRE ( mctx->last_relevant_sample(15) == 148783403 ); // skipping

		REQUIRE ( mctx->last_relevant_sample(16) == 148783403 ); // not a track
		REQUIRE ( mctx->last_relevant_sample(99) == 148783403 ); // not a track
	}

	SECTION ("track()")
	{
		// Test the bounds of each track

		REQUIRE ( mctx->track(0) == 0);
		REQUIRE ( mctx->track(1) == 0);

		REQUIRE ( mctx->track(19402) == 0);
		REQUIRE ( mctx->track(19403) == 0);
		REQUIRE ( mctx->track(19404) == 1);

		REQUIRE ( mctx->track(3072298) == 1);
		REQUIRE ( mctx->track(3072299) == 1);
		REQUIRE ( mctx->track(3072300) == 2);

		REQUIRE ( mctx->track(4345318) == 2);
		REQUIRE ( mctx->track(4345319) == 2);
		REQUIRE ( mctx->track(4345320) == 3);

		REQUIRE ( mctx->track(13747438) == 3);
		REQUIRE ( mctx->track(13747439) == 3);
		REQUIRE ( mctx->track(13747440) == 4);

		REQUIRE ( mctx->track(20937502) == 4);
		REQUIRE ( mctx->track(20937503) == 4);
		REQUIRE ( mctx->track(20937504) == 5);

		REQUIRE ( mctx->track(29294158) == 5);
		REQUIRE ( mctx->track(29294159) == 5);
		REQUIRE ( mctx->track(29294160) == 6);

		REQUIRE ( mctx->track(40870702) == 6);
		REQUIRE ( mctx->track(40870703) == 6);
		REQUIRE ( mctx->track(40870704) == 7);

		REQUIRE ( mctx->track(51587002) == 7);
		REQUIRE ( mctx->track(51587003) == 7);
		REQUIRE ( mctx->track(51587004) == 8);

		REQUIRE ( mctx->track(62523802) == 8);
		REQUIRE ( mctx->track(62523803) == 8);
		REQUIRE ( mctx->track(62523804) == 9);

		REQUIRE ( mctx->track(82023058) == 9);
		REQUIRE ( mctx->track(82023059) == 9);
		REQUIRE ( mctx->track(82023060) == 10);

		REQUIRE ( mctx->track(92823442) == 10);
		REQUIRE ( mctx->track(92823443) == 10);
		REQUIRE ( mctx->track(92823444) == 11);

		REQUIRE ( mctx->track(116715058) == 11);
		REQUIRE ( mctx->track(116715059) == 11);
		REQUIRE ( mctx->track(116715060) == 12);

		REQUIRE ( mctx->track(125460382) == 12);
		REQUIRE ( mctx->track(125460383) == 12);
		REQUIRE ( mctx->track(125460384) == 13);

		REQUIRE ( mctx->track(132488158) == 13);
		REQUIRE ( mctx->track(132488159) == 13);
		REQUIRE ( mctx->track(132488160) == 14);

		REQUIRE ( mctx->track(137652562) == 14);
		REQUIRE ( mctx->track(137652563) == 14);
		REQUIRE ( mctx->track(137652564) == 15);

		REQUIRE ( mctx->track(148783403) == 15);
		REQUIRE ( mctx->track(148783404) > mctx->track_count() );
	}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) > 0, TOC with lenghts",
	"[calculate] [calccontext]" )
{
	arcs::details::TOCBuilder  toc_builder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	std::unique_ptr<arcs::TOC> toc = toc_builder.build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// lengths
		{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075, 18368,
			40152, 14798, 11952, 8463, 18935 },
		std::vector<std::string>()
	);

	auto mctx = arcs::make_context(std::string(), *toc);


	// NOTE that this TOC and the TOC from the previous test case are
	// completely equivalent, but now we specify the lengths instead of the
	// leadout! The REQUIREments are exactly the same.


	SECTION ("pcm_byte_count() and filename()")
	{
		REQUIRE ( mctx->audio_size().pcm_byte_count() == 595145376 );
		REQUIRE ( mctx->filename() == std::string() );
	}

	SECTION ("leadout_frame() and sample_count()")
	{
		REQUIRE ( mctx->audio_size().leadout_frame()  == 253038 );
		REQUIRE ( mctx->audio_size().sample_count()   == 148786344 );
	}

	SECTION ("track_count(), offset(), length()" )
	{
		REQUIRE ( mctx->track_count() == 15 );

		REQUIRE ( mctx->offset(0)  ==     33 );
		REQUIRE ( mctx->offset(1)  ==   5225 );
		REQUIRE ( mctx->offset(2)  ==   7390 );
		REQUIRE ( mctx->offset(3)  ==  23380 );
		REQUIRE ( mctx->offset(4)  ==  35608 );
		REQUIRE ( mctx->offset(5)  ==  49820 );
		REQUIRE ( mctx->offset(6)  ==  69508 );
		REQUIRE ( mctx->offset(7)  ==  87733 );
		REQUIRE ( mctx->offset(8)  == 106333 );
		REQUIRE ( mctx->offset(9)  == 139495 );
		REQUIRE ( mctx->offset(10) == 157863 );
		REQUIRE ( mctx->offset(11) == 198495 );
		REQUIRE ( mctx->offset(12) == 213368 );
		REQUIRE ( mctx->offset(13) == 225320 );
		REQUIRE ( mctx->offset(14) == 234103 );
		REQUIRE ( mctx->offset(15) ==      0 );

		// The lengths parsed from the CUEsheet differ from the lengths
		// computed by CalcContext. The cause is that for CalcContext the length
		// of track i is the difference offset(i+1) - offset(i). This accepts
		// the gaps as part of the track and appends each gap to the end of
		// the track. Libcue on the other hand seems to just ignore the gaps and
		// subtract them from the actual length.

		REQUIRE ( mctx->length(0)  ==  5192 );
		REQUIRE ( mctx->length(1)  ==  2165 );
		REQUIRE ( mctx->length(2)  == 15990 ); // TOC: 15885
		REQUIRE ( mctx->length(3)  == 12228 );
		REQUIRE ( mctx->length(4)  == 14212 ); // TOC: 13925
		REQUIRE ( mctx->length(5)  == 19688 ); // TOC: 19513
		REQUIRE ( mctx->length(6)  == 18225 ); // TOC: 18155
		REQUIRE ( mctx->length(7)  == 18600 ); // TOC: 18325
		REQUIRE ( mctx->length(8)  == 33162 ); // TOC: 33075
		REQUIRE ( mctx->length(9)  == 18368 );
		REQUIRE ( mctx->length(10) == 40632 ); // TOC: 40152
		REQUIRE ( mctx->length(11) == 14873 ); // TOC: 14798
		REQUIRE ( mctx->length(12) == 11952 );
		REQUIRE ( mctx->length(13) ==  8783 ); // TOC: 8463
		REQUIRE ( mctx->length(14) == 18935 ); // TOC: 18935
		REQUIRE ( mctx->length(15) ==     0 );
	}

	SECTION ("id(), skips_front(), skips_back() and is_multi_track()")
	{
		REQUIRE ( mctx->id() ==
				arcs::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		REQUIRE ( mctx->skips_front() );
		REQUIRE ( mctx->skips_back() );
		REQUIRE ( mctx->is_multi_track() );
	}

	SECTION ("first_relevant_sample()")
	{
		REQUIRE ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		REQUIRE ( mctx->first_relevant_sample(1)  ==     22343 ); // skipping
		REQUIRE ( mctx->first_relevant_sample(2)  ==   3072300 );
		REQUIRE ( mctx->first_relevant_sample(3)  ==   4345320 );
		REQUIRE ( mctx->first_relevant_sample(4)  ==  13747440 );
		REQUIRE ( mctx->first_relevant_sample(5)  ==  20937504 );
		REQUIRE ( mctx->first_relevant_sample(6)  ==  29294160 );
		REQUIRE ( mctx->first_relevant_sample(7)  ==  40870704 );
		REQUIRE ( mctx->first_relevant_sample(8)  ==  51587004 );
		REQUIRE ( mctx->first_relevant_sample(9)  ==  62523804 );
		REQUIRE ( mctx->first_relevant_sample(10) ==  82023060 );
		REQUIRE ( mctx->first_relevant_sample(11) ==  92823444 );
		REQUIRE ( mctx->first_relevant_sample(12) == 116715060 );
		REQUIRE ( mctx->first_relevant_sample(13) == 125460384 );
		REQUIRE ( mctx->first_relevant_sample(14) == 132488160 );
		REQUIRE ( mctx->first_relevant_sample(15) == 137652564 );
		REQUIRE ( mctx->first_relevant_sample(16) ==         0 ); // not a track
		REQUIRE ( mctx->first_relevant_sample(99) ==         0 ); // not a track
	}

	SECTION ("last_relevant_sample()")
	{
		REQUIRE ( mctx->last_relevant_sample() == 148783403 );

		REQUIRE ( mctx->last_relevant_sample(0)  ==     19403 ); // not a track

		REQUIRE ( mctx->last_relevant_sample(1)  ==   3072299 );
		REQUIRE ( mctx->last_relevant_sample(2)  ==   4345319 );
		REQUIRE ( mctx->last_relevant_sample(3)  ==  13747439 );
		REQUIRE ( mctx->last_relevant_sample(4)  ==  20937503 );
		REQUIRE ( mctx->last_relevant_sample(5)  ==  29294159 );
		REQUIRE ( mctx->last_relevant_sample(6)  ==  40870703 );
		REQUIRE ( mctx->last_relevant_sample(7)  ==  51587003 );
		REQUIRE ( mctx->last_relevant_sample(8)  ==  62523803 );
		REQUIRE ( mctx->last_relevant_sample(9)  ==  82023059 );
		REQUIRE ( mctx->last_relevant_sample(10) ==  92823443 );
		REQUIRE ( mctx->last_relevant_sample(11) == 116715059 );
		REQUIRE ( mctx->last_relevant_sample(12) == 125460383 );
		REQUIRE ( mctx->last_relevant_sample(13) == 132488159 );
		REQUIRE ( mctx->last_relevant_sample(14) == 137652563 );
		REQUIRE ( mctx->last_relevant_sample(15) == 148783403 ); // skipping

		REQUIRE ( mctx->last_relevant_sample(16) == 148783403 ); // not a track
		REQUIRE ( mctx->last_relevant_sample(99) == 148783403 ); // not a track
	}

	SECTION ("track()")
	{
		// Test the bounds of each track

		REQUIRE ( mctx->track(0) == 0);
		REQUIRE ( mctx->track(1) == 0);

		REQUIRE ( mctx->track(19402) == 0);
		REQUIRE ( mctx->track(19403) == 0);
		REQUIRE ( mctx->track(19404) == 1);

		REQUIRE ( mctx->track(3072298) == 1);
		REQUIRE ( mctx->track(3072299) == 1);
		REQUIRE ( mctx->track(3072300) == 2);

		REQUIRE ( mctx->track(4345318) == 2);
		REQUIRE ( mctx->track(4345319) == 2);
		REQUIRE ( mctx->track(4345320) == 3);

		REQUIRE ( mctx->track(13747438) == 3);
		REQUIRE ( mctx->track(13747439) == 3);
		REQUIRE ( mctx->track(13747440) == 4);

		REQUIRE ( mctx->track(20937502) == 4);
		REQUIRE ( mctx->track(20937503) == 4);
		REQUIRE ( mctx->track(20937504) == 5);

		REQUIRE ( mctx->track(29294158) == 5);
		REQUIRE ( mctx->track(29294159) == 5);
		REQUIRE ( mctx->track(29294160) == 6);

		REQUIRE ( mctx->track(40870702) == 6);
		REQUIRE ( mctx->track(40870703) == 6);
		REQUIRE ( mctx->track(40870704) == 7);

		REQUIRE ( mctx->track(51587002) == 7);
		REQUIRE ( mctx->track(51587003) == 7);
		REQUIRE ( mctx->track(51587004) == 8);

		REQUIRE ( mctx->track(62523802) == 8);
		REQUIRE ( mctx->track(62523803) == 8);
		REQUIRE ( mctx->track(62523804) == 9);

		REQUIRE ( mctx->track(82023058) == 9);
		REQUIRE ( mctx->track(82023059) == 9);
		REQUIRE ( mctx->track(82023060) == 10);

		REQUIRE ( mctx->track(92823442) == 10);
		REQUIRE ( mctx->track(92823443) == 10);
		REQUIRE ( mctx->track(92823444) == 11);

		REQUIRE ( mctx->track(116715058) == 11);
		REQUIRE ( mctx->track(116715059) == 11);
		REQUIRE ( mctx->track(116715060) == 12);

		REQUIRE ( mctx->track(125460382) == 12);
		REQUIRE ( mctx->track(125460383) == 12);
		REQUIRE ( mctx->track(125460384) == 13);

		REQUIRE ( mctx->track(132488158) == 13);
		REQUIRE ( mctx->track(132488159) == 13);
		REQUIRE ( mctx->track(132488160) == 14);

		REQUIRE ( mctx->track(137652562) == 14);
		REQUIRE ( mctx->track(137652563) == 14);
		REQUIRE ( mctx->track(137652564) == 15);

		REQUIRE ( mctx->track(148783403) == 15);
		REQUIRE ( mctx->track(148783404) > mctx->track_count() );
	}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) == 0, TOC with leadout",
		"[calculate] [calccontext]" )
{
	arcs::details::TOCBuilder toc_builder;

	// Bent: Programmed to Love

	std::unique_ptr<arcs::TOC> toc = toc_builder.build(
		// track count
		18,
		// offsets
		{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
			174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
		// leadout
		332075
	);


	REQUIRE ( toc->track_count() == 18 );

	REQUIRE ( toc->offset( 1) ==      0 );
	REQUIRE ( toc->offset( 2) ==  29042 );
	REQUIRE ( toc->offset( 3) ==  53880 );
	REQUIRE ( toc->offset( 4) ==  58227 );
	REQUIRE ( toc->offset( 5) ==  84420 );
	REQUIRE ( toc->offset( 6) ==  94192 );
	REQUIRE ( toc->offset( 7) == 119165 );
	REQUIRE ( toc->offset( 8) == 123030 );
	REQUIRE ( toc->offset( 9) == 147500 );
	REQUIRE ( toc->offset(10) == 148267 );
	REQUIRE ( toc->offset(11) == 174602 );
	REQUIRE ( toc->offset(12) == 208125 );
	REQUIRE ( toc->offset(13) == 212705 );
	REQUIRE ( toc->offset(14) == 239890 );
	REQUIRE ( toc->offset(15) == 268705 );
	REQUIRE ( toc->offset(16) == 272055 );
	REQUIRE ( toc->offset(17) == 291720 );
	REQUIRE ( toc->offset(18) == 319992 );

	auto mctx = arcs::make_context(std::string(), *toc);


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
		REQUIRE ( mctx->track_count() == 18 );

		// TODO
	}

	SECTION ("id(), skips_front(), skips_back() and is_multi_track()")
	{
		REQUIRE ( mctx->skips_front() );
		REQUIRE ( mctx->skips_back() );
		REQUIRE ( mctx->is_multi_track() );
	}

	SECTION ("first_relevant_sample()")
	{
		REQUIRE ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		REQUIRE ( mctx->first_relevant_sample(1)  ==      2939 ); // skipping!
		REQUIRE ( mctx->first_relevant_sample(2)  ==  17076696 );
		REQUIRE ( mctx->first_relevant_sample(3)  ==  31681440 );
		REQUIRE ( mctx->first_relevant_sample(4)  ==  34237476 );
		REQUIRE ( mctx->first_relevant_sample(5)  ==  49638960 );
		REQUIRE ( mctx->first_relevant_sample(6)  ==  55384896 );
		REQUIRE ( mctx->first_relevant_sample(7)  ==  70069020 );
		REQUIRE ( mctx->first_relevant_sample(8)  ==  72341640 );
		REQUIRE ( mctx->first_relevant_sample(9)  ==  86730000 );
		REQUIRE ( mctx->first_relevant_sample(10) ==  87180996 );
		REQUIRE ( mctx->first_relevant_sample(11) == 102665976 );
		REQUIRE ( mctx->first_relevant_sample(12) == 122377500 );
		REQUIRE ( mctx->first_relevant_sample(13) == 125070540 );
		REQUIRE ( mctx->first_relevant_sample(14) == 141055320 );
		REQUIRE ( mctx->first_relevant_sample(15) == 157998540 );
		REQUIRE ( mctx->first_relevant_sample(16) == 159968340 );
		REQUIRE ( mctx->first_relevant_sample(17) == 171531360 );
		REQUIRE ( mctx->first_relevant_sample(18) == 188155296 );

		REQUIRE ( mctx->first_relevant_sample(19) == 0 ); // not a track
	}

	SECTION ("last_relevant_sample()")
	{
		REQUIRE ( mctx->last_relevant_sample() == 195257159 );

		REQUIRE ( mctx->last_relevant_sample(0)  ==         0 ); // not a track

		REQUIRE ( mctx->last_relevant_sample(1)  ==  17076695 );
		REQUIRE ( mctx->last_relevant_sample(2)  ==  31681439 );
		REQUIRE ( mctx->last_relevant_sample(3)  ==  34237475 );
		REQUIRE ( mctx->last_relevant_sample(4)  ==  49638959 );
		REQUIRE ( mctx->last_relevant_sample(5)  ==  55384895 );
		REQUIRE ( mctx->last_relevant_sample(6)  ==  70069019 );
		REQUIRE ( mctx->last_relevant_sample(7)  ==  72341639 );
		REQUIRE ( mctx->last_relevant_sample(8)  ==  86729999 );
		REQUIRE ( mctx->last_relevant_sample(9)  ==  87180995 );
		REQUIRE ( mctx->last_relevant_sample(10) == 102665975 );
		REQUIRE ( mctx->last_relevant_sample(11) == 122377499 );
		REQUIRE ( mctx->last_relevant_sample(12) == 125070539 );
		REQUIRE ( mctx->last_relevant_sample(13) == 141055319 );
		REQUIRE ( mctx->last_relevant_sample(14) == 157998539 );
		REQUIRE ( mctx->last_relevant_sample(15) == 159968339 );
		REQUIRE ( mctx->last_relevant_sample(16) == 171531359 );
		REQUIRE ( mctx->last_relevant_sample(17) == 188155295 );
		REQUIRE ( mctx->last_relevant_sample(18) == 195257159 ); // skipping

		REQUIRE ( mctx->last_relevant_sample(19) == 195257159 ); // not a track
	}


	SECTION ("track()")
	{
		// TODO
	}
}


TEST_CASE ( "MultitrackCalcContext for offset(0) == 0, TOC with lenghts",
		"[calculate] [calccontext]" )
{
	arcs::details::TOCBuilder toc_builder;

	// Bent: Programmed to Love

	std::unique_ptr<arcs::TOC> toc = toc_builder.build(
		// track count
		18,
		// offsets
		{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
			174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
		// lengths
		{ 29042, 24673, 4347, 26035, 9772, 24973, 3865, 24325, 767, 26335,
			33523, 4580, 27185, 28737, 3350, 19665, 28272, -1},
		std::vector<std::string>()
	);

	// TODO
}


TEST_CASE ( "MultitrackCalcContext copying", "[calculate] [calccontext]" )
{
	arcs::details::TOCBuilder toc_builder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	std::unique_ptr<arcs::TOC> toc = toc_builder.build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	auto mctx = arcs::make_context(std::string(), *toc);

	REQUIRE ( mctx->audio_size().pcm_byte_count() == 595145376 );
	REQUIRE ( mctx->filename() == std::string() );

	REQUIRE ( mctx->track_count() == 15 );

	REQUIRE ( mctx->offset(0)  ==     33 );
	REQUIRE ( mctx->offset(1)  ==   5225 );
	REQUIRE ( mctx->offset(2)  ==   7390 );
	REQUIRE ( mctx->offset(3)  ==  23380 );
	REQUIRE ( mctx->offset(4)  ==  35608 );
	REQUIRE ( mctx->offset(5)  ==  49820 );
	REQUIRE ( mctx->offset(6)  ==  69508 );
	REQUIRE ( mctx->offset(7)  ==  87733 );
	REQUIRE ( mctx->offset(8)  == 106333 );
	REQUIRE ( mctx->offset(9)  == 139495 );
	REQUIRE ( mctx->offset(10) == 157863 );
	REQUIRE ( mctx->offset(11) == 198495 );
	REQUIRE ( mctx->offset(12) == 213368 );
	REQUIRE ( mctx->offset(13) == 225320 );
	REQUIRE ( mctx->offset(14) == 234103 );
	REQUIRE ( mctx->offset(15) ==      0 );

	// The lengths parsed from the CUEsheet differ from the lengths
	// computed by CalcContext. The cause is that for CalcContext the length
	// of track i is the difference offset(i+1) - offset(i). This accepts
	// the gaps as part of the track and appends each gap to the end of
	// the track. Libcue on the other hand seems to just ignore the gaps and
	// subtract them from the actual length.

	REQUIRE ( mctx->length(0)  ==  5192 );
	REQUIRE ( mctx->length(1)  ==  2165 );
	REQUIRE ( mctx->length(2)  == 15990 ); // TOC: 15885
	REQUIRE ( mctx->length(3)  == 12228 );
	REQUIRE ( mctx->length(4)  == 14212 ); // TOC: 13925
	REQUIRE ( mctx->length(5)  == 19688 ); // TOC: 19513
	REQUIRE ( mctx->length(6)  == 18225 ); // TOC: 18155
	REQUIRE ( mctx->length(7)  == 18600 ); // TOC: 18325
	REQUIRE ( mctx->length(8)  == 33162 ); // TOC: 33075
	REQUIRE ( mctx->length(9)  == 18368 );
	REQUIRE ( mctx->length(10) == 40632 ); // TOC: 40152
	REQUIRE ( mctx->length(11) == 14873 ); // TOC: 14798
	REQUIRE ( mctx->length(12) == 11952 );
	REQUIRE ( mctx->length(13) ==  8783 ); // TOC: 8463
	REQUIRE ( mctx->length(14) == 18935 ); // TOC: 18935
	REQUIRE ( mctx->length(15) ==     0 );


	SECTION ( "clone()" )
	{
		auto ctx_copy = mctx->clone();

		// TODO Implement and test equality/inequality before this
		//REQUIRE ( ctx_copy == ctx0 );
		//REQUIRE ( not(ctx_copy != ctx0 ) );

		REQUIRE ( ctx_copy->audio_size().leadout_frame()  == 253038 );
		REQUIRE ( ctx_copy->audio_size().sample_count()   == 148786344 );
		REQUIRE ( ctx_copy->audio_size().pcm_byte_count() == 595145376 );

		REQUIRE ( ctx_copy->track_count() == 15 );

		REQUIRE ( ctx_copy->offset(0)  ==     33 );
		REQUIRE ( ctx_copy->offset(1)  ==   5225 );
		REQUIRE ( ctx_copy->offset(2)  ==   7390 );
		REQUIRE ( ctx_copy->offset(3)  ==  23380 );
		REQUIRE ( ctx_copy->offset(4)  ==  35608 );
		REQUIRE ( ctx_copy->offset(5)  ==  49820 );
		REQUIRE ( ctx_copy->offset(6)  ==  69508 );
		REQUIRE ( ctx_copy->offset(7)  ==  87733 );
		REQUIRE ( ctx_copy->offset(8)  == 106333 );
		REQUIRE ( ctx_copy->offset(9)  == 139495 );
		REQUIRE ( ctx_copy->offset(10) == 157863 );
		REQUIRE ( ctx_copy->offset(11) == 198495 );
		REQUIRE ( ctx_copy->offset(12) == 213368 );
		REQUIRE ( ctx_copy->offset(13) == 225320 );
		REQUIRE ( ctx_copy->offset(14) == 234103 );
		REQUIRE ( ctx_copy->offset(15) ==      0 );

		REQUIRE ( ctx_copy->length(0)  ==  5192 );
		REQUIRE ( ctx_copy->length(1)  ==  2165 );
		REQUIRE ( ctx_copy->length(2)  == 15990 );
		REQUIRE ( ctx_copy->length(3)  == 12228 );
		REQUIRE ( ctx_copy->length(4)  == 14212 );
		REQUIRE ( ctx_copy->length(5)  == 19688 );
		REQUIRE ( ctx_copy->length(6)  == 18225 );
		REQUIRE ( ctx_copy->length(7)  == 18600 );
		REQUIRE ( ctx_copy->length(8)  == 33162 );
		REQUIRE ( ctx_copy->length(9)  == 18368 );
		REQUIRE ( ctx_copy->length(10) == 40632 );
		REQUIRE ( ctx_copy->length(11) == 14873 );
		REQUIRE ( ctx_copy->length(12) == 11952 );
		REQUIRE ( ctx_copy->length(13) ==  8783 );
		REQUIRE ( ctx_copy->length(14) == 18935 );
		REQUIRE ( ctx_copy->length(15) ==     0 );

		REQUIRE ( ctx_copy->filename() == std::string() );
		REQUIRE ( ctx_copy->id() ==
				arcs::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		REQUIRE ( ctx_copy->skips_front() );
		REQUIRE ( ctx_copy->skips_back() );
		REQUIRE ( ctx_copy->is_multi_track() );


		REQUIRE ( ctx_copy->first_relevant_sample(0)  ==         0 ); // not a track

		REQUIRE ( ctx_copy->first_relevant_sample(1)  ==     22343 ); // skipping
		REQUIRE ( ctx_copy->first_relevant_sample(2)  ==   3072300 );
		REQUIRE ( ctx_copy->first_relevant_sample(3)  ==   4345320 );
		REQUIRE ( ctx_copy->first_relevant_sample(4)  ==  13747440 );
		REQUIRE ( ctx_copy->first_relevant_sample(5)  ==  20937504 );
		REQUIRE ( ctx_copy->first_relevant_sample(6)  ==  29294160 );
		REQUIRE ( ctx_copy->first_relevant_sample(7)  ==  40870704 );
		REQUIRE ( ctx_copy->first_relevant_sample(8)  ==  51587004 );
		REQUIRE ( ctx_copy->first_relevant_sample(9)  ==  62523804 );
		REQUIRE ( ctx_copy->first_relevant_sample(10) ==  82023060 );
		REQUIRE ( ctx_copy->first_relevant_sample(11) ==  92823444 );
		REQUIRE ( ctx_copy->first_relevant_sample(12) == 116715060 );
		REQUIRE ( ctx_copy->first_relevant_sample(13) == 125460384 );
		REQUIRE ( ctx_copy->first_relevant_sample(14) == 132488160 );
		REQUIRE ( ctx_copy->first_relevant_sample(15) == 137652564 );
		REQUIRE ( ctx_copy->first_relevant_sample(16) ==         0 ); // not a track
		REQUIRE ( ctx_copy->first_relevant_sample(99) ==         0 ); // not a track

		REQUIRE ( ctx_copy->last_relevant_sample()    == 148783403 );

		REQUIRE ( ctx_copy->last_relevant_sample(0)   ==     19403 ); // not a track

		REQUIRE ( ctx_copy->last_relevant_sample(1)   ==   3072299 );
		REQUIRE ( ctx_copy->last_relevant_sample(2)   ==   4345319 );
		REQUIRE ( ctx_copy->last_relevant_sample(3)   ==  13747439 );
		REQUIRE ( ctx_copy->last_relevant_sample(4)   ==  20937503 );
		REQUIRE ( ctx_copy->last_relevant_sample(5)   ==  29294159 );
		REQUIRE ( ctx_copy->last_relevant_sample(6)   ==  40870703 );
		REQUIRE ( ctx_copy->last_relevant_sample(7)   ==  51587003 );
		REQUIRE ( ctx_copy->last_relevant_sample(8)   ==  62523803 );
		REQUIRE ( ctx_copy->last_relevant_sample(9)   ==  82023059 );
		REQUIRE ( ctx_copy->last_relevant_sample(10)  ==  92823443 );
		REQUIRE ( ctx_copy->last_relevant_sample(11)  == 116715059 );
		REQUIRE ( ctx_copy->last_relevant_sample(12)  == 125460383 );
		REQUIRE ( ctx_copy->last_relevant_sample(13)  == 132488159 );
		REQUIRE ( ctx_copy->last_relevant_sample(14)  == 137652563 );
		REQUIRE ( ctx_copy->last_relevant_sample(15)  == 148783403 ); // skipping

		REQUIRE ( ctx_copy->last_relevant_sample(16)  == 148783403 ); // not a track
		REQUIRE ( ctx_copy->last_relevant_sample(99)  == 148783403 ); // not a track

		REQUIRE ( ctx_copy->track(0) == 0);
		REQUIRE ( ctx_copy->track(1) == 0);

		REQUIRE ( ctx_copy->track(19402) == 0);
		REQUIRE ( ctx_copy->track(19403) == 0);
		REQUIRE ( ctx_copy->track(19404) == 1);

		REQUIRE ( ctx_copy->track(3072298) == 1);
		REQUIRE ( ctx_copy->track(3072299) == 1);
		REQUIRE ( ctx_copy->track(3072300) == 2);

		REQUIRE ( ctx_copy->track(4345318) == 2);
		REQUIRE ( ctx_copy->track(4345319) == 2);
		REQUIRE ( ctx_copy->track(4345320) == 3);

		REQUIRE ( ctx_copy->track(13747438) == 3);
		REQUIRE ( ctx_copy->track(13747439) == 3);
		REQUIRE ( ctx_copy->track(13747440) == 4);

		REQUIRE ( ctx_copy->track(20937502) == 4);
		REQUIRE ( ctx_copy->track(20937503) == 4);
		REQUIRE ( ctx_copy->track(20937504) == 5);

		REQUIRE ( ctx_copy->track(29294158) == 5);
		REQUIRE ( ctx_copy->track(29294159) == 5);
		REQUIRE ( ctx_copy->track(29294160) == 6);

		REQUIRE ( ctx_copy->track(40870702) == 6);
		REQUIRE ( ctx_copy->track(40870703) == 6);
		REQUIRE ( ctx_copy->track(40870704) == 7);

		REQUIRE ( ctx_copy->track(51587002) == 7);
		REQUIRE ( ctx_copy->track(51587003) == 7);
		REQUIRE ( ctx_copy->track(51587004) == 8);

		REQUIRE ( ctx_copy->track(62523802) == 8);
		REQUIRE ( ctx_copy->track(62523803) == 8);
		REQUIRE ( ctx_copy->track(62523804) == 9);

		REQUIRE ( ctx_copy->track(82023058) == 9);
		REQUIRE ( ctx_copy->track(82023059) == 9);
		REQUIRE ( ctx_copy->track(82023060) == 10);

		REQUIRE ( ctx_copy->track(92823442) == 10);
		REQUIRE ( ctx_copy->track(92823443) == 10);
		REQUIRE ( ctx_copy->track(92823444) == 11);

		REQUIRE ( ctx_copy->track(116715058) == 11);
		REQUIRE ( ctx_copy->track(116715059) == 11);
		REQUIRE ( ctx_copy->track(116715060) == 12);

		REQUIRE ( ctx_copy->track(125460382) == 12);
		REQUIRE ( ctx_copy->track(125460383) == 12);
		REQUIRE ( ctx_copy->track(125460384) == 13);

		REQUIRE ( ctx_copy->track(132488158) == 13);
		REQUIRE ( ctx_copy->track(132488159) == 13);
		REQUIRE ( ctx_copy->track(132488160) == 14);

		REQUIRE ( ctx_copy->track(137652562) == 14);
		REQUIRE ( ctx_copy->track(137652563) == 14);
		REQUIRE ( ctx_copy->track(137652564) == 15);

		REQUIRE ( ctx_copy->track(148783403) == 15);
		REQUIRE ( ctx_copy->track(148783404) > ctx_copy->track_count() );
	}

}

