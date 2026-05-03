#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for dbar.hpp.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"               // TO BE TESTED
#endif
#ifndef LIBARCSTK_DBAR_DETAILS_HPP_
#include "dbar_details.hpp"       // for parse_dbar_stream
#endif

#include <string>                 // for string
#include <utility>                // for move


TEST_CASE ( "DBAR Construction by CheckingDBARBuilder", "[dbar]")
{
	using arcstk::CheckingDBARBuilder;
	using arcstk::DBAR;
	using arcstk::details::parse_dbar_file;

	CheckingDBARBuilder cbuilder1;

	{
		const auto b = parse_dbar_file(
				"data/dBAR-015-001b9178-014be24e-b40d2d0f.bin", &cbuilder1,
				nullptr);

		REQUIRE ( b == 444 );
	}

	const auto dBAR_1 = cbuilder1.result();

	SECTION ( "DBAR 1 is valid and builder agrees" )
	{
		CHECK ( is_valid(dBAR_1) );
		CHECK ( cbuilder1.result_is_valid() );
	}

	SECTION ( "DBAR 1 is uniform and builder agrees" )
	{
		CHECK ( is_uniform(dBAR_1) );
		CHECK ( cbuilder1.result_is_uniform() );
	}

	SECTION ( "DBAR 1 is regular and builder agrees" )
	{
		CHECK ( is_regular(dBAR_1) );
		CHECK ( cbuilder1.result_is_regular() );
	}
}


TEST_CASE ( "DBAR Construction by DBARBuilder", "[dbar]")
{
	using arcstk::CheckingDBARBuilder;
	using arcstk::DBAR;
	using arcstk::DBARBuilder;
	using arcstk::details::parse_dbar_file;

	DBARBuilder builder1;

	{
		const auto b = parse_dbar_file(
				"data/dBAR-015-001b9178-014be24e-b40d2d0f.bin", &builder1,
				nullptr);

		REQUIRE ( b == 444 );
	}

	const auto dBAR = builder1.result();


	SECTION ( "DBARBuilder constructs DBAR with correct values" )
	{
		CHECK ( dBAR.size() == 3 );

		CHECK ( dBAR.block(0).size() == 15 );
		CHECK ( dBAR.block(1).size() == 15 );
		CHECK ( dBAR.block(2).size() == 15 );

		const auto block0 = dBAR.block(0);

		CHECK ( block0.size() == 15 );
		CHECK ( block0.index() == 0 );

		const auto header0 = dBAR.block(0).header();

		CHECK ( header0.total_tracks() == 15 );
		CHECK ( header0.id1()          == 0x001b9178 );
		CHECK ( header0.id2()          == 0x014be24e );
		CHECK ( header0.cddb_id()      == 0xb40d2d0f );

		const auto block1 = dBAR.block(1);
		CHECK ( block1.index() == 1 );

		const auto header1 = dBAR.block(1).header();

		CHECK ( header1.total_tracks() == 15 );
		CHECK ( header1.id1()          == 0x001b9178 );
		CHECK ( header1.id2()          == 0x014be24e );
		CHECK ( header1.cddb_id()      == 0xb40d2d0f );

		const auto block2 = dBAR.block(2);
		CHECK ( block2.index() == 2 );

		const auto header2 = dBAR.block(2).header();

		CHECK ( header2.total_tracks() == 15 );
		CHECK ( header2.id1()          == 0x001b9178 );
		CHECK ( header2.id2()          == 0x014be24e );
		CHECK ( header2.cddb_id()      == 0xb40d2d0f );

		// block 0, track 0 - 14

		CHECK ( block0.triplet(0).arcs()          == 0xB89992E5 );
		CHECK ( block0.triplet(0).frame450_arcs() == 0x126D875E );
		CHECK ( block0.triplet(0).confidence()    == 24 );

		CHECK ( block0.triplet(1).arcs()          == 0x4F77EB03 );
		CHECK ( block0.triplet(1).frame450_arcs() == 0xABF770DA );
		CHECK ( block0.triplet(1).confidence()    == 24 );

		CHECK ( block0.triplet(2).arcs()          == 0x56582282 );
		CHECK ( block0.triplet(2).frame450_arcs() == 0x80ACAF3C );
		CHECK ( block0.triplet(2).confidence()    == 24 );

		CHECK ( block0.triplet(3).arcs()          == 0x9E2187F9 );
		CHECK ( block0.triplet(3).frame450_arcs() == 0x8EB77C86 );
		CHECK ( block0.triplet(3).confidence()    == 24 );

		CHECK ( block0.triplet(4).arcs()          == 0x6BE71E50 );
		CHECK ( block0.triplet(4).frame450_arcs() == 0xF62D90FC );
		CHECK ( block0.triplet(4).confidence()    == 24 );

		CHECK ( block0.triplet(5).arcs()          == 0x01E7235F );
		CHECK ( block0.triplet(5).frame450_arcs() == 0x56C6AF06 );
		CHECK ( block0.triplet(5).confidence()    == 24 );

		CHECK ( block0.triplet(6).arcs()          == 0xD8F7763C );
		CHECK ( block0.triplet(6).frame450_arcs() == 0x76274140 );
		CHECK ( block0.triplet(6).confidence()    == 24 );

		CHECK ( block0.triplet(7).arcs()          == 0x8480223E );
		CHECK ( block0.triplet(7).frame450_arcs() == 0x73A608D0 );
		CHECK ( block0.triplet(7).confidence()    == 24 );

		CHECK ( block0.triplet(8).arcs()          == 0x42C5061C );
		CHECK ( block0.triplet(8).frame450_arcs() == 0x9D7A1F4B );
		CHECK ( block0.triplet(8).confidence()    == 24 );

		CHECK ( block0.triplet(9).arcs()          == 0x47A70F02 );
		CHECK ( block0.triplet(9).frame450_arcs() == 0x37871A8C );
		CHECK ( block0.triplet(9).confidence()    == 23 );

		CHECK ( block0.triplet(10).arcs()          == 0xBABF08CC );
		CHECK ( block0.triplet(10).frame450_arcs() == 0xF6360C0B );
		CHECK ( block0.triplet(10).confidence()    == 23 );

		CHECK ( block0.triplet(11).arcs()          == 0x563EDCCB );
		CHECK ( block0.triplet(11).frame450_arcs() == 0xCB1FE45D );
		CHECK ( block0.triplet(11).confidence()    == 23 );

		CHECK ( block0.triplet(12).arcs()          == 0xAB123C7C );
		CHECK ( block0.triplet(12).frame450_arcs() == 0xBCC08EDA );
		CHECK ( block0.triplet(12).confidence()    == 23 );

		CHECK ( block0.triplet(13).arcs()          == 0xC65C20E4 );
		CHECK ( block0.triplet(13).frame450_arcs() == 0xE467DE8E );
		CHECK ( block0.triplet(13).confidence()    == 22 );

		CHECK ( block0.triplet(14).arcs()          == 0x58FC3C3E );
		CHECK ( block0.triplet(14).frame450_arcs() == 0x9537953F );
		CHECK ( block0.triplet(14).confidence()    == 22 );

		// block 1, track 0 - 14

		CHECK ( block1.triplet(0).arcs()           == 0x98B10E0F );
		CHECK ( block1.triplet(0).frame450_arcs()  == 0x35DC25F3 );
		CHECK ( block1.triplet(0).confidence()     == 20 );

		CHECK ( block1.triplet(1).arcs()           == 0x475F57E9 );
		CHECK ( block1.triplet(1).frame450_arcs()  == 0xDCCF2356 );
		CHECK ( block1.triplet(1).confidence()     == 20 );

		CHECK ( block1.triplet(2).arcs()           == 0x7304F1C4 );
		CHECK ( block1.triplet(2).frame450_arcs()  == 0x5FA89D66 );
		CHECK ( block1.triplet(2).confidence()     == 20 );

		CHECK ( block1.triplet(3).arcs()           == 0xF2472287 );
		CHECK ( block1.triplet(3).frame450_arcs()  == 0xB0330387 );
		CHECK ( block1.triplet(3).confidence()     == 20 );

		CHECK ( block1.triplet(4).arcs()           == 0x881BC504 );
		CHECK ( block1.triplet(4).frame450_arcs()  == 0x8442806E );
		CHECK ( block1.triplet(4).confidence()     == 20 );

		CHECK ( block1.triplet(5).arcs()           == 0xBB94BFD4 );
		CHECK ( block1.triplet(5).frame450_arcs()  == 0xF13BC09B );
		CHECK ( block1.triplet(5).confidence()     == 20 );

		CHECK ( block1.triplet(6).arcs()           == 0xF9CAEE76 );
		CHECK ( block1.triplet(6).frame450_arcs()  == 0xC0AB9412 );
		CHECK ( block1.triplet(6).confidence()     == 20 );

		CHECK ( block1.triplet(7).arcs()           == 0xF9F60BC1 );
		CHECK ( block1.triplet(7).frame450_arcs()  == 0xC7836441 );
		CHECK ( block1.triplet(7).confidence()     == 20 );

		CHECK ( block1.triplet(8).arcs()           == 0x2C736302 );
		CHECK ( block1.triplet(8).frame450_arcs()  == 0xF1FD38D3 );
		CHECK ( block1.triplet(8).confidence()     == 19 );

		CHECK ( block1.triplet(9).arcs()           == 0x1C955978 );
		CHECK ( block1.triplet(9).frame450_arcs()  == 0x0860E08B );
		CHECK ( block1.triplet(9).confidence()     == 20 );

		CHECK ( block1.triplet(10).arcs()          == 0xFDA6D833 );
		CHECK ( block1.triplet(10).frame450_arcs() == 0x9C8202BE );
		CHECK ( block1.triplet(10).confidence()    == 19 );

		CHECK ( block1.triplet(11).arcs()          == 0x3A57E5D1 );
		CHECK ( block1.triplet(11).frame450_arcs() == 0xC6FF01AE );
		CHECK ( block1.triplet(11).confidence()    == 21 );

		CHECK ( block1.triplet(12).arcs()          == 0x6ED5F3E7 );
		CHECK ( block1.triplet(12).frame450_arcs() == 0x4C92FCBE  );
		CHECK ( block1.triplet(12).confidence()    == 19 );

		CHECK ( block1.triplet(13).arcs()          == 0x4A5C3872 );
		CHECK ( block1.triplet(13).frame450_arcs() == 0x8FB684A7 );
		CHECK ( block1.triplet(13).confidence()    == 20 );

		CHECK ( block1.triplet(14).arcs()          == 0x5FE8B032 );
		CHECK ( block1.triplet(14).frame450_arcs() == 0x405711AA );
		CHECK ( block1.triplet(14).confidence()    == 19 );

		// block 2, track 0 - 14

		CHECK ( block2.triplet(0).arcs()           == 0x54FE0533 );
		CHECK ( block2.triplet(0).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(0).confidence()     == 2 );

		CHECK ( block2.triplet(1).arcs()           == 0xAFEC147E );
		CHECK ( block2.triplet(1).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(1).confidence()     == 2 );

		CHECK ( block2.triplet(2).arcs()           == 0x2BFB5AEC );
		CHECK ( block2.triplet(2).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(2).confidence()     == 2 );

		CHECK ( block2.triplet(3).arcs()           == 0xED6E7215 );
		CHECK ( block2.triplet(3).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(3).confidence()     == 2 );

		CHECK ( block2.triplet(4).arcs()           == 0xA0847CEF );
		CHECK ( block2.triplet(4).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(4).confidence()     == 2 );

		CHECK ( block2.triplet(5).arcs()           == 0xAE066CD1 );
		CHECK ( block2.triplet(5).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(5).confidence()     == 2 );

		CHECK ( block2.triplet(6).arcs()           == 0xE1AB3B46 );
		CHECK ( block2.triplet(6).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(6).confidence()     == 2 );

		CHECK ( block2.triplet(7).arcs()           == 0xE75E70BA );
		CHECK ( block2.triplet(7).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(7).confidence()     == 2 );

		CHECK ( block2.triplet(8).arcs()           == 0x6D72D1AC );
		CHECK ( block2.triplet(8).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(8).confidence()     == 2 );

		CHECK ( block2.triplet(9).arcs()           == 0x89C19A02 );
		CHECK ( block2.triplet(9).frame450_arcs()  == 0x00000000 );
		CHECK ( block2.triplet(9).confidence()     == 2 );

		CHECK ( block2.triplet(10).arcs()          == 0x4A5CE2AB );
		CHECK ( block2.triplet(10).frame450_arcs() == 0x00000000 );
		CHECK ( block2.triplet(10).confidence()    == 2 );

		CHECK ( block2.triplet(11).arcs()          == 0x4D23C1D4 );
		CHECK ( block2.triplet(11).frame450_arcs() == 0x00000000 );
		CHECK ( block2.triplet(11).confidence()    == 2 );

		CHECK ( block2.triplet(12).arcs()          == 0x80AA0FB6 );
		CHECK ( block2.triplet(12).frame450_arcs() == 0x00000000 );
		CHECK ( block2.triplet(12).confidence()    == 2 );

		CHECK ( block2.triplet(13).arcs()          == 0x9378FD52 );
		CHECK ( block2.triplet(13).frame450_arcs() == 0x00000000 );
		CHECK ( block2.triplet(13).confidence()    == 2 );

		CHECK ( block2.triplet(14).arcs()          == 0x6A8A614C );
		CHECK ( block2.triplet(14).frame450_arcs() == 0x00000000 );
		CHECK ( block2.triplet(14).confidence()    == 2 );
	}

	SECTION ( "Range-based for loop works correctly" )
	{
		for (const auto& block : dBAR)
		{
			CHECK ( block.header().total_tracks() == 15 );

			for (const auto& triplet : block)
			{
				CHECK ( triplet.arcs() );
			}
		}
	}
}

