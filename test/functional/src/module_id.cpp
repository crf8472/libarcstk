#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Functional tests for module id.
 */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"         // TO BE TESTED
#endif
#ifndef LIBARCSTK_IDENTIFIER_DETAILS_HPP_
#include "identifier_details.hpp" // for make_arid
#endif


// make_arid


TEST_CASE ( "make_arid builds valid ARIds", "[make_arid] [id]" )
{
	SECTION ( "ARId from total_tracks, offsets, leadout, example 1" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG

		auto id1 = arcstk::arid::make(
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		CHECK ( id1.total_tracks() == 15 );
		CHECK ( id1.disc_id_1()    == 0x001b9178 );
		CHECK ( id1.disc_id_2()    == 0x014be24e );
		CHECK ( id1.cddb_id()      == 0xb40d2d0f );

		CHECK ( id1.url()          ==
				"http://www.accuraterip.com/accuraterip"
				"/8/7/1/"
				"dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		CHECK ( id1.filename()     ==
				"dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		CHECK ( not id1.empty() );
	}


	SECTION ( "ARId from total_tracks, offsets, leadout, example 2" )
	{
		// "Saint-Saens: Symphony No. 3, Poulenc: Organ Concerto",
		// Berliner Sinfonie-Orchester, C.-P. Flor, ETERNA

		auto id2 = arcstk::arid::make(
			// offsets
			{ 32, 96985, 166422 },
			// leadout
			264957
		);

		CHECK ( id2.total_tracks() == 3 );
		CHECK ( id2.disc_id_1()    == 0x0008100c );
		CHECK ( id2.disc_id_2()    == 0x001ac008 );
		CHECK ( id2.cddb_id()      == 0x190dcc03 );

		CHECK ( id2.url()          ==
				"http://www.accuraterip.com/accuraterip"
				"/c/0/0/"
				"dBAR-003-0008100c-001ac008-190dcc03.bin" );

		CHECK ( id2.filename()     ==
				"dBAR-003-0008100c-001ac008-190dcc03.bin" );

		CHECK ( not id2.empty() );
	}


	SECTION ( "ARId from total_tracks, offsets, leadout, example 3" )
	{
		// "Bach: Brandenburg Concertos 3,4 & 5",
		// Academy of St.-Martin-in-the-Fields, Sir Neville Marriner, Philips

		auto id3 = arcstk::arid::make(
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 },
			// leadout
			210143
		);

		CHECK ( id3.total_tracks() == 9 );
		CHECK ( id3.disc_id_1()    == 0x001008a6 );
		CHECK ( id3.disc_id_2()    == 0x007469b8 );
		CHECK ( id3.cddb_id()      == 0x870af109 );

		CHECK ( id3.url()          ==
				"http://www.accuraterip.com/accuraterip"
				"/6/a/8/"
				"dBAR-009-001008a6-007469b8-870af109.bin" );

		CHECK ( id3.filename()     ==
				"dBAR-009-001008a6-007469b8-870af109.bin" );

		CHECK ( not id3.empty() );
	}


	SECTION ( "ARId from total_tracks, offsets, leadout, example 4" )
	{
		// Bent: "Programmed to Love"

		auto id4 = arcstk::arid::make(
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500,
				148267, 174602, 208125, 212705, 239890, 268705, 272055, 291720,
				319992 },
			// leadout
			332075
		);


		CHECK ( id4.total_tracks() == 18 );
		CHECK ( id4.disc_id_1()    == 0x00307c78 );
		CHECK ( id4.disc_id_2()    == 0x0281351d );
		CHECK ( id4.cddb_id()      == 0x27114b12 );

		CHECK ( id4.url()          ==
				"http://www.accuraterip.com/accuraterip"
				"/8/7/c/"
				"dBAR-018-00307c78-0281351d-27114b12.bin" );

		CHECK ( id4.filename()     ==
				"dBAR-018-00307c78-0281351d-27114b12.bin" );

		CHECK ( not id4.empty() );
	}


	SECTION ( "ARId from total_tracks, offsets, leadout, example 5" )
	{
		// "Wir entdecken Komponisten: Ludwig van Beethoven Vol. 1", DGG

		auto id5 = arcstk::arid::make(
			// offsets
			{ 33 },
			// leadout
			233484
		);


		CHECK ( id5.total_tracks() == 1 );
		CHECK ( id5.disc_id_1()    == 0x0003902d );
		CHECK ( id5.disc_id_2()    == 0x00072039 );
		CHECK ( id5.cddb_id()      == 0x020c2901 );

		CHECK ( id5.url() ==
				"http://www.accuraterip.com/accuraterip"
				"/d/2/0/"
				"dBAR-001-0003902d-00072039-020c2901.bin" );

		CHECK ( id5.filename() ==
				"dBAR-001-0003902d-00072039-020c2901.bin" );

		CHECK ( not id5.empty() );
	}
}


// TODO validated_arid

// TEST_CASE ( "make_arid refuses to build invalid ARIds",
// 		"[identifier] [id] [aridbuilder]" )
// {
// 	using arcstk::CDDA;
//
// 	SECTION ( "Build fails for inconsistent offsets" )
// 	{
// 		// one track too short (no minimal distance)
// 		// offset[1] has not minimal distance to offset[0]
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
// 				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		// one offset exceeds legal maximum
// 		// offset[14] exceeds maximal block address
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
// 				106333, 139495, 157863, 198495, 213368, 225320,
// 				CDDA::MAX_OFFSET + 1 /* BOOM */ },
// 			// leadout
// 			253038
// 		));
//
// 		// not ascending order
// 		// offsets[9] is smaller than offsets[8]
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
// 				106333, 106000 /* BOOM */, 157863, 198495, 213368, 225320,
// 				234103 },
// 			// leadout
// 			253038
// 		));
//
// 		// two offsets equal
// 		// offsets[9] is equal to offsets[8]
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
// 				106333, 106333 /* BOOM */, 157863, 198495, 213368, 225320,
// 				234103 },
// 			// leadout
// 			253038
// 		));
// 	}
//
//
// 	SECTION ( "Build fails for offsets and leadout inconsistent" )
// 	{
// 		// Leadout 0 is illegal (smaller than minimum)
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			0 /* BOOM */
// 		));
//
// 		// Leadout exceeds maximal legal value
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			CDDA::MAX_BLOCK_ADDRESS + 1 /* BOOM */
// 		));
//
// 		// Leadout is smaller than biggest offset
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			234095 /* BOOM */
// 		));
//
// 		// Leadout is equal to biggest offset
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			234103 /* BOOM */
// 		));
//
// 		// Leadout has not minimal distance to biggest offset
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			234103 + CDDA::MIN_TRACK_LEN_FRAMES - 1 /* BOOM */
// 		));
// 	}
//
//
// 	SECTION ( "Build fails for offsets and trackcount inconsistent" )
// 	{
// 		// illegal track count: smaller than offsets count
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			14, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			8, /* BOOM */
// 			// offsets
// 			{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 },
// 			// leadout
// 			210143
// 		));
//
// 		// illegal track count: bigger than offsets count
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			16, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			9, /* BOOM */
// 			// offsets
// 			{ 33, 34283, 49908, 71508, 97983, 111183, 126708,
// 				161883/*, 187158 */ },
// 			// leadout
// 			210143
// 		));
//
// 		// illegal track count: smaller than minimum
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			0, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		// illegal track count: bigger than maximum
//
// 		CHECK_THROWS ( arcstk::make_arid(
// 			// track count
// 			CDDA::MAX_TRACKCOUNT+1, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
// 	}
// }

