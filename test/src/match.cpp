#include "catch2/catch.hpp"

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_MATCH_HPP__
#include "match.hpp"
#endif
#ifndef __LIBARCSTK_PARSE_HPP__
#include "parse.hpp"
#endif


/**
 * \file match.cpp Fixtures for classes in module "match"
 */


TEST_CASE ( "Match", "[match]" )
{
	using arcstk::details::create_match;

	auto m = create_match(2, 10); // 2 block, 10 tracks each, every flag is 0

	REQUIRE ( m->total_blocks() == 2 );
	REQUIRE ( m->tracks_per_block() == 10 );
	REQUIRE ( m->size() == 42 );

	REQUIRE ( not m->id(0) );
	REQUIRE ( not m->track(0, 0, false) );
	REQUIRE ( not m->track(0, 1, false) );
	REQUIRE ( not m->track(0, 2, false) );
	REQUIRE ( not m->track(0, 3, false) );
	REQUIRE ( not m->track(0, 4, false) );
	REQUIRE ( not m->track(0, 5, false) );
	REQUIRE ( not m->track(0, 6, false) );
	REQUIRE ( not m->track(0, 7, false) );
	REQUIRE ( not m->track(0, 8, false) );
	REQUIRE ( not m->track(0, 9, false) );
	REQUIRE ( not m->track(0, 0, true) );
	REQUIRE ( not m->track(0, 1, true) );
	REQUIRE ( not m->track(0, 2, true) );
	REQUIRE ( not m->track(0, 3, true) );
	REQUIRE ( not m->track(0, 4, true) );
	REQUIRE ( not m->track(0, 5, true) );
	REQUIRE ( not m->track(0, 6, true) );
	REQUIRE ( not m->track(0, 7, true) );
	REQUIRE ( not m->track(0, 8, true) );
	REQUIRE ( not m->track(0, 9, true) );

	REQUIRE ( not m->id(1) );
	REQUIRE ( not m->track(1, 0, false) );
	REQUIRE ( not m->track(1, 1, false) );
	REQUIRE ( not m->track(1, 2, false) );
	REQUIRE ( not m->track(1, 3, false) );
	REQUIRE ( not m->track(1, 4, false) );
	REQUIRE ( not m->track(1, 5, false) );
	REQUIRE ( not m->track(1, 6, false) );
	REQUIRE ( not m->track(1, 7, false) );
	REQUIRE ( not m->track(1, 8, false) );
	REQUIRE ( not m->track(1, 9, false) );
	REQUIRE ( not m->track(1, 0, true) );
	REQUIRE ( not m->track(1, 1, true) );
	REQUIRE ( not m->track(1, 2, true) );
	REQUIRE ( not m->track(1, 3, true) );
	REQUIRE ( not m->track(1, 4, true) );
	REQUIRE ( not m->track(1, 5, true) );
	REQUIRE ( not m->track(1, 6, true) );
	REQUIRE ( not m->track(1, 7, true) );
	REQUIRE ( not m->track(1, 8, true) );
	REQUIRE ( not m->track(1, 9, true) );


	SECTION ( "Setting id and track flags has intended effects" )
	{
		m->verify_id(0);
		m->verify_track(0, 2, false);
		m->verify_track(0, 3, false);
		m->verify_track(0, 9, false);
		m->verify_track(0, 5, true);
		m->verify_track(0, 7, true);
		m->verify_id(1);
		m->verify_track(1, 1, false);
		m->verify_track(1, 4, false);
		m->verify_track(1, 6, false);
		m->verify_track(1, 2, true);
		m->verify_track(1, 9, true);

		CHECK ( m->id(0) );
		CHECK ( not m->track(0, 0, false) );
		CHECK ( not m->track(0, 1, false) );
		CHECK (     m->track(0, 2, false) );
		CHECK (     m->track(0, 3, false) );
		CHECK ( not m->track(0, 4, false) );
		CHECK ( not m->track(0, 5, false) );
		CHECK ( not m->track(0, 6, false) );
		CHECK ( not m->track(0, 7, false) );
		CHECK ( not m->track(0, 8, false) );
		CHECK (     m->track(0, 9, false) );
		CHECK ( not m->track(0, 0, true) );
		CHECK ( not m->track(0, 1, true) );
		CHECK ( not m->track(0, 2, true) );
		CHECK ( not m->track(0, 3, true) );
		CHECK ( not m->track(0, 4, true) );
		CHECK (     m->track(0, 5, true) );
		CHECK ( not m->track(0, 6, true) );
		CHECK (     m->track(0, 7, true) );
		CHECK ( not m->track(0, 8, true) );
		CHECK ( not m->track(0, 9, true) );

		CHECK ( m->id(1) );
		CHECK ( not m->track(1, 0, false) );
		CHECK (     m->track(1, 1, false) );
		CHECK ( not m->track(1, 2, false) );
		CHECK ( not m->track(1, 3, false) );
		CHECK (     m->track(1, 4, false) );
		CHECK ( not m->track(1, 5, false) );
		CHECK (     m->track(1, 6, false) );
		CHECK ( not m->track(1, 7, false) );
		CHECK ( not m->track(1, 8, false) );
		CHECK ( not m->track(1, 9, false) );
		CHECK ( not m->track(1, 0, true) );
		CHECK ( not m->track(1, 1, true) );
		CHECK (     m->track(1, 2, true) );
		CHECK ( not m->track(1, 3, true) );
		CHECK ( not m->track(1, 4, true) );
		CHECK ( not m->track(1, 5, true) );
		CHECK ( not m->track(1, 6, true) );
		CHECK ( not m->track(1, 7, true) );
		CHECK ( not m->track(1, 8, true) );
		CHECK (     m->track(1, 9, true) );
	}

	SECTION ( "Moved Match can be manipulated as intended" )
	{
		auto m_moved = std::move(m);

		m_moved->verify_id(0);
		m_moved->verify_track(0, 2, false);
		m_moved->verify_track(0, 3, false);
		m_moved->verify_track(0, 9, false);
		m_moved->verify_track(0, 5, true);
		m_moved->verify_track(0, 7, true);
		m_moved->verify_id(1);
		m_moved->verify_track(1, 1, false);
		m_moved->verify_track(1, 4, false);
		m_moved->verify_track(1, 6, false);
		m_moved->verify_track(1, 2, true);
		m_moved->verify_track(1, 9, true);

		CHECK ( m_moved->id(0) );
		CHECK ( not m_moved->track(0, 0, false) );
		CHECK ( not m_moved->track(0, 1, false) );
		CHECK (     m_moved->track(0, 2, false) );
		CHECK (     m_moved->track(0, 3, false) );
		CHECK ( not m_moved->track(0, 4, false) );
		CHECK ( not m_moved->track(0, 5, false) );
		CHECK ( not m_moved->track(0, 6, false) );
		CHECK ( not m_moved->track(0, 7, false) );
		CHECK ( not m_moved->track(0, 8, false) );
		CHECK (     m_moved->track(0, 9, false) );
		CHECK ( not m_moved->track(0, 0, true) );
		CHECK ( not m_moved->track(0, 1, true) );
		CHECK ( not m_moved->track(0, 2, true) );
		CHECK ( not m_moved->track(0, 3, true) );
		CHECK ( not m_moved->track(0, 4, true) );
		CHECK (     m_moved->track(0, 5, true) );
		CHECK ( not m_moved->track(0, 6, true) );
		CHECK (     m_moved->track(0, 7, true) );
		CHECK ( not m_moved->track(0, 8, true) );
		CHECK ( not m_moved->track(0, 9, true) );

		CHECK ( m_moved->id(1) );
		CHECK ( not m_moved->track(1, 0, false) );
		CHECK (     m_moved->track(1, 1, false) );
		CHECK ( not m_moved->track(1, 2, false) );
		CHECK ( not m_moved->track(1, 3, false) );
		CHECK (     m_moved->track(1, 4, false) );
		CHECK ( not m_moved->track(1, 5, false) );
		CHECK (     m_moved->track(1, 6, false) );
		CHECK ( not m_moved->track(1, 7, false) );
		CHECK ( not m_moved->track(1, 8, false) );
		CHECK ( not m_moved->track(1, 9, false) );
		CHECK ( not m_moved->track(1, 0, true) );
		CHECK ( not m_moved->track(1, 1, true) );
		CHECK (     m_moved->track(1, 2, true) );
		CHECK ( not m_moved->track(1, 3, true) );
		CHECK ( not m_moved->track(1, 4, true) );
		CHECK ( not m_moved->track(1, 5, true) );
		CHECK ( not m_moved->track(1, 6, true) );
		CHECK ( not m_moved->track(1, 7, true) );
		CHECK ( not m_moved->track(1, 8, true) );
		CHECK (     m_moved->track(1, 9, true) );
	}
}


TEST_CASE ( "Matcher", "[match] [matcher]" )
{
	using arcstk::AlbumMatcher;
	using arcstk::ARBlock;
	using arcstk::ARId;
	using arcstk::ARResponse;
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::Checksums;
	using arcstk::ChecksumSet;
	using arcstk::Match;
	using arcstk::TracksetMatcher;
	using arcstk::ListMatcher;

	// Construct the checksums by hand

	// From: "Bach: Organ Concertos", Simon Preston, DGG
	// URL:       http://www.accuraterip.com/accuraterip/8/7/1/dBAR-015-001b9178-014be24e-b40d2d0f.bin
	// Filename:  dBAR-015-001b9178-014be24e-b40d2d0f.bin

	ChecksumSet track01( 5192);
	track01.insert(type::ARCS2, Checksum(0xB89992E5));
	track01.insert(type::ARCS1, Checksum(0x98B10E0F));

	ChecksumSet track02( 2165);
	track02.insert(type::ARCS2, Checksum(0x4F77EB03));
	track02.insert(type::ARCS1, Checksum(0x475F57E9));

	ChecksumSet track03(15885);
	track03.insert(type::ARCS2, Checksum(0x56582282));
	track03.insert(type::ARCS1, Checksum(0x7304F1C4));

	ChecksumSet track04(12228);
	track04.insert(type::ARCS2, Checksum(0x9E2187F9));
	track04.insert(type::ARCS1, Checksum(0xF2472287));

	ChecksumSet track05(13925);
	track05.insert(type::ARCS2, Checksum(0x6BE71E50));
	track05.insert(type::ARCS1, Checksum(0x881BC504));

	ChecksumSet track06(19513);
	track06.insert(type::ARCS2, Checksum(0x01E7235F));
	track06.insert(type::ARCS1, Checksum(0xBB94BFD4));

	ChecksumSet track07(18155);
	track07.insert(type::ARCS2, Checksum(0xD8F7763C));
	track07.insert(type::ARCS1, Checksum(0xF9CAEE76));

	ChecksumSet track08(18325);
	track08.insert(type::ARCS2, Checksum(0x8480223E));
	track08.insert(type::ARCS1, Checksum(0xF9F60BC1));

	ChecksumSet track09(33075);
	track09.insert(type::ARCS2, Checksum(0x42C5061C));
	track09.insert(type::ARCS1, Checksum(0x2C736302));

	ChecksumSet track10(18368);
	track10.insert(type::ARCS2, Checksum(0x47A70F02));
	track10.insert(type::ARCS1, Checksum(0x1C955978));

	ChecksumSet track11(40152);
	track11.insert(type::ARCS2, Checksum(0xBABF08CC));
	track11.insert(type::ARCS1, Checksum(0xFDA6D833));

	ChecksumSet track12(14798);
	track12.insert(type::ARCS2, Checksum(0x563EDCCB));
	track12.insert(type::ARCS1, Checksum(0x3A57E5D1));

	ChecksumSet track13(11952);
	track13.insert(type::ARCS2, Checksum(0xAB123C7C));
	track13.insert(type::ARCS1, Checksum(0x6ED5F3E7));

	ChecksumSet track14( 8463);
	track14.insert(type::ARCS2, Checksum(0xC65C20E4));
	track14.insert(type::ARCS1, Checksum(0x4A5C3872));

	ChecksumSet track15(18935);
	track15.insert(type::ARCS2, Checksum(0x58FC3C3E));
	track15.insert(type::ARCS1, Checksum(0x5FE8B032));

	Checksums result1 {
		track01,
		track02,
		track03,
		track04,
		track05,
		track06,
		track07,
		track08,
		track09,
		track10,
		track11,
		track12,
		track13,
		track14,
		track15
	};

	REQUIRE ( result1.size() == 15 );


	// Construct ARResponse by hand

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F, 24, 0 },
			{ 0x475F57E9, 24, 0 },
			{ 0x7304F1C4, 24, 0 },
			{ 0xF2472287, 24, 0 },
			{ 0x881BC504, 24, 0 },
			{ 0xBB94BFD4, 24, 0 },
			{ 0xF9CAEE76, 24, 0 },
			{ 0xF9F60BC1, 24, 0 },
			{ 0x2C736302, 24, 0 },
			{ 0x1C955978, 24, 0 },
			{ 0xFDA6D833, 24, 0 },
			{ 0x3A57E5D1, 24, 0 },
			{ 0x6ED5F3E7, 24, 0 },
			{ 0x4A5C3872, 24, 0 },
			{ 0x5FE8B032, 24, 0 }
		}
	);

	// Define block: v2 values, but different id
	ARBlock block1( /* id */ { 1523, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5, 24, 0 },
			{ 0x4F77EB03, 24, 0 },
			{ 0x56582282, 24, 0 },
			{ 0x9E2187F9, 24, 0 },
			{ 0x6BE71E50, 24, 0 },
			{ 0x01E7235F, 24, 0 },
			{ 0xD8F7763C, 24, 0 },
			{ 0x8480223E, 24, 0 },
			{ 0x42C5061C, 24, 0 },
			{ 0x47A70F02, 24, 0 },
			{ 0xBABF08CC, 24, 0 },
			{ 0x563EDCCB, 24, 0 },
			{ 0xAB123C7C, 24, 0 },
			{ 0xC65C20E4, 24, 0 },
			{ 0x58FC3C3E, 24, 0 }
		}
	);

	// Define block: v2 values same as block 2, id from block 1
	ARBlock block2( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5, 24, 0 },
			{ 0x4F77EB03, 24, 0 },
			{ 0x56582282, 24, 0 },
			{ 0x9E2187F9, 24, 0 },
			{ 0x6BE71E50, 24, 0 },
			{ 0x01E7235F, 24, 0 },
			{ 0xD8F7763C, 24, 0 },
			{ 0x8480223E, 24, 0 },
			{ 0x42C5061C, 24, 0 },
			{ 0x47A70F02, 24, 0 },
			{ 0xBABF08CC, 24, 0 },
			{ 0x563EDCCB, 24, 0 },
			{ 0xAB123C7C, 24, 0 },
			{ 0xC65C20E4, 24, 0 },
			{ 0x58FC3C3E, 24, 0 }
		}
	);

	// TODO Check content of the block instances

	ARResponse response { block0, block1, block2 };

	// TODO Check content of the ARResponse


	REQUIRE ( response.size() == 3 );
	REQUIRE ( response[0] == block0 );
	REQUIRE ( response[1] == block1 );
	REQUIRE ( response[2] == block2 );


	// common constants

	const bool v1 = false;
	const bool v2 = true;


	AlbumMatcher am_diff(result1, id, response);


	SECTION ( "AlbumMatcher Copy construct" )
	{
		AlbumMatcher am_diff_copy(am_diff);
	}


	SECTION ( "AlbumMatcher finds best match" )
	{
		CHECK ( am_diff.matches() );

		CHECK ( am_diff.best_match() == 2 );
		CHECK ( am_diff.best_difference() == 0 );
		CHECK ( am_diff.matches_v2() );
	}


	SECTION ( "AlbumMatcher's Match loads as declared" )
	{
		const Match* match = am_diff.match();

		CHECK ( match->tracks_per_block() == 15 );

		CHECK ( match->size() == 93 ); // 2 * blocks * tracks + blocks

		// 0
		CHECK ( match->id(0) );

		CHECK ( match->track(0,  0, v1) );
		CHECK ( match->track(0,  1, v1) );
		CHECK ( match->track(0,  2, v1) );
		CHECK ( match->track(0,  3, v1) );
		CHECK ( match->track(0,  4, v1) );
		CHECK ( match->track(0,  5, v1) );
		CHECK ( match->track(0,  6, v1) );
		CHECK ( match->track(0,  7, v1) );
		CHECK ( match->track(0,  8, v1) );
		CHECK ( match->track(0,  9, v1) );
		CHECK ( match->track(0, 10, v1) );
		CHECK ( match->track(0, 11, v1) );
		CHECK ( match->track(0, 12, v1) );
		CHECK ( match->track(0, 13, v1) );
		CHECK ( match->track(0, 14, v1) );

		CHECK ( not match->track(0,  0, v2) );
		CHECK ( not match->track(0,  1, v2) );
		CHECK ( not match->track(0,  2, v2) );
		CHECK ( not match->track(0,  3, v2) );
		CHECK ( not match->track(0,  4, v2) );
		CHECK ( not match->track(0,  5, v2) );
		CHECK ( not match->track(0,  6, v2) );
		CHECK ( not match->track(0,  7, v2) );
		CHECK ( not match->track(0,  8, v2) );
		CHECK ( not match->track(0,  9, v2) );
		CHECK ( not match->track(0, 10, v2) );
		CHECK ( not match->track(0, 11, v2) );
		CHECK ( not match->track(0, 12, v2) );
		CHECK ( not match->track(0, 13, v2) );
		CHECK ( not match->track(0, 14, v2) );

		// 1
		CHECK ( not match->id(1) );

		CHECK ( not match->track(1,  0, v1) );
		CHECK ( not match->track(1,  1, v1) );
		CHECK ( not match->track(1,  2, v1) );
		CHECK ( not match->track(1,  3, v1) );
		CHECK ( not match->track(1,  4, v1) );
		CHECK ( not match->track(1,  5, v1) );
		CHECK ( not match->track(1,  6, v1) );
		CHECK ( not match->track(1,  7, v1) );
		CHECK ( not match->track(1,  8, v1) );
		CHECK ( not match->track(1,  9, v1) );
		CHECK ( not match->track(1, 10, v1) );
		CHECK ( not match->track(1, 11, v1) );
		CHECK ( not match->track(1, 12, v1) );
		CHECK ( not match->track(1, 13, v1) );
		CHECK ( not match->track(1, 14, v1) );

		CHECK ( match->track(1,  0, v2) );
		CHECK ( match->track(1,  1, v2) );
		CHECK ( match->track(1,  2, v2) );
		CHECK ( match->track(1,  3, v2) );
		CHECK ( match->track(1,  4, v2) );
		CHECK ( match->track(1,  5, v2) );
		CHECK ( match->track(1,  6, v2) );
		CHECK ( match->track(1,  7, v2) );
		CHECK ( match->track(1,  8, v2) );
		CHECK ( match->track(1,  9, v2) );
		CHECK ( match->track(1, 10, v2) );
		CHECK ( match->track(1, 11, v2) );
		CHECK ( match->track(1, 12, v2) );
		CHECK ( match->track(1, 13, v2) );
		CHECK ( match->track(1, 14, v2) );

		// 2
		CHECK ( match->id(2) );

		CHECK ( not match->track(2,  0, v1) );
		CHECK ( not match->track(2,  1, v1) );
		CHECK ( not match->track(2,  2, v1) );
		CHECK ( not match->track(2,  3, v1) );
		CHECK ( not match->track(2,  4, v1) );
		CHECK ( not match->track(2,  5, v1) );
		CHECK ( not match->track(2,  6, v1) );
		CHECK ( not match->track(2,  7, v1) );
		CHECK ( not match->track(2,  8, v1) );
		CHECK ( not match->track(2,  9, v1) );
		CHECK ( not match->track(2, 10, v1) );
		CHECK ( not match->track(2, 11, v1) );
		CHECK ( not match->track(2, 12, v1) );
		CHECK ( not match->track(2, 13, v1) );
		CHECK ( not match->track(2, 14, v1) );

		CHECK ( match->track(2,  0, v2) );
		CHECK ( match->track(2,  1, v2) );
		CHECK ( match->track(2,  2, v2) );
		CHECK ( match->track(2,  3, v2) );
		CHECK ( match->track(2,  4, v2) );
		CHECK ( match->track(2,  5, v2) );
		CHECK ( match->track(2,  6, v2) );
		CHECK ( match->track(2,  7, v2) );
		CHECK ( match->track(2,  8, v2) );
		CHECK ( match->track(2,  9, v2) );
		CHECK ( match->track(2, 10, v2) );
		CHECK ( match->track(2, 11, v2) );
		CHECK ( match->track(2, 12, v2) );
		CHECK ( match->track(2, 13, v2) );
		CHECK ( match->track(2, 14, v2) );


		CHECK_THROWS ( match->id(3) );            // illegal block
		CHECK_THROWS ( match->track(3, 14, v2) ); //         block
		CHECK_THROWS ( match->track(2, 15, v2) ); //         track


		CHECK ( match->difference(0, v1) ==  0 );
		CHECK ( match->difference(0, v2) == 15 );

		CHECK ( match->difference(1, v1) == 16 );
		CHECK ( match->difference(1, v2) ==  1 );

		CHECK ( match->difference(2, v1) == 15 );
		CHECK ( match->difference(2, v2) ==  0 );

		CHECK_THROWS ( match->difference(3, v1) == 0 );
		CHECK_THROWS ( match->difference(3, v2) == 0 );
	}


	TracksetMatcher tsm_diff(result1, response);


	SECTION ( "TracksetMatcher Copy construct" )
	{
		TracksetMatcher tsm_diff_copy(tsm_diff);
	}


	SECTION ( "TracksetMatcher finds best match on verifying album input" )
	{
		CHECK ( tsm_diff.matches() );

		CHECK ( tsm_diff.best_match() == 2 );
		CHECK ( tsm_diff.best_difference() == 0 );
		CHECK ( tsm_diff.matches_v2() );
	}


	SECTION ( "TracksetMatcher's Match loads as declared on album input" )
	{
		const Match* match = tsm_diff.match();

		CHECK ( match->tracks_per_block() == 15 );

		CHECK ( match->size() == 93 ); // 2 * blocks * tracks + blocks

		// 0
		CHECK ( match->id(0) );

		CHECK ( match->track(0,  0, v1) );
		CHECK ( match->track(0,  1, v1) );
		CHECK ( match->track(0,  2, v1) );
		CHECK ( match->track(0,  3, v1) );
		CHECK ( match->track(0,  4, v1) );
		CHECK ( match->track(0,  5, v1) );
		CHECK ( match->track(0,  6, v1) );
		CHECK ( match->track(0,  7, v1) );
		CHECK ( match->track(0,  8, v1) );
		CHECK ( match->track(0,  9, v1) );
		CHECK ( match->track(0, 10, v1) );
		CHECK ( match->track(0, 11, v1) );
		CHECK ( match->track(0, 12, v1) );
		CHECK ( match->track(0, 13, v1) );
		CHECK ( match->track(0, 14, v1) );

		CHECK ( not match->track(0,  0, v2) );
		CHECK ( not match->track(0,  1, v2) );
		CHECK ( not match->track(0,  2, v2) );
		CHECK ( not match->track(0,  3, v2) );
		CHECK ( not match->track(0,  4, v2) );
		CHECK ( not match->track(0,  5, v2) );
		CHECK ( not match->track(0,  6, v2) );
		CHECK ( not match->track(0,  7, v2) );
		CHECK ( not match->track(0,  8, v2) );
		CHECK ( not match->track(0,  9, v2) );
		CHECK ( not match->track(0, 10, v2) );
		CHECK ( not match->track(0, 11, v2) );
		CHECK ( not match->track(0, 12, v2) );
		CHECK ( not match->track(0, 13, v2) );
		CHECK ( not match->track(0, 14, v2) );

		// 1
		CHECK ( match->id(1) ); // differs from AlbumMatcher: any id is verified

		CHECK ( not match->track(1,  0, v1) );
		CHECK ( not match->track(1,  1, v1) );
		CHECK ( not match->track(1,  2, v1) );
		CHECK ( not match->track(1,  3, v1) );
		CHECK ( not match->track(1,  4, v1) );
		CHECK ( not match->track(1,  5, v1) );
		CHECK ( not match->track(1,  6, v1) );
		CHECK ( not match->track(1,  7, v1) );
		CHECK ( not match->track(1,  8, v1) );
		CHECK ( not match->track(1,  9, v1) );
		CHECK ( not match->track(1, 10, v1) );
		CHECK ( not match->track(1, 11, v1) );
		CHECK ( not match->track(1, 12, v1) );
		CHECK ( not match->track(1, 13, v1) );
		CHECK ( not match->track(1, 14, v1) );

		CHECK ( match->track(1,  0, v2) );
		CHECK ( match->track(1,  1, v2) );
		CHECK ( match->track(1,  2, v2) );
		CHECK ( match->track(1,  3, v2) );
		CHECK ( match->track(1,  4, v2) );
		CHECK ( match->track(1,  5, v2) );
		CHECK ( match->track(1,  6, v2) );
		CHECK ( match->track(1,  7, v2) );
		CHECK ( match->track(1,  8, v2) );
		CHECK ( match->track(1,  9, v2) );
		CHECK ( match->track(1, 10, v2) );
		CHECK ( match->track(1, 11, v2) );
		CHECK ( match->track(1, 12, v2) );
		CHECK ( match->track(1, 13, v2) );
		CHECK ( match->track(1, 14, v2) );

		// 2
		CHECK ( match->id(2) );

		CHECK ( not match->track(2,  0, v1) );
		CHECK ( not match->track(2,  1, v1) );
		CHECK ( not match->track(2,  2, v1) );
		CHECK ( not match->track(2,  3, v1) );
		CHECK ( not match->track(2,  4, v1) );
		CHECK ( not match->track(2,  5, v1) );
		CHECK ( not match->track(2,  6, v1) );
		CHECK ( not match->track(2,  7, v1) );
		CHECK ( not match->track(2,  8, v1) );
		CHECK ( not match->track(2,  9, v1) );
		CHECK ( not match->track(2, 10, v1) );
		CHECK ( not match->track(2, 11, v1) );
		CHECK ( not match->track(2, 12, v1) );
		CHECK ( not match->track(2, 13, v1) );
		CHECK ( not match->track(2, 14, v1) );

		CHECK ( match->track(2,  0, v2) );
		CHECK ( match->track(2,  1, v2) );
		CHECK ( match->track(2,  2, v2) );
		CHECK ( match->track(2,  3, v2) );
		CHECK ( match->track(2,  4, v2) );
		CHECK ( match->track(2,  5, v2) );
		CHECK ( match->track(2,  6, v2) );
		CHECK ( match->track(2,  7, v2) );
		CHECK ( match->track(2,  8, v2) );
		CHECK ( match->track(2,  9, v2) );
		CHECK ( match->track(2, 10, v2) );
		CHECK ( match->track(2, 11, v2) );
		CHECK ( match->track(2, 12, v2) );
		CHECK ( match->track(2, 13, v2) );
		CHECK ( match->track(2, 14, v2) );


		CHECK_THROWS ( match->id(3) );            // illegal block
		CHECK_THROWS ( match->track(3, 14, v2) ); //         block
		CHECK_THROWS ( match->track(2, 15, v2) ); //         track


		CHECK ( match->difference(0, v1) ==  0 );
		CHECK ( match->difference(0, v2) == 15 );

		CHECK ( match->difference(1, v1) == 15 ); // differs from AlbumMatcher
		CHECK ( match->difference(1, v2) ==  0 ); // differs from AlbumMatcher

		CHECK ( match->difference(2, v1) == 15 );
		CHECK ( match->difference(2, v2) ==  0 );

		CHECK_THROWS ( match->difference(3, v1) == 0 );
		CHECK_THROWS ( match->difference(3, v2) == 0 );
	}

	const auto refsums_v1 = std::vector<Checksum>
	{
			Checksum(0x98B10E0F),
			Checksum(0x475F57E9),
			Checksum(0x7304F1C4),
			Checksum(0xF2472287),
			Checksum(0x881BC504),
			Checksum(0xBB94BFD4),
			Checksum(0xF9CAEE76),
			Checksum(0xF9F60BC1),
			Checksum(0x2C736302),
			Checksum(0x1C955978),
			Checksum(0xFDA6D833),
			Checksum(0x3A57E5D1),
			Checksum(0x6ED5F3E7),
			Checksum(0x4A5C3872),
			Checksum(0x5FE8B032)
	};

	REQUIRE ( refsums_v1.size() == 15 );
	REQUIRE ( refsums_v1.size() == result1.size() );

	const auto refsums_v2 = std::vector<Checksum>
	{
			Checksum(0xB89992E5),
			Checksum(0x4F77EB03),
			Checksum(0x56582282),
			Checksum(0x9E2187F9),
			Checksum(0x6BE71E50),
			Checksum(0x01E7235F),
			Checksum(0xD8F7763C),
			Checksum(0x8480223E),
			Checksum(0x42C5061C),
			Checksum(0x47A70F02),
			Checksum(0xBABF08CC),
			Checksum(0x563EDCCB),
			Checksum(0xAB123C7C),
			Checksum(0xC65C20E4),
			Checksum(0x58FC3C3E)
	};

	REQUIRE ( refsums_v2.size() == 15 );
	REQUIRE ( refsums_v2.size() == result1.size() );

	auto lm_diff_v1 = ListMatcher(result1, refsums_v1); // expected to NOT match

	auto lm_diff_v2 = ListMatcher(result1, refsums_v2); // expected to match

	SECTION ( "ListMatcher Copy construct" )
	{
		ListMatcher lm_diff_copy(lm_diff_v2);
	}

	SECTION ( "ListMatcher finds best match on verifying album input" )
	{
		CHECK ( lm_diff_v2.matches() );

		CHECK ( lm_diff_v2.best_match() == 0 );
		CHECK ( lm_diff_v2.best_difference() == 0 );
		CHECK ( lm_diff_v2.matches_v2() );
	}

	SECTION ( "ListMatcher's v1 Match loads as declared on album input" )
	{
		const Match* match = lm_diff_v1.match();

		CHECK ( match->tracks_per_block() == 15 );

		CHECK ( match->size() == 31 );

		// block 0 (only block)
		CHECK ( match->id(0) );

		CHECK ( match->track(0,  0, v1) );
		CHECK ( match->track(0,  1, v1) );
		CHECK ( match->track(0,  2, v1) );
		CHECK ( match->track(0,  3, v1) );
		CHECK ( match->track(0,  4, v1) );
		CHECK ( match->track(0,  5, v1) );
		CHECK ( match->track(0,  6, v1) );
		CHECK ( match->track(0,  7, v1) );
		CHECK ( match->track(0,  8, v1) );
		CHECK ( match->track(0,  9, v1) );
		CHECK ( match->track(0, 10, v1) );
		CHECK ( match->track(0, 11, v1) );
		CHECK ( match->track(0, 12, v1) );
		CHECK ( match->track(0, 13, v1) );
		CHECK ( match->track(0, 14, v1) );

		CHECK ( not match->track(0,  0, v2) );
		CHECK ( not match->track(0,  1, v2) );
		CHECK ( not match->track(0,  2, v2) );
		CHECK ( not match->track(0,  3, v2) );
		CHECK ( not match->track(0,  4, v2) );
		CHECK ( not match->track(0,  5, v2) );
		CHECK ( not match->track(0,  6, v2) );
		CHECK ( not match->track(0,  7, v2) );
		CHECK ( not match->track(0,  8, v2) );
		CHECK ( not match->track(0,  9, v2) );
		CHECK ( not match->track(0, 10, v2) );
		CHECK ( not match->track(0, 11, v2) );
		CHECK ( not match->track(0, 12, v2) );
		CHECK ( not match->track(0, 13, v2) );
		CHECK ( not match->track(0, 14, v2) );


		CHECK_THROWS ( match->id(2) );            // illegal block
		CHECK_THROWS ( match->difference(1, v1) == 0 );
		CHECK_THROWS ( match->difference(1, v2) == 0 );
		CHECK_THROWS ( match->track(1, 14, v2) ); //         block
		CHECK_THROWS ( match->track(0, 15, v2) ); //         track

		CHECK ( match->difference(0, v2) == 15 );
		CHECK ( match->difference(0, v1) ==  0 );
	}

	SECTION ( "ListMatcher's v2 Match loads as declared on album input" )
	{
		const Match* match = lm_diff_v2.match();

		CHECK ( match->tracks_per_block() == 15 );

		CHECK ( match->size() == 31 );

		// block 0 (only block)
		CHECK ( match->id(0) );

		CHECK ( not match->track(0,  0, v1) );
		CHECK ( not match->track(0,  1, v1) );
		CHECK ( not match->track(0,  2, v1) );
		CHECK ( not match->track(0,  3, v1) );
		CHECK ( not match->track(0,  4, v1) );
		CHECK ( not match->track(0,  5, v1) );
		CHECK ( not match->track(0,  6, v1) );
		CHECK ( not match->track(0,  7, v1) );
		CHECK ( not match->track(0,  8, v1) );
		CHECK ( not match->track(0,  9, v1) );
		CHECK ( not match->track(0, 10, v1) );
		CHECK ( not match->track(0, 11, v1) );
		CHECK ( not match->track(0, 12, v1) );
		CHECK ( not match->track(0, 13, v1) );
		CHECK ( not match->track(0, 14, v1) );

		CHECK ( match->track(0,  0, v2) );
		CHECK ( match->track(0,  1, v2) );
		CHECK ( match->track(0,  2, v2) );
		CHECK ( match->track(0,  3, v2) );
		CHECK ( match->track(0,  4, v2) );
		CHECK ( match->track(0,  5, v2) );
		CHECK ( match->track(0,  6, v2) );
		CHECK ( match->track(0,  7, v2) );
		CHECK ( match->track(0,  8, v2) );
		CHECK ( match->track(0,  9, v2) );
		CHECK ( match->track(0, 10, v2) );
		CHECK ( match->track(0, 11, v2) );
		CHECK ( match->track(0, 12, v2) );
		CHECK ( match->track(0, 13, v2) );
		CHECK ( match->track(0, 14, v2) );


		CHECK_THROWS ( match->id(2) );            // illegal block
		CHECK_THROWS ( match->difference(1, v1) == 0 );
		CHECK_THROWS ( match->difference(1, v2) == 0 );
		CHECK_THROWS ( match->track(1, 14, v2) ); //         block
		CHECK_THROWS ( match->track(0, 15, v2) ); //         track

		CHECK ( match->difference(0, v1) == 15 );
		CHECK ( match->difference(0, v2) ==  0 );
	}
}

