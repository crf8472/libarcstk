#include "catch2/catch_test_macros.hpp"

#ifndef __LIBARCSTK_VERIFY_HPP__
#include "verify.hpp"
#endif
#ifndef __LIBARCSTK_VERIFY_DETAILS_HPP__
#include "verify_details.hpp"
#endif

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_PARSE_HPP__
#include "parse.hpp"
#endif


/**
 * \file Fixtures for classes in module "verify"
 */


TEST_CASE ( "FromResponse", "[FromResponse]" )
{
	using arcstk::ARId;
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::FromResponse;

	// Construct ARResponse by hand

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: v2 values, id from block 1
	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: different id, slightly different values than block 1
	ARBlock block2( /* id */ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
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
		}
	);

	// TODO Check content of the block instances

	ARResponse response { block0, block1, block2 };

	auto r = FromResponse { &response };
	auto r_copy_ctor = FromResponse { r }; // copy constructed

	auto r_copy_ass = FromResponse { nullptr };

	REQUIRE ( r_copy_ass.source() == nullptr );

	r_copy_ass = r; // copy assigned

	SECTION ( "ChecksumSoure of ARResponse is constructed correctly" )
	{
		CHECK ( &response == r.source() );
	}

	SECTION ( "ChecksumSoure of ARResponse is copy-constructed correctly" )
	{
		CHECK (  r_copy_ctor.source() == &response );
		CHECK ( &r_copy_ctor          != &r );
	}

	SECTION ( "ChecksumSoure of ARResponse is copy-assigned correctly" )
	{
		CHECK (  r_copy_ass.source() == &response );
		CHECK ( &r_copy_ass          != &r );
	}

	SECTION ( "Access on response data is correct" )
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


// TODO details::BestBlock
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
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::FromResponse;

	// Construct ARResponse by hand

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: v2 values, id from block 1
	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: different id, slightly different values than block 1
	ARBlock block2( /* id */ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
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
		}
	);

	const auto response = ARResponse { block0, block1, block2 };
	const auto r = FromResponse { &response };
	const auto b = BlockSelector {};

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
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::FromResponse;

	// Construct ARResponse by hand

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: v2 values, id from block 1
	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: different id, slightly different values than block 1
	ARBlock block2( /* id */ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
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
		}
	);

	const auto response = ARResponse { block0, block1, block2 };
	const auto r = FromResponse { &response };
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
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::FromResponse;

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: v2 values, id from block 1
	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: different id, slightly different values than block 1
	ARBlock block2( /* id */ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
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
		}
	);

	const auto response = ARResponse { block0, block1, block2 };
	const auto r = FromResponse { &response };
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
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::FromResponse;

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: v2 values, id from block 1
	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: different id, slightly different values than block 1
	ARBlock block2( /* id */ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
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
		}
	);

	const auto response = ARResponse { block0, block1, block2 };

	REQUIRE ( response.size() == 3 );
	REQUIRE ( response[0] == block0 );
	REQUIRE ( response[1] == block1 );
	REQUIRE ( response[2] == block2 );

	const auto r = FromResponse { &response };
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
		auto i = block_start;
		CHECK ( i.current() == 1 ); // block
		CHECK ( i.counter() == 0 ); // track

		CHECK (     i->value() == 0xB89992E5 );
		CHECK ( (++i)->value() == 0x4F77EB03 );
		CHECK ( (++i)->value() == 0x56582282 );
		CHECK ( (++i)->value() == 0x9E2187F9 );
		CHECK ( (++i)->value() == 0x6BE71E50 );
		CHECK ( (++i)->value() == 0x01E7235F );
		CHECK ( (++i)->value() == 0xD8F7763C );
		CHECK ( (++i)->value() == 0x8480223E );
		CHECK ( (++i)->value() == 0x42C5061C );
		CHECK ( (++i)->value() == 0x47A70F02 );
		CHECK ( (++i)->value() == 0xBABF08CC );
		CHECK ( (++i)->value() == 0x563EDCCB );
		CHECK ( (++i)->value() == 0xAB123C7C );
		CHECK ( (++i)->value() == 0xC65C20E4 );
		CHECK ( (++i)->value() == 0x58FC3C3E );

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
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::FromResponse;

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: v2 values, id from block 1
	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: different id, slightly different values than block 1
	ARBlock block2( /* id */ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
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
		}
	);

	const auto response = ARResponse { block0, block1, block2 };

	REQUIRE ( response.size() == 3 );
	REQUIRE ( response[0] == block0 );
	REQUIRE ( response[1] == block1 );
	REQUIRE ( response[2] == block2 );

	const auto r = FromResponse { &response };
	TrackTraversal t;

	REQUIRE ( not t.get_policy()->is_strict() );

	t.set_source(r);
	REQUIRE ( t.source() == &r );

	t.set_current(3);
	REQUIRE ( t.current() == 3 ); // 0-based track

	REQUIRE ( t.current_block(t.begin()) == 0 );
	REQUIRE ( t.current_track(t.begin()) == 3 );

	auto track_start = t.begin();
	REQUIRE ( track_start.counter() == 0 );

	auto track_end = t.end();
	REQUIRE ( track_end.counter() == 3 );


	SECTION ( "TrackTraversal traverses current() correctly" )
	{
		auto i = track_start;
		CHECK ( i.current() == 3 ); // 0-based track
		CHECK ( i.counter() == 0 );

		CHECK (     i->value() == 0xF2472287 );
		CHECK ( (++i)->value() == 0x9E2187F9 );
		CHECK ( (++i)->value() == 0x0E2187F9 );

		CHECK ( i.current() == 3 ); // 0-based track
		CHECK ( i.counter() == 2 );

		CHECK ( ++i == track_end );
		CHECK ( i.counter() == 3 );
	}
}


TEST_CASE ( "details::TrackOrderPolicy", "[trackorderpolicy]" )
{
	using arcstk::details::TrackOrderPolicy;

	// Construct the checksums by hand

	// From: "Bach: Organ Concertos", Simon Preston, DGG
	// URL:       http://www.accuraterip.com/accuraterip/8/7/1/dBAR-015-001b9178-014be24e-b40d2d0f.bin
	// Filename:  dBAR-015-001b9178-014be24e-b40d2d0f.bin

	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;
	using arcstk::checksum::type;

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


	using arcstk::ARBlock;
	ARBlock block( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
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
		}
	);

	//

	auto result = arcstk::details::create_result(3, 15,
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
	using arcstk::details::FindOrderPolicy;

	// Construct the checksums by hand

	// From: "Bach: Organ Concertos", Simon Preston, DGG
	// URL:       http://www.accuraterip.com/accuraterip/8/7/1/dBAR-015-001b9178-014be24e-b40d2d0f.bin
	// Filename:  dBAR-015-001b9178-014be24e-b40d2d0f.bin

	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;
	using arcstk::checksum::type;

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


	using arcstk::ARBlock;
	ARBlock block( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
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
		}
	);

	//

	auto result = arcstk::details::create_result(3, 15,
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
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::FromResponse;

	// Construct ARResponse by hand

	ARId id { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F };

	// Define block: v1 values
	ARBlock block0( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: v2 values, id from block 1
	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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
		}
	);

	// Define block: different id, slightly different values than block 1
	ARBlock block2( /* id */ { 23, 0x001F9177, 0x024BE24E, 0xFF0D2D0F },
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
		}
	);

	// TODO Check content of the block instances

	ARResponse response { block0, block1, block2 };

	auto ref_sums = FromResponse { &response };

	using arcstk::details::Verification;
	auto v = std::make_unique<Verification>();

	SECTION ("Strict verification by track order finds best block")
	{
		using arcstk::Checksum;
		using arcstk::ChecksumSet;
		using arcstk::Checksums;
		using arcstk::checksum::type;

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

		using arcstk::details::BlockTraversal;
		using arcstk::details::TrackOrderPolicy;

		const auto order = std::make_unique<TrackOrderPolicy>();

		auto traversal = std::make_unique<BlockTraversal>();
		auto result = arcstk::details::create_result(ref_sums.size(),
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
		using arcstk::Checksum;
		using arcstk::ChecksumSet;
		using arcstk::Checksums;
		using arcstk::checksum::type;

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

		using arcstk::details::BlockTraversal;
		using arcstk::details::TrackTraversal;
		using arcstk::details::TrackOrderPolicy;

		const auto order = std::make_unique<TrackOrderPolicy>();

		// strict version matching one block
		auto block = std::make_unique<BlockTraversal>();
		auto b_result = arcstk::details::create_result(ref_sums.size(),
			actual_sums.size(), block->get_policy());

		REQUIRE ( b_result->total_blocks() == 3 );
		REQUIRE ( b_result->tracks_per_block() == 15 );
		REQUIRE ( b_result->size() == 3 + 2 * 3 * 15 );

		// non-strict version just matching every track in at least one block
		auto track = std::make_unique<TrackTraversal>();
		auto t_result = arcstk::details::create_result(ref_sums.size(),
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


TEST_CASE ( "AlbumVerifier", "[albumverifier]" )
{
	using arcstk::ARId;
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;

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

	auto response = ARResponse { block0, block1, block2 };

	// TODO Check content of the ARResponse

	REQUIRE ( response.size() == 3 );
	REQUIRE ( response[0] == block0 );
	REQUIRE ( response[1] == block1 );
	REQUIRE ( response[2] == block2 );


	// common constants

	const bool v1 = false;
	const bool v2 = true;

	SECTION ( "Strict verification is correct for matching block" )
	{
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

		arcstk::AlbumVerifier a(actual_sums, id);

		REQUIRE ( a.strict() );
		REQUIRE ( a.actual_id() == id );
		REQUIRE ( a.actual_checksums() == actual_sums );

		const auto result = a.perform(response);
		const auto best_block = result->best_block();

		REQUIRE ( a.strict() );

		CHECK ( result->total_blocks() == 3 );
		CHECK ( result->tracks_per_block() == 15 );
		CHECK ( result->size() == 93 ); // blocks + 2 * blocks * tracks

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
		CHECK_THROWS ( result->is_verified(15) );

		CHECK ( std::get<0>(best_block) == 2 );
		CHECK ( std::get<1>(best_block) == v2 );
		CHECK ( std::get<2>(best_block) == 0 );
		CHECK ( result->best_block_difference() == 0 );

		// 0
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
		CHECK ( not result->id(1) ); // different id! nothing verifies!

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
		CHECK ( result->id(2) );

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

		CHECK_THROWS ( result->id(3) );            // illegal block
		CHECK_THROWS ( result->track(3, 14, v2) ); //         block
		CHECK_THROWS ( result->track(2, 15, v2) ); //         track

		CHECK ( result->difference(0, v1) ==  0 );
		CHECK ( result->difference(0, v2) == 15 );

		CHECK ( result->difference(1, v1) == 16 );
		CHECK ( result->difference(1, v2) == 16 );

		CHECK ( result->difference(2, v1) == 15 );
		CHECK ( result->difference(2, v2) ==  0 );

		CHECK_THROWS ( result->difference(3, v1) == 0 );
		CHECK_THROWS ( result->difference(3, v2) == 0 );
	}
}


TEST_CASE ( "TracksetVerifier", "[tracksetverifier]" )
{
	using arcstk::ARId;
	using arcstk::ARBlock;
	using arcstk::ARResponse;
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;

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


	// construct strict order-free verification result

	//t = std::make_unique<arcstk::details::TraverseBlock>();
	//o = std::make_unique<arcstk::details::UnknownOrder>();
	//const auto sov_result = arcstk::verify(result1, arcstk::EmptyARId,
	//		arcstk::FromResponse(&response), *t, *o);
	arcstk::TracksetVerifier t(result1);
	const auto sov_result = t.perform(response);

	const auto sov_best_block = sov_result->best_block();

	SECTION ( "Non-Strict TrackOrder Verification is successful" )
	{
		//CHECK ( sov_result->all_tracks_verified() );

		CHECK ( sov_result->is_verified(0) );
		CHECK ( sov_result->is_verified(1) );
		CHECK ( sov_result->is_verified(2) );
		CHECK ( sov_result->is_verified(3) );
		CHECK ( sov_result->is_verified(4) );
		CHECK ( sov_result->is_verified(5) );
		CHECK ( sov_result->is_verified(6) );
		CHECK ( sov_result->is_verified(7) );
		CHECK ( sov_result->is_verified(8) );
		CHECK ( sov_result->is_verified(9) );
		CHECK ( sov_result->is_verified(10) );
		CHECK ( sov_result->is_verified(11) );
		CHECK ( sov_result->is_verified(12) );
		CHECK ( sov_result->is_verified(13) );
		CHECK ( sov_result->is_verified(14) );
	}

	SECTION ( "Non-Strict TrackOrder Verification result fails on accessing"
			" illegal track" )
	{
		CHECK_THROWS ( sov_result->is_verified(15) );
	}

	SECTION ( "Non-Strict TrackOrder Verification result has correct size" )
	{
		CHECK ( sov_result->total_blocks() == 3 );
		CHECK ( sov_result->tracks_per_block() == 15 );
		CHECK ( sov_result->size() == 93 ); // 2 * blocks * tracks + blocks
	}

	SECTION ( "Non-Strict TrackOrder Verification finds best block" )
	{
		CHECK ( std::get<0>(sov_best_block) == 2 );

		CHECK ( std::get<1>(sov_best_block) == v2 );

		CHECK ( std::get<2>(sov_best_block) == 0 );
		CHECK ( sov_result->best_block_difference() == 0 );
	}

	SECTION ( "Non-Strict TrackOrder Verification result has correct flags" )
	{
		// 0
		CHECK ( sov_result->id(0) );

		CHECK ( sov_result->track(0,  0, v1) );
		CHECK ( sov_result->track(0,  1, v1) );
		CHECK ( sov_result->track(0,  2, v1) );
		CHECK ( sov_result->track(0,  3, v1) );
		CHECK ( sov_result->track(0,  4, v1) );
		CHECK ( sov_result->track(0,  5, v1) );
		CHECK ( sov_result->track(0,  6, v1) );
		CHECK ( sov_result->track(0,  7, v1) );
		CHECK ( sov_result->track(0,  8, v1) );
		CHECK ( sov_result->track(0,  9, v1) );
		CHECK ( sov_result->track(0, 10, v1) );
		CHECK ( sov_result->track(0, 11, v1) );
		CHECK ( sov_result->track(0, 12, v1) );
		CHECK ( sov_result->track(0, 13, v1) );
		CHECK ( sov_result->track(0, 14, v1) );

		CHECK ( not sov_result->track(0,  0, v2) );
		CHECK ( not sov_result->track(0,  1, v2) );
		CHECK ( not sov_result->track(0,  2, v2) );
		CHECK ( not sov_result->track(0,  3, v2) );
		CHECK ( not sov_result->track(0,  4, v2) );
		CHECK ( not sov_result->track(0,  5, v2) );
		CHECK ( not sov_result->track(0,  6, v2) );
		CHECK ( not sov_result->track(0,  7, v2) );
		CHECK ( not sov_result->track(0,  8, v2) );
		CHECK ( not sov_result->track(0,  9, v2) );
		CHECK ( not sov_result->track(0, 10, v2) );
		CHECK ( not sov_result->track(0, 11, v2) );
		CHECK ( not sov_result->track(0, 12, v2) );
		CHECK ( not sov_result->track(0, 13, v2) );
		CHECK ( not sov_result->track(0, 14, v2) );

		// 1
		CHECK ( sov_result->id(1) ); // differs from stv_result: any id is verified

		CHECK ( not sov_result->track(1,  0, v1) );
		CHECK ( not sov_result->track(1,  1, v1) );
		CHECK ( not sov_result->track(1,  2, v1) );
		CHECK ( not sov_result->track(1,  3, v1) );
		CHECK ( not sov_result->track(1,  4, v1) );
		CHECK ( not sov_result->track(1,  5, v1) );
		CHECK ( not sov_result->track(1,  6, v1) );
		CHECK ( not sov_result->track(1,  7, v1) );
		CHECK ( not sov_result->track(1,  8, v1) );
		CHECK ( not sov_result->track(1,  9, v1) );
		CHECK ( not sov_result->track(1, 10, v1) );
		CHECK ( not sov_result->track(1, 11, v1) );
		CHECK ( not sov_result->track(1, 12, v1) );
		CHECK ( not sov_result->track(1, 13, v1) );
		CHECK ( not sov_result->track(1, 14, v1) );

		CHECK ( sov_result->track(1,  0, v2) );
		CHECK ( sov_result->track(1,  1, v2) );
		CHECK ( sov_result->track(1,  2, v2) );
		CHECK ( sov_result->track(1,  3, v2) );
		CHECK ( sov_result->track(1,  4, v2) );
		CHECK ( sov_result->track(1,  5, v2) );
		CHECK ( sov_result->track(1,  6, v2) );
		CHECK ( sov_result->track(1,  7, v2) );
		CHECK ( sov_result->track(1,  8, v2) );
		CHECK ( sov_result->track(1,  9, v2) );
		CHECK ( sov_result->track(1, 10, v2) );
		CHECK ( sov_result->track(1, 11, v2) );
		CHECK ( sov_result->track(1, 12, v2) );
		CHECK ( sov_result->track(1, 13, v2) );
		CHECK ( sov_result->track(1, 14, v2) );

		// 2
		CHECK ( sov_result->id(2) );

		CHECK ( not sov_result->track(2,  0, v1) );
		CHECK ( not sov_result->track(2,  1, v1) );
		CHECK ( not sov_result->track(2,  2, v1) );
		CHECK ( not sov_result->track(2,  3, v1) );
		CHECK ( not sov_result->track(2,  4, v1) );
		CHECK ( not sov_result->track(2,  5, v1) );
		CHECK ( not sov_result->track(2,  6, v1) );
		CHECK ( not sov_result->track(2,  7, v1) );
		CHECK ( not sov_result->track(2,  8, v1) );
		CHECK ( not sov_result->track(2,  9, v1) );
		CHECK ( not sov_result->track(2, 10, v1) );
		CHECK ( not sov_result->track(2, 11, v1) );
		CHECK ( not sov_result->track(2, 12, v1) );
		CHECK ( not sov_result->track(2, 13, v1) );
		CHECK ( not sov_result->track(2, 14, v1) );

		CHECK ( sov_result->track(2,  0, v2) );
		CHECK ( sov_result->track(2,  1, v2) );
		CHECK ( sov_result->track(2,  2, v2) );
		CHECK ( sov_result->track(2,  3, v2) );
		CHECK ( sov_result->track(2,  4, v2) );
		CHECK ( sov_result->track(2,  5, v2) );
		CHECK ( sov_result->track(2,  6, v2) );
		CHECK ( sov_result->track(2,  7, v2) );
		CHECK ( sov_result->track(2,  8, v2) );
		CHECK ( sov_result->track(2,  9, v2) );
		CHECK ( sov_result->track(2, 10, v2) );
		CHECK ( sov_result->track(2, 11, v2) );
		CHECK ( sov_result->track(2, 12, v2) );
		CHECK ( sov_result->track(2, 13, v2) );
		CHECK ( sov_result->track(2, 14, v2) );


		CHECK_THROWS ( sov_result->id(3) );            // illegal block
		CHECK_THROWS ( sov_result->track(3, 14, v2) ); //         block
		CHECK_THROWS ( sov_result->track(2, 15, v2) ); //         track

		CHECK ( sov_result->difference(0, v1) ==  0 );
		CHECK ( sov_result->difference(0, v2) == 15 );

		CHECK ( sov_result->difference(1, v1) == 15 ); // differs from stv_result
		CHECK ( sov_result->difference(1, v2) ==  0 ); // differs from stv_result

		CHECK ( sov_result->difference(2, v1) == 15 );
		CHECK ( sov_result->difference(2, v2) ==  0 );

		CHECK_THROWS ( sov_result->difference(3, v1) == 0 );
		CHECK_THROWS ( sov_result->difference(3, v2) == 0 );
	}
}


// TODO details::StrictPolicy
// TODO details::LiberalPolicy
// TODO details::VerifierBase
// TODO details::AlbumVerifier::Impl
// TODO details::TracksetVerifier::Impl

