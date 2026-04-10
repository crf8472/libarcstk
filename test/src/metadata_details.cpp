#include "catch2/catch_test_macros.hpp"
#include <stdexcept>

/**
 * \file
 *
 * \brief Fixtures for metadata_details.hpp.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // for AudioSize, CDDA, ToC
#endif
#ifndef LIBARCSTK_METADATA_DETAILS_HPP_
#include "metadata_details.hpp"   // TO BE TESTED
#endif


TEST_CASE ("convert_to_bytes()", "[meta]")
{
	using arcstk::details::convert_to_bytes;
	using arcstk::UNIT;

	SECTION ( "Converts samples to bytes correctly" )
	{
		CHECK ( convert_to_bytes(3072300, UNIT::SAMPLES) == 12289200 );
	}

	SECTION ( "Converts frames to bytes correctly" )
	{
		CHECK ( convert_to_bytes(5225, UNIT::FRAMES) == 12289200 );
	}

	SECTION ( "Returns bytes when bytes were passed" )
	{
		CHECK ( convert_to_bytes(12345, UNIT::BYTES) == 12345 );
	}
}


TEST_CASE ( "toc::construct()", "[meta]" )
{
	using arcstk::AudioSize;
	using arcstk::UNIT;
	using arcstk::ToCData;
	using arcstk::toc::construct;

	// "Bach: Organ Concertos", Simon Preston, DGG
	const auto toc0 = construct(
		// leadout
		253038,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
		157863, 198495, 213368, 225320, 234103 }
	);

	SECTION ( "Succeeds for legal trackcount, offsets, non-zero leadout" )
	{
		const auto toc_data_0 = ToCData
		{
			AudioSize { 253038, UNIT::FRAMES },
			AudioSize {     33, UNIT::FRAMES },
			AudioSize {   5225, UNIT::FRAMES },
			AudioSize {   7390, UNIT::FRAMES },
			AudioSize {  23380, UNIT::FRAMES },
			AudioSize {  35608, UNIT::FRAMES },
			AudioSize {  49820, UNIT::FRAMES },
			AudioSize {  69508, UNIT::FRAMES },
			AudioSize {  87733, UNIT::FRAMES },
			AudioSize { 106333, UNIT::FRAMES },
			AudioSize { 139495, UNIT::FRAMES },
			AudioSize { 157863, UNIT::FRAMES },
			AudioSize { 198495, UNIT::FRAMES },
			AudioSize { 213368, UNIT::FRAMES },
			AudioSize { 225320, UNIT::FRAMES },
			AudioSize { 234103, UNIT::FRAMES }
		};

		CHECK ( toc0 == toc_data_0 );
	}
}


TEST_CASE ( "toc::total_tracks()", "[meta]" )
{
	using arcstk::ToCData;
	using arcstk::toc::total_tracks;

	using arcstk::AudioSize;
	using arcstk::UNIT;

	const auto toc_data_0 = ToCData
	{
		AudioSize { 253038, UNIT::FRAMES },
		AudioSize {     33, UNIT::FRAMES },
		AudioSize {   5225, UNIT::FRAMES },
		AudioSize {   7390, UNIT::FRAMES },
		AudioSize {  23380, UNIT::FRAMES },
		AudioSize {  35608, UNIT::FRAMES },
		AudioSize {  49820, UNIT::FRAMES },
		AudioSize {  69508, UNIT::FRAMES },
		AudioSize {  87733, UNIT::FRAMES },
		AudioSize { 106333, UNIT::FRAMES },
		AudioSize { 139495, UNIT::FRAMES },
		AudioSize { 157863, UNIT::FRAMES },
		AudioSize { 198495, UNIT::FRAMES },
		AudioSize { 213368, UNIT::FRAMES },
		AudioSize { 225320, UNIT::FRAMES },
		AudioSize { 234103, UNIT::FRAMES }
	};

	SECTION ( "Succeeds for non-empty ToCData" )
	{
		CHECK ( total_tracks(toc_data_0) == 15 );
	}
}


TEST_CASE ( "toc::offsets()", "[meta]" )
{
	using arcstk::ToCData;
	using arcstk::toc::offsets;

	using arcstk::AudioSize;
	using arcstk::UNIT;

	const auto toc_data_0 = ToCData
	{
		AudioSize { 253038, UNIT::FRAMES },
		AudioSize {     33, UNIT::FRAMES },
		AudioSize {   5225, UNIT::FRAMES },
		AudioSize {   7390, UNIT::FRAMES },
		AudioSize {  23380, UNIT::FRAMES },
		AudioSize {  35608, UNIT::FRAMES },
		AudioSize {  49820, UNIT::FRAMES },
		AudioSize {  69508, UNIT::FRAMES },
		AudioSize {  87733, UNIT::FRAMES },
		AudioSize { 106333, UNIT::FRAMES },
		AudioSize { 139495, UNIT::FRAMES },
		AudioSize { 157863, UNIT::FRAMES },
		AudioSize { 198495, UNIT::FRAMES },
		AudioSize { 213368, UNIT::FRAMES },
		AudioSize { 225320, UNIT::FRAMES },
		AudioSize { 234103, UNIT::FRAMES }
	};

	SECTION ( "Succeeds for valid, non-exceeding track number" )
	{
		CHECK ( offsets(toc_data_0) == std::vector<AudioSize>{
			AudioSize {     33, UNIT::FRAMES },
			AudioSize {   5225, UNIT::FRAMES },
			AudioSize {   7390, UNIT::FRAMES },
			AudioSize {  23380, UNIT::FRAMES },
			AudioSize {  35608, UNIT::FRAMES },
			AudioSize {  49820, UNIT::FRAMES },
			AudioSize {  69508, UNIT::FRAMES },
			AudioSize {  87733, UNIT::FRAMES },
			AudioSize { 106333, UNIT::FRAMES },
			AudioSize { 139495, UNIT::FRAMES },
			AudioSize { 157863, UNIT::FRAMES },
			AudioSize { 198495, UNIT::FRAMES },
			AudioSize { 213368, UNIT::FRAMES },
			AudioSize { 225320, UNIT::FRAMES },
			AudioSize { 234103, UNIT::FRAMES } } );
	}
}


TEST_CASE ( "toc::leadout()", "[meta]" )
{
	using arcstk::ToCData;
	using arcstk::toc::leadout;

	using arcstk::AudioSize;
	using arcstk::UNIT;

	const auto toc_data_0 = ToCData
	{
		AudioSize { 253038, UNIT::FRAMES },
		AudioSize {     33, UNIT::FRAMES },
		AudioSize {   5225, UNIT::FRAMES },
		AudioSize {   7390, UNIT::FRAMES },
		AudioSize {  23380, UNIT::FRAMES },
		AudioSize {  35608, UNIT::FRAMES },
		AudioSize {  49820, UNIT::FRAMES },
		AudioSize {  69508, UNIT::FRAMES },
		AudioSize {  87733, UNIT::FRAMES },
		AudioSize { 106333, UNIT::FRAMES },
		AudioSize { 139495, UNIT::FRAMES },
		AudioSize { 157863, UNIT::FRAMES },
		AudioSize { 198495, UNIT::FRAMES },
		AudioSize { 213368, UNIT::FRAMES },
		AudioSize { 225320, UNIT::FRAMES },
		AudioSize { 234103, UNIT::FRAMES }
	};

	SECTION ( "Succeeds for valid, non-exceeding track number" )
	{
		CHECK ( leadout(toc_data_0).frames() == 253038 );
	}
}


TEST_CASE ( "toc::offset()", "[meta]" )
{
	using arcstk::ToCData;
	using arcstk::toc::offset;

	using arcstk::AudioSize;
	using arcstk::UNIT;

	const auto toc_data_0 = ToCData
	{
		AudioSize { 253038, UNIT::FRAMES },
		AudioSize {     33, UNIT::FRAMES },
		AudioSize {   5225, UNIT::FRAMES },
		AudioSize {   7390, UNIT::FRAMES },
		AudioSize {  23380, UNIT::FRAMES },
		AudioSize {  35608, UNIT::FRAMES },
		AudioSize {  49820, UNIT::FRAMES },
		AudioSize {  69508, UNIT::FRAMES },
		AudioSize {  87733, UNIT::FRAMES },
		AudioSize { 106333, UNIT::FRAMES },
		AudioSize { 139495, UNIT::FRAMES },
		AudioSize { 157863, UNIT::FRAMES },
		AudioSize { 198495, UNIT::FRAMES },
		AudioSize { 213368, UNIT::FRAMES },
		AudioSize { 225320, UNIT::FRAMES },
		AudioSize { 234103, UNIT::FRAMES }
	};

	SECTION ( "Succeeds for valid, non-exceeding track number" )
	{
		CHECK ( offset( 1, toc_data_0).frames() ==     33 );
		CHECK ( offset( 2, toc_data_0).frames() ==   5225 );
		CHECK ( offset( 3, toc_data_0).frames() ==   7390 );
		CHECK ( offset( 4, toc_data_0).frames() ==  23380 );
		CHECK ( offset( 5, toc_data_0).frames() ==  35608 );
		CHECK ( offset( 6, toc_data_0).frames() ==  49820 );
		CHECK ( offset( 7, toc_data_0).frames() ==  69508 );
		CHECK ( offset( 8, toc_data_0).frames() ==  87733 );
		CHECK ( offset( 9, toc_data_0).frames() == 106333 );
		CHECK ( offset(10, toc_data_0).frames() == 139495 );
		CHECK ( offset(11, toc_data_0).frames() == 157863 );
		CHECK ( offset(12, toc_data_0).frames() == 198495 );
		CHECK ( offset(13, toc_data_0).frames() == 213368 );
		CHECK ( offset(14, toc_data_0).frames() == 225320 );
		CHECK ( offset(15, toc_data_0).frames() == 234103 );
	}

	SECTION ( "Throws for invalid track number" )
	{
		CHECK_THROWS ( offset( 0, toc_data_0) );
		CHECK_THROWS_AS ( offset(16, toc_data_0), std::out_of_range );
	}

}


TEST_CASE ("validate_offsets()", "[meta]")
{
	using arcstk::details::validate::validate_offsets_leadout;
	using arcstk::details::validate::validate_offsets;
	using arcstk::CDDA;
	using arcstk::InvalidMetadataException;
	using Req = arcstk::MetadataRequirement;

	using arcstk::toc::construct;

	// some legal values

	const auto valid_nz = construct(
		253038,
		{
			33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
			139495, 157863, 198495, 213368, 225320, 234103
		}
	);

	const auto valid_z = construct(
		0,
		{
			33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
			139495, 157863, 198495, 213368, 225320, 234103
		}
	);

	const auto valid_z2 = construct( // leadout + track 1 are 0
		0,
		{
			0, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
			139495, 157863, 198495, 213368, 225320, 234103
		}
	);

	SECTION ( "Succeeds for correct offsets with non-zero leadout" )
	{
		CHECK_NOTHROW ( validate_offsets_leadout(valid_nz) );
		CHECK_NOTHROW ( validate_offsets(valid_nz) );
	}

	SECTION ( "Succeeds for correct offsets with zero leadout" )
	{
		CHECK_NOTHROW ( validate_offsets(valid_z) );
		CHECK_NOTHROW ( validate_offsets(valid_z2) );
	}

	SECTION ( "Fails if OFFSETS_ARE_NONNEGATIVE is violated" )
	{
		// offset[15] is bigger than legal maximum
		const auto fail = construct(
			253038,
			{
				33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				139495, 157863, 198495, -17 /* BOOM */, 225320, 234103
			}
		);

		try
		{
			validate_offsets_leadout(fail);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     == Req::OFFSETS_ARE_NONNEGATIVE );
			CHECK ( e.violating_value() == -17 );
			CHECK ( e.pos()             ==  13 );
		}
	}

	SECTION ( "Fails if OFFSETS_ARE_NOT_GREATER_THAN_MAX is violated" )
	{
		const auto ILLEGAL_OFFSET = int32_t { CDDA::MAX_BLOCK_ADDRESS + 1 };

		// offset[15] is bigger than legal maximum
		const auto fail = construct(
			253038,
			{
				33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
				139495, 157863, 198495, 213368, 225320, ILLEGAL_OFFSET/* BOOM */
			}
		);

		try
		{
			validate_offsets_leadout(fail);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     ==
					Req::OFFSETS_ARE_NOT_GREATER_THAN_MAX );
			CHECK ( e.violating_value() == ILLEGAL_OFFSET );
			CHECK ( e.pos()             == 15 );
		}

	}

	SECTION ( "Fails if OFFSETS_ARE_STRICTLY_WELLORDERED is violated" )
	{
		// offset[8] is less than offset[7], so length < 0
		const auto fail_1 = construct(
			253038,
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 69507 /* BOOM */,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		);

		try
		{
			validate_offsets_leadout(fail_1);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     ==
					Req::OFFSETS_ARE_STRICTLY_WELLORDERED );
			CHECK ( e.violating_value() == 69507 );
			CHECK ( e.pos()             ==     8 );
		}

		// offset[7] and offset[8] are equal, so length == 0,
		// but neither on leadout nor on track 1
		const auto fail_2 = construct(
			253038,
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 69508 /* BOOM */,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		);

		try
		{
			validate_offsets_leadout(fail_2);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     ==
					Req::OFFSETS_ARE_STRICTLY_WELLORDERED );
			CHECK ( e.violating_value() == 69508 );
			CHECK ( e.pos()             ==     8 );
		}

	}

	SECTION ( "Fails if OFFSETS_HAVE_MIN_DIST is violated" )
	{
		// offset[2] has less than minimal legal distance to offset[1]
		// (produces a legal track length though)
		const auto fail = construct(
			253038,
			{ 33, 184 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		);

		try
		{
			validate_offsets_leadout(fail);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     == Req::OFFSETS_HAVE_MIN_DIST );
			CHECK ( e.violating_value() == 184 );
			CHECK ( e.pos()             ==  2 );
		}

		// for safety
		CHECK_THROWS_AS ( validate_offsets(fail), InvalidMetadataException );

	}

	SECTION ( "Fails if OFFSETS_PRODUCE_MIN_LENGTHS is violated" )
	{
		// distance between offset[1] and offset[2] is not a legal track length
		const auto fail = construct(
			253038,
			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		);

		try
		{
			validate_offsets_leadout(fail);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     == Req::OFFSETS_PRODUCE_MIN_LENGTHS );
			CHECK ( e.violating_value() == 34 );
			CHECK ( e.pos()             ==  2 );
		}

		// for safety
		CHECK_THROWS_AS ( validate_offsets(fail), InvalidMetadataException );
	}

	SECTION ( "Fails if TOTAL_TRACKS_IS_POSITIVE is violated" )
	{
		// track count smaller than legal minimum
		const auto fail = construct(300, { /* 0 offsets */});

		try
		{
			validate_offsets_leadout(fail);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     == Req::TOTAL_TRACKS_IS_POSITIVE );
			CHECK ( e.violating_value() == 0 );
			CHECK ( e.pos()             > CDDA::MAX_TRACKCOUNT );
		}

		// for safety
		CHECK_THROWS_AS ( validate_offsets(fail), InvalidMetadataException );
	}

	SECTION ( "Fails if TOTAL_TRACKS_IS_NOT_GREATER_THAN_MAX is violated" )
	{
		// track count bigger than legal maximum
		const auto fail = construct(300, std::vector<int32_t>(100));

		try
		{
			validate_offsets_leadout(fail);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     ==
					Req::TOTAL_TRACKS_IS_NOT_GREATER_THAN_MAX );
			CHECK ( e.violating_value() == 100 );
			CHECK ( e.pos()             > CDDA::MAX_TRACKCOUNT );
		}

		// for safety
		CHECK_THROWS_AS ( validate_offsets(fail), InvalidMetadataException );
	}

	SECTION ( "Fails if LEADOUT_IS_NONNEGATIVE is violated" )
	{
		try
		{
			validate_offsets_leadout(valid_z);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     == Req::LEADOUT_IS_NONNEGATIVE );
			CHECK ( e.violating_value() == 0 );
			CHECK ( e.pos()             == 0 );
		}

		try
		{
			validate_offsets_leadout(valid_z2);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     == Req::LEADOUT_IS_NONNEGATIVE );
			CHECK ( e.violating_value() == 0 );
			CHECK ( e.pos()             == 0 );
		}
	}

	SECTION ( "Fails if TOTAL_FILENAMES_MATCH_TOTAL_TRACKS is violated" )
	{
		using arcstk::details::validate_filenames_impl;

		const auto toc = construct(901, { 0, 301, 601 });

		// total tracks == 3, requires 3 filenames

		const auto too_many_filenames = std::vector<std::string>
			{ "foo", "bar", "baz", "quux" };

		const auto too_less_filenames = std::vector<std::string>
			{ "foo", "bar" };

		try
		{
			validate_filenames_impl(toc, too_many_filenames);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     ==
					Req::TOTAL_FILENAMES_MATCH_TOTAL_TRACKS );
			CHECK ( e.violating_value() == 4 );
			CHECK ( e.pos()             > CDDA::MAX_TRACKCOUNT );
		}

		try
		{
			validate_filenames_impl(toc, too_less_filenames);

			FAIL ( "Expected InvalidMetadataException was not thrown" );
		} catch (const InvalidMetadataException& e)
		{
			CHECK ( e.requirement()     ==
					Req::TOTAL_FILENAMES_MATCH_TOTAL_TRACKS );
			CHECK ( e.violating_value() == 2 );
			CHECK ( e.pos()             > CDDA::MAX_TRACKCOUNT );
		}
	}

// 	SECTION ( "Validation succeeds for correct lengths" )
// 	{
// 		// complete correct lengths
// 		CHECK_NOTHROW ( TOCValidator::validate_lengths(
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, 18935 })
// 		);
//
// 		// incomplete correct lengths
// 		CHECK_NOTHROW ( TOCValidator::validate_lengths(
// 			{ 5192, 2165, 15885, -1 }) );
// 	}
//
//
// 	SECTION ( "Validation fails for incorrect lengths" )
// 	{
// 		// one length smaller than legal minimum
// 		CHECK_THROWS ( TOCValidator::validate_lengths(
// 			{ 5192, 2165, 15885,
// 				CDDA::MIN_TRACK_LEN_FRAMES - 1 /* BOOM */,
// 				5766 }
// 		));
//
// 		// sum of lengths greater than legal maximum of 99 min
// 		CHECK_THROWS ( TOCValidator::validate_lengths(
// 			{ 5192, 2165, 15885,
// 				360000 /* Redbook maximum */,
// 				100000 /* Exceeds maximum of 99 min */,
// 				-1 }
// 		));
//
// 		// track count bigger than legal maximum
// 		CHECK_THROWS ( TOCValidator::validate_lengths(
// 					std::vector<int32_t>(100)) );
//
// 		// last length smaller than legal minimum
// 		CHECK_THROWS ( TOCValidator::validate_lengths(
// 			{ 5192, 2165, 15885, CDDA::MIN_TRACK_LEN_FRAMES - 1 /* BOOM */ }
// 		));
//
// 		// track count smaller than legal minimum
// 		CHECK_THROWS (TOCValidator::validate_lengths(std::vector<int32_t>()));
// 	}
//
//
// 	SECTION ( "Validation succeeds for correct leadouts" )
// 	{
// 		// legal minimum
// 		CHECK_NOTHROW (
// 			TOCValidator::validate_leadout(CDDA::MIN_TRACK_OFFSET_DIST)
// 		);
//
// 		// some legal value
// 		CHECK_NOTHROW ( TOCValidator::validate_leadout(253038) );
//
// 		// legal maximum
// 		CHECK_NOTHROW ( TOCValidator::validate_leadout(CDDA::MAX_OFFSET)     );
// 		CHECK_THROWS  ( TOCValidator::validate_leadout(CDDA::MAX_OFFSET + 1) );
//
// 		// TODO more values
// 	}
//
//
// 	SECTION ( "Validation fails for non-standard leadouts" )
// 	{
// 		// legal maximum
// 		CHECK_THROWS ( TOCValidator::validate_leadout(CDDA::MAX_BLOCK_ADDRESS) );
// 	}
//
//
// 	SECTION ( "Validation fails for incorrect leadouts" )
// 	{
// 		// 0 (smaller than legal minimum)
// 		CHECK_THROWS ( TOCValidator::validate_leadout(0) );
//
// 		// greater than 0, but smaller than legal minimum
// 		CHECK_THROWS ( TOCValidator::validate_leadout(
// 			CDDA::MIN_TRACK_OFFSET_DIST - 1
// 		));
//
// 		// bigger than legal maximum
// 		CHECK_THROWS ( TOCValidator::validate_leadout(
// 			CDDA::MAX_BLOCK_ADDRESS + 1
// 		));
// 	}
//
//
// 	SECTION ( "Validation succeeds for correct trackcounts" )
// 	{
// 		// legal minimum
// 		CHECK_NOTHROW ( TOCValidator::validate_trackcount(1) );
//
// 		// legal values greater than minimum and smaller than maximum
// 		for (int i = 2; i < 99; ++i)
// 		{
// 			// 2 - 98
// 			CHECK_NOTHROW ( TOCValidator::validate_trackcount(i) );
// 		}
//
// 		// legal maximum
// 		CHECK_NOTHROW ( TOCValidator::validate_trackcount(99) );
// 	}
//
//
// 	SECTION ( "Validation fails for incorrect trackcounts" )
// 	{
// 		// smaller than legal minimum
// 		CHECK_THROWS ( TOCValidator::validate_trackcount(0) );
//
// 		// bigger than legal maximum
// 		CHECK_THROWS ( TOCValidator::validate_trackcount(100) );
//
// 		// bigger than legal maximum
// 		CHECK_THROWS ( TOCValidator::validate_trackcount(500) );
//
// 		// bigger than legal maximum
// 		CHECK_THROWS ( TOCValidator::validate_trackcount(999) );
//
// 		// bigger than legal maximum
// 		CHECK_THROWS ( TOCValidator::validate_trackcount(65535) );
// 	}

}

//TEST_CASE ("validate_lengths()", "[meta]")
//{
//	using arcstk::details::validate::validate_lengths;
//
//	using arcstk::toc::construct;
//
//	const auto toc_data { construct(
//		253038,
//		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
//			157863, 198495, 213368, 225320, 234103 }
//	)};
//
//	SECTION ("Validates correct distances correctly")
//	{
//		CHECK_NOTHROW ( validate_lengths(toc_data) );
//	}
//
//	SECTION ("Throws on leadout too short")
//	{
//		const auto toc_data1 { construct(
//			234104, // last track is 1 frame
//			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
//				157863, 198495, 213368, 225320, 234103 }
//		)};
//
//		CHECK_THROWS ( validate_lengths(toc_data1) );
//	}
//
//	SECTION ("Throws on first track too short")
//	{
//		const auto toc_data1 { construct(
//			253038,
//			{ 33, 33 /* 0 length */, 7390, 23380, 35608, 49820, 69508, 87733,
//				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
//		)};
//
//		CHECK_THROWS ( validate_lengths(toc_data1) );
//	}
//
//	SECTION ("Throws on some mid track too short")
//	{
//		const auto toc_data1 { construct(
//			253038,
//			{ 33, 5225, 7390, 23380, 35608, 49820, 49825/* too short */, 87733,
//				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
//		)};
//
//		CHECK_THROWS ( validate_lengths(toc_data1) );
//	}
//}

// // TOCBuilder
//
//
// TEST_CASE ( "TOCBuilder: build with leadout", "[identifier] [tocbuilder]" )
// {
// 	using arcstk::details::TOCBuilder;
//
//
// }
//
//
// TEST_CASE ( "TOCBuilder: build with lengths and files",
// 	"[identifier] [tocbuilder]" )
// {
// 	using arcstk::details::TOCBuilder;
//
//
// 	SECTION ( "Build succeeds for correct trackcount, offsets, lengths" )
// 	{
// 		// "Bach: Organ Concertos", Simon Preston, DGG
// 		auto toc1 = make_toc(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075, 18368,
// 				40152, 14798, 11952, 8463, 18935 }
// 		);
//
// 		CHECK ( toc1->total_tracks() == 15 );
// 		CHECK ( toc1->leadout()      == 253038 );
//
// 		CHECK_THROWS ( toc1->offset(0)  ==      0 );
//
// 		CHECK ( toc1->offset(1)  ==     33 );
// 		CHECK ( toc1->offset(2)  ==   5225 );
// 		CHECK ( toc1->offset(3)  ==   7390 );
// 		CHECK ( toc1->offset(4)  ==  23380 );
// 		CHECK ( toc1->offset(5)  ==  35608 );
// 		CHECK ( toc1->offset(6)  ==  49820 );
// 		CHECK ( toc1->offset(7)  ==  69508 );
// 		CHECK ( toc1->offset(8)  ==  87733 );
// 		CHECK ( toc1->offset(9)  == 106333 );
// 		CHECK ( toc1->offset(10) == 139495 );
// 		CHECK ( toc1->offset(11) == 157863 );
// 		CHECK ( toc1->offset(12) == 198495 );
// 		CHECK ( toc1->offset(13) == 213368 );
// 		CHECK ( toc1->offset(14) == 225320 );
// 		CHECK ( toc1->offset(15) == 234103 );
//
// 		CHECK_THROWS ( toc1->offset(16) == 0 );
//
// 		CHECK_THROWS ( toc1->parsed_length(0) == 0 );
//
// 		CHECK ( toc1->parsed_length(1)  ==  5192 );
// 		CHECK ( toc1->parsed_length(2)  ==  2165 );
// 		CHECK ( toc1->parsed_length(3)  == 15885 );
// 		CHECK ( toc1->parsed_length(4)  == 12228 );
// 		CHECK ( toc1->parsed_length(5)  == 13925 );
// 		CHECK ( toc1->parsed_length(6)  == 19513 );
// 		CHECK ( toc1->parsed_length(7)  == 18155 );
// 		CHECK ( toc1->parsed_length(8)  == 18325 );
// 		CHECK ( toc1->parsed_length(9)  == 33075 );
// 		CHECK ( toc1->parsed_length(10) == 18368 );
// 		CHECK ( toc1->parsed_length(11) == 40152 );
// 		CHECK ( toc1->parsed_length(12) == 14798 );
// 		CHECK ( toc1->parsed_length(13) == 11952 );
// 		CHECK ( toc1->parsed_length(14) ==  8463 );
// 		CHECK ( toc1->parsed_length(15) == 18935 );
//
// 		CHECK_THROWS ( toc1->parsed_length(16) == 0 );
// 	}
//
//
// 	SECTION ( "Build succeeds for trackcount, offsets and incomplete lengths" )
// 	{
// 		auto toc2 = make_toc(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
// 		);
//
//
// 		CHECK ( toc2->total_tracks() == 15 );
// 		CHECK ( toc2->leadout()      == 0 ); // missing !
//
// 		CHECK_THROWS ( toc2->offset(0)  ==      0 );
//
// 		CHECK ( toc2->offset(1)  ==     33 );
// 		CHECK ( toc2->offset(2)  ==   5225 );
// 		CHECK ( toc2->offset(3)  ==   7390 );
// 		CHECK ( toc2->offset(4)  ==  23380 );
// 		CHECK ( toc2->offset(5)  ==  35608 );
// 		CHECK ( toc2->offset(6)  ==  49820 );
// 		CHECK ( toc2->offset(7)  ==  69508 );
// 		CHECK ( toc2->offset(8)  ==  87733 );
// 		CHECK ( toc2->offset(9)  == 106333 );
// 		CHECK ( toc2->offset(10) == 139495 );
// 		CHECK ( toc2->offset(11) == 157863 );
// 		CHECK ( toc2->offset(12) == 198495 );
// 		CHECK ( toc2->offset(13) == 213368 );
// 		CHECK ( toc2->offset(14) == 225320 );
// 		CHECK ( toc2->offset(15) == 234103 );
//
// 		CHECK_THROWS ( toc2->offset(16) == 0 );
//
// 		CHECK_THROWS ( toc2->parsed_length(0) == 0 ); // as defined!
//
// 		CHECK ( toc2->parsed_length(1)  ==  5192 );
// 		CHECK ( toc2->parsed_length(2)  ==  2165 );
// 		CHECK ( toc2->parsed_length(3)  == 15885 );
// 		CHECK ( toc2->parsed_length(4)  == 12228 );
// 		CHECK ( toc2->parsed_length(5)  == 13925 );
// 		CHECK ( toc2->parsed_length(6)  == 19513 );
// 		CHECK ( toc2->parsed_length(7)  == 18155 );
// 		CHECK ( toc2->parsed_length(8)  == 18325 );
// 		CHECK ( toc2->parsed_length(9)  == 33075 );
// 		CHECK ( toc2->parsed_length(10) == 18368 );
// 		CHECK ( toc2->parsed_length(11) == 40152 );
// 		CHECK ( toc2->parsed_length(12) == 14798 );
// 		CHECK ( toc2->parsed_length(13) == 11952 );
// 		CHECK ( toc2->parsed_length(14) ==  8463 );
// 		CHECK ( toc2->parsed_length(15) ==     0 );  // missing !
//
// 		CHECK_THROWS ( toc2->parsed_length(16) == 0 );
// 	}
// }
//
//
// TEST_CASE ( "TOCBuilder: build fails with illegal values",
// 	"[identifier] [tocbuilder]" )
// {
// 	using arcstk::details::TOCBuilder;
// 	using arcstk::CDDA;
//
// 	SECTION ( "Build fails for incorrect offsets" )
// 	{
// 		// no minimal distance: with leadout
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offset[1] has not minimal distance to offset[0])
// 			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
// 				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 			)
// 		);
//
// 		// no minimal distance: with lengths + files
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offset[1] has not minimal distance to offset[0])
// 			{ 33, 34 /* BOOM */, 7390, 23380, 35608, 49820, 69508, 87733,
// 				106333, 139495, 157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, 18935 }
// 			)
// 		);
//
// 		// exeeds maximum: with leadout
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offset[14] exceeds maximal block address)
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, CDDA::MAX_OFFSET + 1/* BOOM */ },
// 			// leadout
// 			253038
// 			)
// 		);
//
// 		// exeeds maximum: with lengths + files
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offset[14] exceeds maximal block address)
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
// 				139495, 157863, 198495, 213368, 225320,
// 				CDDA::MAX_OFFSET + 1 /* BOOM */ },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, 18935 }
// 			)
// 		);
//
// 		// not ascending order: with leadout
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offsets[9] is smaller than offsets[8])
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
// 				106000 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 			)
// 		);
//
// 		// not ascending order: with lengths + files
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offsets[9] is smaller than offsets[8])
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
// 				106000 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, 18935 }
// 			)
// 		);
//
// 		// not ascending order: with leadout
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offsets[9] is equal to offsets[8])
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
// 				106333 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 			)
// 		);
//
// 		// not ascending order: with lengths + files
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets (offsets[9] is equal to offsets[8])
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
// 				106333 /* BOOM */, 157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, 18935 }
// 			)
// 		);
// 	}
//
//
// 	SECTION ( "Build fails for inconsistent trackcount and offsets" )
// 	{
// 		// Track count 0 is illegal
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			0, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			0, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
// 		));
//
//
// 		// Track count is smaller than number of offsets
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			5, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			5, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
// 		));
//
//
// 		// Track count is bigger than number of offsets
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			18, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			18, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
// 		));
//
//
// 		// Track count is bigger than legal maximum
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			CDDA::MAX_TRACKCOUNT + 1, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			253038
// 		));
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			CDDA::MAX_TRACKCOUNT + 1, /* BOOM */
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
// 				18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
// 		));
// 	}
//
//
// 	SECTION ( "Build fails for inconsistent leadout and offsets" )
// 	{
// 		// Leadout 0 is illegal
//
// 		CHECK_THROWS ( make_toc(
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
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 			157863, 198495, 213368, 225320, 234103 },
// 			// leadout
// 			CDDA::MAX_BLOCK_ADDRESS + 1 /* BOOM */
// 		));
//
// 		// Leadout has not minimal distance to last offset
//
// 		CHECK_THROWS ( make_toc(
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
// 	SECTION ( "Build fails for inconsistent lengths" )
// 	{
// 		// length[4] is smaller than legal minimum
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, CDDA::MIN_TRACK_LEN_FRAMES - 1/* BOOM */,
// 				19513, 18155, 18325, 33075, 18368, 40152, 14798, 11952, 8463,
// 				18935 }
// 		));
//
// 		// sum of lengths exceeds legal maximum
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ 5192, 2165, 15885, 12228, CDDA::MAX_OFFSET /* BOOM */, 19513,
// 				18155, 18325, 33075, 18368, 40152, 14798, 11952, 8463, 18935 }
// 		));
//
// 		// more than 99 tracks + more lengths than offsets
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			std::vector<int32_t>(100) /* BOOM */
// 		));
//
// 		// no lengths
//
// 		CHECK_THROWS ( make_toc(
// 			// track count
// 			15,
// 			// offsets
// 			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
// 				157863, 198495, 213368, 225320, 234103 },
// 			// lengths
// 			{ } /* BOOM */
// 		));
// 	}
// }

