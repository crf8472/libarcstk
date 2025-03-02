#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for identifier.hpp.
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"         // TO BE TESTED
#endif
#ifndef __LIBARCSTK_IDENTIFIER_DETAILS_HPP__
#include "identifier_details.hpp" // for make_arid
#endif

#include <memory>                 // for unique_ptr


TEST_CASE ( "ARId", "[arid] [id]" )
{
	using arcstk::ARId;

	ARId id(10, 0x02c34fd0, 0x01f880cc, 0xbc55023f);


	SECTION ( "Constructor" )
	{
		CHECK ( id.track_count() == 10 );
		CHECK ( id.disc_id_1()   == 0x02c34fd0 );
		CHECK ( id.disc_id_2()   == 0x01f880cc );
		CHECK ( id.cddb_id()     == 0xbc55023f );

		CHECK ( id.url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( id.filename()    ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( not id.empty() );
	}


	SECTION ( "Equality and inequality operators" )
	{
		ARId same_id(10, 0x02c34fd0, 0x01f880cc, 0xbc55023f);

		CHECK ( id == id );
		CHECK ( not (id != id) );

		CHECK ( id == same_id );
		CHECK ( not (id != same_id) );


		ARId id1(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F);
		ARId id2(16, 0x001B9178, 0x014BE24E, 0xB40D2D0F); // different track
		ARId id3(15, 0x001B9179, 0x014BE24E, 0xB40D2D0F); // different id 1
		ARId id4(15, 0x001B9178, 0x014BE24D, 0xB40D2D0F); // different id 2
		ARId id5(15, 0x001B9178, 0x014BE24E, 0xC40D2D0F); // different cddb id

		CHECK ( id1 != id2 );
		CHECK ( not (id1 == id2) );

		CHECK ( id1 != id3 );
		CHECK ( not (id1 == id3) );

		CHECK ( id1 != id4 );
		CHECK ( not (id1 == id4) );

		CHECK ( id1 != id5 );
		CHECK ( not (id1 == id5) );

		CHECK ( id2 != id3 );
		CHECK ( not (id2 == id3) );

		CHECK ( id2 != id4 );
		CHECK ( not (id2 == id4) );

		CHECK ( id2 != id5 );
		CHECK ( not (id2 == id5) );

		CHECK ( id3 != id4 );
		CHECK ( not (id3 == id4) );

		CHECK ( id3 != id5 );
		CHECK ( not (id3 == id5) );

		CHECK ( id4 != id5 );
		CHECK ( not (id4 == id5) );
	}


	SECTION ( "Copy constructor" )
	{
		ARId copied_id(id);


		CHECK ( copied_id.track_count() == 10 );
		CHECK ( copied_id.disc_id_1()   == 0x02c34fd0 );
		CHECK ( copied_id.disc_id_2()   == 0x01f880cc );
		CHECK ( copied_id.cddb_id()     == 0xbc55023f );

		CHECK ( copied_id.url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( copied_id.filename()    ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( not copied_id.empty() );
	}


	SECTION ( "Copy assignment operator" )
	{
		ARId other_id(11, 0x02c34fd0, 0x04e880bb, 0xbc55023f);
		id = other_id;

		CHECK ( id == other_id );

		CHECK ( id.track_count() == 11 );
		CHECK ( id.disc_id_1()   == 0x02c34fd0 );
		CHECK ( id.disc_id_2()   == 0x04e880bb );
		CHECK ( id.cddb_id()     == 0xbc55023f );

		CHECK ( id.url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( id.filename()    ==
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( not id.empty() );
	}
}


// make_arid


TEST_CASE ( "make_arid builds valid ARIds", "[make_arid] [id]" )
{
	SECTION ( "ARId from track_count, offsets, leadout, example 1" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG

		std::unique_ptr<arcstk::ARId> id1 = arcstk::details::make_arid(
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		CHECK ( id1->track_count() == 15 );
		CHECK ( id1->disc_id_1()   == 0x001b9178 );
		CHECK ( id1->disc_id_2()   == 0x014be24e );
		CHECK ( id1->cddb_id()     == 0xb40d2d0f );

		CHECK ( id1->url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/8/7/1/"
				"dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		CHECK ( id1->filename()    ==
				"dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		CHECK ( not id1->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 2" )
	{
		// "Saint-Saens: Symphony No. 3, Poulenc: Organ Concerto",
		// Berliner Sinfonie-Orchester, C.-P. Flor, ETERNA

		std::unique_ptr<arcstk::ARId> id2 = arcstk::details::make_arid(
			// offsets
			{ 32, 96985, 166422 },
			// leadout
			264957
		);

		CHECK ( id2->track_count() == 3 );
		CHECK ( id2->disc_id_1()   == 0x0008100c );
		CHECK ( id2->disc_id_2()   == 0x001ac008 );
		CHECK ( id2->cddb_id()     == 0x190dcc03 );

		CHECK ( id2->url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/c/0/0/"
				"dBAR-003-0008100c-001ac008-190dcc03.bin" );

		CHECK ( id2->filename()    ==
				"dBAR-003-0008100c-001ac008-190dcc03.bin" );

		CHECK ( not id2->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 3" )
	{
		// "Bach: Brandenburg Concertos 3,4 & 5",
		// Academy of St.-Martin-in-the-Fields, Sir Neville Marriner, Philips

		std::unique_ptr<arcstk::ARId> id3 = arcstk::details::make_arid(
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 },
			// leadout
			210143
		);

		CHECK ( id3->track_count() == 9 );
		CHECK ( id3->disc_id_1()   == 0x001008a6 );
		CHECK ( id3->disc_id_2()   == 0x007469b8 );
		CHECK ( id3->cddb_id()     == 0x870af109 );

		CHECK ( id3->url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/6/a/8/"
				"dBAR-009-001008a6-007469b8-870af109.bin" );

		CHECK ( id3->filename()    ==
				"dBAR-009-001008a6-007469b8-870af109.bin" );

		CHECK ( not id3->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 4" )
	{
		// Bent: "Programmed to Love"

		std::unique_ptr<arcstk::ARId> id4 = arcstk::details::make_arid(
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500,
				148267, 174602, 208125, 212705, 239890, 268705, 272055, 291720,
				319992 },
			// leadout
			332075
		);


		CHECK ( id4->track_count() == 18 );
		CHECK ( id4->disc_id_1()   == 0x00307c78 );
		CHECK ( id4->disc_id_2()   == 0x0281351d );
		CHECK ( id4->cddb_id()     == 0x27114b12 );

		CHECK ( id4->url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/8/7/c/"
				"dBAR-018-00307c78-0281351d-27114b12.bin" );

		CHECK ( id4->filename()    ==
				"dBAR-018-00307c78-0281351d-27114b12.bin" );

		CHECK ( not id4->empty() );
	}


	SECTION ( "ARId from track_count, offsets, leadout, example 5" )
	{
		// "Wir entdecken Komponisten: Ludwig van Beethoven Vol. 1", DGG

		std::unique_ptr<arcstk::ARId> id5 = arcstk::details::make_arid(
			// offsets
			{ 33 },
			// leadout
			233484
		);


		CHECK ( id5->track_count() == 1 );
		CHECK ( id5->disc_id_1()   == 0x0003902d );
		CHECK ( id5->disc_id_2()   == 0x00072039 );
		CHECK ( id5->cddb_id()     == 0x020c2901 );

		CHECK ( id5->url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/d/2/0/"
				"dBAR-001-0003902d-00072039-020c2901.bin" );

		CHECK ( id5->filename()    ==
				"dBAR-001-0003902d-00072039-020c2901.bin" );

		CHECK ( not id5->empty() );
	}
}


TEST_CASE ( "make_empty_arid builds empty ARIds", "[make_empty_arid] [id]" )
{
	SECTION ( "Empty ARId" )
	{
		std::unique_ptr<arcstk::ARId> empty_id = arcstk::make_empty_arid();


		CHECK ( empty_id->track_count() == 0 );
		CHECK ( empty_id->disc_id_1()   == 0x00000000 );
		CHECK ( empty_id->disc_id_2()   == 0x00000000 );
		CHECK ( empty_id->cddb_id()     == 0x00000000 );

		CHECK ( empty_id->url()         ==
				"http://www.accuraterip.com/accuraterip"
				"/0/0/0/"
				"dBAR-000-00000000-00000000-00000000.bin" );

		CHECK ( empty_id->filename()    ==
				"dBAR-000-00000000-00000000-00000000.bin" );

		CHECK ( empty_id->empty() );
	}
}


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

