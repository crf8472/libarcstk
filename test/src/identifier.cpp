#include "catch2/catch.hpp"

#include <cstdint>
#include <memory>

#ifndef __LIBARCS_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCS_IDENTIFIER_DETAILS_HPP__
#include "identifier_details.hpp"
#endif


/**
 * \file identifier.cpp Fixtures for classes in module 'identifier'
 */


// ARId


TEST_CASE ( "ARId", "[identifier] [arid]" )
{
	arcs::ARId id(10, 0x02c34fd0, 0x01f880cc, 0xbc55023f);


	SECTION ( "Constructor" )
	{
		REQUIRE ( id.track_count() == 10 );
		REQUIRE ( id.disc_id_1()   == 0x02c34fd0 );
		REQUIRE ( id.disc_id_2()   == 0x01f880cc );
		REQUIRE ( id.cddb_id()     == 0xbc55023f );

		REQUIRE ( id.url()         ==
				"http://www.accuraterip.com/accuraterip/0/d/f/dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );
		REQUIRE ( id.filename()    ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		REQUIRE ( not id.empty() );
	}


	SECTION ( "Equality and inequality operators" )
	{
		arcs::ARId same_id(10, 0x02c34fd0, 0x01f880cc, 0xbc55023f);

		REQUIRE ( id == id );
		REQUIRE ( not (id != id) );

		REQUIRE ( id == same_id );
		REQUIRE ( not (id != same_id) );


		arcs::ARId id1(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F);
		arcs::ARId id2(16, 0x001B9178, 0x014BE24E, 0xB40D2D0F); // different track
		arcs::ARId id3(15, 0x001B9179, 0x014BE24E, 0xB40D2D0F); // different id 1
		arcs::ARId id4(15, 0x001B9178, 0x014BE24D, 0xB40D2D0F); // different id 2
		arcs::ARId id5(15, 0x001B9178, 0x014BE24E, 0xC40D2D0F); // different cddb id

		REQUIRE ( id1 != id2 );
		REQUIRE ( not (id1 == id2) );

		REQUIRE ( id1 != id3 );
		REQUIRE ( not (id1 == id3) );

		REQUIRE ( id1 != id4 );
		REQUIRE ( not (id1 == id4) );

		REQUIRE ( id1 != id5 );
		REQUIRE ( not (id1 == id5) );

		REQUIRE ( id2 != id3 );
		REQUIRE ( not (id2 == id3) );

		REQUIRE ( id2 != id4 );
		REQUIRE ( not (id2 == id4) );

		REQUIRE ( id2 != id5 );
		REQUIRE ( not (id2 == id5) );

		REQUIRE ( id3 != id4 );
		REQUIRE ( not (id3 == id4) );

		REQUIRE ( id3 != id5 );
		REQUIRE ( not (id3 == id5) );

		REQUIRE ( id4 != id5 );
		REQUIRE ( not (id4 == id5) );
	}


	SECTION ( "Copy constructor" )
	{
		arcs::ARId copied_id(id);


		REQUIRE ( copied_id.track_count() == 10 );
		REQUIRE ( copied_id.disc_id_1()   == 0x02c34fd0 );
		REQUIRE ( copied_id.disc_id_2()   == 0x01f880cc );
		REQUIRE ( copied_id.cddb_id()     == 0xbc55023f );

		REQUIRE ( copied_id.url()         ==
				"http://www.accuraterip.com/accuraterip/0/d/f/dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );
		REQUIRE ( copied_id.filename()    ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		REQUIRE ( not copied_id.empty() );
	}


	SECTION ( "Copy assignment operator" )
	{
		arcs::ARId other_id(11, 0x02c34fd0, 0x04e880bb, 0xbc55023f);
		id = other_id;

		REQUIRE ( id == other_id );

		REQUIRE ( id.track_count() == 11 );
		REQUIRE ( id.disc_id_1()   == 0x02c34fd0 );
		REQUIRE ( id.disc_id_2()   == 0x04e880bb );
		REQUIRE ( id.cddb_id()     == 0xbc55023f );

		REQUIRE ( id.url()         ==
				"http://www.accuraterip.com/accuraterip/0/d/f/dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );
		REQUIRE ( id.filename()    ==
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		REQUIRE ( not id.empty() );
	}
}


// ARIdBuilder


TEST_CASE ( "ARIdBuilder builds valid ARIds", "[identifier] [aridbuilder]" )
{
	arcs::details::ARIdBuilder builder;


	SECTION ( "Reference ARId 1" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG

		std::unique_ptr<arcs::ARId> id1 = builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		REQUIRE ( id1->track_count() == 15 );
		REQUIRE ( id1->disc_id_1()   == 0x001b9178 );
		REQUIRE ( id1->disc_id_2()   == 0x014be24e );
		REQUIRE ( id1->cddb_id()     == 0xb40d2d0f );

		REQUIRE ( id1->url()         ==
				"http://www.accuraterip.com/accuraterip/8/7/1/dBAR-015-001b9178-014be24e-b40d2d0f.bin" );
		REQUIRE ( id1->filename()    ==
				"dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		REQUIRE ( not id1->empty() );
	}


	SECTION ( "Reference ARId 2" )
	{
		// "Saint-Saens: Symphony No. 3, Poulenc: Organ Concerto", Berliner
		// Sinfonie-Orchester, C.-P. Flor, ETERNA

		std::unique_ptr<arcs::ARId> id2 = builder.build(
			// track count
			3,
			// offsets
			{ 32, 96985, 166422 },
			// leadout
			264957
		);

		REQUIRE ( id2->track_count() == 3 );
		REQUIRE ( id2->disc_id_1()   == 0x0008100c );
		REQUIRE ( id2->disc_id_2()   == 0x001ac008 );
		REQUIRE ( id2->cddb_id()     == 0x190dcc03 );

		REQUIRE ( id2->url()         ==
				"http://www.accuraterip.com/accuraterip/c/0/0/dBAR-003-0008100c-001ac008-190dcc03.bin" );
		REQUIRE ( id2->filename()    ==
				"dBAR-003-0008100c-001ac008-190dcc03.bin" );

		REQUIRE ( not id2->empty() );
	}


	SECTION ( "Reference ARId 3" )
	{
		// "Bach: Brandenburg Concertos 3,4 & 5",
		// Academy of St.-Martin-in-the-Fields, Sir Neville Marriner, Philips

		std::unique_ptr<arcs::ARId> id3 = builder.build(
			// track count
			9,
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 },
			// leadout
			210143
		);


		REQUIRE ( id3->track_count() == 9 );
		REQUIRE ( id3->disc_id_1()   == 0x001008a6 );
		REQUIRE ( id3->disc_id_2()   == 0x007469b8 );
		REQUIRE ( id3->cddb_id()     == 0x870af109 );

		REQUIRE ( id3->url()         ==
				"http://www.accuraterip.com/accuraterip/6/a/8/dBAR-009-001008a6-007469b8-870af109.bin" );
		REQUIRE ( id3->filename()    ==
				"dBAR-009-001008a6-007469b8-870af109.bin" );

		REQUIRE ( not id3->empty() );
	}


	SECTION ( "Reference ARId 4" )
	{
		// Bent: Programmed to Love

		std::unique_ptr<arcs::ARId> id4 = builder.build(
			// track count
			18,
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500,
				148267, 174602, 208125, 212705, 239890, 268705, 272055, 291720,
				319992 },
			// leadout
			332075
		);


		REQUIRE ( id4->track_count() == 18 );
		REQUIRE ( id4->disc_id_1()   == 0x00307c78 );
		REQUIRE ( id4->disc_id_2()   == 0x0281351d );
		REQUIRE ( id4->cddb_id()     == 0x27114b12 );

		REQUIRE ( id4->url()         ==
				"http://www.accuraterip.com/accuraterip/8/7/c/dBAR-018-00307c78-0281351d-27114b12.bin" );
		REQUIRE ( id4->filename()    ==
				"dBAR-018-00307c78-0281351d-27114b12.bin" );

		REQUIRE ( not id4->empty() );
	}
}


TEST_CASE ( "ARIdBuilder refuses to build invalid ARIds",
		"[identifier] [aridbuilder]" )
{
	arcs::details::ARIdBuilder builder;


	SECTION ( "Build fails for inconsistent offsets" )
	{
		// one track too short (no minimal distance)
		// offset[1] has not minimal distance to offset[0]

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		// one offset exceeds legal maximum
		// offset[14] exceeds maximal block address

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320,
				static_cast<int32_t>(arcs::CDDA.MAX_OFFSET) + 1 /* BOOM */ },
			// leadout
			253038
		));

		// not ascending order
		// offsets[9] is smaller than offsets[8]

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 106000 /* BOOM */, 157863, 198495, 213368, 225320,
				234103 },
			// leadout
			253038
		));

		// two offsets equal
		// offsets[9] is equal to offsets[8]

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 106333 /* BOOM */, 157863, 198495, 213368, 225320,
				234103 },
			// leadout
			253038
		));
	}


	SECTION ( "Build fails for offsets and leadout inconsistent" )
	{
		// Leadout 0 is illegal (smaller than minimum)

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			0 /* BOOM */
		));

		// Leadout exceeds maximal legal value

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			arcs::CDDA.MAX_BLOCK_ADDRESS + 1 /* BOOM */
		));

		// Leadout is smaller than biggest offset

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234095 /* BOOM */
		));

		// Leadout is equal to biggest offset

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234103 /* BOOM */
		));

		// Leadout has not minimal distance to biggest offset

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234103 + arcs::CDDA.MIN_TRACK_LEN_FRAMES - 1 /* BOOM */
		));
    }


	SECTION ( "Build fails for offsets and trackcount inconsistent" )
	{
		// illegal track count: smaller than offsets count

		CHECK_THROWS ( builder.build(
			// track count
			14, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( builder.build(
			// track count
			8, /* BOOM */
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 },
			// leadout
			210143
		));

		// illegal track count: bigger than offsets count

		CHECK_THROWS ( builder.build(
			// track count
			16, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( builder.build(
			// track count
			9, /* BOOM */
			// offsets
			{ 33, 34283, 49908, 71508, 97983, 111183, 126708,
				161883/*, 187158 */ },
			// leadout
			210143
		));

		// illegal track count: smaller than minimum

		CHECK_THROWS ( builder.build(
			// track count
			0, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		// illegal track count: bigger than maximum

		CHECK_THROWS ( builder.build(
			// track count
			arcs::CDDA.MAX_TRACKCOUNT+1, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));
	}
}


TEST_CASE ( "ARIdBuilder builds empty ARIds", "[identifier] [aridbuilder]" )
{
	arcs::details::ARIdBuilder builder;


	SECTION ( "Empty ARId" )
	{
		std::unique_ptr<arcs::ARId> empty_id = builder.build_empty_id();


		REQUIRE ( empty_id->track_count() == 0 );
		REQUIRE ( empty_id->disc_id_1()   == 0x00000000 );
		REQUIRE ( empty_id->disc_id_2()   == 0x00000000 );
		REQUIRE ( empty_id->cddb_id()     == 0x00000000 );

		REQUIRE ( empty_id->url()         ==
				"http://www.accuraterip.com/accuraterip/0/0/0/dBAR-000-00000000-00000000-00000000.bin" );
		REQUIRE ( empty_id->filename()    ==
				"dBAR-000-00000000-00000000-00000000.bin" );

		REQUIRE ( empty_id->empty() );
	}
}


// TOCValidator


TEST_CASE ( "TOCValidator", "[identifier]" )
{
	arcs::details::TOCValidator validator;


    SECTION ( "Validation succeeds for correct offsets" )
    {
		// some legal values
		CHECK_NOTHROW ( validator.validate_offsets( { 33, 5225, 7390, 23380,
				35608, 49820, 69508, 87733, 106333, 139495, 157863, 198495,
				213368, 225320, 234103 })
		);

		// some legal values
		CHECK_NOTHROW ( validator.validate_offsets(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 })
		);

		// some legal values
		CHECK_NOTHROW ( validator.validate(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038)
		);
    }


    SECTION ( "Validation fails for incorrect offsets" )
    {
		// offset[1] has less than minimal legal distance to offset[0]
		CHECK_THROWS ( validator.validate_offsets(
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		));

		// offset[14] bigger than legal maximum
		CHECK_THROWS ( validator.validate_offsets(
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320,
				static_cast<int32_t>(arcs::CDDA.MAX_OFFSET + 1) /* BOOM */ }
		));

		// offset[6] is greater than offset[7]
		CHECK_THROWS ( validator.validate_offsets(
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 69507 /* BOOM */,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		));

		// offset[7] and offset[8] are equal
		CHECK_THROWS ( validator.validate_offsets(
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 69508 /* BOOM */,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		));

		// track count bigger than legal maximum
		CHECK_THROWS ( validator.validate_offsets(std::vector<int32_t>(100)) );

		// track count smaller than legal minimum
		CHECK_THROWS ( validator.validate_offsets(std::vector<int32_t>()) );
    }


    SECTION ( "Validation succeeds for correct lengths" )
    {
		// complete correct lengths
		CHECK_NOTHROW ( validator.validate_lengths({ 5192, 2165, 15885, 12228,
			13925, 19513, 18155, 18325, 33075, 18368, 40152, 14798, 11952, 8463,
			18935 })
		);

		// incomplete correct lengths
		CHECK_NOTHROW ( validator.validate_lengths({ 5192, 2165, 15885, -1 }) );
    }


    SECTION ( "Validation fails for incorrect lengths" )
    {
		// one length smaller than legal minimum
		CHECK_THROWS ( validator.validate_lengths(
			{ 5192, 2165, 15885,
				static_cast<int32_t>(arcs::CDDA.MIN_TRACK_LEN_FRAMES - 1)
				/* BOOM */,
				5766 }
		));

		// sum of lengths greater than legal maximum of 99 min
		CHECK_THROWS ( validator.validate_lengths(
			{ 5192, 2165, 15885,
				360000 /* Redbook maximum */,
				100000 /* Exceeds maximum of 99 min */,
				-1 }
		));

		// track count bigger than legal maximum
		CHECK_THROWS ( validator.validate_lengths(std::vector<int32_t>(100)) );

		// last length smaller than legal minimum
		CHECK_THROWS ( validator.validate_lengths(
			{ 5192, 2165, 15885,
				static_cast<int32_t>(arcs::CDDA.MIN_TRACK_LEN_FRAMES - 1)
				/* BOOM */ }
		));

		// track count smaller than legal minimum
		CHECK_THROWS (
		validator.validate_lengths(std::vector<int32_t>())
		);
    }


    SECTION ( "Validation succeeds for correct leadouts" )
    {
		// legal minimum
		CHECK_NOTHROW (
		validator.validate_leadout(arcs::CDDA.MIN_TRACK_OFFSET_DIST)
		);

		// some legal value
		CHECK_NOTHROW ( validator.validate_leadout(253038) );

		// TODO more values

		// legal maximum
		CHECK_NOTHROW (
		validator.validate_leadout(arcs::CDDA.MAX_BLOCK_ADDRESS)
		);
    }


    SECTION ( "Validation fails for incorrect leadouts" )
    {
		// 0 (smaller than legal minimum)
		CHECK_THROWS ( validator.validate_leadout(0) );

		// greater than 0, but smaller than legal minimum
		CHECK_THROWS ( validator.validate_leadout(
			arcs::CDDA.MIN_TRACK_OFFSET_DIST - 1
		));

		// bigger than legal maximum
		CHECK_THROWS ( validator.validate_leadout(
			arcs::CDDA.MAX_BLOCK_ADDRESS + 1
		));
    }


    SECTION ( "Validation succeeds for correct trackcounts" )
    {
		// legal minimum
		CHECK_NOTHROW ( validator.validate_trackcount(1) );

		// legal values greater than minimum and smaller than maximum
		for (int i = 2; i < 99; ++i)
		{
			// 2 - 98
			CHECK_NOTHROW ( validator.validate_trackcount(i) );
		}

		// legal maximum
		CHECK_NOTHROW ( validator.validate_trackcount(99) );
    }


    SECTION ( "Validation fails for incorrect trackcounts" )
    {
		// smaller than legal minimum
		CHECK_THROWS ( validator.validate_trackcount(0) );

		// bigger than legal maximum
		CHECK_THROWS ( validator.validate_trackcount(100) );

		// bigger than legal maximum
		CHECK_THROWS ( validator.validate_trackcount(500) );

		// bigger than legal maximum
		CHECK_THROWS ( validator.validate_trackcount(999) );

		// bigger than legal maximum
		CHECK_THROWS ( validator.validate_trackcount(65535) );
    }
}


// TOCBuilder


TEST_CASE ( "details::TOCBuilder: build with leadout", "[identifier] [tocbuilder]" )
{
	arcs::details::TOCBuilder builder;


    SECTION ( "Build succeeds for correct trackcount, offsets, leadout" )
    {
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);


		REQUIRE ( toc0->track_count() == 15 );
		REQUIRE ( toc0->leadout()     == 253038 );

		REQUIRE ( toc0->offset(0)  ==      0 );
		REQUIRE ( toc0->offset(1)  ==     33 );
		REQUIRE ( toc0->offset(2)  ==   5225 );
		REQUIRE ( toc0->offset(3)  ==   7390 );
		REQUIRE ( toc0->offset(4)  ==  23380 );
		REQUIRE ( toc0->offset(5)  ==  35608 );
		REQUIRE ( toc0->offset(6)  ==  49820 );
		REQUIRE ( toc0->offset(7)  ==  69508 );
		REQUIRE ( toc0->offset(8)  ==  87733 );
		REQUIRE ( toc0->offset(9)  == 106333 );
		REQUIRE ( toc0->offset(10) == 139495 );
		REQUIRE ( toc0->offset(11) == 157863 );
		REQUIRE ( toc0->offset(12) == 198495 );
		REQUIRE ( toc0->offset(13) == 213368 );
		REQUIRE ( toc0->offset(14) == 225320 );
		REQUIRE ( toc0->offset(15) == 234103 );

		REQUIRE ( toc0->offset(16) == 0 );
    }
}


TEST_CASE ( "details::TOCBuilder: build with lengths and files",
	"[identifier] [tocbuilder]" )
{
	arcs::details::TOCBuilder builder;


    SECTION ( "Build succeeds for correct trackcount, offsets, lengths" )
    {
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc1 = builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075, 18368,
				40152, 14798, 11952, 8463, 18935 },
			// files
			{ }
		);

		REQUIRE ( toc1->track_count() == 15 );
		REQUIRE ( toc1->leadout()     == 253038 );

		REQUIRE ( toc1->offset(0)  ==      0 );
		REQUIRE ( toc1->offset(1)  ==     33 );
		REQUIRE ( toc1->offset(2)  ==   5225 );
		REQUIRE ( toc1->offset(3)  ==   7390 );
		REQUIRE ( toc1->offset(4)  ==  23380 );
		REQUIRE ( toc1->offset(5)  ==  35608 );
		REQUIRE ( toc1->offset(6)  ==  49820 );
		REQUIRE ( toc1->offset(7)  ==  69508 );
		REQUIRE ( toc1->offset(8)  ==  87733 );
		REQUIRE ( toc1->offset(9)  == 106333 );
		REQUIRE ( toc1->offset(10) == 139495 );
		REQUIRE ( toc1->offset(11) == 157863 );
		REQUIRE ( toc1->offset(12) == 198495 );
		REQUIRE ( toc1->offset(13) == 213368 );
		REQUIRE ( toc1->offset(14) == 225320 );
		REQUIRE ( toc1->offset(15) == 234103 );

		REQUIRE ( toc1->offset(16) == 0 );

		REQUIRE ( toc1->parsed_length(1)  ==  5192 );
		REQUIRE ( toc1->parsed_length(2)  ==  2165 );
		REQUIRE ( toc1->parsed_length(3)  == 15885 );
		REQUIRE ( toc1->parsed_length(4)  == 12228 );
		REQUIRE ( toc1->parsed_length(5)  == 13925 );
		REQUIRE ( toc1->parsed_length(6)  == 19513 );
		REQUIRE ( toc1->parsed_length(7)  == 18155 );
		REQUIRE ( toc1->parsed_length(8)  == 18325 );
		REQUIRE ( toc1->parsed_length(9)  == 33075 );
		REQUIRE ( toc1->parsed_length(10) == 18368 );
		REQUIRE ( toc1->parsed_length(11) == 40152 );
		REQUIRE ( toc1->parsed_length(12) == 14798 );
		REQUIRE ( toc1->parsed_length(13) == 11952 );
		REQUIRE ( toc1->parsed_length(14) ==  8463 );
		REQUIRE ( toc1->parsed_length(15) == 18935 );

		REQUIRE ( toc1->parsed_length(16) == 0 );
    }


    SECTION ( "Build succeeds for trackcount, offsets and incomplete lengths" )
    {
		auto toc2 = builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ },
			// files
			{ }
		);


		REQUIRE ( toc2->track_count() == 15 );
		REQUIRE ( toc2->leadout()     == 0 ); // missing !

		REQUIRE ( toc2->offset(0)  ==      0 );
		REQUIRE ( toc2->offset(1)  ==     33 );
		REQUIRE ( toc2->offset(2)  ==   5225 );
		REQUIRE ( toc2->offset(3)  ==   7390 );
		REQUIRE ( toc2->offset(4)  ==  23380 );
		REQUIRE ( toc2->offset(5)  ==  35608 );
		REQUIRE ( toc2->offset(6)  ==  49820 );
		REQUIRE ( toc2->offset(7)  ==  69508 );
		REQUIRE ( toc2->offset(8)  ==  87733 );
		REQUIRE ( toc2->offset(9)  == 106333 );
		REQUIRE ( toc2->offset(10) == 139495 );
		REQUIRE ( toc2->offset(11) == 157863 );
		REQUIRE ( toc2->offset(12) == 198495 );
		REQUIRE ( toc2->offset(13) == 213368 );
		REQUIRE ( toc2->offset(14) == 225320 );
		REQUIRE ( toc2->offset(15) == 234103 );

		REQUIRE ( toc2->offset(16) == 0 );

		REQUIRE ( toc2->parsed_length(0)  ==     0 ); // as defined!
		REQUIRE ( toc2->parsed_length(1)  ==  5192 );
		REQUIRE ( toc2->parsed_length(2)  ==  2165 );
		REQUIRE ( toc2->parsed_length(3)  == 15885 );
		REQUIRE ( toc2->parsed_length(4)  == 12228 );
		REQUIRE ( toc2->parsed_length(5)  == 13925 );
		REQUIRE ( toc2->parsed_length(6)  == 19513 );
		REQUIRE ( toc2->parsed_length(7)  == 18155 );
		REQUIRE ( toc2->parsed_length(8)  == 18325 );
		REQUIRE ( toc2->parsed_length(9)  == 33075 );
		REQUIRE ( toc2->parsed_length(10) == 18368 );
		REQUIRE ( toc2->parsed_length(11) == 40152 );
		REQUIRE ( toc2->parsed_length(12) == 14798 );
		REQUIRE ( toc2->parsed_length(13) == 11952 );
		REQUIRE ( toc2->parsed_length(14) ==  8463 );
		REQUIRE ( toc2->parsed_length(15) ==     0 );  // missing !

		REQUIRE ( toc2->parsed_length(16) == 0 );
    }
}


TEST_CASE ( "details::TOCBuilder: build fails with illegal values",
	"[identifier] [tocbuilder]" )
{
	arcs::details::TOCBuilder builder;


    SECTION ( "Build fails for incorrect offsets" )
    {
		// no minimal distance: with leadout
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offset[1] has not minimal distance to offset[0])
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
			)
		);

		// no minimal distance: with lengths + files
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offset[1] has not minimal distance to offset[0])
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 },
			// files
			{ }
			)
		);

		// exeeds maximum: with leadout
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offset[14] exceeds maximal block address)
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320,
				static_cast<int32_t>(arcs::CDDA.MAX_OFFSET + 1) /* BOOM */ },
			// leadout
			253038
			)
		);

		// exeeds maximum: with lengths + files
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offset[14] exceeds maximal block address)
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				139495, 157863, 198495, 213368, 225320,
				static_cast<int32_t>(arcs::CDDA.MAX_OFFSET + 1) /* BOOM */ },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 },
			// files
			{ }
			)
		);

		// not ascending order: with leadout
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offsets[9] is smaller than offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106000 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
			)
		);

		// not ascending order: with lengths + files
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offsets[9] is smaller than offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106000 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 },
			// files
			{ }
			)
		);

		// not ascending order: with leadout
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offsets[9] is equal to offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106333 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
			)
		);

		// not ascending order: with lengths + files
		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets (offsets[9] is equal to offsets[8])
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				106333 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, 18935 },
			// files
			{ }
			)
		);
    }


    SECTION ( "Build fails for inconsistent trackcount and offsets" )
    {
		// Track count 0 is illegal

		CHECK_THROWS ( builder.build(
			// track count
			0, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( builder.build(
			// track count
			0, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ },
			// files
			{ }
		));


		// Track count is smaller than number of offsets

		CHECK_THROWS ( builder.build(
			// track count
			5, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( builder.build(
			// track count
			5, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ },
			// files
			{ }
		));


		// Track count is bigger than number of offsets

		CHECK_THROWS ( builder.build(
			// track count
			18, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( builder.build(
			// track count
			18, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ },
			// files
			{ }
		));


		// Track count is bigger than legal maximum

		CHECK_THROWS ( builder.build(
			// track count
			arcs::CDDA.MAX_TRACKCOUNT + 1, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		));

		CHECK_THROWS ( builder.build(
			// track count
			arcs::CDDA.MAX_TRACKCOUNT + 1, /* BOOM */
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ },
			// files
			{ }
		));
    }


    SECTION ( "Build fails for inconsistent leadout and offsets" )
    {
		// Leadout 0 is illegal

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			0 /* BOOM */
		));

		// Leadout exceeds maximal legal value

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			arcs::CDDA.MAX_BLOCK_ADDRESS + 1 /* BOOM */
		));

		// Leadout has not minimal distance to last offset

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			234103 + arcs::CDDA.MIN_TRACK_LEN_FRAMES - 1 /* BOOM */
		));
    }


    SECTION ( "Build fails for inconsistent lengths" )
    {
		// length[4] is smaller than legal minimum

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228,
				static_cast<int32_t>(arcs::CDDA.MIN_TRACK_LEN_FRAMES - 1)
				/* BOOM */,
				19513, 18155, 18325, 33075, 18368, 40152, 14798, 11952, 8463,
				18935 },
			// files
			{ }
		));

		// sum of lengths exceeds legal maximum

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228,
				static_cast<int32_t>(arcs::CDDA.MAX_OFFSET) /* BOOM */, 19513,
				18155, 18325, 33075, 18368, 40152, 14798, 11952, 8463, 18935 },
			// files
			{ }
		));

		// more than 99 tracks + more lengths than offsets

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			std::vector<int32_t>(100), /* BOOM */
			// files
			{ }
		));

		// no lengths

		CHECK_THROWS ( builder.build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ }, /* BOOM */
			// files
			{ }
		));
    }
}
