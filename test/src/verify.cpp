#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for verify.hpp.
 */

#ifndef __LIBARCSTK_VERIFY_HPP__
#include "verify.hpp"             // TO BE TESTED
#endif

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"           // for Checksum, ChecksumSet, Checksums,...
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"         // for ARId
#endif
#ifndef __LIBARCSTK_DBAR_HPP__
#include "dbar.hpp"               // for DBAR
#endif

#include <tuple>                  // for get


// TODO ChecksumSourceOf


TEST_CASE ( "DBARSource", "[dbarsource] [verify]" )
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


TEST_CASE ( "AlbumVerifier", "[albumverifier] [verify]" )
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
	auto a = AlbumVerifier { actual_sums, id };

	// Check defaults
	REQUIRE ( a.strict() );
	REQUIRE ( a.actual_id() == &id );
	REQUIRE ( a.actual_checksums() == &actual_sums );

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


TEST_CASE ( "TracksetVerifier", "[tracksetverifier] [verify]" )
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


	auto t = TracksetVerifier { actual_sums };

	REQUIRE ( t.strict() );
	REQUIRE ( t.actual_id() == nullptr );
	REQUIRE ( t.actual_checksums() == &actual_sums );

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

