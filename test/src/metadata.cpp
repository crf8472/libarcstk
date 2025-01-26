#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for details in module metadata
 */

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"          // for AudioSize, CDDA, ToC
#endif


TEST_CASE ( "toc", "[metadata]" )
{
	using arcstk::AudioSize;
	using arcstk::toc::construct;

	const auto leadout { 253038 };

	const std::vector<int32_t> offsets { 33, 5225, 7390, 23380, 35608, 49820,
		69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 };

	const auto toc_data { construct(leadout, offsets) };

	SECTION ( "toc::construct() constructs object correctly" )
	{
		CHECK ( toc_data.size() == 16 );
		CHECK ( toc_data[ 0] == AudioSize { 253038, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 1] == AudioSize {     33, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 2] == AudioSize {   5225, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 3] == AudioSize {   7390, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 4] == AudioSize {  23380, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 5] == AudioSize {  35608, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 6] == AudioSize {  49820, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 7] == AudioSize {  69508, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 8] == AudioSize {  87733, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[ 9] == AudioSize { 106333, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[10] == AudioSize { 139495, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[11] == AudioSize { 157863, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[12] == AudioSize { 198495, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[13] == AudioSize { 213368, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[14] == AudioSize { 225320, AudioSize::UNIT::FRAMES } );
		CHECK ( toc_data[15] == AudioSize { 234103, AudioSize::UNIT::FRAMES } );
	}


	SECTION ( "toc::construct() does not throw on empty offsets" )
	{
		const auto data { construct(100, {}) };

		CHECK ( data[0].total_frames() == 100 );
		CHECK ( data.size() == 1 );
	}


	SECTION ( "toc::leadout() yields correct leadout" )
	{
		const auto l { arcstk::toc::leadout(toc_data) };

		CHECK ( l == AudioSize { 253038, AudioSize::UNIT::FRAMES } );
	}


	SECTION ( "toc::set_leadout() set leadout correctly" )
	{
		const auto size = AudioSize { 8472, AudioSize::UNIT::FRAMES };

		auto data2 { toc_data };
		arcstk::toc::set_leadout(size, data2);

		CHECK ( arcstk::toc::leadout(data2) == size );
	}


	SECTION ( "toc::offsets() yields correct offsets" )
	{
		const auto offset { arcstk::toc::offsets(toc_data) };

		CHECK ( offset.size() == 15 );

		CHECK ( offset[ 0] == AudioSize {     33, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 1] == AudioSize {   5225, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 2] == AudioSize {   7390, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 3] == AudioSize {  23380, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 4] == AudioSize {  35608, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 5] == AudioSize {  49820, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 6] == AudioSize {  69508, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 7] == AudioSize {  87733, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 8] == AudioSize { 106333, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[ 9] == AudioSize { 139495, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[10] == AudioSize { 157863, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[11] == AudioSize { 198495, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[12] == AudioSize { 213368, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[13] == AudioSize { 225320, AudioSize::UNIT::FRAMES } );
		CHECK ( offset[14] == AudioSize { 234103, AudioSize::UNIT::FRAMES } );
	}


	SECTION ( "toc::total_tracks() yields correct total tracks" )
	{
		CHECK ( arcstk::toc::total_tracks(toc_data) == 15 );
	}


	SECTION ( "toc::complete() is TRUE iff the ToCData is complete" )
	{
		CHECK ( arcstk::toc::complete(toc_data) );
	}


	SECTION ( "toc::complete() is FALSE iff the ToCData is not complete" )
	{
		auto data_with_zero_leadout { toc_data };
		arcstk::toc::set_leadout(AudioSize { 0, AudioSize::UNIT::FRAMES },
				data_with_zero_leadout);

		REQUIRE ( data_with_zero_leadout.front().zero() );

		const auto empty_data = arcstk::ToCData{};

		REQUIRE ( empty_data.empty() );

		CHECK ( not arcstk::toc::complete(data_with_zero_leadout) );
		CHECK ( not arcstk::toc::complete(empty_data) );
	}
}


TEST_CASE ( "ToC", "[metadata]" )
{
	using arcstk::AudioSize;
	using arcstk::ToC;

	// "Bach: Organ Concertos", Simon Preston, DGG
	const auto toc = ToC {
		// leadout
		253038,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		{ "file" }
	};

	const auto toc_leadout { 253038 };

	const std::vector<int32_t> toc_offsets { 33, 5225, 7390, 23380, 35608, 49820,
		69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 };

	const std::vector<std::string> toc_filenames { "file" };

	const auto toc2 = ToC ( toc_leadout, toc_offsets, toc_filenames );


	SECTION ( "Returns correct leadout from ToC" )
	{
		CHECK ( toc.leadout().total_frames() == 253038 );
	}

	SECTION ( "Returns correct offsets from ToC" )
	{
		const auto offsets = toc.offsets();

		CHECK ( offsets.size() == 15 );

		CHECK ( offsets[ 0] == AudioSize {     33, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 1] == AudioSize {   5225, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 2] == AudioSize {   7390, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 3] == AudioSize {  23380, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 4] == AudioSize {  35608, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 5] == AudioSize {  49820, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 6] == AudioSize {  69508, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 7] == AudioSize {  87733, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 8] == AudioSize { 106333, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[ 9] == AudioSize { 139495, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[10] == AudioSize { 157863, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[11] == AudioSize { 198495, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[12] == AudioSize { 213368, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[13] == AudioSize { 225320, AudioSize::UNIT::FRAMES } );
		CHECK ( offsets[14] == AudioSize { 234103, AudioSize::UNIT::FRAMES } );
	}

	SECTION ( "Returns correct filenames from ToC" )
	{
		const auto filenames = toc.filenames();

		CHECK ( filenames.size() == 1 );

		CHECK ( filenames[0] == "file" );
	}

	SECTION ( "Equality operator is correct" )
	{
		CHECK ( toc == toc2 );
	}
}

