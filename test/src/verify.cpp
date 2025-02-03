#include "catch2/catch_test_macros.hpp"

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_DBAR_HPP__
#include "dbar.hpp"
#endif

#ifndef __LIBARCSTK_VERIFY_HPP__
#include "verify.hpp"
#endif
#ifndef __LIBARCSTK_VERIFY_DETAILS_HPP__
#include "verify_details.hpp"
#endif



/**
 * \file Fixtures for classes in module "verify"
 */


TEST_CASE ( "DBARSource", "[dbarsource]" )
{
	using arcstk::ARId;
	using arcstk::DBAR;
	using arcstk::DBARSource;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0 },
			{ 0x475F57E9,  4, 0 },
			{ 0x7304F1C4,  5, 0 },
			{ 0xF2472287,  6, 0 },
			{ 0x881BC504,  7, 0 },
			{ 0xBB94BFD4,  9, 0 },
			{ 0xF9CAEE76, 10, 0 },
			{ 0xF9F60BC1, 11, 0 },
			{ 0x2C736302, 12, 0 },
			{ 0x1C955978, 13, 0 },
			{ 0xFDA6D833, 16, 0 },
			{ 0x3A57E5D1, 17, 0 },
			{ 0x6ED5F3E7, 18, 0 },
			{ 0x4A5C3872, 21, 0 },
			{ 0x5FE8B032, 24, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	REQUIRE ( dBAR.size() == 3 );

	auto r = DBARSource { &dBAR };
	auto r_copy_ctor = DBARSource { r }; // copy constructed

	auto r_copy_ass = DBARSource { nullptr };

	REQUIRE ( r_copy_ass.source() == nullptr );

	r_copy_ass = r; // copy assigned

	SECTION ( "ChecksumSoure of DBAR is constructed correctly" )
	{
		CHECK ( &dBAR == r.source() );
	}

	SECTION ( "ChecksumSoure of DBAR is copy-constructed correctly" )
	{
		CHECK (  r_copy_ctor.source() == &dBAR );
		CHECK ( &r_copy_ctor          != &r );
	}

	SECTION ( "ChecksumSoure of DBAR is copy-assigned correctly" )
	{
		CHECK (  r_copy_ass.source() == &dBAR );
		CHECK ( &r_copy_ass          != &r );
	}

	SECTION ( "Access on DBAR data is correct" )
	{
		CHECK ( r.checksum(0,  0) == 0x98B10E0Fu );
		CHECK ( r.checksum(0,  1) == 0x475F57E9u );
		CHECK ( r.checksum(0,  2) == 0x7304F1C4u );
		CHECK ( r.checksum(0,  3) == 0xF2472287u );
		CHECK ( r.checksum(0,  4) == 0x881BC504u );
		CHECK ( r.checksum(0,  5) == 0xBB94BFD4u );
		CHECK ( r.checksum(0,  6) == 0xF9CAEE76u );
		CHECK ( r.checksum(0,  7) == 0xF9F60BC1u );
		CHECK ( r.checksum(0,  8) == 0x2C736302u );
		CHECK ( r.checksum(0,  9) == 0x1C955978u );
		CHECK ( r.checksum(0, 10) == 0xFDA6D833u );
		CHECK ( r.checksum(0, 11) == 0x3A57E5D1u );
		CHECK ( r.checksum(0, 12) == 0x6ED5F3E7u );
		CHECK ( r.checksum(0, 13) == 0x4A5C3872u );
		CHECK ( r.checksum(0, 14) == 0x5FE8B032u );

		CHECK ( r.checksum(1,  0) == 0xB89992E5u );
		CHECK ( r.checksum(1,  1) == 0x4F77EB03u );
		CHECK ( r.checksum(1,  2) == 0x56582282u );
		CHECK ( r.checksum(1,  3) == 0x9E2187F9u );
		CHECK ( r.checksum(1,  4) == 0x6BE71E50u );
		CHECK ( r.checksum(1,  5) == 0x01E7235Fu );
		CHECK ( r.checksum(1,  6) == 0xD8F7763Cu );
		CHECK ( r.checksum(1,  7) == 0x8480223Eu );
		CHECK ( r.checksum(1,  8) == 0x42C5061Cu );
		CHECK ( r.checksum(1,  9) == 0x47A70F02u );
		CHECK ( r.checksum(1, 10) == 0xBABF08CCu );
		CHECK ( r.checksum(1, 11) == 0x563EDCCBu );
		CHECK ( r.checksum(1, 12) == 0xAB123C7Cu );
		CHECK ( r.checksum(1, 13) == 0xC65C20E4u );
		CHECK ( r.checksum(1, 14) == 0x58FC3C3Eu );

		CHECK ( r.checksum(2,  0) == 0xC89192E5u );
		CHECK ( r.checksum(2,  1) == 0x4F78EB03u );
		CHECK ( r.checksum(2,  2) == 0x56582281u );
		CHECK ( r.checksum(2,  3) == 0x0E2187F9u );
		CHECK ( r.checksum(2,  4) == 0x2BE71E50u );
		CHECK ( r.checksum(2,  5) == 0x01E7235Du );
		CHECK ( r.checksum(2,  6) == 0xD8F6763Cu );
		CHECK ( r.checksum(2,  7) == 0x8480331Eu );
		CHECK ( r.checksum(2,  8) == 0x42F5061Cu );
		CHECK ( r.checksum(2,  9) == 0x47D70F02u );
		CHECK ( r.checksum(2, 10) == 0xBABF08AAu );
		CHECK ( r.checksum(2, 11) == 0x563EFECBu );
		CHECK ( r.checksum(2, 12) == 0xAB123C9Cu );
		CHECK ( r.checksum(2, 13) == 0xB65C20E4u );
		CHECK ( r.checksum(2, 14) == 0x68FC3C3Eu );
	}
}


TEST_CASE ( "details::BestBlock", "[bestblock]" )
{
	using arcstk::details::BestBlock;
	using arcstk::details::create_result;
	using arcstk::details::StrictPolicy;

	auto r = create_result(4, 15, std::make_unique<StrictPolicy>());

	r->verify_id(0);
	r->verify_track(0,  1, true);
	r->verify_track(0,  2, true);
	r->verify_track(0,  3, true);
	r->verify_track(0,  4, true);
	r->verify_track(0,  5, true);
	r->verify_track(0,  6, true);
	r->verify_track(0,  7, true);
	r->verify_track(0,  8, true);
	r->verify_track(0,  9, true);
	r->verify_track(0, 10, true);
	r->verify_track(0, 11, true);
	r->verify_track(0, 12, true);
	r->verify_track(0, 13, true);
	r->verify_track(0, 14, true);

	const auto get_best_block = BestBlock{};

	const auto best_block = get_best_block.from(*r);

	SECTION ( "Best block is found correctly" )
	{
		CHECK ( std::get<0>(best_block) == 0 );
	}
}


// TODO details::ResultBits


TEST_CASE ( "details::Result", "[result]" )
{
	using arcstk::details::create_result;
	using arcstk::details::StrictPolicy;

	auto r = create_result(2, 10, std::make_unique<StrictPolicy>());
	// 2 block, 10 tracks each, every flag is 0

	REQUIRE ( r->total_blocks() == 2 );
	REQUIRE ( r->tracks_per_block() == 10 );
	REQUIRE ( r->size() == 42 );
	REQUIRE ( r->strict() );

	REQUIRE ( not r->id(0) );

	REQUIRE ( not r->track(0, 0, false) );
	REQUIRE ( not r->track(0, 1, false) );
	REQUIRE ( not r->track(0, 2, false) );
	REQUIRE ( not r->track(0, 3, false) );
	REQUIRE ( not r->track(0, 4, false) );
	REQUIRE ( not r->track(0, 5, false) );
	REQUIRE ( not r->track(0, 6, false) );
	REQUIRE ( not r->track(0, 7, false) );
	REQUIRE ( not r->track(0, 8, false) );
	REQUIRE ( not r->track(0, 9, false) );

	REQUIRE ( not r->track(0, 0, true) );
	REQUIRE ( not r->track(0, 1, true) );
	REQUIRE ( not r->track(0, 2, true) );
	REQUIRE ( not r->track(0, 3, true) );
	REQUIRE ( not r->track(0, 4, true) );
	REQUIRE ( not r->track(0, 5, true) );
	REQUIRE ( not r->track(0, 6, true) );
	REQUIRE ( not r->track(0, 7, true) );
	REQUIRE ( not r->track(0, 8, true) );
	REQUIRE ( not r->track(0, 9, true) );

	REQUIRE ( not r->id(1) );

	REQUIRE ( not r->track(1, 0, false) );
	REQUIRE ( not r->track(1, 1, false) );
	REQUIRE ( not r->track(1, 2, false) );
	REQUIRE ( not r->track(1, 3, false) );
	REQUIRE ( not r->track(1, 4, false) );
	REQUIRE ( not r->track(1, 5, false) );
	REQUIRE ( not r->track(1, 6, false) );
	REQUIRE ( not r->track(1, 7, false) );
	REQUIRE ( not r->track(1, 8, false) );
	REQUIRE ( not r->track(1, 9, false) );

	REQUIRE ( not r->track(1, 0, true) );
	REQUIRE ( not r->track(1, 1, true) );
	REQUIRE ( not r->track(1, 2, true) );
	REQUIRE ( not r->track(1, 3, true) );
	REQUIRE ( not r->track(1, 4, true) );
	REQUIRE ( not r->track(1, 5, true) );
	REQUIRE ( not r->track(1, 6, true) );
	REQUIRE ( not r->track(1, 7, true) );
	REQUIRE ( not r->track(1, 8, true) );
	REQUIRE ( not r->track(1, 9, true) );


	SECTION ( "Setting id and track flags has intended effects" )
	{
		r->verify_id(0);
		r->verify_track(0, 2, false);
		r->verify_track(0, 3, false);
		r->verify_track(0, 9, false);
		r->verify_track(0, 5, true);
		r->verify_track(0, 7, true);
		r->verify_id(1);
		r->verify_track(1, 1, false);
		r->verify_track(1, 4, false);
		r->verify_track(1, 6, false);
		r->verify_track(1, 2, true);
		r->verify_track(1, 9, true);

		CHECK ( r->id(0) );

		CHECK ( not r->track(0, 0, false) );
		CHECK ( not r->track(0, 1, false) );
		CHECK (     r->track(0, 2, false) );
		CHECK (     r->track(0, 3, false) );
		CHECK ( not r->track(0, 4, false) );
		CHECK ( not r->track(0, 5, false) );
		CHECK ( not r->track(0, 6, false) );
		CHECK ( not r->track(0, 7, false) );
		CHECK ( not r->track(0, 8, false) );
		CHECK (     r->track(0, 9, false) );

		CHECK ( not r->track(0, 0, true) );
		CHECK ( not r->track(0, 1, true) );
		CHECK ( not r->track(0, 2, true) );
		CHECK ( not r->track(0, 3, true) );
		CHECK ( not r->track(0, 4, true) );
		CHECK (     r->track(0, 5, true) );
		CHECK ( not r->track(0, 6, true) );
		CHECK (     r->track(0, 7, true) );
		CHECK ( not r->track(0, 8, true) );
		CHECK ( not r->track(0, 9, true) );

		CHECK ( r->id(1) );

		CHECK ( not r->track(1, 0, false) );
		CHECK (     r->track(1, 1, false) );
		CHECK ( not r->track(1, 2, false) );
		CHECK ( not r->track(1, 3, false) );
		CHECK (     r->track(1, 4, false) );
		CHECK ( not r->track(1, 5, false) );
		CHECK (     r->track(1, 6, false) );
		CHECK ( not r->track(1, 7, false) );
		CHECK ( not r->track(1, 8, false) );
		CHECK ( not r->track(1, 9, false) );

		CHECK ( not r->track(1, 0, true) );
		CHECK ( not r->track(1, 1, true) );
		CHECK (     r->track(1, 2, true) );
		CHECK ( not r->track(1, 3, true) );
		CHECK ( not r->track(1, 4, true) );
		CHECK ( not r->track(1, 5, true) );
		CHECK ( not r->track(1, 6, true) );
		CHECK ( not r->track(1, 7, true) );
		CHECK ( not r->track(1, 8, true) );
		CHECK (     r->track(1, 9, true) );
	}

	SECTION ( "Moved Result can be manipulated as intended" )
	{
		auto m = std::move(r);

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
}


TEST_CASE ( "details::BlockSelector", "[blockselector]")
{
	using arcstk::details::BlockSelector;
	using arcstk::ARId;
	using arcstk::DBAR;
	using arcstk::DBARSource;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0 },
			{ 0x475F57E9,  4, 0 },
			{ 0x7304F1C4,  5, 0 },
			{ 0xF2472287,  6, 0 },
			{ 0x881BC504,  7, 0 },
			{ 0xBB94BFD4,  9, 0 },
			{ 0xF9CAEE76, 10, 0 },
			{ 0xF9F60BC1, 11, 0 },
			{ 0x2C736302, 12, 0 },
			{ 0x1C955978, 13, 0 },
			{ 0xFDA6D833, 16, 0 },
			{ 0x3A57E5D1, 17, 0 },
			{ 0x6ED5F3E7, 18, 0 },
			{ 0x4A5C3872, 21, 0 },
			{ 0x5FE8B032, 24, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	const auto r = DBARSource { &dBAR };
	const auto b = BlockSelector {};

//  Commented out: Values for testing the index calculation
//
//	SECTION ( "Indices" )
//	{
//		auto dbar = dBAR.impl(); // get the DBARObject
//
//		CHECK ( dbar->total_tracks_accumulated(0) ==  0 );
//		CHECK ( dbar->total_tracks_accumulated(1) == 15 );
//		CHECK ( dbar->total_tracks_accumulated(2) == 30 );
//
//		CHECK ( dbar->arcs_idx(0, 13) == 29 );
//		CHECK ( dbar->arcs_idx(1,  2) == 40 );
//		CHECK ( dbar->arcs_idx(1,  7) == 50 );
//		CHECK ( dbar->arcs_idx(1, 14) == 64 );
//		CHECK ( dbar->arcs_idx(2,  0) == 69 );
//		CHECK ( dbar->arcs_idx(2, 14) == 97 );
//	}

	SECTION ( "BlockSelector gets Checksum by <block, track>" )
	{
		CHECK ( b.get(r, 0, 13) == 0x4A5C3872u );

		CHECK ( b.get(r, 1,  2) == 0x56582282u );
		CHECK ( b.get(r, 1,  7) == 0x8480223Eu );
		CHECK ( b.get(r, 1, 14) == 0x58FC3C3Eu );

		CHECK ( b.get(r, 2,  0) == 0xC89192E5u );
		CHECK ( b.get(r, 2, 14) == 0x68FC3C3Eu );
	}
}


TEST_CASE ( "details::TrackSelector", "[trackselector]")
{
	using arcstk::details::TrackSelector;
	using arcstk::ARId;
	using arcstk::DBAR;
	using arcstk::DBARSource;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0 },
			{ 0x475F57E9,  4, 0 },
			{ 0x7304F1C4,  5, 0 },
			{ 0xF2472287,  6, 0 },
			{ 0x881BC504,  7, 0 },
			{ 0xBB94BFD4,  9, 0 },
			{ 0xF9CAEE76, 10, 0 },
			{ 0xF9F60BC1, 11, 0 },
			{ 0x2C736302, 12, 0 },
			{ 0x1C955978, 13, 0 },
			{ 0xFDA6D833, 16, 0 },
			{ 0x3A57E5D1, 17, 0 },
			{ 0x6ED5F3E7, 18, 0 },
			{ 0x4A5C3872, 21, 0 },
			{ 0x5FE8B032, 24, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	const auto r = DBARSource { &dBAR };
	const auto t = TrackSelector {};

	SECTION ( "TrackSelector gets Checksum by <track, block>" )
	{
		CHECK ( t.get(r, 14, 0) == 0x5FE8B032u );
		CHECK ( t.get(r,  7, 1) == 0x8480223Eu );
		CHECK ( t.get(r,  1, 2) == 0x4F78EB03u );
	}
}


TEST_CASE( "details::SourceIterator", "[sourceiterator]" )
{
	using arcstk::details::SourceIterator;
	using arcstk::details::BlockSelector;
	using arcstk::details::TrackSelector;
	using arcstk::ARId;
	using arcstk::DBAR;
	using arcstk::DBARSource;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0 },
			{ 0x475F57E9,  4, 0 },
			{ 0x7304F1C4,  5, 0 },
			{ 0xF2472287,  6, 0 },
			{ 0x881BC504,  7, 0 },
			{ 0xBB94BFD4,  9, 0 },
			{ 0xF9CAEE76, 10, 0 },
			{ 0xF9F60BC1, 11, 0 },
			{ 0x2C736302, 12, 0 },
			{ 0x1C955978, 13, 0 },
			{ 0xFDA6D833, 16, 0 },
			{ 0x3A57E5D1, 17, 0 },
			{ 0x6ED5F3E7, 18, 0 },
			{ 0x4A5C3872, 21, 0 },
			{ 0x5FE8B032, 24, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	const auto r = DBARSource { &dBAR };
	const auto block = BlockSelector {};
	const auto track = TrackSelector {};

	auto b = SourceIterator(r, 1, 0, block);

	REQUIRE ( b.current() == 1 );
	REQUIRE ( b.counter() == 0 );

	auto t = SourceIterator(r, 3, 0, track);

	REQUIRE ( t.current() == 3 );
	REQUIRE ( t.counter() == 0 );

	SECTION ( "SourceIterator does correct forward iteration over block" )
	{
		CHECK (    *b  == 0xB89992E5u );
		CHECK ( *(++b) == 0x4F77EB03u );
		CHECK ( *(++b) == 0x56582282u );
		CHECK ( *(++b) == 0x9E2187F9u );
		CHECK ( *(++b) == 0x6BE71E50u );
		CHECK ( *(++b) == 0x01E7235Fu );
		CHECK ( *(++b) == 0xD8F7763Cu );
		CHECK ( *(++b) == 0x8480223Eu );
		CHECK ( *(++b) == 0x42C5061Cu );
		CHECK ( *(++b) == 0x47A70F02u );
		CHECK ( *(++b) == 0xBABF08CCu );
		CHECK ( *(++b) == 0x563EDCCBu );
		CHECK ( *(++b) == 0xAB123C7Cu );
		CHECK ( *(++b) == 0xC65C20E4u );
		CHECK ( *(++b) == 0x58FC3C3Eu );

		CHECK ( b.current() ==  1 ); // block
		CHECK ( b.counter() == 14 );
	}

	SECTION ( "SourceIterator does correct forward iteration over track" )
	{
		CHECK (    *t  == 0xF2472287u );
		CHECK ( *(++t) == 0x9E2187F9u );
		CHECK ( *(++t) == 0x0E2187F9u );

		CHECK ( t.current() == 3 ); // track
		CHECK ( t.counter() == 2 );
	}
}


TEST_CASE ( "details::BlockTraversal", "[blocktraversal]" )
{
	using arcstk::details::BlockTraversal;

	using arcstk::ARId;
	using arcstk::DBAR;
	using arcstk::DBARSource;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0 },
			{ 0x475F57E9,  4, 0 },
			{ 0x7304F1C4,  5, 0 },
			{ 0xF2472287,  6, 0 },
			{ 0x881BC504,  7, 0 },
			{ 0xBB94BFD4,  9, 0 },
			{ 0xF9CAEE76, 10, 0 },
			{ 0xF9F60BC1, 11, 0 },
			{ 0x2C736302, 12, 0 },
			{ 0x1C955978, 13, 0 },
			{ 0xFDA6D833, 16, 0 },
			{ 0x3A57E5D1, 17, 0 },
			{ 0x6ED5F3E7, 18, 0 },
			{ 0x4A5C3872, 21, 0 },
			{ 0x5FE8B032, 24, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	REQUIRE ( dBAR.size() == 3 );
	//REQUIRE ( dBAR[0] == dbar.block(0) );
	//REQUIRE ( dBAR[1] == dbar.block(1) );
	//REQUIRE ( dBAR[2] == dbar.block(2) );

	const auto r = DBARSource { &dBAR };
	BlockTraversal b;

	REQUIRE ( b.get_policy()->is_strict() );

	b.set_source(r);
	REQUIRE ( b.source() == &r );
	REQUIRE ( b.source() != nullptr );

	b.set_current(1);
	REQUIRE ( b.current() == 1 ); // block

	REQUIRE ( b.current_block(b.begin()) == 1 );
	REQUIRE ( b.current_track(b.begin()) == 0 );

	auto block_start = b.begin();
	REQUIRE ( block_start.counter() == 0 );

	auto block_end = b.end();
	REQUIRE ( block_end.counter() == 15 );


	SECTION ( "BlockTraversal traverses current() correctly" )
	{
		auto i = b.begin();

		CHECK ( i.current() == 1 ); // block
		CHECK ( i.counter() == 0 ); // track

		CHECK (     *i == 0xB89992E5 );
		CHECK ( *(++i) == 0x4F77EB03 );
		CHECK ( *(++i) == 0x56582282 );
		CHECK ( *(++i) == 0x9E2187F9 );
		CHECK ( *(++i) == 0x6BE71E50 );
		CHECK ( *(++i) == 0x01E7235F );
		CHECK ( *(++i) == 0xD8F7763C );
		CHECK ( *(++i) == 0x8480223E );
		CHECK ( *(++i) == 0x42C5061C );
		CHECK ( *(++i) == 0x47A70F02 );
		CHECK ( *(++i) == 0xBABF08CC );
		CHECK ( *(++i) == 0x563EDCCB );
		CHECK ( *(++i) == 0xAB123C7C );
		CHECK ( *(++i) == 0xC65C20E4 );
		CHECK ( *(++i) == 0x58FC3C3E );

		CHECK ( i.current() ==  1 ); // block
		CHECK ( i.counter() == 14 ); // track

		CHECK ( ++i == block_end );
		CHECK ( i.counter() == 15 );
	}

	SECTION ( "BlockTraversal performs correctly in classical for-loop" )
	{
		REQUIRE ( b.source() == &r );
		REQUIRE ( b.source() != nullptr );
		REQUIRE ( b.current() == 1 ); // block

		auto it = b.begin();
		const auto stop = b.end();

		for (; it != stop; ++it)
		{
			CHECK ( it != stop );
		}

		CHECK ( it == stop );
	}
}


TEST_CASE ( "details::TrackTraversal", "[tracktraversal]" )
{
	using arcstk::details::TrackTraversal;
	using arcstk::ARId;
	using arcstk::DBAR;
	using arcstk::DBARSource;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0 },
			{ 0x475F57E9,  4, 0 },
			{ 0x7304F1C4,  5, 0 },
			{ 0xF2472287,  6, 0 },
			{ 0x881BC504,  7, 0 },
			{ 0xBB94BFD4,  9, 0 },
			{ 0xF9CAEE76, 10, 0 },
			{ 0xF9F60BC1, 11, 0 },
			{ 0x2C736302, 12, 0 },
			{ 0x1C955978, 13, 0 },
			{ 0xFDA6D833, 16, 0 },
			{ 0x3A57E5D1, 17, 0 },
			{ 0x6ED5F3E7, 18, 0 },
			{ 0x4A5C3872, 21, 0 },
			{ 0x5FE8B032, 24, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	REQUIRE ( dBAR.size() == 3 );
	//REQUIRE ( dBAR[0] == block0 );
	//REQUIRE ( dBAR[1] == block1 );
	//REQUIRE ( dBAR[2] == block2 );

	const auto r = DBARSource { &dBAR };
	auto t = TrackTraversal {};

	REQUIRE ( not t.get_policy()->is_strict() );

	t.set_source(r);
	REQUIRE ( t.source() == &r );

	t.set_current(3);
	REQUIRE ( t.current() == 3 ); // 0-based track

	REQUIRE ( t.current_block(t.begin()) == 0 );
	REQUIRE ( t.current_track(t.begin()) == 3 );

	const auto track_start = t.begin();
	REQUIRE ( track_start.counter() == 0 );

	const auto track_end = t.end();
	REQUIRE ( track_end.counter() == 3 );


	SECTION ( "TrackTraversal traverses current() correctly" )
	{
		auto i = t.begin();
		CHECK ( i.current() == 3 ); // 0-based track
		CHECK ( i.counter() == 0 );

		CHECK (     *i == 0xF2472287 );
		CHECK ( *(++i) == 0x9E2187F9 );
		CHECK ( *(++i) == 0x0E2187F9 );

		CHECK ( i.current() == 3 ); // 0-based track
		CHECK ( i.counter() == 2 );

		CHECK ( ++i == track_end );
		CHECK ( i.counter() == 3 );
	}
}


TEST_CASE ( "details::TrackOrderPolicy", "[trackorderpolicy]" )
{
	using arcstk::details::TrackOrderPolicy;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;
	using arcstk::checksum::type;
	using arcstk::DBARTriplet;

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

	Checksums actual_sums {
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

	REQUIRE ( actual_sums.size() == 15 );

	std::vector<DBARTriplet> block = {
			{ 0xB89992E5, 0, 0 },
			{ 0x4F77EB03, 0, 0 },
			{ 0x56582282, 0, 0 },
			{ 0x9E2187F9, 0, 0 },
			{ 0x6BE71E50, 0, 0 },
			{ 0x01E7235F, 0, 0 },
			{ 0xD8F7763C, 0, 0 },
			{ 0x8480223E, 0, 0 },
			{ 0x42C5061C, 0, 0 },
			{ 0x47A70F02, 0, 0 },
			{ 0xBABF08CC, 0, 0 },
			{ 0x563EDCCB, 0, 0 },
			{ 0xAB123C7C, 0, 0 },
			{ 0xC65C20E4, 0, 0 },
			{ 0x58FC3C3E, 0, 0 }
	};

	const auto result = arcstk::details::create_result(3, 15,
			std::make_unique<arcstk::details::StrictPolicy>());

	REQUIRE ( result->difference(0, true) == 16);
	REQUIRE ( result->total_unverified_tracks() == 15 );

	REQUIRE ( !result->all_tracks_verified() );
	REQUIRE ( !result->is_verified(0) );
	REQUIRE ( !result->is_verified(1) );
	REQUIRE ( !result->is_verified(2) );
	REQUIRE ( !result->is_verified(3) );
	REQUIRE ( !result->is_verified(4) );
	REQUIRE ( !result->is_verified(5) );
	REQUIRE ( !result->is_verified(6) );
	REQUIRE ( !result->is_verified(7) );
	REQUIRE ( !result->is_verified(8) );
	REQUIRE ( !result->is_verified(9) );
	REQUIRE ( !result->is_verified(10) );
	REQUIRE ( !result->is_verified(11) );
	REQUIRE ( !result->is_verified(12) );
	REQUIRE ( !result->is_verified(13) );
	REQUIRE ( !result->is_verified(14) );

	const auto track_order =
		std::make_unique<arcstk::details::TrackOrderPolicy>();

	auto track = Checksums::size_type { 0 };
	for (const auto& ref : block)
	{
		track_order->perform(*result, actual_sums, ref.arcs(), 0, track);
		++track;
	}

	SECTION ( "TrackOrderPolicy counts verified tracks correctly" )
	{
		REQUIRE ( not result->id(0) );

		CHECK ( result->difference(0, true) == 1);
		CHECK ( result->total_unverified_tracks() == 0 );
		CHECK ( result->all_tracks_verified() );
	}

	SECTION ( "TrackOrderPolicy traverses actual checksums in track order" )
	{
		result->verify_id(0);

		REQUIRE ( result->id(0) );

		CHECK ( result->difference(0, true) == 0);
		CHECK ( result->total_unverified_tracks() == 0 );
		CHECK ( result->all_tracks_verified() );

		CHECK ( result->is_verified(0) );
		CHECK ( result->is_verified(1) );
		CHECK ( result->is_verified(2) );
		CHECK ( result->is_verified(3) );
		CHECK ( result->is_verified(4) );
		CHECK ( result->is_verified(5) );
		CHECK ( result->is_verified(6) );
		CHECK ( result->is_verified(7) );
		CHECK ( result->is_verified(8) );
		CHECK ( result->is_verified(9) );
		CHECK ( result->is_verified(10) );
		CHECK ( result->is_verified(11) );
		CHECK ( result->is_verified(12) );
		CHECK ( result->is_verified(13) );
		CHECK ( result->is_verified(14) );
	}
}


TEST_CASE ( "details::FindOrderPolicy", "[findorderpolicy]" )
{
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;
	using arcstk::checksum::type;
	using arcstk::DBAR;
	using arcstk::DBARTriplet;
	using arcstk::details::FindOrderPolicy;

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

	Checksums actual_sums {
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

	REQUIRE ( actual_sums.size() == 15 );


	std::vector<DBARTriplet> block = {
			{ 0xB89992E5, 0, 0 },
			{ 0x4F77EB03, 0, 0 },
			{ 0x56582282, 0, 0 },
			{ 0x9E2187F9, 0, 0 },
			{ 0x6BE71E50, 0, 0 },
			{ 0x01E7235F, 0, 0 },
			{ 0xD8F7763C, 0, 0 },
			{ 0x8480223E, 0, 0 },
			{ 0x42C5061C, 0, 0 },
			{ 0x47A70F02, 0, 0 },
			{ 0xBABF08CC, 0, 0 },
			{ 0x563EDCCB, 0, 0 },
			{ 0xAB123C7C, 0, 0 },
			{ 0xC65C20E4, 0, 0 },
			{ 0x58FC3C3E, 0, 0 }
	};

	const auto result = arcstk::details::create_result(3, 15,
			std::make_unique<arcstk::details::StrictPolicy>());

	REQUIRE ( result->difference(0, true) == 16);

	REQUIRE ( !result->all_tracks_verified() );
	REQUIRE ( !result->is_verified(0) );
	REQUIRE ( !result->is_verified(1) );
	REQUIRE ( !result->is_verified(2) );
	REQUIRE ( !result->is_verified(3) );
	REQUIRE ( !result->is_verified(4) );
	REQUIRE ( !result->is_verified(5) );
	REQUIRE ( !result->is_verified(6) );
	REQUIRE ( !result->is_verified(7) );
	REQUIRE ( !result->is_verified(8) );
	REQUIRE ( !result->is_verified(9) );
	REQUIRE ( !result->is_verified(10) );
	REQUIRE ( !result->is_verified(11) );
	REQUIRE ( !result->is_verified(12) );
	REQUIRE ( !result->is_verified(13) );
	REQUIRE ( !result->is_verified(14) );

	const auto track_order =
		std::make_unique<arcstk::details::FindOrderPolicy>();

	auto track = Checksums::size_type { 0 };
	for (const auto& ref : block)
	{
		track_order->perform(*result, actual_sums, ref.arcs(), 0, track);
		++track;
	}

	SECTION ( "FindOrderPolicy counts verified tracks correctly" )
	{
		REQUIRE ( not result->id(0) );

		CHECK ( result->difference(0, true) == 1);
		CHECK ( result->total_unverified_tracks() == 0 );
		CHECK ( result->all_tracks_verified() );
	}

	SECTION ( "FindOrderPolicy finds order in actual checksums" )
	{
		CHECK ( result->difference(0, true) == 1); // id was not matched

		CHECK ( result->total_unverified_tracks() == 0 );
		CHECK ( result->all_tracks_verified() );

		CHECK ( result->is_verified(0) );
		CHECK ( result->is_verified(1) );
		CHECK ( result->is_verified(2) );
		CHECK ( result->is_verified(3) );
		CHECK ( result->is_verified(4) );
		CHECK ( result->is_verified(5) );
		CHECK ( result->is_verified(6) );
		CHECK ( result->is_verified(7) );
		CHECK ( result->is_verified(8) );
		CHECK ( result->is_verified(9) );
		CHECK ( result->is_verified(10) );
		CHECK ( result->is_verified(11) );
		CHECK ( result->is_verified(12) );
		CHECK ( result->is_verified(13) );
		CHECK ( result->is_verified(14) );
	}
}


TEST_CASE ( "details::Verification", "[sourcetraversal]" )
{
	using arcstk::ARId;
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;
	using arcstk::DBAR;
	using arcstk::details::BlockTraversal;
	using arcstk::details::TrackOrderPolicy;
	using arcstk::details::TrackTraversal;
	using arcstk::details::Verification;
	using arcstk::DBARSource;

	const auto id = ARId { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0 },
			{ 0x475F57E9,  4, 0 },
			{ 0x7304F1C4,  5, 0 },
			{ 0xF2472287,  6, 0 },
			{ 0x881BC504,  7, 0 },
			{ 0xBB94BFD4,  9, 0 },
			{ 0xF9CAEE76, 10, 0 },
			{ 0xF9F60BC1, 11, 0 },
			{ 0x2C736302, 12, 0 },
			{ 0x1C955978, 13, 0 },
			{ 0xFDA6D833, 16, 0 },
			{ 0x3A57E5D1, 17, 0 },
			{ 0x6ED5F3E7, 18, 0 },
			{ 0x4A5C3872, 21, 0 },
			{ 0x5FE8B032, 24, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	const auto ref_sums = DBARSource { &dBAR };

	auto v = std::make_unique<Verification>();

	SECTION ("Strict verification by track order finds best block")
	{
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

		Checksums actual_sums { // v1 matches block0, v2 matches block1
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

		REQUIRE ( actual_sums.size() == 15 );

		const auto order = std::make_unique<TrackOrderPolicy>();

		auto traversal = std::make_unique<BlockTraversal>();

		const auto result = arcstk::details::create_result(ref_sums.size(),
			actual_sums.size(), traversal->get_policy());

		REQUIRE ( result->total_blocks() == 3 );
		REQUIRE ( result->tracks_per_block() == 15 );
		REQUIRE ( result->size() == 3 + 2 * 3 * 15 );

		v->perform(*result, actual_sums, id, ref_sums, *traversal, *order);

		CHECK ( std::get<0>(result->best_block()) == 1 );
		// Best is 1, the v2 block, but 0, the v1 block, also matches entirely!

		CHECK ( result->best_block_difference() == 0 );

		CHECK ( result->total_unverified_tracks() == 0 );
		CHECK ( result->all_tracks_verified() );

		CHECK ( result->is_verified(0) );
		CHECK ( result->is_verified(1) );
		CHECK ( result->is_verified(2) );
		CHECK ( result->is_verified(3) );
		CHECK ( result->is_verified(4) );
		CHECK ( result->is_verified(5) );
		CHECK ( result->is_verified(6) );
		CHECK ( result->is_verified(7) );
		CHECK ( result->is_verified(8) );
		CHECK ( result->is_verified(9) );
		CHECK ( result->is_verified(10) );
		CHECK ( result->is_verified(11) );
		CHECK ( result->is_verified(12) );
		CHECK ( result->is_verified(13) );
		CHECK ( result->is_verified(14) );

		CHECK ( result->id(0) );
		CHECK ( result->difference(0, false) == 0);
		CHECK ( result->difference(0, true)  == 15);

		CHECK ( result->id(1) );
		CHECK ( result->difference(1, true)  == 0);
		CHECK ( result->difference(1, false) == 15);

		CHECK ( not result->id(2) );
		CHECK ( result->difference(2, true)  == 16); // id does not match either
		CHECK ( result->difference(2, false) == 16);
	}

	SECTION ("Verification by track order is correct")
	{
		ChecksumSet track01( 5192);
		track01.insert(type::ARCS2, Checksum(0xB89992E5));
		track01.insert(type::ARCS1, Checksum(0xFFFFFFFF)); // mismatch

		ChecksumSet track02( 2165);
		track02.insert(type::ARCS2, Checksum(0xFFFFFFFF)); // mismatch
		track02.insert(type::ARCS1, Checksum(0x475F57E9));

		ChecksumSet track03(15885);
		track03.insert(type::ARCS2, Checksum(0xFFFFFFFF)); // mismatch
		track03.insert(type::ARCS1, Checksum(0x7304F1C4));

		ChecksumSet track04(12228);
		track04.insert(type::ARCS2, Checksum(0x9E2187F9));
		track04.insert(type::ARCS1, Checksum(0xF2472287));

		ChecksumSet track05(13925);
		track05.insert(type::ARCS2, Checksum(0x6BE71E50));
		track05.insert(type::ARCS1, Checksum(0x881BC504));

		ChecksumSet track06(19513);
		track06.insert(type::ARCS2, Checksum(0xFFFFFFFF)); // mismatch
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
		track10.insert(type::ARCS1, Checksum(0xFFFFFFFF)); // mismatch

		ChecksumSet track11(40152);
		track11.insert(type::ARCS2, Checksum(0xFFFFFFFF)); // mismatch
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
		track15.insert(type::ARCS1, Checksum(0xFFFFFFFF)); // mismatch

		Checksums actual_sums { // no single block matches each track
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

		REQUIRE ( actual_sums.size() == 15 );

		const auto order = std::make_unique<TrackOrderPolicy>();

		// strict version matching one block
		auto block = std::make_unique<BlockTraversal>();
		const auto b_result = arcstk::details::create_result(ref_sums.size(),
			actual_sums.size(), block->get_policy());

		REQUIRE ( b_result->total_blocks() == 3 );
		REQUIRE ( b_result->tracks_per_block() == 15 );
		REQUIRE ( b_result->size() == 3 + 2 * 3 * 15 );

		// non-strict version just matching every track in at least one block
		auto track = std::make_unique<TrackTraversal>();
		const auto t_result = arcstk::details::create_result(ref_sums.size(),
			actual_sums.size(), track->get_policy());

		REQUIRE ( t_result->total_blocks() == 3 );
		REQUIRE ( t_result->tracks_per_block() == 15 );
		REQUIRE ( t_result->size() == 3 + 2 * 3 * 15 );

		// b_result is result of BlockTraversal
		v->perform(*b_result, actual_sums, id, ref_sums, *block, *order);

		// t_result is result of TrackTraversal
		v->perform(*t_result, actual_sums, id, ref_sums, *track, *order);

		// BlockTraversal:
		// There is no single block that matches all tracks, hence some tracks
		// got verified and others won't!

		CHECK ( std::get<0>(b_result->best_block()) == 0 ); // 0 is the v1 block
		CHECK ( b_result->best_block_difference() == 3 );

		CHECK ( b_result->total_unverified_tracks() == 3 );
		CHECK ( not b_result->all_tracks_verified() );

		CHECK ( not b_result->is_verified(0) ); // mismatch v1
		CHECK ( b_result->is_verified(1) );
		CHECK ( b_result->is_verified(2) );
		CHECK ( b_result->is_verified(3) );
		CHECK ( b_result->is_verified(4) );
		CHECK ( b_result->is_verified(5) );
		CHECK ( b_result->is_verified(6) );
		CHECK ( b_result->is_verified(7) );
		CHECK ( b_result->is_verified(8) );
		CHECK ( not b_result->is_verified(9) ); // mismatch v1
		CHECK ( b_result->is_verified(10) );
		CHECK ( b_result->is_verified(11) );
		CHECK ( b_result->is_verified(12) );
		CHECK ( b_result->is_verified(13) );
		CHECK ( not b_result->is_verified(14) ); // mismatch v1

		CHECK ( b_result->id(0) );
		CHECK ( b_result->difference(0, false) == 3); // total v1 mismatches
		CHECK ( b_result->difference(0, true)  == 15);

		CHECK ( b_result->id(1) );
		CHECK ( b_result->difference(1, true)  == 4);  // total v2 mismatches
		CHECK ( b_result->difference(1, false) == 15);

		CHECK ( not b_result->id(2) );
		CHECK ( b_result->difference(2, true)  == 16); // id does not match either
		CHECK ( b_result->difference(2, false) == 16);

		// TrackTraversal:
		// There is no single block that matches all tracks, but all tracks
		// got verified!

		CHECK ( t_result->total_unverified_tracks() == 0 );
		CHECK ( t_result->all_tracks_verified() );

		CHECK ( t_result->is_verified(0) );
		CHECK ( t_result->is_verified(1) );
		CHECK ( t_result->is_verified(2) );
		CHECK ( t_result->is_verified(3) );
		CHECK ( t_result->is_verified(4) );
		CHECK ( t_result->is_verified(5) );
		CHECK ( t_result->is_verified(6) );
		CHECK ( t_result->is_verified(7) );
		CHECK ( t_result->is_verified(8) );
		CHECK ( t_result->is_verified(9) );
		CHECK ( t_result->is_verified(10) );
		CHECK ( t_result->is_verified(11) );
		CHECK ( t_result->is_verified(12) );
		CHECK ( t_result->is_verified(13) );
		CHECK ( t_result->is_verified(14) );

		CHECK ( t_result->id(0) );
		CHECK ( t_result->difference(0, false) ==  3); // total v1 mismatches
		CHECK ( t_result->difference(0, true)  == 15);

		CHECK ( t_result->id(1) );
		CHECK ( t_result->difference(1, true)  ==  4);  // total v2 mismatches
		CHECK ( t_result->difference(1, false) == 15);

		CHECK ( not t_result->id(2) );
		CHECK ( t_result->difference(2, true)  == 16); // id does not match either
		CHECK ( t_result->difference(2, false) == 16);
	}
}


TEST_CASE ( "details::StrictPolicy", "[strictpolicy]" )
{
	using arcstk::details::create_result;
	using arcstk::details::StrictPolicy;

	auto result = create_result(4, 8, std::make_unique<StrictPolicy>());

	REQUIRE ( result->total_blocks() == 4 );
	REQUIRE ( result->tracks_per_block() == 8 );
	REQUIRE ( result->size() == 68 );
	REQUIRE ( result->strict() );

	result->verify_id(0);
	result->verify_id(1);
	result->verify_id(2);
	result->verify_id(3);

	result->verify_track(0, 2, false); // v1
	result->verify_track(0, 6, true); // v2
	result->verify_track(0, 5, true);

	result->verify_track(1, 1, false); // v1
	result->verify_track(1, 4, false);
	result->verify_track(1, 2, false);
	result->verify_track(1, 5, true); // v2

	result->verify_track(2, 2, false); // v1
	result->verify_track(2, 4, true); // v2
	result->verify_track(2, 5, true);

	result->verify_track(3, 5, false); // v1
	result->verify_track(3, 2, false);
	result->verify_track(3, 4, true); // v2
	result->verify_track(3, 6, true);

	// Best block is 1 with best_diff in v1 type, hence 1,2 and 4 are verified.
	// Tracks 0,3,5,6 and 7 are not verified.

	REQUIRE ( result->strict() );
	REQUIRE ( std::get<0>(result->best_block()) == 1 );
	// FIXME best_block should yield 3, since 3 has more v2 matches than 1)

	REQUIRE ( result->total_unverified_tracks() == 5 );

	REQUIRE ( not result->is_verified(0) );
	REQUIRE (     result->is_verified(1) );
	REQUIRE (     result->is_verified(2) );
	REQUIRE ( not result->is_verified(3) );
	REQUIRE (     result->is_verified(4) );
	REQUIRE ( not result->is_verified(5) );
	REQUIRE ( not result->is_verified(6) );
	REQUIRE ( not result->is_verified(7) );


	SECTION( "is_verified() yields verified tracks correctly" )
	{
		const auto policy = std::make_unique<StrictPolicy>();

		CHECK ( not policy->is_verified(0, *result) );
		CHECK (     policy->is_verified(1, *result) );
		CHECK (     policy->is_verified(2, *result) );
		CHECK ( not policy->is_verified(3, *result) );
		CHECK (     policy->is_verified(4, *result) );
		CHECK ( not policy->is_verified(5, *result) );
		CHECK ( not policy->is_verified(6, *result) );
		CHECK ( not policy->is_verified(7, *result) );
	}

	SECTION( "total_unverified_tracks() counts unverified tracks correctly" )
	{
		const auto policy = std::make_unique<StrictPolicy>();

		CHECK ( policy->total_unverified_tracks(*result) == 5 );
	}
}


TEST_CASE ( "details::LiberalPolicy", "[liberalpolicy]" )
{
	using arcstk::details::create_result;
	using arcstk::details::LiberalPolicy;

	auto result = create_result(4, 8, std::make_unique<LiberalPolicy>());

	REQUIRE ( result->total_blocks() == 4 );
	REQUIRE ( result->tracks_per_block() == 8 );
	REQUIRE ( result->size() == 68 );
	REQUIRE ( not result->strict() );

	result->verify_id(0);
	result->verify_id(1);
	result->verify_id(2);
	result->verify_id(3);

	result->verify_track(0, 2, false); // v1
	result->verify_track(0, 6, true); // v2
	result->verify_track(0, 5, true);

	result->verify_track(1, 1, false); // v1
	result->verify_track(1, 4, false);
	result->verify_track(1, 2, false);
	result->verify_track(1, 5, true); // v2

	result->verify_track(2, 2, false); // v1
	result->verify_track(2, 4, true); // v2
	result->verify_track(2, 5, true);

	result->verify_track(3, 5, false); // v1
	result->verify_track(3, 2, false);
	result->verify_track(3, 4, true); // v2
	result->verify_track(3, 6, true);

	// Tracks 0,3 and 7 are not verified.

	REQUIRE ( not result->strict() );

	REQUIRE ( not result->is_verified(0) );
	REQUIRE ( result->is_verified(1) );
	REQUIRE ( result->is_verified(2) );
	REQUIRE ( not result->is_verified(3) );
	REQUIRE ( result->is_verified(4) );
	REQUIRE ( result->is_verified(5) );
	REQUIRE ( result->is_verified(6) );
	REQUIRE ( not result->is_verified(7) );
	REQUIRE ( result->total_unverified_tracks() == 3 );

	SECTION( "is_verified() yields verified tracks correctly" )
	{
		const auto policy = std::make_unique<LiberalPolicy>();

		CHECK ( not policy->is_verified(0, *result) );
		CHECK ( policy->is_verified(1, *result) );
		CHECK ( policy->is_verified(2, *result) );
		CHECK ( not policy->is_verified(3, *result) );
		CHECK ( policy->is_verified(4, *result) );
		CHECK ( policy->is_verified(5, *result) );
		CHECK ( policy->is_verified(6, *result) );
		CHECK ( not policy->is_verified(7, *result) );
	}

	SECTION( "total_unverified_tracks() counts unverified tracks correctly" )
	{
		const auto policy = std::make_unique<LiberalPolicy>();

		CHECK ( policy->total_unverified_tracks(*result) == 3 );
	}
}


// TODO details::VerifierBase


TEST_CASE ( "AlbumVerifier", "[albumverifier]" )
{
	using arcstk::ARId;
	using arcstk::AlbumVerifier;
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;
	using arcstk::DBAR;

	const auto id = ARId { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} },
		{ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} }
	};

	// TODO Check content of the DBAR

	REQUIRE ( dBAR.size() == 3 );

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

	Checksums actual_sums {
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

	REQUIRE ( actual_sums.size() == 15 );

	// Represents verification in track order, maybe either strict or non-strict
	AlbumVerifier a {actual_sums, id};

	// Check defaults
	REQUIRE ( a.strict() );
	REQUIRE ( a.actual_id() == id );
	REQUIRE ( a.actual_checksums() == actual_sums );

	// common constants

	const bool v1 = false;
	const bool v2 = true;

	// Strict verification (requires all matches in the same blocks)

	SECTION ( "Strict track order verification result has correct size" )
	{
		REQUIRE ( a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( a.strict() );

		CHECK ( result->total_blocks() == 3 );
		CHECK ( result->tracks_per_block() == 15 );
		CHECK ( result->size() == 93 ); // blocks + 2 * blocks * tracks

		CHECK_THROWS ( result->is_verified(15) ); // illegal track

		CHECK_THROWS ( result->id(3) );            // illegal block
		CHECK_THROWS ( result->track(3, 14, v2) ); //         block
		CHECK_THROWS ( result->track(2, 15, v2) ); //         track

		CHECK_THROWS ( result->difference(3, v1) == 0 ); // illegal block
		CHECK_THROWS ( result->difference(3, v2) == 0 );
	}

	SECTION ( "Strict track order verification result has correct differences" )
	{
		REQUIRE ( a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( a.strict() );

		CHECK ( result->difference(0, v1) ==  0 );
		CHECK ( result->difference(0, v2) == 15 );

		CHECK ( result->difference(1, v1) == 15 );
		CHECK ( result->difference(1, v2) ==  0 );

		CHECK ( result->difference(2, v1) == 16 );
		CHECK ( result->difference(2, v2) == 16 );
	}

	SECTION ( "Strict track order verification contains correct flags" )
	{
		REQUIRE ( a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( a.strict() );


		CHECK ( result->strict() );

		// block 0
		CHECK ( result->id(0) );

		CHECK ( result->track(0,  0, v1) );
		CHECK ( result->track(0,  1, v1) );
		CHECK ( result->track(0,  2, v1) );
		CHECK ( result->track(0,  3, v1) );
		CHECK ( result->track(0,  4, v1) );
		CHECK ( result->track(0,  5, v1) );
		CHECK ( result->track(0,  6, v1) );
		CHECK ( result->track(0,  7, v1) );
		CHECK ( result->track(0,  8, v1) );
		CHECK ( result->track(0,  9, v1) );
		CHECK ( result->track(0, 10, v1) );
		CHECK ( result->track(0, 11, v1) );
		CHECK ( result->track(0, 12, v1) );
		CHECK ( result->track(0, 13, v1) );
		CHECK ( result->track(0, 14, v1) );

		CHECK ( not result->track(0,  0, v2) );
		CHECK ( not result->track(0,  1, v2) );
		CHECK ( not result->track(0,  2, v2) );
		CHECK ( not result->track(0,  3, v2) );
		CHECK ( not result->track(0,  4, v2) );
		CHECK ( not result->track(0,  5, v2) );
		CHECK ( not result->track(0,  6, v2) );
		CHECK ( not result->track(0,  7, v2) );
		CHECK ( not result->track(0,  8, v2) );
		CHECK ( not result->track(0,  9, v2) );
		CHECK ( not result->track(0, 10, v2) );
		CHECK ( not result->track(0, 11, v2) );
		CHECK ( not result->track(0, 12, v2) );
		CHECK ( not result->track(0, 13, v2) );
		CHECK ( not result->track(0, 14, v2) );

		// block 1
		CHECK ( result->id(1) );

		CHECK ( not result->track(1,  0, v1) );
		CHECK ( not result->track(1,  1, v1) );
		CHECK ( not result->track(1,  2, v1) );
		CHECK ( not result->track(1,  3, v1) );
		CHECK ( not result->track(1,  4, v1) );
		CHECK ( not result->track(1,  5, v1) );
		CHECK ( not result->track(1,  6, v1) );
		CHECK ( not result->track(1,  7, v1) );
		CHECK ( not result->track(1,  8, v1) );
		CHECK ( not result->track(1,  9, v1) );
		CHECK ( not result->track(1, 10, v1) );
		CHECK ( not result->track(1, 11, v1) );
		CHECK ( not result->track(1, 12, v1) );
		CHECK ( not result->track(1, 13, v1) );
		CHECK ( not result->track(1, 14, v1) );

		CHECK ( result->track(1,  0, v2) );
		CHECK ( result->track(1,  1, v2) );
		CHECK ( result->track(1,  2, v2) );
		CHECK ( result->track(1,  3, v2) );
		CHECK ( result->track(1,  4, v2) );
		CHECK ( result->track(1,  5, v2) );
		CHECK ( result->track(1,  6, v2) );
		CHECK ( result->track(1,  7, v2) );
		CHECK ( result->track(1,  8, v2) );
		CHECK ( result->track(1,  9, v2) );
		CHECK ( result->track(1, 10, v2) );
		CHECK ( result->track(1, 11, v2) );
		CHECK ( result->track(1, 12, v2) );
		CHECK ( result->track(1, 13, v2) );
		CHECK ( result->track(1, 14, v2) );

		// block 2
		CHECK ( not result->id(2) ); // different id! nothing verifies!

		CHECK ( not result->track(2,  0, v1) );
		CHECK ( not result->track(2,  1, v1) );
		CHECK ( not result->track(2,  2, v1) );
		CHECK ( not result->track(2,  3, v1) );
		CHECK ( not result->track(2,  4, v1) );
		CHECK ( not result->track(2,  5, v1) );
		CHECK ( not result->track(2,  6, v1) );
		CHECK ( not result->track(2,  7, v1) );
		CHECK ( not result->track(2,  8, v1) );
		CHECK ( not result->track(2,  9, v1) );
		CHECK ( not result->track(2, 10, v1) );
		CHECK ( not result->track(2, 11, v1) );
		CHECK ( not result->track(2, 12, v1) );
		CHECK ( not result->track(2, 13, v1) );
		CHECK ( not result->track(2, 14, v1) );

		CHECK ( not result->track(2,  0, v2) );
		CHECK ( not result->track(2,  1, v2) );
		CHECK ( not result->track(2,  2, v2) );
		CHECK ( not result->track(2,  3, v2) );
		CHECK ( not result->track(2,  4, v2) );
		CHECK ( not result->track(2,  5, v2) );
		CHECK ( not result->track(2,  6, v2) );
		CHECK ( not result->track(2,  7, v2) );
		CHECK ( not result->track(2,  8, v2) );
		CHECK ( not result->track(2,  9, v2) );
		CHECK ( not result->track(2, 10, v2) );
		CHECK ( not result->track(2, 11, v2) );
		CHECK ( not result->track(2, 12, v2) );
		CHECK ( not result->track(2, 13, v2) );
		CHECK ( not result->track(2, 14, v2) );
	}

	SECTION ( "Strict track order verification yields best block" )
	{
		REQUIRE ( a.strict() );

		const auto result = a.perform(dBAR);
		const auto best_block = result->best_block();

		REQUIRE ( a.strict() );


		CHECK ( std::get<0>(best_block) == 1 );
		CHECK ( std::get<1>(best_block) == v2 );
		CHECK ( std::get<2>(best_block) == 0 );
		CHECK ( result->best_block_difference() == 0 );
	}

	SECTION ( "Strict track order verification verifies tracks correctly" )
	{
		REQUIRE ( a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( a.strict() );

		CHECK ( result->all_tracks_verified() );

		CHECK ( result->is_verified(0) );
		CHECK ( result->is_verified(1) );
		CHECK ( result->is_verified(2) );
		CHECK ( result->is_verified(3) );
		CHECK ( result->is_verified(4) );
		CHECK ( result->is_verified(5) );
		CHECK ( result->is_verified(6) );
		CHECK ( result->is_verified(7) );
		CHECK ( result->is_verified(8) );
		CHECK ( result->is_verified(9) );
		CHECK ( result->is_verified(10) );
		CHECK ( result->is_verified(11) );
		CHECK ( result->is_verified(12) );
		CHECK ( result->is_verified(13) );
		CHECK ( result->is_verified(14) );
	}


	// Non-strict verification (allows matches in multiple blocks)


	SECTION ( "Non-strict track order verification result has correct size" )
	{
		a.set_strict(false);

		REQUIRE ( not a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( not a.strict() );


		CHECK ( result->total_blocks() == 3 );
		CHECK ( result->tracks_per_block() == 15 );
		CHECK ( result->size() == 93 ); // blocks + 2 * blocks * tracks

		CHECK_THROWS ( result->is_verified(15) );

		CHECK_THROWS ( result->id(3) );            // illegal block
		CHECK_THROWS ( result->track(3, 14, v2) ); //         block
		CHECK_THROWS ( result->track(2, 15, v2) ); //         track

		CHECK_THROWS ( result->difference(3, v1) == 0 ); // illegal block
		CHECK_THROWS ( result->difference(3, v2) == 0 );
	}

	SECTION ( "Non-strict track order verification result has correct differences" )
	{
		a.set_strict(false);

		REQUIRE ( not a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( not a.strict() );


		CHECK ( result->difference(0, v1) ==  0 );
		CHECK ( result->difference(0, v2) == 15 );

		CHECK ( result->difference(1, v1) == 15 );
		CHECK ( result->difference(1, v2) ==  0 );

		CHECK ( result->difference(2, v1) == 16 );
		CHECK ( result->difference(2, v2) == 16 );
	}

	SECTION ( "Non-strict track order verification result has correct flags" )
	{
		a.set_strict(false);

		REQUIRE ( not a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( not a.strict() );


		CHECK ( not result->strict() );

		// block 0
		CHECK ( result->id(0) );

		CHECK ( result->track(0,  0, v1) );
		CHECK ( result->track(0,  1, v1) );
		CHECK ( result->track(0,  2, v1) );
		CHECK ( result->track(0,  3, v1) );
		CHECK ( result->track(0,  4, v1) );
		CHECK ( result->track(0,  5, v1) );
		CHECK ( result->track(0,  6, v1) );
		CHECK ( result->track(0,  7, v1) );
		CHECK ( result->track(0,  8, v1) );
		CHECK ( result->track(0,  9, v1) );
		CHECK ( result->track(0, 10, v1) );
		CHECK ( result->track(0, 11, v1) );
		CHECK ( result->track(0, 12, v1) );
		CHECK ( result->track(0, 13, v1) );
		CHECK ( result->track(0, 14, v1) );

		CHECK ( not result->track(0,  0, v2) );
		CHECK ( not result->track(0,  1, v2) );
		CHECK ( not result->track(0,  2, v2) );
		CHECK ( not result->track(0,  3, v2) );
		CHECK ( not result->track(0,  4, v2) );
		CHECK ( not result->track(0,  5, v2) );
		CHECK ( not result->track(0,  6, v2) );
		CHECK ( not result->track(0,  7, v2) );
		CHECK ( not result->track(0,  8, v2) );
		CHECK ( not result->track(0,  9, v2) );
		CHECK ( not result->track(0, 10, v2) );
		CHECK ( not result->track(0, 11, v2) );
		CHECK ( not result->track(0, 12, v2) );
		CHECK ( not result->track(0, 13, v2) );
		CHECK ( not result->track(0, 14, v2) );

		// 1
		CHECK ( result->id(1) );

		CHECK ( not result->track(1,  0, v1) );
		CHECK ( not result->track(1,  1, v1) );
		CHECK ( not result->track(1,  2, v1) );
		CHECK ( not result->track(1,  3, v1) );
		CHECK ( not result->track(1,  4, v1) );
		CHECK ( not result->track(1,  5, v1) );
		CHECK ( not result->track(1,  6, v1) );
		CHECK ( not result->track(1,  7, v1) );
		CHECK ( not result->track(1,  8, v1) );
		CHECK ( not result->track(1,  9, v1) );
		CHECK ( not result->track(1, 10, v1) );
		CHECK ( not result->track(1, 11, v1) );
		CHECK ( not result->track(1, 12, v1) );
		CHECK ( not result->track(1, 13, v1) );
		CHECK ( not result->track(1, 14, v1) );

		CHECK ( result->track(1,  0, v2) );
		CHECK ( result->track(1,  1, v2) );
		CHECK ( result->track(1,  2, v2) );
		CHECK ( result->track(1,  3, v2) );
		CHECK ( result->track(1,  4, v2) );
		CHECK ( result->track(1,  5, v2) );
		CHECK ( result->track(1,  6, v2) );
		CHECK ( result->track(1,  7, v2) );
		CHECK ( result->track(1,  8, v2) );
		CHECK ( result->track(1,  9, v2) );
		CHECK ( result->track(1, 10, v2) );
		CHECK ( result->track(1, 11, v2) );
		CHECK ( result->track(1, 12, v2) );
		CHECK ( result->track(1, 13, v2) );
		CHECK ( result->track(1, 14, v2) );

		// 2
		CHECK ( not result->id(2) ); // different id! nothing verifies!

		CHECK ( not result->track(2,  0, v1) );
		CHECK ( not result->track(2,  1, v1) );
		CHECK ( not result->track(2,  2, v1) );
		CHECK ( not result->track(2,  3, v1) );
		CHECK ( not result->track(2,  4, v1) );
		CHECK ( not result->track(2,  5, v1) );
		CHECK ( not result->track(2,  6, v1) );
		CHECK ( not result->track(2,  7, v1) );
		CHECK ( not result->track(2,  8, v1) );
		CHECK ( not result->track(2,  9, v1) );
		CHECK ( not result->track(2, 10, v1) );
		CHECK ( not result->track(2, 11, v1) );
		CHECK ( not result->track(2, 12, v1) );
		CHECK ( not result->track(2, 13, v1) );
		CHECK ( not result->track(2, 14, v1) );

		CHECK ( not result->track(2,  0, v2) );
		CHECK ( not result->track(2,  1, v2) );
		CHECK ( not result->track(2,  2, v2) );
		CHECK ( not result->track(2,  3, v2) );
		CHECK ( not result->track(2,  4, v2) );
		CHECK ( not result->track(2,  5, v2) );
		CHECK ( not result->track(2,  6, v2) );
		CHECK ( not result->track(2,  7, v2) );
		CHECK ( not result->track(2,  8, v2) );
		CHECK ( not result->track(2,  9, v2) );
		CHECK ( not result->track(2, 10, v2) );
		CHECK ( not result->track(2, 11, v2) );
		CHECK ( not result->track(2, 12, v2) );
		CHECK ( not result->track(2, 13, v2) );
		CHECK ( not result->track(2, 14, v2) );
	}

	SECTION ( "Non-strict track order verification yields best block" )
	{
		a.set_strict(false);

		REQUIRE ( not a.strict() );

		const auto result = a.perform(dBAR);
		const auto best_block = result->best_block();

		REQUIRE ( not a.strict() );

		CHECK ( std::get<0>(best_block) == 1 );
		CHECK ( std::get<1>(best_block) == v2 );
		CHECK ( std::get<2>(best_block) == 0 );
		CHECK ( result->best_block_difference() == 0 );
	}

	SECTION ( "Non-strict track order verification verifies tracks correctly" )
	{
		a.set_strict(false);

		REQUIRE ( not a.strict() );

		const auto result = a.perform(dBAR);

		REQUIRE ( not a.strict() );


		CHECK ( result->all_tracks_verified() );

		CHECK ( result->is_verified(0) );
		CHECK ( result->is_verified(1) );
		CHECK ( result->is_verified(2) );
		CHECK ( result->is_verified(3) );
		CHECK ( result->is_verified(4) );
		CHECK ( result->is_verified(5) );
		CHECK ( result->is_verified(6) );
		CHECK ( result->is_verified(7) );
		CHECK ( result->is_verified(8) );
		CHECK ( result->is_verified(9) );
		CHECK ( result->is_verified(10) );
		CHECK ( result->is_verified(11) );
		CHECK ( result->is_verified(12) );
		CHECK ( result->is_verified(13) );
		CHECK ( result->is_verified(14) );
	}
}


TEST_CASE ( "TracksetVerifier", "[tracksetverifier]" )
{
	using arcstk::ARId;
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;
	using arcstk::DBAR;
	using arcstk::TracksetVerifier;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xC89192E5, 0, 0 },
			{ 0x4F78EB03, 0, 0 },
			{ 0x56582281, 0, 0 },
			{ 0x0E2187F9, 0, 0 },
			{ 0x2BE71E50, 0, 0 },
			{ 0x01E7235D, 0, 0 },
			{ 0xD8F6763C, 0, 0 },
			{ 0x8480331E, 0, 0 },
			{ 0x42F5061C, 0, 0 },
			{ 0x47D70F02, 0, 0 },
			{ 0xBABF08AA, 0, 0 },
			{ 0x563EFECB, 0, 0 },
			{ 0xAB123C9C, 0, 0 },
			{ 0xB65C20E4, 0, 0 },
			{ 0x68FC3C3E, 0, 0 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0 },
			{ 0x4F77EB03,  8, 0 },
			{ 0x56582282,  7, 0 },
			{ 0x9E2187F9,  9, 0 },
			{ 0x6BE71E50,  2, 0 },
			{ 0x01E7235F,  1, 0 },
			{ 0xD8F7763C,  0, 0 },
			{ 0x8480223E, 13, 0 },
			{ 0x42C5061C, 16, 0 },
			{ 0x47A70F02, 17, 0 },
			{ 0xBABF08CC, 18, 0 },
			{ 0x563EDCCB, 21, 0 },
			{ 0xAB123C7C, 14, 0 },
			{ 0xC65C20E4, 26, 0 },
			{ 0x58FC3C3E, 28, 0 }
		} }
	};

	REQUIRE ( dBAR.size() == 3 );

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

	Checksums actual_sums { /* some random order */
		track04,
		track11,
		track03,
		track05,
		track07,
		track06,
		track15,
		track08,
		track02,
		track09,
		track01,
		track10,
		track12,
		track14,
		track13,
	};

	REQUIRE ( actual_sums.size() == 15 );


	TracksetVerifier t { actual_sums };

	REQUIRE ( t.strict() );
	REQUIRE ( t.actual_id() == arcstk::EmptyARId);
	REQUIRE ( t.actual_checksums() == actual_sums );

	// common constants

	const bool v1 = false;
	const bool v2 = true;


	SECTION ( "Strict random order verification result has correct size" )
	{
		REQUIRE ( t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( t.strict() );


		CHECK ( result->total_blocks() == 3 );
		CHECK ( result->tracks_per_block() == 15 );
		CHECK ( result->size() == 93 ); // 2 * blocks * tracks + blocks

		CHECK_THROWS ( result->is_verified(15) );

		CHECK_THROWS ( result->id(3) );            // illegal block
		CHECK_THROWS ( result->track(3, 14, v2) ); //         block
		CHECK_THROWS ( result->track(2, 15, v2) ); //         track

		CHECK_THROWS ( result->difference(3, v1) == 0 ); // illegal block
		CHECK_THROWS ( result->difference(3, v2) == 0 );
	}

	SECTION ( "Strict random order verification result has correct "
			"differences" )
	{
		REQUIRE ( t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( t.strict() );


		CHECK ( result->difference(0, v1) == 15 );
		CHECK ( result->difference(0, v2) == 15 );

		CHECK ( result->difference(1, v1) ==  0 );
		CHECK ( result->difference(1, v2) == 15 );

		CHECK ( result->difference(2, v1) == 15 );
		CHECK ( result->difference(2, v2) ==  0 );
	}

	SECTION ( "Strict random order verification result has correct flags" )
	{
		REQUIRE ( t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( t.strict() );


		CHECK ( result->strict() );

		// 0
		CHECK ( result->id(0) ); // only mismatches

		CHECK ( not result->track(0,  0, v1) );
		CHECK ( not result->track(0,  1, v1) );
		CHECK ( not result->track(0,  2, v1) );
		CHECK ( not result->track(0,  3, v1) );
		CHECK ( not result->track(0,  4, v1) );
		CHECK ( not result->track(0,  5, v1) );
		CHECK ( not result->track(0,  6, v1) );
		CHECK ( not result->track(0,  7, v1) );
		CHECK ( not result->track(0,  8, v1) );
		CHECK ( not result->track(0,  9, v1) );
		CHECK ( not result->track(0, 10, v1) );
		CHECK ( not result->track(0, 11, v1) );
		CHECK ( not result->track(0, 12, v1) );
		CHECK ( not result->track(0, 13, v1) );
		CHECK ( not result->track(0, 14, v1) );

		CHECK ( not result->track(0,  0, v2) );
		CHECK ( not result->track(0,  1, v2) );
		CHECK ( not result->track(0,  2, v2) );
		CHECK ( not result->track(0,  3, v2) );
		CHECK ( not result->track(0,  4, v2) );
		CHECK ( not result->track(0,  5, v2) );
		CHECK ( not result->track(0,  6, v2) );
		CHECK ( not result->track(0,  7, v2) );
		CHECK ( not result->track(0,  8, v2) );
		CHECK ( not result->track(0,  9, v2) );
		CHECK ( not result->track(0, 10, v2) );
		CHECK ( not result->track(0, 11, v2) );
		CHECK ( not result->track(0, 12, v2) );
		CHECK ( not result->track(0, 13, v2) );
		CHECK ( not result->track(0, 14, v2) );

		// 1
		CHECK ( result->id(1) ); // all v1 match

		CHECK ( result->track(1,  0, v1) );
		CHECK ( result->track(1,  1, v1) );
		CHECK ( result->track(1,  2, v1) );
		CHECK ( result->track(1,  3, v1) );
		CHECK ( result->track(1,  4, v1) );
		CHECK ( result->track(1,  5, v1) );
		CHECK ( result->track(1,  6, v1) );
		CHECK ( result->track(1,  7, v1) );
		CHECK ( result->track(1,  8, v1) );
		CHECK ( result->track(1,  9, v1) );
		CHECK ( result->track(1, 10, v1) );
		CHECK ( result->track(1, 11, v1) );
		CHECK ( result->track(1, 12, v1) );
		CHECK ( result->track(1, 13, v1) );
		CHECK ( result->track(1, 14, v1) );

		CHECK ( not result->track(1,  0, v2) );
		CHECK ( not result->track(1,  1, v2) );
		CHECK ( not result->track(1,  2, v2) );
		CHECK ( not result->track(1,  3, v2) );
		CHECK ( not result->track(1,  4, v2) );
		CHECK ( not result->track(1,  5, v2) );
		CHECK ( not result->track(1,  6, v2) );
		CHECK ( not result->track(1,  7, v2) );
		CHECK ( not result->track(1,  8, v2) );
		CHECK ( not result->track(1,  9, v2) );
		CHECK ( not result->track(1, 10, v2) );
		CHECK ( not result->track(1, 11, v2) );
		CHECK ( not result->track(1, 12, v2) );
		CHECK ( not result->track(1, 13, v2) );
		CHECK ( not result->track(1, 14, v2) );

		// 2
		CHECK ( result->id(2) ); // all v2 match

		CHECK ( not result->track(2,  0, v1) );
		CHECK ( not result->track(2,  1, v1) );
		CHECK ( not result->track(2,  2, v1) );
		CHECK ( not result->track(2,  3, v1) );
		CHECK ( not result->track(2,  4, v1) );
		CHECK ( not result->track(2,  5, v1) );
		CHECK ( not result->track(2,  6, v1) );
		CHECK ( not result->track(2,  7, v1) );
		CHECK ( not result->track(2,  8, v1) );
		CHECK ( not result->track(2,  9, v1) );
		CHECK ( not result->track(2, 10, v1) );
		CHECK ( not result->track(2, 11, v1) );
		CHECK ( not result->track(2, 12, v1) );
		CHECK ( not result->track(2, 13, v1) );
		CHECK ( not result->track(2, 14, v1) );

		CHECK ( result->track(2,  0, v2) );
		CHECK ( result->track(2,  1, v2) );
		CHECK ( result->track(2,  2, v2) );
		CHECK ( result->track(2,  3, v2) );
		CHECK ( result->track(2,  4, v2) );
		CHECK ( result->track(2,  5, v2) );
		CHECK ( result->track(2,  6, v2) );
		CHECK ( result->track(2,  7, v2) );
		CHECK ( result->track(2,  8, v2) );
		CHECK ( result->track(2,  9, v2) );
		CHECK ( result->track(2, 10, v2) );
		CHECK ( result->track(2, 11, v2) );
		CHECK ( result->track(2, 12, v2) );
		CHECK ( result->track(2, 13, v2) );
		CHECK ( result->track(2, 14, v2) );
	}

	SECTION ( "Strict random order verification yields best block" )
	{
		REQUIRE ( t.strict() );

		const auto result = t.perform(dBAR);
		const auto best_block = result->best_block();

		REQUIRE ( t.strict() );


		CHECK ( std::get<0>(best_block) == 2 );    // correct block
		CHECK ( std::get<1>(best_block) == true ); // is v2
		CHECK ( std::get<2>(best_block) == 0 );    // has zero difference

		CHECK ( result->best_block_difference() == 0 );
	}

	SECTION ( "Strict random order verification verifies tracks correctly" )
	{
		REQUIRE ( t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( t.strict() );


		CHECK ( result->all_tracks_verified() );

		CHECK ( result->is_verified(0) );
		CHECK ( result->is_verified(1) );
		CHECK ( result->is_verified(2) );
		CHECK ( result->is_verified(3) );
		CHECK ( result->is_verified(4) );
		CHECK ( result->is_verified(5) );
		CHECK ( result->is_verified(6) );
		CHECK ( result->is_verified(7) );
		CHECK ( result->is_verified(8) );
		CHECK ( result->is_verified(9) );
		CHECK ( result->is_verified(10) );
		CHECK ( result->is_verified(11) );
		CHECK ( result->is_verified(12) );
		CHECK ( result->is_verified(13) );
		CHECK ( result->is_verified(14) );
	}


	// Non-strict verification (allows matches in multiple blocks)


	SECTION ( "Non-strict random order verification result has correct size" )
	{
		t.set_strict(false);

		REQUIRE ( not t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( not t.strict() );

		CHECK ( result->total_blocks() == 3 );
		CHECK ( result->tracks_per_block() == 15 );
		CHECK ( result->size() == 93 ); // 2 * blocks * tracks + blocks

		CHECK_THROWS ( result->is_verified(15) );

		CHECK_THROWS ( result->id(3) );            // illegal block
		CHECK_THROWS ( result->track(3, 14, v2) ); //         block
		CHECK_THROWS ( result->track(2, 15, v2) ); //         track

		CHECK_THROWS ( result->difference(3, v1) == 0 ); // illegal block
		CHECK_THROWS ( result->difference(3, v2) == 0 );
	}

	SECTION ( "Non-strict random order verification result has correct "
			"differences" )
	{
		t.set_strict(false);

		REQUIRE ( not t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( not t.strict() );


		CHECK ( result->difference(0, v1) == 15 );
		CHECK ( result->difference(0, v2) == 15 );

		CHECK ( result->difference(1, v1) ==  0 );
		CHECK ( result->difference(1, v2) == 15 );

		CHECK ( result->difference(2, v1) == 15 );
		CHECK ( result->difference(2, v2) ==  0 );
	}

	SECTION ( "Non-strict random order verification result has correct flags" )
	{
		t.set_strict(false);

		REQUIRE ( not t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( not t.strict() );


		CHECK ( not result->strict() );

		// 0
		CHECK ( result->id(0) ); // only mismatches

		CHECK ( not result->track(0,  0, v1) );
		CHECK ( not result->track(0,  1, v1) );
		CHECK ( not result->track(0,  2, v1) );
		CHECK ( not result->track(0,  3, v1) );
		CHECK ( not result->track(0,  4, v1) );
		CHECK ( not result->track(0,  5, v1) );
		CHECK ( not result->track(0,  6, v1) );
		CHECK ( not result->track(0,  7, v1) );
		CHECK ( not result->track(0,  8, v1) );
		CHECK ( not result->track(0,  9, v1) );
		CHECK ( not result->track(0, 10, v1) );
		CHECK ( not result->track(0, 11, v1) );
		CHECK ( not result->track(0, 12, v1) );
		CHECK ( not result->track(0, 13, v1) );
		CHECK ( not result->track(0, 14, v1) );

		CHECK ( not result->track(0,  0, v2) );
		CHECK ( not result->track(0,  1, v2) );
		CHECK ( not result->track(0,  2, v2) );
		CHECK ( not result->track(0,  3, v2) );
		CHECK ( not result->track(0,  4, v2) );
		CHECK ( not result->track(0,  5, v2) );
		CHECK ( not result->track(0,  6, v2) );
		CHECK ( not result->track(0,  7, v2) );
		CHECK ( not result->track(0,  8, v2) );
		CHECK ( not result->track(0,  9, v2) );
		CHECK ( not result->track(0, 10, v2) );
		CHECK ( not result->track(0, 11, v2) );
		CHECK ( not result->track(0, 12, v2) );
		CHECK ( not result->track(0, 13, v2) );
		CHECK ( not result->track(0, 14, v2) );

		// 1
		CHECK ( result->id(1) ); // all v1 match

		CHECK ( result->track(1,  0, v1) );
		CHECK ( result->track(1,  1, v1) );
		CHECK ( result->track(1,  2, v1) );
		CHECK ( result->track(1,  3, v1) );
		CHECK ( result->track(1,  4, v1) );
		CHECK ( result->track(1,  5, v1) );
		CHECK ( result->track(1,  6, v1) );
		CHECK ( result->track(1,  7, v1) );
		CHECK ( result->track(1,  8, v1) );
		CHECK ( result->track(1,  9, v1) );
		CHECK ( result->track(1, 10, v1) );
		CHECK ( result->track(1, 11, v1) );
		CHECK ( result->track(1, 12, v1) );
		CHECK ( result->track(1, 13, v1) );
		CHECK ( result->track(1, 14, v1) );

		CHECK ( not result->track(1,  0, v2) );
		CHECK ( not result->track(1,  1, v2) );
		CHECK ( not result->track(1,  2, v2) );
		CHECK ( not result->track(1,  3, v2) );
		CHECK ( not result->track(1,  4, v2) );
		CHECK ( not result->track(1,  5, v2) );
		CHECK ( not result->track(1,  6, v2) );
		CHECK ( not result->track(1,  7, v2) );
		CHECK ( not result->track(1,  8, v2) );
		CHECK ( not result->track(1,  9, v2) );
		CHECK ( not result->track(1, 10, v2) );
		CHECK ( not result->track(1, 11, v2) );
		CHECK ( not result->track(1, 12, v2) );
		CHECK ( not result->track(1, 13, v2) );
		CHECK ( not result->track(1, 14, v2) );

		// 2
		CHECK ( result->id(2) ); // all v2 match

		CHECK ( not result->track(2,  0, v1) );
		CHECK ( not result->track(2,  1, v1) );
		CHECK ( not result->track(2,  2, v1) );
		CHECK ( not result->track(2,  3, v1) );
		CHECK ( not result->track(2,  4, v1) );
		CHECK ( not result->track(2,  5, v1) );
		CHECK ( not result->track(2,  6, v1) );
		CHECK ( not result->track(2,  7, v1) );
		CHECK ( not result->track(2,  8, v1) );
		CHECK ( not result->track(2,  9, v1) );
		CHECK ( not result->track(2, 10, v1) );
		CHECK ( not result->track(2, 11, v1) );
		CHECK ( not result->track(2, 12, v1) );
		CHECK ( not result->track(2, 13, v1) );
		CHECK ( not result->track(2, 14, v1) );

		CHECK ( result->track(2,  0, v2) );
		CHECK ( result->track(2,  1, v2) );
		CHECK ( result->track(2,  2, v2) );
		CHECK ( result->track(2,  3, v2) );
		CHECK ( result->track(2,  4, v2) );
		CHECK ( result->track(2,  5, v2) );
		CHECK ( result->track(2,  6, v2) );
		CHECK ( result->track(2,  7, v2) );
		CHECK ( result->track(2,  8, v2) );
		CHECK ( result->track(2,  9, v2) );
		CHECK ( result->track(2, 10, v2) );
		CHECK ( result->track(2, 11, v2) );
		CHECK ( result->track(2, 12, v2) );
		CHECK ( result->track(2, 13, v2) );
		CHECK ( result->track(2, 14, v2) );
	}

	SECTION ( "Non-strict random order verification yields best block" )
	{
		t.set_strict(false);

		REQUIRE ( not t.strict() );

		const auto result = t.perform(dBAR);
		const auto best_block = result->best_block();

		REQUIRE ( not t.strict() );


		CHECK ( std::get<0>(best_block) == 2 );    // correct block
		CHECK ( std::get<1>(best_block) == true ); // is v2
		CHECK ( std::get<2>(best_block) == 0 );    // has zero difference

		CHECK ( result->best_block_difference() == 0 );
	}

	SECTION ( "Non-strict random order verification verifies tracks correctly" )
	{
		t.set_strict(false);

		REQUIRE ( not t.strict() );

		const auto result = t.perform(dBAR);

		REQUIRE ( not t.strict() );


		CHECK ( result->all_tracks_verified() );

		CHECK ( result->is_verified(0) );
		CHECK ( result->is_verified(1) );
		CHECK ( result->is_verified(2) );
		CHECK ( result->is_verified(3) );
		CHECK ( result->is_verified(4) );
		CHECK ( result->is_verified(5) );
		CHECK ( result->is_verified(6) );
		CHECK ( result->is_verified(7) );
		CHECK ( result->is_verified(8) );
		CHECK ( result->is_verified(9) );
		CHECK ( result->is_verified(10) );
		CHECK ( result->is_verified(11) );
		CHECK ( result->is_verified(12) );
		CHECK ( result->is_verified(13) );
		CHECK ( result->is_verified(14) );
	}
}

