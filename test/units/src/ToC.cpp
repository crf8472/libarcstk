#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ToC.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


/*
TEST_CASE ( "ToC", "[toc] [metadata] [metadata]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::AudioSize;
	using arcstk::make_toc;
	using arcstk::ToC;
	using arcstk::UNIT;

	// "Bach: Organ Concertos", Simon Preston, DGG
	const auto toc = make_toc(
		// leadout
		253038,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		{ "file" }
	);

	const auto toc_leadout { 253038 };

	const std::vector<int32_t> toc_offsets { 33, 5225, 7390, 23380, 35608, 49820,
		69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 };

	const std::vector<std::string> toc_filenames { "file" };

	const auto toc2 = make_toc(toc_leadout, toc_offsets, toc_filenames);



	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const ToC copied { Copy<ToC>::construct(instance) };

		// --

        //CHECK(copied.value() == instance.value());
        //CHECK(copied.name()  == instance.name());
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const ToC moved { Move<ToC>::construct(instance) };

		// --

        //CHECK(moved.value() == instance.value());
        //CHECK(moved.name()  == instance.name());
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = ToC {};
		Copy<ToC>::assign(copied, instance);

		// --

        //CHECK(copied.value() == instance.value());
        //CHECK(copied.name()  == instance.name());
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = ToC {};
		Move<ToC>::assign(moved, instance);

		// --

        //CHECK(moved.value() == instance.value());
        //CHECK(moved.name()  == instance.name());
		FAIL ( "Move assignment test is missing" );
	}
}
*/

TEST_CASE ( "ToC default constructed instance",
		"[toc] [metadata] [metadata]" )
{
	const auto instance = arcstk::ToC{};

	SECTION ( "is empty()")
	{
		CHECK ( instance.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !instance );
	}
}

/*
TEST_CASE ( "ToC property", "[toc] [metadata] [metadata]" )
{
	using arcstk::ToC;

	auto defaulted = ToC {};


//	SECTION ("Equality operator == is correct")
//	{
//		FAIL ( "Equality operator test is missing" );
//	}
//
//	SECTION ("Stream-in operator << is correct")
//	{
//		FAIL ( "Stream-in operator << test is missing" );
//	}
//
//	SECTION ("operator bool() is correct")
//	{
//		FAIL ( "operator bool() test is missing" );
//	}
//
//	SECTION ("swap() is correct")
//	{
//		FAIL ( "swap() test is missing" );
//	}
//
//	SECTION ("to_string() is correct")
//	{
//		FAIL ( "to_string() test is missing" );
//	}
//
//	SECTION ("clone() is correct")
//	{
//		FAIL ( "clone() test is missing" );
//	}
//
//	SECTION ("size() is correct")
//	{
//		FAIL ( "size() test is missing" );
//	}
//
//	SECTION ("empty() is correct")
//	{
//		FAIL ( "empty() test is missing" );
//	}
}
*/

TEST_CASE ( "ToC functions", "[toc] [metadata] [metadata]" )
{
	using arcstk::AudioSize;
	using arcstk::make_toc;
	using arcstk::ToC;
	using arcstk::UNIT;

	// "Bach: Organ Concertos", Simon Preston, DGG
	const auto toc = make_toc(
		// leadout
		253038,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		{ "file" }
	);

	const auto toc_leadout { 253038 };

	const std::vector<int32_t> toc_offsets { 33, 5225, 7390, 23380, 35608, 49820,
		69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 };

	const std::vector<std::string> toc_filenames { "file" };

	const auto toc2 = make_toc(toc_leadout, toc_offsets, toc_filenames);



	// SECTION ("total_tracks is correct")
	// {
	// 	FAIL ("total_tracks test is missing");
	// }

	SECTION ("offsets is correct")
	{
		const auto offsets = toc.offsets();

		CHECK ( offsets.size() == 15 );

		CHECK ( offsets[ 0] == AudioSize {     33, UNIT::FRAMES } );
		CHECK ( offsets[ 1] == AudioSize {   5225, UNIT::FRAMES } );
		CHECK ( offsets[ 2] == AudioSize {   7390, UNIT::FRAMES } );
		CHECK ( offsets[ 3] == AudioSize {  23380, UNIT::FRAMES } );
		CHECK ( offsets[ 4] == AudioSize {  35608, UNIT::FRAMES } );
		CHECK ( offsets[ 5] == AudioSize {  49820, UNIT::FRAMES } );
		CHECK ( offsets[ 6] == AudioSize {  69508, UNIT::FRAMES } );
		CHECK ( offsets[ 7] == AudioSize {  87733, UNIT::FRAMES } );
		CHECK ( offsets[ 8] == AudioSize { 106333, UNIT::FRAMES } );
		CHECK ( offsets[ 9] == AudioSize { 139495, UNIT::FRAMES } );
		CHECK ( offsets[10] == AudioSize { 157863, UNIT::FRAMES } );
		CHECK ( offsets[11] == AudioSize { 198495, UNIT::FRAMES } );
		CHECK ( offsets[12] == AudioSize { 213368, UNIT::FRAMES } );
		CHECK ( offsets[13] == AudioSize { 225320, UNIT::FRAMES } );
		CHECK ( offsets[14] == AudioSize { 234103, UNIT::FRAMES } );
	}

	SECTION ("leadout is correct")
	{
		CHECK ( toc.leadout().frames() == 253038 );
	}

	SECTION ("filenames is correct")
	{
		const auto filenames = toc.filenames();

		CHECK ( filenames.size() == 1 );

		CHECK ( filenames[0] == "file" );
	}

	// SECTION ("set_offsets is correct")
	// {
	// 	FAIL ("set_offsets test is missing");
	// }

	// SECTION ("set_leadout is correct")
	// {
	// 	FAIL ("set_leadout test is missing");
	// }

	// SECTION ("set_filenames is correct")
	// {
	// 	FAIL ("set_filenames test is missing");
	// }

	// SECTION ("is_single_file is correct")
	// {
	// 	FAIL ("is_single_file test is missing");
	// }

	// SECTION ("validate is correct")
	// {
	// 	FAIL ("validate test is missing");
	// }

	// SECTION ("valid is correct")
	// {
	// 	FAIL ("valid test is missing");
	// }

	// SECTION ("complete is correct")
	// {
	// 	FAIL ("complete test is missing");
	// }

	// SECTION ("empty is correct")
	// {
	// 	FAIL ("empty test is missing");
	// }

	// SECTION ("operator bool is correct")
	// {
	// 	FAIL ("operator bool test is missing");
	// }

	// SECTION ("swap is correct")
	// {
	// 	FAIL ("swap test is missing");
	// }

	// SECTION ("equals is correct")
	// {
	// 	FAIL ("equals test is missing");
	// }

	// SECTION ("to_string is correct")
	// {
	// 	FAIL ("to_string test is missing");
	// }
}

