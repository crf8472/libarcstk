#include "catch2/catch.hpp"

#include <cstdint>

#ifndef __LIBARCS_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif


/**
 * \file identifier_toc.cpp Fixtures for classes \ref TOCValidator and \ref details::TOCBuilder
 */


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

