#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for metadata.hpp.
 */

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"           // TO BE TESTED
#endif

#include <cstdint>                // for int32_t
#include <type_traits>            // for is_copy_constructible
#include <string>                 // for string
#include <vector>                 // for vector


TEST_CASE ( "Units", "[meta]" )
{
	SECTION ("cdda_max<>() yields correct maximum values")
	{
		using arcstk::UNIT;
		using arcstk::cdda_max;

		CHECK (     449999 == cdda_max<UNIT::FRAMES>  );
		CHECK (  264599412 == cdda_max<UNIT::SAMPLES> );
		CHECK ( 1058397648 == cdda_max<UNIT::BYTES>   );
	}
}


// 	const auto size_too_big = AudioSize { // bigger than allowed MAX
// 		CDDA::MAX_OFFSET + 1, UNIT::FRAMES };
//
// 	const auto size_too_small = AudioSize { // smaller than allowed MIN
// 		CDDA::MIN_TRACK_LEN_FRAMES - 1, UNIT::FRAMES };


TEST_CASE ( "convert<>()", "[convert] [meta]" )
{
	using arcstk::AudioSize;
	using arcstk::convert;
	using arcstk::UNIT;

	// 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
	//	157863, 198495, 213368, 225320, 234103

	SECTION ( "converts frames to bytes correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::BYTES>(5225) == 12289200 );
	}

	SECTION ( "converts frames to samples correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(  5225) ==   3072300 );
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(253038) == 148786344 );
	}

	SECTION ( "converts bytes to frames correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::FRAMES>(12289200) == 5225 );
	}

	SECTION ( "converts bytes to samples correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::SAMPLES>(12289200) == 3072300 );
	}

	SECTION ( "converts samples to frames correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(  3072300) ==   5225 );
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(148786344) == 253038 );
	}

	SECTION ( "converts samples to bytes correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::BYTES>(3072300) == 12289200 );
	}
}


TEST_CASE ( "ToCData", "[tocdata] [meta]" )
{
	using arcstk::AudioSize;
	using arcstk::toc::construct;
	using arcstk::UNIT;

	const auto leadout { 253038 };

	const std::vector<int32_t> offsets { 33, 5225, 7390, 23380, 35608, 49820,
		69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 };

	const auto toc_data { construct(leadout, offsets) };

	SECTION ( "toc::construct() constructs object correctly" )
	{
		CHECK ( toc_data.size() == 16 );
		CHECK ( toc_data[ 0] == AudioSize { 253038, UNIT::FRAMES } );
		CHECK ( toc_data[ 1] == AudioSize {     33, UNIT::FRAMES } );
		CHECK ( toc_data[ 2] == AudioSize {   5225, UNIT::FRAMES } );
		CHECK ( toc_data[ 3] == AudioSize {   7390, UNIT::FRAMES } );
		CHECK ( toc_data[ 4] == AudioSize {  23380, UNIT::FRAMES } );
		CHECK ( toc_data[ 5] == AudioSize {  35608, UNIT::FRAMES } );
		CHECK ( toc_data[ 6] == AudioSize {  49820, UNIT::FRAMES } );
		CHECK ( toc_data[ 7] == AudioSize {  69508, UNIT::FRAMES } );
		CHECK ( toc_data[ 8] == AudioSize {  87733, UNIT::FRAMES } );
		CHECK ( toc_data[ 9] == AudioSize { 106333, UNIT::FRAMES } );
		CHECK ( toc_data[10] == AudioSize { 139495, UNIT::FRAMES } );
		CHECK ( toc_data[11] == AudioSize { 157863, UNIT::FRAMES } );
		CHECK ( toc_data[12] == AudioSize { 198495, UNIT::FRAMES } );
		CHECK ( toc_data[13] == AudioSize { 213368, UNIT::FRAMES } );
		CHECK ( toc_data[14] == AudioSize { 225320, UNIT::FRAMES } );
		CHECK ( toc_data[15] == AudioSize { 234103, UNIT::FRAMES } );
	}


	SECTION ( "toc::construct() does not throw on empty offsets" )
	{
		const auto data { construct(100, {}) };

		CHECK ( data[0].frames() == 100 );
		CHECK ( data.size() == 1 );
	}


	SECTION ( "toc::leadout() yields correct leadout" )
	{
		const auto l { arcstk::toc::leadout(toc_data) };

		CHECK ( l == AudioSize { 253038, UNIT::FRAMES } );
	}


	SECTION ( "toc::set_leadout() set leadout correctly" )
	{
		const auto size = AudioSize { 8472, UNIT::FRAMES };

		auto data2 { toc_data };
		arcstk::toc::set_leadout(size, data2);

		CHECK ( arcstk::toc::leadout(data2) == size );
	}


	SECTION ( "toc::offsets() yields correct offsets" )
	{
		const auto offset { arcstk::toc::offsets(toc_data) };

		CHECK ( offset.size() == 15 );

		CHECK ( offset[ 0] == AudioSize {     33, UNIT::FRAMES } );
		CHECK ( offset[ 1] == AudioSize {   5225, UNIT::FRAMES } );
		CHECK ( offset[ 2] == AudioSize {   7390, UNIT::FRAMES } );
		CHECK ( offset[ 3] == AudioSize {  23380, UNIT::FRAMES } );
		CHECK ( offset[ 4] == AudioSize {  35608, UNIT::FRAMES } );
		CHECK ( offset[ 5] == AudioSize {  49820, UNIT::FRAMES } );
		CHECK ( offset[ 6] == AudioSize {  69508, UNIT::FRAMES } );
		CHECK ( offset[ 7] == AudioSize {  87733, UNIT::FRAMES } );
		CHECK ( offset[ 8] == AudioSize { 106333, UNIT::FRAMES } );
		CHECK ( offset[ 9] == AudioSize { 139495, UNIT::FRAMES } );
		CHECK ( offset[10] == AudioSize { 157863, UNIT::FRAMES } );
		CHECK ( offset[11] == AudioSize { 198495, UNIT::FRAMES } );
		CHECK ( offset[12] == AudioSize { 213368, UNIT::FRAMES } );
		CHECK ( offset[13] == AudioSize { 225320, UNIT::FRAMES } );
		CHECK ( offset[14] == AudioSize { 234103, UNIT::FRAMES } );
	}


	SECTION ( "toc::lengths() yields correct offsets" )
	{
		const auto length { arcstk::toc::lengths(toc_data) };

		// parsed (with silence):
		//{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
		//		18368, 40152, 14798, 11952, 8463, 18935 }

		CHECK ( length.size() == 15 );
													// parsed from disc
		CHECK ( length[ 0].frames() ==  5192 );
		CHECK ( length[ 1].frames() ==  2165 );
		CHECK ( length[ 2].frames() == 15990 ); // 15885
		CHECK ( length[ 3].frames() == 12228 );
		CHECK ( length[ 4].frames() == 14212 ); // 13925
		CHECK ( length[ 5].frames() == 19688 ); // 19513
		CHECK ( length[ 6].frames() == 18225 ); // 18155
		CHECK ( length[ 7].frames() == 18600 ); // 18325
		CHECK ( length[ 8].frames() == 33162 ); // 33075
		CHECK ( length[ 9].frames() == 18368 );
		CHECK ( length[10].frames() == 40632 ); // 40152
		CHECK ( length[11].frames() == 14873 ); // 14798
		CHECK ( length[12].frames() == 11952 );
		CHECK ( length[13].frames() ==  8783 ); //  8463
		CHECK ( length[14].frames() == 18935 );
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
		arcstk::toc::set_leadout(AudioSize { 0, UNIT::FRAMES },
				data_with_zero_leadout);

		REQUIRE ( data_with_zero_leadout.front().zero() );

		const auto empty_data = arcstk::ToCData{};

		REQUIRE ( empty_data.empty() );

		CHECK ( not arcstk::toc::complete(data_with_zero_leadout) );
		CHECK ( not arcstk::toc::complete(empty_data) );
	}
}


TEST_CASE ( "ToC", "[toc] [meta]" )
{
	using arcstk::AudioSize;
	using arcstk::ToC;
	using arcstk::make_toc;
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


	SECTION ( "Returns correct leadout from ToC" )
	{
		CHECK ( toc->leadout().frames() == 253038 );
	}

	SECTION ( "Returns correct offsets from ToC" )
	{
		const auto offsets = toc->offsets();

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

	SECTION ( "Returns correct filenames from ToC" )
	{
		const auto filenames = toc->filenames();

		CHECK ( filenames.size() == 1 );

		CHECK ( filenames[0] == "file" );
	}

	SECTION ( "Equality operator is correct" )
	{
		CHECK ( *toc == *toc2 );
	}
}


TEST_CASE ( "AudioSize", "[audiosize] [meta]" )
{
	using arcstk::AudioSize;
	using arcstk::UNIT;

	AudioSize empty_size{};

	REQUIRE ( empty_size.zero() );

	//

	AudioSize size1;

	REQUIRE ( size1.zero() );

	size1.set_frames(253038);

	//

	AudioSize size2; // equals size1
					 //
	REQUIRE ( size2.zero() );

	size2.set_frames(253038);

	//

	AudioSize size3(253038,    UNIT::FRAMES);  // equal to size1 and size2
	AudioSize size4(148786344, UNIT::SAMPLES); // equal to size1 and size2
	AudioSize size5(595145376, UNIT::BYTES);   // equal to size1 and size2

	//

	AudioSize different_size; // not equal to size1-5

	REQUIRE ( different_size.zero() );

	different_size.set_frames(14827);

	//

	SECTION ("Default construction is as declared")
	{
		CHECK ( std::is_default_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_default_constructible<AudioSize>::value );
	}

	SECTION ("Parametized construction is as declared")
	{
		CHECK ( std::is_constructible<AudioSize,
				const int32_t, const UNIT>::value );

		CHECK ( std::is_nothrow_constructible<AudioSize,
				const int32_t, const UNIT>::value );
	}

	SECTION ("Parametized construction is correct")
	{
		CHECK ( empty_size.zero() );
		CHECK ( 0 == empty_size.bytes() );
		CHECK ( 0 == empty_size.frames() );

		// constructed with frames
		CHECK ( size1.frames()   ==    253038 );
		CHECK ( size1.samples()   == 148786344 );
		CHECK ( size1.bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size2.frames()   ==    253038 );
		CHECK ( size2.samples()   == 148786344 );
		CHECK ( size2.bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size3.frames()   ==    253038 );
		CHECK ( size3.samples()   == 148786344 );
		CHECK ( size3.bytes() == 595145376 );

		// constructed with samples
		CHECK ( size4.frames()   ==    253038 );
		CHECK ( size4.samples()   == 148786344 );
		CHECK ( size4.bytes() == 595145376 );

		// constructed with bytes
		CHECK ( size5.frames()   ==    253038 );
		CHECK ( size5.samples()   == 148786344 );
		CHECK ( size5.bytes() == 595145376 );

		// different size, constructed with frames
		CHECK ( different_size.frames()   ==    14827 );
		CHECK ( different_size.samples()   ==  8718276 );
		CHECK ( different_size.bytes() == 34873104 );
	}

	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_copy_constructible<AudioSize>::value );
	}

	// TODO Copy construction correct?

	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_move_constructible<AudioSize>::value );
	}

	SECTION ( "Assignment of frames is correct" )
	{
		using arcstk::AudioSize;

		AudioSize s{};

		REQUIRE ( s.zero() );

		s.set_frames(253038);

		CHECK ( s.frames()  ==    253038 );
		CHECK ( s.samples() == 148786344 );
		CHECK ( s.bytes()   == 595145376 );
	}

	SECTION ( "Assignment of samples is correct" )
	{
		using arcstk::AudioSize;

		AudioSize s{};

		REQUIRE ( s.zero() );

		s.set_samples(148786344);

		CHECK ( s.frames()  ==    253038 );
		CHECK ( s.samples() == 148786344 );
		CHECK ( s.bytes()   == 595145376 );
	}

	SECTION ( "Assignment of bytes is correct" )
	{
		using arcstk::AudioSize;

		AudioSize s{};

		REQUIRE ( s.zero() );

		s.set_bytes(595145376);

		CHECK ( s.frames()  ==    253038 );
		CHECK ( s.samples() == 148786344 );
		CHECK ( s.bytes()   == 595145376 );
	}

	// TODO Move construction correct?

	SECTION ("Equality operator is correct")
	{
		CHECK ( size1 == size1 );
		CHECK ( size2 == size2 );
		CHECK ( size3 == size3 );
		CHECK ( size4 == size4 );
		CHECK ( size5 == size5 );

		CHECK ( size1 == size2 );
		CHECK ( size2 == size1 );

		CHECK ( size2 == size3 );
		CHECK ( size3 == size2 );

		CHECK ( size3 == size4 );
		CHECK ( size4 == size3 );

		CHECK ( size4 == size5 );
		CHECK ( size5 == size4 );

		CHECK ( size1 == size3 );
		CHECK ( size1 == size4 );
		CHECK ( size1 == size5 );
		CHECK ( size2 == size4 );
		CHECK ( size2 == size5 );
		CHECK ( size3 == size5 );

		CHECK ( different_size != size1 );
		CHECK ( different_size != size2 );
		CHECK ( different_size != size4 );
		CHECK ( different_size != size5 );
	}

	SECTION ("Less-than operator is correct")
	{
		CHECK ( different_size < size1 );
		CHECK ( different_size < size2 );
		CHECK ( different_size < size3 );
		CHECK ( different_size < size4 );
		CHECK ( different_size < size5 );

		CHECK ( !(different_size > size1) );
		CHECK ( !(different_size > size2) );
		CHECK ( !(different_size > size3) );
		CHECK ( !(different_size > size4) );
		CHECK ( !(different_size > size5) );

		CHECK ( empty_size < different_size );
	}

	SECTION ("Swap operation is correct")
	{
		using std::swap;

		swap(size2, different_size);

		CHECK (  14827 == size2.frames() );
		CHECK ( 253038 == different_size.frames() );

		swap(empty_size, size3);

		CHECK (      0 == size3.frames() );
		CHECK ( 253038 == empty_size.frames() );
	}
}



// Check whether this may be of use
/*
TEST_CASE ( "toc::get_offsets", "[identifier]" )
{
	using arcstk::make_toc;

	SECTION ( "Returns correct offsets from TOC" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = make_toc(
			// leadout
			253038,
			// offsets
			std::vector<arcstk::int32_t>{ 33, 5225, 7390, 23380, 35608, 49820,
			69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320,
			234103 },
			// filenames
			std::vector<std::string>{ "file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file" }
		);

		auto offsets = get_offsets(*toc0);

		CHECK ( offsets.size() == 15 );

		CHECK ( offsets[ 0] ==     33 );
		CHECK ( offsets[ 1] ==   5225 );
		CHECK ( offsets[ 2] ==   7390 );
		CHECK ( offsets[ 3] ==  23380 );
		CHECK ( offsets[ 4] ==  35608 );
		CHECK ( offsets[ 5] ==  49820 );
		CHECK ( offsets[ 6] ==  69508 );
		CHECK ( offsets[ 7] ==  87733 );
		CHECK ( offsets[ 8] == 106333 );
		CHECK ( offsets[ 9] == 139495 );
		CHECK ( offsets[10] == 157863 );
		CHECK ( offsets[11] == 198495 );
		CHECK ( offsets[12] == 213368 );
		CHECK ( offsets[13] == 225320 );
		CHECK ( offsets[14] == 234103 );
	}
}


TEST_CASE ( "toc::get_filenames", "[identifier]" )
{
	using arcstk::toc::get_filenames;

	using arcstk::details::TOCBuilder;

	SECTION ( "Returns empty list when TOC does not contain filenames" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = make_toc(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		auto fnames = get_filenames(*toc0);

		CHECK ( fnames.empty() );
	}

	SECTION ( "Returns list of size track_count when TOC contains 1 filename" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = make_toc(
			// track count
			15,
			// offsets
			std::vector<arcstk::int32_t>{ 33, 5225, 7390, 23380, 35608, 49820,
			69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320,
			234103 },
			// leadout
			253038,
			// filenames
			std::vector<std::string>{ "file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file",
			"file", "file", "file", "file", "file" }
		);

		auto fnames = get_filenames(*toc0);

		CHECK ( fnames.size() == 15 );
	}

	SECTION ( "Returns list of size track_count when TOC contains"
			" multiple filenames" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG
		auto toc0 = make_toc(
			// track count
			15,
			// offsets
			std::vector<arcstk::int32_t>{ 33, 5225, 7390, 23380, 35608, 49820,
			69508, 87733, 106333, 139495, 157863, 198495, 213368, 225320,
			234103 },
			// leadout
			253038,
			// filenames
			std::vector<std::string>{ "file1", "file2", "file3", "file4",
			"file5", "file6", "file7", "file8", "file9", "file10",
			"file11", "file12", "file13", "file14", "file15" }
		);

		auto fnames = get_filenames(*toc0);

		CHECK ( fnames.size() == 15 );
	}
}
*/

/*
TEST_CASE ( "get_offset_sample_indices", "[get_offset_sample_indices]" )
{
	using arcstk::make_toc;

	const auto toc = make_toc(
		// leadout
		253038,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 }
	);

	SECTION ( "is correct" )
	{
		const auto points = arcstk::details::get_offset_sample_indices(*toc);

		CHECK ( points.size() == 15 );

		CHECK ( points[ 0] ==     19404 );
		CHECK ( points[ 1] ==   3072300 );
		CHECK ( points[ 2] ==   4345320 );
		CHECK ( points[ 3] ==  13747440 );
		CHECK ( points[ 4] ==  20937504 );
		CHECK ( points[ 5] ==  29294160 );
		CHECK ( points[ 6] ==  40870704 );
		CHECK ( points[ 7] ==  51587004 );
		CHECK ( points[ 8] ==  62523804 );
		CHECK ( points[ 9] ==  82023060 );
		CHECK ( points[10] ==  92823444 );
		CHECK ( points[11] == 116715060 );
		CHECK ( points[12] == 125460384 );
		CHECK ( points[13] == 132488160 );
		CHECK ( points[14] == 137652564 );
	}
}
*/


// TEST_CASE ( "get_track", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::get_track;
//
// 	auto container1 = std::vector<uint32_t>{ 0, 1, 2, 3, 4, 5 };
// 	auto container2 = std::list<uint32_t>  { 0, 1, 2, 3, 4, 5 };
// 	auto container3 = std::list<int32_t>   { 0, 1, 2, 3, 4, 5 };
//
// 	CHECK_THROWS ( get_track(container1, 0) );
// 	CHECK_THROWS ( get_track(container1, 7) );
//
// 	CHECK ( get_track(container1, 1) == 0 );
// 	CHECK ( get_track(container1, 2) == 1 );
// 	CHECK ( get_track(container1, 3) == 2 );
// 	CHECK ( get_track(container1, 4) == 3 );
// 	CHECK ( get_track(container1, 5) == 4 );
// 	CHECK ( get_track(container1, 6) == 5 );
//
// 	CHECK_THROWS ( get_track(container2, 0) );
// 	CHECK_THROWS ( get_track(container2, 7) );
//
// 	CHECK ( get_track(container2, 1) == 0 );
// 	CHECK ( get_track(container2, 2) == 1 );
// 	CHECK ( get_track(container2, 3) == 2 );
// 	CHECK ( get_track(container2, 4) == 3 );
// 	CHECK ( get_track(container2, 5) == 4 );
// 	CHECK ( get_track(container2, 6) == 5 );
//
// 	CHECK_THROWS ( get_track(container3, 0) );
// 	CHECK_THROWS ( get_track(container3, 7) );
//
// 	CHECK ( get_track(container3, 1) == 0 );
// 	CHECK ( get_track(container3, 2) == 1 );
// 	CHECK ( get_track(container3, 3) == 2 );
// 	CHECK ( get_track(container3, 4) == 3 );
// 	CHECK ( get_track(container3, 5) == 4 );
// 	CHECK ( get_track(container3, 6) == 5 );
// }
//
//
// TEST_CASE ( "calculate_leadout", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::calculate_leadout;
//
// 	auto offsets1 = std::vector<uint32_t>{ 33, 69163, 87321 };
// 	auto lengths1 = std::vector<uint32_t>{ 69130, 18158, 49123 };
//
// 	// identical to 1 except offset[0] is 0
// 	auto offsets2 = std::vector<uint32_t>{ 0, 69163, 87321 };
// 	auto lengths2 = std::vector<uint32_t>{ 69163, 18158, 49123 };
//
// 	// identical to 2 except length[2] is different
// 	auto offsets3 = std::list<uint32_t>{ 0, 69163, 87321 };
// 	auto lengths3 = std::list<uint32_t>{ 69163, 18158, 21002 };
//
// 	auto leadout1 = calculate_leadout(lengths1, offsets1);
// 	auto leadout2 = calculate_leadout(lengths2, offsets2);
// 	auto leadout3 = calculate_leadout(lengths3, offsets3);
//
// 	CHECK ( leadout1 == 136444 );
// 	CHECK ( leadout2 == 136444 );
// 	CHECK ( leadout3 == 108323 );
// }
//
//
// TEST_CASE ( "is_lba_type", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::is_lba_type;
//
// 	SECTION( "is_lba_type", "[identifier] [make_toc]" )
// 	{
// 		CHECK ( is_lba_type<int>::value  );
// 		CHECK ( is_lba_type<long>::value  );
// 		CHECK ( is_lba_type<long long>::value  );
//
// 		CHECK ( is_lba_type<unsigned>::value  );
// 		CHECK ( is_lba_type<unsigned long>::value  );
// 		CHECK ( is_lba_type<unsigned long long>::value  );
//
// 		CHECK ( is_lba_type<uint32_t>::value );
// 		CHECK ( is_lba_type<int32_t>::value  );
//
// 		CHECK ( not is_lba_type<uint32_t*>::value );
// 		CHECK ( not is_lba_type<int32_t*>::value  );
//
// 		CHECK ( not is_lba_type<uint16_t>::value );
// 		CHECK ( not is_lba_type<int16_t>::value  );
//
// 		CHECK ( not is_lba_type<char>::value  );
// 		CHECK ( not is_lba_type<bool>::value  );
// 		CHECK ( not is_lba_type<std::string>::value  );
//
// 		CHECK ( not is_lba_type<std::vector<int32_t>>::value  );
// 		CHECK ( not is_lba_type<std::vector<uint32_t>>::value );
// 		CHECK ( not is_lba_type<std::list<int32_t>>::value    );
// 		CHECK ( not is_lba_type<std::list<uint32_t>>::value   );
// 	}
// }
//
//
// TEST_CASE ( "has_lba_value_type", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::has_lba_value_type;
//
// 	SECTION ( "has_lba_value_type for some std containers" )
// 	{
// 		CHECK ( has_lba_value_type<std::vector<int32_t>>::value  );
// 		CHECK ( has_lba_value_type<std::vector<uint32_t>>::value );
// 		CHECK ( has_lba_value_type<std::list<int32_t>>::value    );
// 		CHECK ( has_lba_value_type<std::list<uint32_t>>::value   );
//
// 		CHECK ( not has_lba_value_type<std::vector<char>>::value );
// 		CHECK ( not has_lba_value_type<char>::value              );
// 		CHECK ( not has_lba_value_type<int>::value               );
// 		CHECK ( not has_lba_value_type<bool>::value              );
// 	}
// }
//
//
// TEST_CASE ( "has_const_iterator", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::has_const_iterator;
//
// 	SECTION ( "has_const_iterator for some std containers" )
// 	{
// 		CHECK ( has_const_iterator<std::vector<char>>::value     );
// 		CHECK ( has_const_iterator<std::vector<int32_t>>::value  );
// 		CHECK ( has_const_iterator<std::vector<uint32_t>>::value );
// 		CHECK ( has_const_iterator<std::list<int32_t>>::value    );
// 		CHECK ( has_const_iterator<std::list<uint32_t>>::value   );
// 		CHECK ( has_const_iterator<const std::vector<int32_t>>::value  );
//
// 		CHECK ( not has_const_iterator<int>::value );
// 		CHECK ( not has_const_iterator<const short>::value );
// 		CHECK ( not has_const_iterator<std::pair<int, bool>>::value );
// 	}
// }
//
//
// TEST_CASE ( "has_size", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::has_size;
//
// 	SECTION ( "has_size for some std containers" )
// 	{
// 		CHECK ( has_size<std::vector<char>>::value     );
// 		CHECK ( has_size<std::vector<int32_t>>::value  );
// 		CHECK ( has_size<std::vector<uint32_t>>::value );
// 		CHECK ( has_size<std::list<int32_t>>::value    );
// 		CHECK ( has_size<std::list<uint32_t>>::value   );
// 		CHECK ( has_size<const std::vector<int32_t>>::value  );
//
// 		CHECK ( not has_size<int>::value );
// 		CHECK ( not has_size<std::pair<char, bool>>::value );
// 	}
// }
//
//
// TEST_CASE ( "has_begin", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::has_begin;
//
// 	SECTION ( "has_begin for some std containers" )
// 	{
// 		CHECK ( has_begin<std::vector<char>>::value     );
// 		CHECK ( has_begin<std::vector<int32_t>>::value  );
// 		CHECK ( has_begin<std::vector<uint32_t>>::value );
// 		CHECK ( has_begin<std::list<int32_t>>::value    );
// 		CHECK ( has_begin<std::list<uint32_t>>::value   );
// 	}
// }
//
//
// TEST_CASE ( "has_end", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::has_end;
//
// 	SECTION ( "has_end for some std containers" )
// 	{
// 		CHECK ( has_end<std::vector<char>>::value     );
// 		CHECK ( has_end<std::vector<int32_t>>::value  );
// 		CHECK ( has_end<std::vector<uint32_t>>::value );
// 		CHECK ( has_end<std::list<int32_t>>::value    );
// 		CHECK ( has_end<std::list<uint32_t>>::value   );
// 	}
// }
//
//
// TEST_CASE ( "is_container", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::is_container;
//
// 	SECTION ( "is_container" )
// 	{
// 		CHECK ( is_container<std::vector<char>>::value     );
// 		CHECK ( is_container<std::vector<int32_t>>::value  );
// 		CHECK ( is_container<std::vector<uint32_t>>::value );
// 		CHECK ( is_container<std::list<int32_t>>::value    );
// 		CHECK ( is_container<std::list<uint32_t>>::value   );
// 	}
// }
//
//
// TEST_CASE ( "is_lba_container", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::is_lba_container;
//
// 	SECTION ( "is_lba_container for non-refererence types" )
// 	{
// 		CHECK ( is_lba_container<std::vector<int32_t>>::value     );
// 		CHECK ( is_lba_container<std::vector<uint32_t>>::value    );
// 		CHECK ( is_lba_container<std::list<int32_t>>::value       );
// 		CHECK ( is_lba_container<std::list<uint32_t>>::value      );
// 		CHECK ( is_lba_container<std::array<int32_t,   1>>::value );
// 		CHECK ( is_lba_container<std::array<int32_t,  99>>::value );
// 		CHECK ( is_lba_container<std::array<uint32_t,  1>>::value );
// 		CHECK ( is_lba_container<std::array<uint32_t, 99>>::value );
// 		CHECK ( is_lba_container<std::list<int32_t>>::value       );
// 		CHECK ( is_lba_container<std::list<uint32_t>>::value      );
//
// 		//CHECK ( is_lba_container<std::vector<char>>::value        );
// 	}
//
// 	SECTION ( "is_lba_container for references" )
// 	{
// 		CHECK ( is_lba_container<std::vector<int32_t> &>::value     );
// 		CHECK ( is_lba_container<std::vector<uint32_t> &>::value    );
// 		CHECK ( is_lba_container<std::list<int32_t> &>::value       );
// 		CHECK ( is_lba_container<std::list<uint32_t> &>::value      );
// 		CHECK ( is_lba_container<std::array<int32_t,   1> &>::value );
// 		CHECK ( is_lba_container<std::array<int32_t,  99> &>::value );
// 		CHECK ( is_lba_container<std::array<uint32_t,  1> &>::value );
// 		CHECK ( is_lba_container<std::array<uint32_t, 99> &>::value );
// 		CHECK ( is_lba_container<std::list<int32_t> &>::value       );
// 		CHECK ( is_lba_container<std::list<uint32_t> &>::value      );
//
// 		CHECK ( not is_lba_container<std::vector<char> &>::value    );
// 	}
// }
//
//
// TEST_CASE ( "has_filename_value_type", "[identifier] [make_toc]" )
// {
// 	using arcstk::details::has_filename_value_type;
//
// 	SECTION ( "has_filename_value_type" )
// 	{
// 		CHECK ( has_filename_value_type<std::vector<std::string>>::value );
// 		CHECK ( has_filename_value_type<std::list<std::wstring>>::value );
//
// 		CHECK ( not has_filename_value_type<std::vector<char>>::value     );
// 		CHECK ( not has_filename_value_type<std::vector<int32_t>>::value  );
// 		CHECK ( not has_filename_value_type<std::vector<uint32_t>>::value );
// 		CHECK ( not has_filename_value_type<std::list<int32_t>>::value    );
// 		CHECK ( not has_filename_value_type<std::list<uint32_t>>::value   );
// 	}
// }
//
//
// TEST_CASE ("is_filename_container", "[identifier] [make_toc]")
// {
// 	using arcstk::details::is_filename_container;
//
// 	SECTION ( "is_filename_container for non-reference types" )
// 	{
// 		CHECK ( is_filename_container<std::vector<std::string>>::value     );
// 		CHECK ( is_filename_container<std::list<std::string>>::value       );
// 		CHECK ( is_filename_container<std::array<std::string,   1>>::value );
// 		CHECK ( is_filename_container<std::array<std::string,  99>>::value );
// 		CHECK ( is_filename_container<std::list<std::string>>::value       );
//
// 		CHECK ( not is_filename_container<std::vector<char>>::value        );
// 	}
//
// 	SECTION ( "is_filename_container for reference types" )
// 	{
// 		CHECK ( is_filename_container<std::vector<std::string> &>::value     );
// 		CHECK ( is_filename_container<std::list<std::string> &>::value       );
// 		CHECK ( is_filename_container<std::array<std::string,   1> &>::value );
// 		CHECK ( is_filename_container<std::array<std::string,  99> &>::value );
// 		CHECK ( is_filename_container<std::list<std::string> &>::value       );
//
// 		CHECK ( not is_filename_container<std::vector<char> &>::value        );
// 	}
// }

