#include "catch2/catch_test_macros.hpp"

#include <algorithm>              // for max
#include <fstream>                // for ifstream, operator|, ios_base::failure
#include <limits>                 // for numeric_limits
#include <memory>                 // for unique_ptr
#include <string>                 // for basic_string, operator==, string
#include <utility>                // for move
#include <vector>                 // for vector

#ifndef __LIBARCSTK_CALCULATE_APIS_HPP__
#include "calculate_apis.hpp"
#endif

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif


/**
 * \file Fixtures for classes in module calculate
 */


TEST_CASE ( "Interval", "[calculate] [interval]" )
{
	using arcstk::v_1_0_0::details::Interval;

	Interval i(10, 20);

	SECTION ("Bounds are correct")
	{
		CHECK ( not i.contains( 9) );
		CHECK (     i.contains(10) );
		CHECK (     i.contains(11) );
		// ...
		CHECK (     i.contains(19) );
		CHECK (     i.contains(20) );
		CHECK ( not i.contains(21) );
	}
}


TEST_CASE ( "AudioSize", "[calculate] [audiosize]" )
{
	using arcstk::AudioSize;
	using UNIT = arcstk::AudioSize::UNIT;

	AudioSize size1;
	size1.set_leadout_frame(253038);

	AudioSize size2; // equals size1
	size2.set_leadout_frame(253038);

	AudioSize size3(253038, UNIT::FRAMES); // equal to size1 and size2

	AudioSize size4(148786344, UNIT::SAMPLES); // equal to size1 and size2

	AudioSize size5(595145376, UNIT::BYTES); // equal to size1 and size2

	AudioSize different_size; // not equal to size1-5
	different_size.set_leadout_frame(14827);


	SECTION ("Constructors")
	{
		// constructed with frames
		CHECK ( size1.leadout_frame()  ==    253038 );
		CHECK ( size1.total_samples()  == 148786344 );
		CHECK ( size1.total_pcm_bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size2.leadout_frame()  ==    253038 );
		CHECK ( size2.total_samples()  == 148786344 );
		CHECK ( size2.total_pcm_bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size3.leadout_frame()  ==    253038 );
		CHECK ( size3.total_samples()  == 148786344 );
		CHECK ( size3.total_pcm_bytes() == 595145376 );

		// constructed with samples
		CHECK ( size4.leadout_frame()  ==    253038 );
		CHECK ( size4.total_samples()  == 148786344 );
		CHECK ( size4.total_pcm_bytes() == 595145376 );

		// constructed with bytes
		CHECK ( size5.leadout_frame()  ==    253038 );
		CHECK ( size5.total_samples()  == 148786344 );
		CHECK ( size5.total_pcm_bytes() == 595145376 );

		// different size, constructed with frames
		CHECK ( different_size.leadout_frame()  ==    14827 );
		CHECK ( different_size.total_samples()  ==  8718276 );
		CHECK ( different_size.total_pcm_bytes() == 34873104 );
	}

	SECTION ("Equality")
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
}


TEST_CASE ( "checksum::type_name provides correct names",
		"[calculate] [checksum::type_name]" )
{
	using arcstk::checksum::type;
	using arcstk::checksum::type_name;

	CHECK ( type_name(type::ARCS1) == "ARCSv1" );
	CHECK ( type_name(type::ARCS2) == "ARCSv2" );
	//CHECK ( type_name(type::CRC32)   == "CRC32" );
	//CHECK ( type_name(type::CRC32ns) == "CRC32ns" );
}


TEST_CASE ( "Checksum", "[calculate]" )
{
	using arcstk::Checksum;

	Checksum track01 { 0xB89992E5 };
	Checksum track02 { 0x98B10E0F };

	CHECK ( track01.value() == 0xB89992E5 );
	CHECK ( track02.value() == 0x98B10E0F );
	CHECK ( track01 != track02 );


	SECTION ( "Copy assignment" )
	{
		track01 = track02;

		CHECK ( track01.value() == 0x98B10E0F );
	}

	SECTION ( "Move assignment" )
	{
		auto sum = Checksum { 0x98B10E0F };
		track01 = std::move(sum);

		CHECK ( track01.value() == 0x98B10E0F );
	}
}


TEST_CASE ( "ChecksumMap", "[calculate] [checksumset]" )
{
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::details::ChecksumMap;

	ChecksumMap<type> track01{};

	CHECK ( track01.empty() );
	CHECK ( track01.size() == 0 );
	CHECK ( track01.begin()  == track01.end() );
	CHECK ( track01.cbegin() == track01.cend() );

	track01.insert(type::ARCS2, Checksum(0xB89992E5));
	track01.insert(type::ARCS1, Checksum(0x98B10E0F));

	CHECK ( not track01.empty() );
	CHECK ( track01.size() == 2 );


	SECTION ( "insert_overwrite: insert" )
	{
		ChecksumMap<type> track{};
		track.insert_overwrite(type::ARCS2, Checksum(0x4F77EB03));
		track.insert_overwrite(type::ARCS1, Checksum(0x475F57E9));

		CHECK ( track.size() == 2 );
		CHECK ( *track.find(type::ARCS2) == Checksum(0x4F77EB03) );
		CHECK ( *track.find(type::ARCS1) == Checksum(0x475F57E9) );
	}

	SECTION ( "insert_overwrite: overwrite" )
	{
		track01.insert_overwrite(type::ARCS2, Checksum(0x4F77EB03));
		track01.insert_overwrite(type::ARCS1, Checksum(0x475F57E9));

		CHECK ( track01.size() == 2 );
		CHECK ( *track01.find(type::ARCS2) == Checksum(0x4F77EB03) );
		CHECK ( *track01.find(type::ARCS1) == Checksum(0x475F57E9) );
	}

	SECTION ( "merge_overwrite: merge" )
	{
		ChecksumMap<type> track02{};
		track02.insert(type::ARCS1, Checksum(0x475F57E9));

		ChecksumMap<type> track03{};
		track03.insert(type::ARCS2, Checksum(0x4F77EB03));

		track02.merge_overwrite(track03);

		CHECK ( track02.size() == 2 );
		CHECK ( *track02.find(type::ARCS2) == Checksum(0x4F77EB03) );
		CHECK ( *track02.find(type::ARCS1) == Checksum(0x475F57E9) );
	}

	SECTION ( "merge_overwrite: overwrite" )
	{
		ChecksumMap<type> track02{};
		track02.insert(type::ARCS2, Checksum(0x4F77EB03));
		track02.insert(type::ARCS1, Checksum(0x475F57E9));

		track01.merge_overwrite(track02);

		CHECK ( track01.size() == 2 );
		CHECK ( *track01.find(type::ARCS2) == Checksum(0x4F77EB03) );
		CHECK ( *track01.find(type::ARCS1) == Checksum(0x475F57E9) );
	}
}


TEST_CASE ( "ChecksumSet", "[calculate] [checksumset]" )
{
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;

	ChecksumSet track01(0);

	CHECK ( track01.empty() );
	CHECK ( track01.size() == 0 );
	CHECK ( track01.begin()  == track01.end() );
	CHECK ( track01.cbegin() == track01.cend() );

	track01.insert(type::ARCS2, Checksum(0xB89992E5));
	track01.insert(type::ARCS1, Checksum(0x98B10E0F));

	CHECK ( not track01.empty() );
	CHECK ( track01.size() == 2 );


	SECTION ( "Equality and Inequality" )
	{
		ChecksumSet track02(0);
		track02.insert(type::ARCS2, Checksum(0xB89992E5));
		track02.insert(type::ARCS1, Checksum(0x98B10E0F));

		CHECK ( track02 == track01 );
		CHECK ( track01 == track02 );
		CHECK ( not (track02 != track01) );
		CHECK ( not (track01 != track02) );
	}


	SECTION ( "copy construction" )
	{
		ChecksumSet track02 { track01 };

		CHECK ( track02 == track01 );
		CHECK ( track01 == track02 );

		CHECK ( track02.get(type::ARCS2) == Checksum(0xB89992E5) );
		CHECK ( track02.get(type::ARCS1) == Checksum(0x98B10E0F) );
	}


	SECTION ( "copy assignment" )
	{
		ChecksumSet track02;
		track02 = track01;

		CHECK ( track02 == track01 );
		CHECK ( track01 == track02 );

		CHECK ( track02.get(type::ARCS2) == Checksum(0xB89992E5) );
		CHECK ( track02.get(type::ARCS1) == Checksum(0x98B10E0F) );
	}


	SECTION ( "insert(type, Checksum)" )
	{
		CHECK ( track01.get(type::ARCS2) == Checksum(0xB89992E5) );
		CHECK ( track01.get(type::ARCS1) == Checksum(0x98B10E0F) );
	}


	SECTION ( "insert(type, Checksum) same type again has no effect" )
	{
		track01.insert(type::ARCS2, Checksum(0x4F77EB03));
		track01.insert(type::ARCS1, Checksum(0x475F57E9));

		CHECK ( track01.get(type::ARCS2) == Checksum(0xB89992E5) );
		CHECK ( track01.get(type::ARCS1) == Checksum(0x98B10E0F) );
	}


	SECTION ( "contains(type)" )
	{
		CHECK ( track01.contains(type::ARCS2) );
		CHECK ( track01.contains(type::ARCS1) );
	}


	SECTION ( "get(type)" )
	{
		CHECK ( track01.get(type::ARCS2) == Checksum(0xB89992E5) );
		CHECK ( track01.get(type::ARCS1) == Checksum(0x98B10E0F) );
	}


	SECTION ( "erase(type)" )
	{
		track01.erase(type::ARCS1);

		CHECK ( track01.size() == 1 );
		CHECK ( not track01.contains(type::ARCS1) );
		CHECK ( track01.contains(type::ARCS2) );
	}


	SECTION ( "clear()" )
	{
		track01.clear();

		CHECK ( track01.size() == 0 );
		CHECK ( track01.empty() );

		CHECK ( not track01.contains(type::ARCS2) );
		CHECK ( not track01.contains(type::ARCS1) );
	}


	SECTION ( "iterator begin() points to first entry" )
	{
		auto it { track01.begin()  };

		CHECK ( it != track01.end() );

		CHECK ( not (*it).empty() );
		CHECK ( not it->empty() );
	}


	SECTION ( "const_iterator cbegin() points to first entry" )
	{
		auto cit { track01.cbegin() };

		CHECK ( cit != track01.cend() );

		CHECK ( not (*cit).empty() );
		CHECK ( not cit->empty() );
	}


	SECTION ( "iterator end() points behind last entry" )
	{
		auto it { track01.begin()  };

		// begin() + size()
		for (auto i = track01.size(); i > 0; --i) { ++it; }

		CHECK ( it == track01.end() );
	}


	SECTION ( "const_iterator cend() points behind last entry" )
	{
		auto cit { track01.cbegin() };

		// begin() + size()
		for (auto i = track01.size(); i > 0; --i) { ++cit; }

		CHECK ( cit == track01.cend() );
	}
}


TEST_CASE ( "Checksums", "[calculate] [checksums]" )
{
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;
	using arcstk::Checksums;

	// Track 1

	ChecksumSet track01 { 5192 };

	REQUIRE ( track01.empty() );
	REQUIRE ( track01.size()   == 0 );
	REQUIRE ( track01.begin()  == track01.end() );
	REQUIRE ( track01.cbegin() == track01.cend() );
	REQUIRE ( track01.length() == 5192 );

	track01.insert(type::ARCS2, Checksum(0xB89992E5));
	track01.insert(type::ARCS1, Checksum(0x98B10E0F));

	REQUIRE ( not track01.empty() );
	REQUIRE ( track01.size()   == 2 );
	REQUIRE ( track01.begin()  != track01.end() );
	REQUIRE ( track01.cbegin() != track01.cend() );
	REQUIRE ( track01.length() == 5192 );
	REQUIRE ( track01.get(type::ARCS2) == Checksum(0xB89992E5) );
	REQUIRE ( track01.get(type::ARCS1) == Checksum(0x98B10E0F) );

	// Track 2

	ChecksumSet track02 { 2165 };

	REQUIRE ( track02.empty() );
	REQUIRE ( track02.size()   == 0 );
	REQUIRE ( track02.begin()  == track02.end() );
	REQUIRE ( track02.cbegin() == track02.cend() );
	REQUIRE ( track02.length() == 2165 );

	track02.insert(type::ARCS2, Checksum(0x4F77EB03));
	track02.insert(type::ARCS1, Checksum(0x475F57E9));

	REQUIRE ( not track02.empty() );
	REQUIRE ( track02.size()   == 2 );
	REQUIRE ( track02.begin()  != track02.end() );
	REQUIRE ( track02.cbegin() != track02.cend() );
	REQUIRE ( track02.length() == 2165 );
	REQUIRE ( track02.get(type::ARCS2) == Checksum(0x4F77EB03) );
	REQUIRE ( track02.get(type::ARCS1) == Checksum(0x475F57E9) );

	// Construct the other 13 tracks

	ChecksumSet track03 { 15885 };
	track03.insert(type::ARCS2, Checksum(0x56582282));
	track03.insert(type::ARCS1, Checksum(0x7304F1C4));

	ChecksumSet track04 { 12228 };
	track04.insert(type::ARCS2, Checksum(0x9E2187F9));
	track04.insert(type::ARCS1, Checksum(0xF2472287));

	ChecksumSet track05 { 13925 };
	track05.insert(type::ARCS2, Checksum(0x6BE71E50));
	track05.insert(type::ARCS1, Checksum(0x881BC504));

	ChecksumSet track06 { 19513 };
	track06.insert(type::ARCS2, Checksum(0x01E7235F));
	track06.insert(type::ARCS1, Checksum(0xBB94BFD4));

	ChecksumSet track07 { 18155 };
	track07.insert(type::ARCS2, Checksum(0xD8F7763C));
	track07.insert(type::ARCS1, Checksum(0xF9CAEE76));

	ChecksumSet track08 { 18325 };
	track08.insert(type::ARCS2, Checksum(0x8480223E));
	track08.insert(type::ARCS1, Checksum(0xF9F60BC1));

	ChecksumSet track09 { 33075 };
	track09.insert(type::ARCS2, Checksum(0x42C5061C));
	track09.insert(type::ARCS1, Checksum(0x2C736302));

	ChecksumSet track10 { 18368 };
	track10.insert(type::ARCS2, Checksum(0x47A70F02));
	track10.insert(type::ARCS1, Checksum(0x1C955978));

	ChecksumSet track11 { 40152 };
	track11.insert(type::ARCS2, Checksum(0xBABF08CC));
	track11.insert(type::ARCS1, Checksum(0xFDA6D833));

	ChecksumSet track12 { 14798 };
	track12.insert(type::ARCS2, Checksum(0x563EDCCB));
	track12.insert(type::ARCS1, Checksum(0x3A57E5D1));

	ChecksumSet track13 { 11952 };
	track13.insert(type::ARCS2, Checksum(0xAB123C7C));
	track13.insert(type::ARCS1, Checksum(0x6ED5F3E7));

	ChecksumSet track14 { 8463 };
	track14.insert(type::ARCS2, Checksum(0xC65C20E4));
	track14.insert(type::ARCS1, Checksum(0x4A5C3872));

	ChecksumSet track15 { 18935 };
	track15.insert(type::ARCS2, Checksum(0x58FC3C3E));
	track15.insert(type::ARCS1, Checksum(0x5FE8B032));

	// And construct the Checksums

	Checksums checksums {
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

	REQUIRE ( checksums.size() == 15 );


	SECTION ( "operator [] read succeeds" )
	{
		CHECK ( checksums[ 0] == track01 );
		CHECK ( checksums[ 1] == track02 );
		CHECK ( checksums[ 2] == track03 );
		CHECK ( checksums[ 3] == track04 );
		CHECK ( checksums[ 4] == track05 );
		CHECK ( checksums[ 5] == track06 );
		CHECK ( checksums[ 6] == track07 );
		CHECK ( checksums[ 7] == track08 );
		CHECK ( checksums[ 8] == track09 );
		CHECK ( checksums[ 9] == track10 );
		CHECK ( checksums[10] == track11 );
		CHECK ( checksums[11] == track12 );
		CHECK ( checksums[12] == track13 );
		CHECK ( checksums[13] == track14 );
		CHECK ( checksums[14] == track15 );
	}


	SECTION ( "at() read succeeds" )
	{
		CHECK ( checksums.at( 0) == track01 );
		CHECK ( checksums.at( 1) == track02 );
		CHECK ( checksums.at( 2) == track03 );
		CHECK ( checksums.at( 3) == track04 );
		CHECK ( checksums.at( 4) == track05 );
		CHECK ( checksums.at( 5) == track06 );
		CHECK ( checksums.at( 6) == track07 );
		CHECK ( checksums.at( 7) == track08 );
		CHECK ( checksums.at( 8) == track09 );
		CHECK ( checksums.at( 9) == track10 );
		CHECK ( checksums.at(10) == track11 );
		CHECK ( checksums.at(11) == track12 );
		CHECK ( checksums.at(12) == track13 );
		CHECK ( checksums.at(13) == track14 );
		CHECK ( checksums.at(14) == track15 );

		CHECK_THROWS ( checksums.at(15) );
	}


	SECTION ( "range-based for with const ref read access succeeds" )
	{
		unsigned i = 0;

		for (const auto& track : checksums)
		{
			CHECK ( not track.empty() );
			CHECK ( track.length() != 0 );

			++i;
		}

		CHECK ( checksums.size() == i  );
		CHECK ( checksums.size() == 15 );
	}


	SECTION ( "Copy construct" )
	{
		auto checksums2 { checksums };

		CHECK ( checksums2[ 0] == track01 );
		CHECK ( checksums2[ 1] == track02 );
		CHECK ( checksums2[ 2] == track03 );
		CHECK ( checksums2[ 3] == track04 );
		CHECK ( checksums2[ 4] == track05 );
		CHECK ( checksums2[ 5] == track06 );
		CHECK ( checksums2[ 6] == track07 );
		CHECK ( checksums2[ 7] == track08 );
		CHECK ( checksums2[ 8] == track09 );
		CHECK ( checksums2[ 9] == track10 );
		CHECK ( checksums2[10] == track11 );
		CHECK ( checksums2[11] == track12 );
		CHECK ( checksums2[12] == track13 );
		CHECK ( checksums2[13] == track14 );
		CHECK ( checksums2[14] == track15 );

		CHECK ( checksums2.size() == 15 );

		CHECK ( checksums2 == checksums );
	}


	SECTION ( "Copy assignment" )
	{
		auto copy = checksums;

		CHECK ( copy[ 0] == track01 );
		CHECK ( copy[ 1] == track02 );
		CHECK ( copy[ 2] == track03 );
		CHECK ( copy[ 3] == track04 );
		CHECK ( copy[ 4] == track05 );
		CHECK ( copy[ 5] == track06 );
		CHECK ( copy[ 6] == track07 );
		CHECK ( copy[ 7] == track08 );
		CHECK ( copy[ 8] == track09 );
		CHECK ( copy[ 9] == track10 );
		CHECK ( copy[10] == track11 );
		CHECK ( copy[11] == track12 );
		CHECK ( copy[12] == track13 );
		CHECK ( copy[13] == track14 );
		CHECK ( copy[14] == track15 );

		CHECK ( copy.size() == 15 );

		CHECK ( checksums == copy );
	}


	SECTION ( "Move construct" )
	{
		auto checksums2 { std::move(checksums) };

		CHECK ( checksums2[ 0] == track01 );
		CHECK ( checksums2[ 1] == track02 );
		CHECK ( checksums2[ 2] == track03 );
		CHECK ( checksums2[ 3] == track04 );
		CHECK ( checksums2[ 4] == track05 );
		CHECK ( checksums2[ 5] == track06 );
		CHECK ( checksums2[ 6] == track07 );
		CHECK ( checksums2[ 7] == track08 );
		CHECK ( checksums2[ 8] == track09 );
		CHECK ( checksums2[ 9] == track10 );
		CHECK ( checksums2[10] == track11 );
		CHECK ( checksums2[11] == track12 );
		CHECK ( checksums2[12] == track13 );
		CHECK ( checksums2[13] == track14 );
		CHECK ( checksums2[14] == track15 );

		CHECK ( checksums2.size() == 15 );
	}


	SECTION ( "Move assignment" )
	{
		auto moved = std::move(checksums);

		CHECK ( moved[ 0] == track01 );
		CHECK ( moved[ 1] == track02 );
		CHECK ( moved[ 2] == track03 );
		CHECK ( moved[ 3] == track04 );
		CHECK ( moved[ 4] == track05 );
		CHECK ( moved[ 5] == track06 );
		CHECK ( moved[ 6] == track07 );
		CHECK ( moved[ 7] == track08 );
		CHECK ( moved[ 8] == track09 );
		CHECK ( moved[ 9] == track10 );
		CHECK ( moved[10] == track11 );
		CHECK ( moved[11] == track12 );
		CHECK ( moved[12] == track13 );
		CHECK ( moved[13] == track14 );
		CHECK ( moved[14] == track15 );

		CHECK ( moved.size() == 15 );
	}

}

// Calculation


TEST_CASE ( "Calculation Constructors", "[calculate] [calculation]" )
{
	using arcstk::Calculation;
	using arcstk::checksum::type;
	using arcstk::details::TOCBuilder;

	auto toc { TOCBuilder::build(
		3, /* track count */
		{ 12, 433, 924 }, /* offsets */
		1233 /* leadout */
	)};

	auto ctx { arcstk::make_context(toc) };

	CHECK ( ctx->total_tracks() == 3 );
	CHECK ( ctx->offset(0) ==  12 );
	CHECK ( ctx->offset(1) == 433 );
	CHECK ( ctx->offset(2) == 924 );
	CHECK ( ctx->audio_size().leadout_frame() == 1233 );
	CHECK ( ctx->is_multi_track() );
	CHECK ( ctx->skips_front() );
	CHECK ( ctx->skips_back() );
	CHECK ( ctx->num_skip_front() == 2939 );
	CHECK ( ctx->num_skip_back()  == 2940 );


	SECTION ( "By CalcContext" )
	{
		Calculation calculation(std::move(ctx)); // use default checksum::type
		auto audiosize { calculation.context().audio_size() };

		// Checks

		CHECK ( calculation.type() == type::ARCS2 ); // default checksum::type

		CHECK ( audiosize.leadout_frame()  ==    1233 );
		CHECK ( audiosize.total_samples()  ==  725004 );
		CHECK ( audiosize.total_pcm_bytes() == 2900016 );

		CHECK ( calculation.context().is_multi_track() );
		CHECK ( calculation.context().skips_front() );
		CHECK ( calculation.context().skips_back() );
		CHECK ( calculation.context().num_skip_front() == 2939 );
		CHECK ( calculation.context().num_skip_back()  == 2940 );

		CHECK ( not calculation.complete() );
	}


	SECTION ( "By checksum::type and CalcContext" )
	{
		Calculation calculation(type::ARCS1, std::move(ctx));
		auto audiosize { calculation.context().audio_size() };

		// Checks

		CHECK ( calculation.type() == type::ARCS1 );

		CHECK ( audiosize.leadout_frame()  ==    1233 );
		CHECK ( audiosize.total_samples()  ==  725004 );
		CHECK ( audiosize.total_pcm_bytes() == 2900016 );

		CHECK ( calculation.context().is_multi_track() );
		CHECK ( calculation.context().skips_front() );
		CHECK ( calculation.context().skips_back() );
		CHECK ( calculation.context().num_skip_front() == 2939 );
		CHECK ( calculation.context().num_skip_back()  == 2940 );

		CHECK ( not calculation.complete() );
	}
}


TEST_CASE ( "Calculation context", "[calculate] [calculation]" )
{
	using arcstk::Calculation;
	using arcstk::AudioSize;
	using arcstk::checksum::type;
	using arcstk::details::TOCBuilder;

	Calculation calculation(arcstk::make_context(false, false, "foo"));

	AudioSize audiosize;
	audiosize.set_total_samples(196608); // fits calculation-test-01.bin
	calculation.update_audiosize(audiosize);

	CHECK ( calculation.context().audio_size().leadout_frame()  ==    334 );
	CHECK ( calculation.context().audio_size().total_samples()  == 196608 );
	CHECK ( calculation.context().audio_size().total_pcm_bytes() == 786432 );
	CHECK ( calculation.context().filename() == "foo" );
	CHECK ( calculation.type() == type::ARCS2 );
	CHECK ( not calculation.complete() );
	CHECK ( not calculation.context().is_multi_track() );
	CHECK ( not calculation.context().skips_front() );
	CHECK ( not calculation.context().skips_back() );
	CHECK ( calculation.context().num_skip_front() == 0 );
	CHECK ( calculation.context().num_skip_back()  == 0 );


	SECTION ( "Updates correctly on change" )
	{
		auto toc { TOCBuilder::build(
			3, /* track count */
			{ 12, 433, 924 }, /* offsets */
			1233 /* leadout */
		)};

		auto ctx { arcstk::make_context(toc, "bar") };

		CHECK ( ctx->total_tracks() == 3 );
		CHECK ( ctx->offset(0) ==  12 );
		CHECK ( ctx->offset(1) == 433 );
		CHECK ( ctx->offset(2) == 924 );
		CHECK ( ctx->audio_size().leadout_frame() == 1233 );
		CHECK ( ctx->is_multi_track() );
		CHECK ( ctx->skips_front() );
		CHECK ( ctx->skips_back() );
		CHECK ( ctx->num_skip_front() == 2939 );
		CHECK ( ctx->num_skip_back()  == 2940 );

		// Perform update

		calculation.set_context(std::move(ctx));
		auto calcsize { calculation.context().audio_size() };


		// Checks

		// unchanged: checksum::type + completeness status
		CHECK ( calculation.type() == type::ARCS2 );
		CHECK ( not calculation.complete() );

		// changed: AudioSize, filename, multi_track + skipping
		CHECK ( calcsize.leadout_frame()  ==    1233 );
		CHECK ( calcsize.total_samples()  ==  725004 );
		CHECK ( calcsize.total_pcm_bytes() == 2900016 );
		CHECK ( calculation.context().filename() == "bar" );
		CHECK ( calculation.context().is_multi_track() );
		CHECK ( calculation.context().skips_front() );
		CHECK ( calculation.context().skips_back() );
		CHECK ( calculation.context().num_skip_front() == 2939 );
		CHECK ( calculation.context().num_skip_back()  == 2940);
	}
}


TEST_CASE ( "Calculation Update in singletrack", "[calculate] [calculation]" )
{
	using arcstk::AudioSize;
	using arcstk::Calculation;
	using arcstk::checksum::type;
	using arcstk::make_context;

	// Initialize Calculation

	Calculation calculation(make_context(false, false, "foo"));

	AudioSize audiosize;
	audiosize.set_total_samples(196608); // fits calculation-test-01.bin
	calculation.update_audiosize(audiosize);

	CHECK ( calculation.type() == arcstk::checksum::type::ARCS2 );
	CHECK ( calculation.context().audio_size().leadout_frame()  ==    334 );
	CHECK ( calculation.context().audio_size().total_samples()  == 196608 );
	CHECK ( calculation.context().audio_size().total_pcm_bytes() == 786432 );
	CHECK ( calculation.context().filename() == "foo" );
	CHECK ( not calculation.context().is_multi_track() );
	CHECK ( not calculation.context().skips_front() );
	CHECK ( not calculation.context().skips_back() );
	CHECK ( not calculation.complete() );


	SECTION ( "Correct ARCS1+2 with aligned blocks" )
	{
		using arcstk::sample_t;

		// Initialize Buffer
		std::vector<sample_t> buffer(65536); // samples

		// => forms 3 blocks with 65536 samples each

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 3; ++i)
		{
			CHECK ( not calculation.complete() );

			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 262144);
				// 262144 bytes == 65536 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				calculation.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}
		}

		in.close();

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 1 );

		auto single_track { checksums[0] };


		// Checks: only one single track with correct ARCS1+2

		CHECK ( single_track.size() == 2 );
		CHECK ( 0xD15BB487 == (single_track.get(type::ARCS2)).value() );
		CHECK ( 0x8FE8D29B == (single_track.get(type::ARCS1)).value() );
	}


	SECTION ( "Correct ARCS1+2 with non-aligned blocks" )
	{
		using arcstk::sample_t;

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				calculation.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		calculation.update(buffer.begin(), buffer.end());

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 1 );

		auto single_track { checksums[0] };


		// Only track with correct ARCSs

		CHECK ( single_track.size() == 2 );
		CHECK ( 0xD15BB487 == (single_track.get(type::ARCS2)).value() );
		CHECK ( 0x8FE8D29B == (single_track.get(type::ARCS1)).value() );
	}
}


TEST_CASE ( "Calculation Update in multitrack", "[calculate] [calculation]" )
{
	using arcstk::Calculation;
	using arcstk::checksum::type;
	using arcstk::make_context;
	using arcstk::details::TOCBuilder;

	auto toc { TOCBuilder::build(
		3, /* track count */
		{ 12, 433, 924 }, /* offsets */
		1233 /* leadout */
	)};

	auto ctx { arcstk::make_context(toc) };

	CHECK ( ctx->total_tracks() == 3 );
	CHECK ( ctx->offset(0) ==  12 );
	CHECK ( ctx->offset(1) == 433 );
	CHECK ( ctx->offset(2) == 924 );
	CHECK ( ctx->audio_size().leadout_frame() == 1233 );
	CHECK ( ctx->is_multi_track() );
	CHECK ( ctx->skips_front() );
	CHECK ( ctx->skips_back() );
	CHECK ( ctx->num_skip_front() == 2939 );
	CHECK ( ctx->num_skip_back()  == 2940 );

	Calculation calculation(std::move(ctx));

	CHECK ( calculation.context().audio_size().leadout_frame()  ==    1233 );
	CHECK ( calculation.context().audio_size().total_samples()  ==  725004 );
	CHECK ( calculation.context().audio_size().total_pcm_bytes() == 2900016 );
	CHECK ( calculation.context().is_multi_track() );
	CHECK ( calculation.context().skips_front() );
	CHECK ( calculation.context().skips_back() );
	CHECK ( calculation.context().num_skip_front() == 2939 );
	CHECK ( calculation.context().num_skip_back()  == 2940 );
	CHECK ( calculation.type() == type::ARCS2 );
	CHECK ( not calculation.complete() );


	SECTION ( "Correct ARCS1+2 with aligned blocks" )
	{
		using arcstk::sample_t;

		// Initialize Buffer

		std::vector<sample_t> buffer(181251); // samples

		// => forms 4 blocks with 181251 samples each
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-02.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-02.bin");
		}
		for (int i = 0; i < 4; ++i)
		{
			CHECK ( not calculation.complete() );

			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 725004);
				// 725004 bytes == 181251 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-02.bin");
			}

			try
			{
				calculation.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}
		}

		in.close();

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 3 );


		// Checks

		auto track1 { checksums[0] };

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (track1.get(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (track1.get(type::ARCS1)).value());

		auto track2 { checksums[1] };

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (track2.get(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (track2.get(type::ARCS1)).value());

		auto track3 { checksums[2] };

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (track3.get(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (track3.get(type::ARCS1)).value());
	}


	SECTION ( "Correct ARCS1+2 with non-aligned blocks" )
	{
		using arcstk::sample_t;

		// Initialize Buffer

		std::vector<sample_t> buffer(241584); // samples

		// => forms 3 blocks: 2 x 241584 samples and 1 x 252 samples
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-02.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-02.bin");
		}
		for (int i = 0; i < 3; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 966336);
				// 966336 bytes == 241584 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-02.bin");
			}

			try
			{
				calculation.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(252);
			in.read(reinterpret_cast<char*>(&buffer[0]), 1008);
		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-02.bin");
		}

		in.close();

		calculation.update(buffer.begin(), buffer.end());

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 3 );


		// Checks

		auto track1 = checksums[0];

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (track1.get(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (track1.get(type::ARCS1)).value());

		auto track2 = checksums[1];

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (track2.get(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (track2.get(type::ARCS1)).value());

		auto track3 = checksums[2];

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (track3.get(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (track3.get(type::ARCS1)).value());
	}
}


// SampleInputIterator


TEST_CASE ( "SampleInputIterator", "[calculate] [iterator]" )
{
	using arcstk::sample_t;
	using arcstk::SampleInputIterator;

	std::vector<sample_t> samples {
		 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20
	};


	SECTION ( "operator * (dereference) works correctly" )
	{
		SampleInputIterator sample { samples.begin() };
		CHECK ( *sample == 1 );
	}


	SECTION ( "operator ++ (preincrement) works correctly" )
	{
		SampleInputIterator sample { samples.begin() };
		CHECK ( *sample == 1 );

		++sample;
		CHECK ( *sample == 2 );

		++sample;
		CHECK ( *sample == 3 );

		++sample;
		CHECK ( *sample == 4 );

		++sample;
		CHECK ( *sample == 5 );

		++sample;
		++sample;
		++sample;
		++sample;
		++sample;
		CHECK ( *sample == 10 );
	}


	SECTION ( "operator ++ (postincrement) works correctly" )
	{
		SampleInputIterator sample { samples.begin() };
		CHECK ( *sample == 1 );

		sample++;
		sample++;
		sample++;
		sample++;
		sample++;
		sample++;
		CHECK ( *sample == 7 );

		sample++;
		CHECK ( *sample == 8 );

		sample++;
		CHECK ( *sample == 9 );

		sample++;
		CHECK ( *sample == 10 );

		sample++;
		sample++;
		sample++;
		sample++;
		sample++;
		CHECK ( *sample == 15 );
	}


	SECTION ( "operator + (addition of an amount) works correctly" )
	{
		SampleInputIterator sample1 { samples.begin() };
		CHECK ( *sample1 == 1 );

		SampleInputIterator sample2 { sample1 + 17 };
		CHECK ( *sample2 == 18 );

		SampleInputIterator sample3 { sample1 + 19 };
		CHECK ( *sample3 == 20 );
	}


	SECTION ( "operator = (copy assignment) works correctly" )
	{
		SampleInputIterator sample1 { samples.begin() };
		CHECK ( *sample1 == 1 );

		SampleInputIterator sample2 { sample1 + 15 };
		CHECK ( *sample2 == 16 );

		sample1 = sample2;

		CHECK ( *sample1 == 16 );
		CHECK ( *sample1 == *sample2 );
		CHECK ( sample1 == sample2 );
	}


	SECTION ( "Lvalues are correctly swappable" )
	{
		SampleInputIterator lhs { samples.begin() };
		SampleInputIterator rhs { samples.end() };

		CHECK ( *lhs == 1 );
		CHECK ( rhs == samples.end() );

		swap(lhs, rhs);

		CHECK ( lhs == samples.end() );
		CHECK ( *rhs == 1 );
	}
}


// CalcContext implementations


TEST_CASE ( "SingletrackCalcContext", "[calculate] [calccontext]" )
{
	using arcstk::sample_t;
	using arcstk::sample_count_t;
	using arcstk::make_context;
	using arcstk::make_empty_arid;

	const auto empty_default_arid = make_empty_arid();

	SECTION ( "Construction without parameters is correct" )
	{
		auto sctx { make_context(false, false) };

		CHECK ( sctx->id() == *empty_default_arid );

		CHECK ( sctx->filename() == std::string() );

		CHECK ( sctx->total_tracks() == 1 );

		CHECK ( sctx->offset(0) == 0 );
		CHECK ( sctx->offset(1) == 0 );
		CHECK ( sctx->offset(99) == 0 );

		CHECK ( sctx->length(0) == 0 );
		CHECK ( sctx->length(1) == 0 );
		CHECK ( sctx->length(99) == 0 );

		CHECK ( sctx->audio_size().leadout_frame()  == 0 );
		CHECK ( sctx->audio_size().total_samples()  == 0 );
		CHECK ( sctx->audio_size().total_pcm_bytes() == 0 );

		CHECK ( not sctx->is_multi_track() );
		CHECK ( not sctx->skips_front() );
		CHECK ( not sctx->skips_back() );

		CHECK ( sctx->first_relevant_sample(0)   == 0 );
		CHECK ( sctx->first_relevant_sample(1)   == 0 );
		CHECK ( sctx->first_relevant_sample(99)  == 0 );
		CHECK ( sctx->first_relevant_sample(255) == 0 );

		CHECK ( sctx->last_relevant_sample()    < 0 );

		CHECK ( sctx->last_relevant_sample(0)   < 0 );
		CHECK ( sctx->last_relevant_sample(1)   < 0 );
		CHECK ( sctx->last_relevant_sample(99)  < 0 );
		CHECK ( sctx->last_relevant_sample(255) < 0 );

		CHECK ( sctx->track(0) == 1 );
		CHECK ( sctx->track(std::numeric_limits<sample_count_t>::max()) == 1);
	}

	// TODO Construction with parameters? (bool, bool, audiofilename)
}


TEST_CASE ( "MultitrackCalcContext", "[calculate] [calccontext] [multitrack]" )
{
	using arcstk::details::TOCBuilder;


	SECTION ("Complete TOC with leadout, offset(1) > 0")
	{
		// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

		auto toc = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		CHECK_THROWS ( toc->offset( 0)    );
		CHECK ( toc->offset( 1) ==     33 );
		CHECK ( toc->offset( 2) ==   5225 );
		CHECK ( toc->offset( 3) ==   7390 );
		CHECK ( toc->offset( 4) ==  23380 );
		CHECK ( toc->offset( 5) ==  35608 );
		CHECK ( toc->offset( 6) ==  49820 );
		CHECK ( toc->offset( 7) ==  69508 );
		CHECK ( toc->offset( 8) ==  87733 );
		CHECK ( toc->offset( 9) == 106333 );
		CHECK ( toc->offset(10) == 139495 );
		CHECK ( toc->offset(11) == 157863 );
		CHECK ( toc->offset(12) == 198495 );
		CHECK ( toc->offset(13) == 213368 );
		CHECK ( toc->offset(14) == 225320 );
		CHECK ( toc->offset(15) == 234103 );
		CHECK_THROWS ( toc->offset(16)    );

		CHECK_THROWS ( toc->parsed_length( 0));
		CHECK ( toc->parsed_length( 1) ==  0 );
		CHECK ( toc->parsed_length(15) ==  0 );
		CHECK_THROWS ( toc->parsed_length(16));

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(15) == "" );
		CHECK_THROWS ( toc->filename(16)       );

		CHECK ( toc->leadout() == 253038 );
		CHECK ( toc->total_tracks() == 15 );
		CHECK ( toc->complete() );

		auto mctx = arcstk::make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		CHECK ( mctx->id() == arid );

		CHECK ( audiosize.leadout_frame()  ==    253038 );
		CHECK ( audiosize.total_samples()  == 148786344 );
		CHECK ( audiosize.total_pcm_bytes() == 595145376 );

		CHECK ( mctx->total_tracks() == 15 );

		CHECK ( mctx->offset(0)  ==     33 );
		CHECK ( mctx->offset(1)  ==   5225 );
		CHECK ( mctx->offset(2)  ==   7390 );
		CHECK ( mctx->offset(3)  ==  23380 );
		CHECK ( mctx->offset(4)  ==  35608 );
		CHECK ( mctx->offset(5)  ==  49820 );
		CHECK ( mctx->offset(6)  ==  69508 );
		CHECK ( mctx->offset(7)  ==  87733 );
		CHECK ( mctx->offset(8)  == 106333 );
		CHECK ( mctx->offset(9)  == 139495 );
		CHECK ( mctx->offset(10) == 157863 );
		CHECK ( mctx->offset(11) == 198495 );
		CHECK ( mctx->offset(12) == 213368 );
		CHECK ( mctx->offset(13) == 225320 );
		CHECK ( mctx->offset(14) == 234103 );
		CHECK ( mctx->offset(15) ==      0 ); // not a track

		// The lengths parsed from the CUEsheet differ from the lengths
		// computed by CalcContext. The cause is that for CalcContext the length
		// of track i is the difference offset(i+1) - offset(i). This accepts
		// the gaps as part of the track and appends each gap to the end of
		// the track. Libcue on the other hand seems to just ignore the gaps and
		// subtract them from the actual length.

		CHECK ( mctx->length(0)  ==  5192 );
		CHECK ( mctx->length(1)  ==  2165 );
		CHECK ( mctx->length(2)  == 15990 ); // TOC: 15885
		CHECK ( mctx->length(3)  == 12228 );
		CHECK ( mctx->length(4)  == 14212 ); // TOC: 13925
		CHECK ( mctx->length(5)  == 19688 ); // TOC: 19513
		CHECK ( mctx->length(6)  == 18225 ); // TOC: 18155
		CHECK ( mctx->length(7)  == 18600 ); // TOC: 18325
		CHECK ( mctx->length(8)  == 33162 ); // TOC: 33075
		CHECK ( mctx->length(9)  == 18368 );
		CHECK ( mctx->length(10) == 40632 ); // TOC: 40152
		CHECK ( mctx->length(11) == 14873 ); // TOC: 14798
		CHECK ( mctx->length(12) == 11952 );
		CHECK ( mctx->length(13) ==  8783 ); // TOC: 8463
		CHECK ( mctx->length(14) == 18935 ); // TOC: 18935
		CHECK ( mctx->length(15) ==     0 ); // not a track

		CHECK ( mctx->id() ==
				arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->first_relevant_sample(0)  ==         0 ); // not a track
		CHECK ( mctx->first_relevant_sample(1)  ==     22343 ); // skipping
		CHECK ( mctx->first_relevant_sample(2)  ==   3072300 );
		CHECK ( mctx->first_relevant_sample(3)  ==   4345320 );
		CHECK ( mctx->first_relevant_sample(4)  ==  13747440 );
		CHECK ( mctx->first_relevant_sample(5)  ==  20937504 );
		CHECK ( mctx->first_relevant_sample(6)  ==  29294160 );
		CHECK ( mctx->first_relevant_sample(7)  ==  40870704 );
		CHECK ( mctx->first_relevant_sample(8)  ==  51587004 );
		CHECK ( mctx->first_relevant_sample(9)  ==  62523804 );
		CHECK ( mctx->first_relevant_sample(10) ==  82023060 );
		CHECK ( mctx->first_relevant_sample(11) ==  92823444 );
		CHECK ( mctx->first_relevant_sample(12) == 116715060 );
		CHECK ( mctx->first_relevant_sample(13) == 125460384 );
		CHECK ( mctx->first_relevant_sample(14) == 132488160 );
		CHECK ( mctx->first_relevant_sample(15) == 137652564 );
		CHECK ( mctx->first_relevant_sample(16) ==         0 ); // not a track
		CHECK ( mctx->first_relevant_sample(99) ==         0 ); // not a track

		CHECK ( mctx->last_relevant_sample() == 148783403 );

		CHECK ( mctx->last_relevant_sample(0)  ==     19403 ); // not a track
		CHECK ( mctx->last_relevant_sample(1)  ==   3072299 );
		CHECK ( mctx->last_relevant_sample(2)  ==   4345319 );
		CHECK ( mctx->last_relevant_sample(3)  ==  13747439 );
		CHECK ( mctx->last_relevant_sample(4)  ==  20937503 );
		CHECK ( mctx->last_relevant_sample(5)  ==  29294159 );
		CHECK ( mctx->last_relevant_sample(6)  ==  40870703 );
		CHECK ( mctx->last_relevant_sample(7)  ==  51587003 );
		CHECK ( mctx->last_relevant_sample(8)  ==  62523803 );
		CHECK ( mctx->last_relevant_sample(9)  ==  82023059 );
		CHECK ( mctx->last_relevant_sample(10) ==  92823443 );
		CHECK ( mctx->last_relevant_sample(11) == 116715059 );
		CHECK ( mctx->last_relevant_sample(12) == 125460383 );
		CHECK ( mctx->last_relevant_sample(13) == 132488159 );
		CHECK ( mctx->last_relevant_sample(14) == 137652563 );
		CHECK ( mctx->last_relevant_sample(15) == 148783403 ); // skipping
		CHECK ( mctx->last_relevant_sample(16) == 148783403 ); // not a track
		CHECK ( mctx->last_relevant_sample(99) == 148783403 ); // not a track

		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
		CHECK ( mctx->track(1) == 0);

		CHECK ( mctx->track(19402) == 0);
		CHECK ( mctx->track(19403) == 0);
		CHECK ( mctx->track(19404) == 1);

		CHECK ( mctx->track(3072298) == 1);
		CHECK ( mctx->track(3072299) == 1);
		CHECK ( mctx->track(3072300) == 2);

		CHECK ( mctx->track(4345318) == 2);
		CHECK ( mctx->track(4345319) == 2);
		CHECK ( mctx->track(4345320) == 3);

		CHECK ( mctx->track(13747438) == 3);
		CHECK ( mctx->track(13747439) == 3);
		CHECK ( mctx->track(13747440) == 4);

		CHECK ( mctx->track(20937502) == 4);
		CHECK ( mctx->track(20937503) == 4);
		CHECK ( mctx->track(20937504) == 5);

		CHECK ( mctx->track(29294158) == 5);
		CHECK ( mctx->track(29294159) == 5);
		CHECK ( mctx->track(29294160) == 6);

		CHECK ( mctx->track(40870702) == 6);
		CHECK ( mctx->track(40870703) == 6);
		CHECK ( mctx->track(40870704) == 7);

		CHECK ( mctx->track(51587002) == 7);
		CHECK ( mctx->track(51587003) == 7);
		CHECK ( mctx->track(51587004) == 8);

		CHECK ( mctx->track(62523802) == 8);
		CHECK ( mctx->track(62523803) == 8);
		CHECK ( mctx->track(62523804) == 9);

		CHECK ( mctx->track(82023058) == 9);
		CHECK ( mctx->track(82023059) == 9);
		CHECK ( mctx->track(82023060) == 10);

		CHECK ( mctx->track(92823442) == 10);
		CHECK ( mctx->track(92823443) == 10);
		CHECK ( mctx->track(92823444) == 11);

		CHECK ( mctx->track(116715058) == 11);
		CHECK ( mctx->track(116715059) == 11);
		CHECK ( mctx->track(116715060) == 12);

		CHECK ( mctx->track(125460382) == 12);
		CHECK ( mctx->track(125460383) == 12);
		CHECK ( mctx->track(125460384) == 13);

		CHECK ( mctx->track(132488158) == 13);
		CHECK ( mctx->track(132488159) == 13);
		CHECK ( mctx->track(132488160) == 14);

		CHECK ( mctx->track(137652562) == 14);
		CHECK ( mctx->track(137652563) == 14);
		CHECK ( mctx->track(137652564) == 15);

		CHECK ( mctx->track(148783403) == 15);
		CHECK ( mctx->track(148783404) > mctx->total_tracks() );
	}


	SECTION ("Complete TOC with lengths, offset(1) > 0")
	{
		// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

		auto toc = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// lengths
			{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075, 18368,
				40152, 14798, 11952, 8463, 18935 }
		);

		CHECK_THROWS ( toc->offset( 0)    );
		CHECK ( toc->offset( 1) ==     33 );
		CHECK ( toc->offset( 2) ==   5225 );
		CHECK ( toc->offset( 3) ==   7390 );
		CHECK ( toc->offset( 4) ==  23380 );
		CHECK ( toc->offset( 5) ==  35608 );
		CHECK ( toc->offset( 6) ==  49820 );
		CHECK ( toc->offset( 7) ==  69508 );
		CHECK ( toc->offset( 8) ==  87733 );
		CHECK ( toc->offset( 9) == 106333 );
		CHECK ( toc->offset(10) == 139495 );
		CHECK ( toc->offset(11) == 157863 );
		CHECK ( toc->offset(12) == 198495 );
		CHECK ( toc->offset(13) == 213368 );
		CHECK ( toc->offset(14) == 225320 );
		CHECK ( toc->offset(15) == 234103 );
		CHECK_THROWS ( toc->offset(16)    );

		CHECK_THROWS ( toc->parsed_length( 0)    );
		CHECK ( toc->parsed_length( 1) ==   5192 );
		CHECK ( toc->parsed_length( 2) ==   2165 );
		CHECK ( toc->parsed_length( 3) ==  15885 );
		CHECK ( toc->parsed_length( 4) ==  12228 );
		CHECK ( toc->parsed_length( 5) ==  13925 );
		CHECK ( toc->parsed_length( 6) ==  19513 );
		CHECK ( toc->parsed_length( 7) ==  18155 );
		CHECK ( toc->parsed_length( 8) ==  18325 );
		CHECK ( toc->parsed_length( 9) ==  33075 );
		CHECK ( toc->parsed_length(10) ==  18368 );
		CHECK ( toc->parsed_length(11) ==  40152 );
		CHECK ( toc->parsed_length(12) ==  14798 );
		CHECK ( toc->parsed_length(13) ==  11952 );
		CHECK ( toc->parsed_length(14) ==   8463 );
		CHECK ( toc->parsed_length(15) ==  18935 );
		CHECK_THROWS ( toc->parsed_length(16)    );

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(15) == "" );
		CHECK_THROWS ( toc->filename(16)       );

		CHECK ( toc->leadout() == 253038 );
		CHECK ( toc->total_tracks() == 15 );
		CHECK ( toc->complete() );

		auto mctx = arcstk::make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		CHECK ( mctx->id() == arid );

		CHECK ( audiosize.leadout_frame()  ==    253038 );
		CHECK ( audiosize.total_samples()  == 148786344 );
		CHECK ( audiosize.total_pcm_bytes() == 595145376 );

		CHECK ( mctx->total_tracks() == 15 );

		CHECK ( mctx->offset(0)  ==     33 );
		CHECK ( mctx->offset(1)  ==   5225 );
		CHECK ( mctx->offset(2)  ==   7390 );
		CHECK ( mctx->offset(3)  ==  23380 );
		CHECK ( mctx->offset(4)  ==  35608 );
		CHECK ( mctx->offset(5)  ==  49820 );
		CHECK ( mctx->offset(6)  ==  69508 );
		CHECK ( mctx->offset(7)  ==  87733 );
		CHECK ( mctx->offset(8)  == 106333 );
		CHECK ( mctx->offset(9)  == 139495 );
		CHECK ( mctx->offset(10) == 157863 );
		CHECK ( mctx->offset(11) == 198495 );
		CHECK ( mctx->offset(12) == 213368 );
		CHECK ( mctx->offset(13) == 225320 );
		CHECK ( mctx->offset(14) == 234103 );
		CHECK ( mctx->offset(15) ==      0 ); // not a track

		// The lengths parsed from the CUEsheet differ from the lengths
		// computed by CalcContext. The cause is that for CalcContext the length
		// of track i is the difference offset(i+1) - offset(i). This accepts
		// the gaps as part of the track and appends each gap to the end of
		// the track. Libcue on the other hand seems to just ignore the gaps and
		// subtract them from the actual length.

		CHECK ( mctx->length(0)  ==  5192 );
		CHECK ( mctx->length(1)  ==  2165 );
		CHECK ( mctx->length(2)  == 15990 ); // TOC: 15885
		CHECK ( mctx->length(3)  == 12228 );
		CHECK ( mctx->length(4)  == 14212 ); // TOC: 13925
		CHECK ( mctx->length(5)  == 19688 ); // TOC: 19513
		CHECK ( mctx->length(6)  == 18225 ); // TOC: 18155
		CHECK ( mctx->length(7)  == 18600 ); // TOC: 18325
		CHECK ( mctx->length(8)  == 33162 ); // TOC: 33075
		CHECK ( mctx->length(9)  == 18368 );
		CHECK ( mctx->length(10) == 40632 ); // TOC: 40152
		CHECK ( mctx->length(11) == 14873 ); // TOC: 14798
		CHECK ( mctx->length(12) == 11952 );
		CHECK ( mctx->length(13) ==  8783 ); // TOC: 8463
		CHECK ( mctx->length(14) == 18935 ); // TOC: 18935
		CHECK ( mctx->length(15) ==     0 );

		CHECK ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->first_relevant_sample(1)  ==     22343 ); // skipping
		CHECK ( mctx->first_relevant_sample(2)  ==   3072300 );
		CHECK ( mctx->first_relevant_sample(3)  ==   4345320 );
		CHECK ( mctx->first_relevant_sample(4)  ==  13747440 );
		CHECK ( mctx->first_relevant_sample(5)  ==  20937504 );
		CHECK ( mctx->first_relevant_sample(6)  ==  29294160 );
		CHECK ( mctx->first_relevant_sample(7)  ==  40870704 );
		CHECK ( mctx->first_relevant_sample(8)  ==  51587004 );
		CHECK ( mctx->first_relevant_sample(9)  ==  62523804 );
		CHECK ( mctx->first_relevant_sample(10) ==  82023060 );
		CHECK ( mctx->first_relevant_sample(11) ==  92823444 );
		CHECK ( mctx->first_relevant_sample(12) == 116715060 );
		CHECK ( mctx->first_relevant_sample(13) == 125460384 );
		CHECK ( mctx->first_relevant_sample(14) == 132488160 );
		CHECK ( mctx->first_relevant_sample(15) == 137652564 );
		CHECK ( mctx->first_relevant_sample(16) ==         0 ); // not a track
		CHECK ( mctx->first_relevant_sample(99) ==         0 ); // not a track

		CHECK ( mctx->last_relevant_sample()   == 148783403 );

		CHECK ( mctx->last_relevant_sample(0)  ==     19403 ); // not a track

		CHECK ( mctx->last_relevant_sample(1)  ==   3072299 );
		CHECK ( mctx->last_relevant_sample(2)  ==   4345319 );
		CHECK ( mctx->last_relevant_sample(3)  ==  13747439 );
		CHECK ( mctx->last_relevant_sample(4)  ==  20937503 );
		CHECK ( mctx->last_relevant_sample(5)  ==  29294159 );
		CHECK ( mctx->last_relevant_sample(6)  ==  40870703 );
		CHECK ( mctx->last_relevant_sample(7)  ==  51587003 );
		CHECK ( mctx->last_relevant_sample(8)  ==  62523803 );
		CHECK ( mctx->last_relevant_sample(9)  ==  82023059 );
		CHECK ( mctx->last_relevant_sample(10) ==  92823443 );
		CHECK ( mctx->last_relevant_sample(11) == 116715059 );
		CHECK ( mctx->last_relevant_sample(12) == 125460383 );
		CHECK ( mctx->last_relevant_sample(13) == 132488159 );
		CHECK ( mctx->last_relevant_sample(14) == 137652563 );
		CHECK ( mctx->last_relevant_sample(15) == 148783403 ); // skipping

		CHECK ( mctx->last_relevant_sample(16) == 148783403 ); // not a track
		CHECK ( mctx->last_relevant_sample(99) == 148783403 ); // not a track

		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
		CHECK ( mctx->track(1) == 0);

		CHECK ( mctx->track(19402) == 0);
		CHECK ( mctx->track(19403) == 0);
		CHECK ( mctx->track(19404) == 1);

		CHECK ( mctx->track(3072298) == 1);
		CHECK ( mctx->track(3072299) == 1);
		CHECK ( mctx->track(3072300) == 2);

		CHECK ( mctx->track(4345318) == 2);
		CHECK ( mctx->track(4345319) == 2);
		CHECK ( mctx->track(4345320) == 3);

		CHECK ( mctx->track(13747438) == 3);
		CHECK ( mctx->track(13747439) == 3);
		CHECK ( mctx->track(13747440) == 4);

		CHECK ( mctx->track(20937502) == 4);
		CHECK ( mctx->track(20937503) == 4);
		CHECK ( mctx->track(20937504) == 5);

		CHECK ( mctx->track(29294158) == 5);
		CHECK ( mctx->track(29294159) == 5);
		CHECK ( mctx->track(29294160) == 6);

		CHECK ( mctx->track(40870702) == 6);
		CHECK ( mctx->track(40870703) == 6);
		CHECK ( mctx->track(40870704) == 7);

		CHECK ( mctx->track(51587002) == 7);
		CHECK ( mctx->track(51587003) == 7);
		CHECK ( mctx->track(51587004) == 8);

		CHECK ( mctx->track(62523802) == 8);
		CHECK ( mctx->track(62523803) == 8);
		CHECK ( mctx->track(62523804) == 9);

		CHECK ( mctx->track(82023058) == 9);
		CHECK ( mctx->track(82023059) == 9);
		CHECK ( mctx->track(82023060) == 10);

		CHECK ( mctx->track(92823442) == 10);
		CHECK ( mctx->track(92823443) == 10);
		CHECK ( mctx->track(92823444) == 11);

		CHECK ( mctx->track(116715058) == 11);
		CHECK ( mctx->track(116715059) == 11);
		CHECK ( mctx->track(116715060) == 12);

		CHECK ( mctx->track(125460382) == 12);
		CHECK ( mctx->track(125460383) == 12);
		CHECK ( mctx->track(125460384) == 13);

		CHECK ( mctx->track(132488158) == 13);
		CHECK ( mctx->track(132488159) == 13);
		CHECK ( mctx->track(132488160) == 14);

		CHECK ( mctx->track(137652562) == 14);
		CHECK ( mctx->track(137652563) == 14);
		CHECK ( mctx->track(137652564) == 15);

		CHECK ( mctx->track(148783403) == 15);
		CHECK ( mctx->track(148783404) > mctx->total_tracks() );
	}


	SECTION ("Complete TOC with leadout, offset(1) == 0")
	{
		// Bent: Programmed to Love

		auto toc = TOCBuilder::build(
			// track count
			18,
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
				174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
			// leadout
			332075
		);

		CHECK_THROWS ( toc->offset( 0)    );
		CHECK ( toc->offset( 1) ==      0 );
		CHECK ( toc->offset( 2) ==  29042 );
		CHECK ( toc->offset( 3) ==  53880 );
		CHECK ( toc->offset( 4) ==  58227 );
		CHECK ( toc->offset( 5) ==  84420 );
		CHECK ( toc->offset( 6) ==  94192 );
		CHECK ( toc->offset( 7) == 119165 );
		CHECK ( toc->offset( 8) == 123030 );
		CHECK ( toc->offset( 9) == 147500 );
		CHECK ( toc->offset(10) == 148267 );
		CHECK ( toc->offset(11) == 174602 );
		CHECK ( toc->offset(12) == 208125 );
		CHECK ( toc->offset(13) == 212705 );
		CHECK ( toc->offset(14) == 239890 );
		CHECK ( toc->offset(15) == 268705 );
		CHECK ( toc->offset(16) == 272055 );
		CHECK ( toc->offset(17) == 291720 );
		CHECK ( toc->offset(18) == 319992 );
		CHECK_THROWS ( toc->offset(19)    );

		CHECK_THROWS ( toc->parsed_length( 0)      );
		CHECK        ( toc->parsed_length( 1) == 0 );
		CHECK        ( toc->parsed_length(18) == 0 );
		CHECK_THROWS ( toc->parsed_length(19)      );

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(18) == "" );
		CHECK_THROWS ( toc->filename(19)       );

		CHECK ( toc->leadout() == 332075 );
		CHECK ( toc->total_tracks() == 18 );
		CHECK ( toc->complete() );


		auto mctx = arcstk::make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(18, 0x00307c78, 0x0281351d, 0x27114b12);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		CHECK ( mctx->id() == arid );

		CHECK ( audiosize.leadout_frame()  ==    332075 );
		CHECK ( audiosize.total_samples()  == 195260100 );
		CHECK ( audiosize.total_pcm_bytes() == 781040400 );

		CHECK ( mctx->total_tracks() == 18 );

		CHECK ( mctx->offset( 0) ==      0 );
		CHECK ( mctx->offset( 1) ==  29042 );
		CHECK ( mctx->offset( 2) ==  53880 );
		CHECK ( mctx->offset( 3) ==  58227 );
		CHECK ( mctx->offset( 4) ==  84420 );
		CHECK ( mctx->offset( 5) ==  94192 );
		CHECK ( mctx->offset( 6) == 119165 );
		CHECK ( mctx->offset( 7) == 123030 );
		CHECK ( mctx->offset( 8) == 147500 );
		CHECK ( mctx->offset( 9) == 148267 );
		CHECK ( mctx->offset(10) == 174602 );
		CHECK ( mctx->offset(11) == 208125 );
		CHECK ( mctx->offset(12) == 212705 );
		CHECK ( mctx->offset(13) == 239890 );
		CHECK ( mctx->offset(14) == 268705 );
		CHECK ( mctx->offset(15) == 272055 );
		CHECK ( mctx->offset(16) == 291720 );
		CHECK ( mctx->offset(17) == 319992 );

		CHECK ( mctx->length( 0) == 29042 );
		CHECK ( mctx->length( 1) == 24838 );
		CHECK ( mctx->length( 2) ==  4347 );
		CHECK ( mctx->length( 3) == 26193 );
		CHECK ( mctx->length( 4) ==  9772 );
		CHECK ( mctx->length( 5) == 24973 );
		CHECK ( mctx->length( 6) ==  3865 );
		CHECK ( mctx->length( 7) == 24470 );
		CHECK ( mctx->length( 8) ==   767 );
		CHECK ( mctx->length( 9) == 26335 );
		CHECK ( mctx->length(10) == 33523 );
		CHECK ( mctx->length(11) ==  4580 );
		CHECK ( mctx->length(12) == 27185 );
		CHECK ( mctx->length(13) == 28815 );
		CHECK ( mctx->length(14) ==  3350 );
		CHECK ( mctx->length(15) == 19665 );
		CHECK ( mctx->length(16) == 28272 );
		CHECK ( mctx->length(17) == 12083 );

		CHECK ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->first_relevant_sample(1)  ==      2939 ); // skipping!
		CHECK ( mctx->first_relevant_sample(2)  ==  17076696 );
		CHECK ( mctx->first_relevant_sample(3)  ==  31681440 );
		CHECK ( mctx->first_relevant_sample(4)  ==  34237476 );
		CHECK ( mctx->first_relevant_sample(5)  ==  49638960 );
		CHECK ( mctx->first_relevant_sample(6)  ==  55384896 );
		CHECK ( mctx->first_relevant_sample(7)  ==  70069020 );
		CHECK ( mctx->first_relevant_sample(8)  ==  72341640 );
		CHECK ( mctx->first_relevant_sample(9)  ==  86730000 );
		CHECK ( mctx->first_relevant_sample(10) ==  87180996 );
		CHECK ( mctx->first_relevant_sample(11) == 102665976 );
		CHECK ( mctx->first_relevant_sample(12) == 122377500 );
		CHECK ( mctx->first_relevant_sample(13) == 125070540 );
		CHECK ( mctx->first_relevant_sample(14) == 141055320 );
		CHECK ( mctx->first_relevant_sample(15) == 157998540 );
		CHECK ( mctx->first_relevant_sample(16) == 159968340 );
		CHECK ( mctx->first_relevant_sample(17) == 171531360 );
		CHECK ( mctx->first_relevant_sample(18) == 188155296 );

		CHECK ( mctx->first_relevant_sample(19) == 0 ); // not a track

		CHECK ( mctx->last_relevant_sample()   == 195257159 );

		CHECK ( mctx->last_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->last_relevant_sample(1)  ==  17076695 );
		CHECK ( mctx->last_relevant_sample(2)  ==  31681439 );
		CHECK ( mctx->last_relevant_sample(3)  ==  34237475 );
		CHECK ( mctx->last_relevant_sample(4)  ==  49638959 );
		CHECK ( mctx->last_relevant_sample(5)  ==  55384895 );
		CHECK ( mctx->last_relevant_sample(6)  ==  70069019 );
		CHECK ( mctx->last_relevant_sample(7)  ==  72341639 );
		CHECK ( mctx->last_relevant_sample(8)  ==  86729999 );
		CHECK ( mctx->last_relevant_sample(9)  ==  87180995 );
		CHECK ( mctx->last_relevant_sample(10) == 102665975 );
		CHECK ( mctx->last_relevant_sample(11) == 122377499 );
		CHECK ( mctx->last_relevant_sample(12) == 125070539 );
		CHECK ( mctx->last_relevant_sample(13) == 141055319 );
		CHECK ( mctx->last_relevant_sample(14) == 157998539 );
		CHECK ( mctx->last_relevant_sample(15) == 159968339 );
		CHECK ( mctx->last_relevant_sample(16) == 171531359 );
		CHECK ( mctx->last_relevant_sample(17) == 188155295 );
		CHECK ( mctx->last_relevant_sample(18) == 195257159 ); // skipping

		CHECK ( mctx->last_relevant_sample(19) == 195257159 ); // not a track

		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
		CHECK ( mctx->track(1) == 1);
		CHECK ( mctx->track(2) == 1);

		CHECK ( mctx->track(17076694) == 1);
		CHECK ( mctx->track(17076695) == 1);
		CHECK ( mctx->track(17076696) == 2);

		CHECK ( mctx->track(31681438) == 2);
		CHECK ( mctx->track(31681439) == 2);
		CHECK ( mctx->track(31681440) == 3);

		CHECK ( mctx->track(34237474) == 3);
		CHECK ( mctx->track(34237475) == 3);
		CHECK ( mctx->track(34237476) == 4);

		CHECK ( mctx->track(49638958) == 4);
		CHECK ( mctx->track(49638959) == 4);
		CHECK ( mctx->track(49638960) == 5);

		CHECK ( mctx->track(55384894) == 5);
		CHECK ( mctx->track(55384895) == 5);
		CHECK ( mctx->track(55384896) == 6);

		CHECK ( mctx->track(70069018) == 6);
		CHECK ( mctx->track(70069019) == 6);
		CHECK ( mctx->track(70069020) == 7);

		CHECK ( mctx->track(72341638) == 7);
		CHECK ( mctx->track(72341639) == 7);
		CHECK ( mctx->track(72341640) == 8);

		CHECK ( mctx->track(86729998) == 8);
		CHECK ( mctx->track(86729999) == 8);
		CHECK ( mctx->track(86730000) == 9);

		CHECK ( mctx->track(87180994) == 9);
		CHECK ( mctx->track(87180995) == 9);
		CHECK ( mctx->track(87180996) == 10);

		CHECK ( mctx->track(102665974) == 10);
		CHECK ( mctx->track(102665975) == 10);
		CHECK ( mctx->track(102665976) == 11);

		CHECK ( mctx->track(122377498) == 11);
		CHECK ( mctx->track(122377499) == 11);
		CHECK ( mctx->track(122377500) == 12);

		CHECK ( mctx->track(125070538) == 12);
		CHECK ( mctx->track(125070539) == 12);
		CHECK ( mctx->track(125070540) == 13);

		CHECK ( mctx->track(141055318) == 13);
		CHECK ( mctx->track(141055319) == 13);
		CHECK ( mctx->track(141055320) == 14);

		CHECK ( mctx->track(157998538) == 14);
		CHECK ( mctx->track(157998539) == 14);
		CHECK ( mctx->track(157998540) == 15);

		CHECK ( mctx->track(159968338) == 15);
		CHECK ( mctx->track(159968339) == 15);
		CHECK ( mctx->track(159968340) == 16);

		CHECK ( mctx->track(171531358) == 16);
		CHECK ( mctx->track(171531359) == 16);
		CHECK ( mctx->track(171531360) == 17);

		CHECK ( mctx->track(188155294) == 17);
		CHECK ( mctx->track(188155295) == 17);
		CHECK ( mctx->track(188155296) == 18);

		CHECK ( mctx->track(195257158) == 18);
		CHECK ( mctx->track(195257159) == 18);
		CHECK ( mctx->track(195257160) > mctx->total_tracks());
	}


	SECTION ("Complete TOC with lengths, offset(1) == 0")
	{
		// Bent: Programmed to Love

		auto toc = TOCBuilder::build(
			// track count
			18,
			// offsets
			{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500, 148267,
				174602, 208125, 212705, 239890, 268705, 272055, 291720, 319992 },
			// lengths
			{ 29042, 24673, 4347, 26035, 9772, 24973, 3865, 24325, 767, 26335,
				33523, 4580, 27185, 28737, 3350, 19665, 28272, -1}
		);

		CHECK ( toc->total_tracks() == 18 );

		CHECK_THROWS ( toc->offset( 0)    );
		CHECK ( toc->offset( 1) ==      0 );
		CHECK ( toc->offset( 2) ==  29042 );
		CHECK ( toc->offset( 3) ==  53880 );
		CHECK ( toc->offset( 4) ==  58227 );
		CHECK ( toc->offset( 5) ==  84420 );
		CHECK ( toc->offset( 6) ==  94192 );
		CHECK ( toc->offset( 7) == 119165 );
		CHECK ( toc->offset( 8) == 123030 );
		CHECK ( toc->offset( 9) == 147500 );
		CHECK ( toc->offset(10) == 148267 );
		CHECK ( toc->offset(11) == 174602 );
		CHECK ( toc->offset(12) == 208125 );
		CHECK ( toc->offset(13) == 212705 );
		CHECK ( toc->offset(14) == 239890 );
		CHECK ( toc->offset(15) == 268705 );
		CHECK ( toc->offset(16) == 272055 );
		CHECK ( toc->offset(17) == 291720 );
		CHECK ( toc->offset(18) == 319992 );
		CHECK_THROWS ( toc->offset(19)    );

		CHECK_THROWS ( toc->parsed_length( 0)   );
		CHECK ( toc->parsed_length( 1) == 29042 );
		CHECK ( toc->parsed_length( 2) == 24673 );
		CHECK ( toc->parsed_length( 3) ==  4347 );
		CHECK ( toc->parsed_length( 4) == 26035 );
		CHECK ( toc->parsed_length( 5) ==  9772 );
		CHECK ( toc->parsed_length( 6) == 24973 );
		CHECK ( toc->parsed_length( 7) ==  3865 );
		CHECK ( toc->parsed_length( 8) == 24325 );
		CHECK ( toc->parsed_length( 9) ==   767 );
		CHECK ( toc->parsed_length(10) == 26335 );
		CHECK ( toc->parsed_length(11) == 33523 );
		CHECK ( toc->parsed_length(12) ==  4580 );
		CHECK ( toc->parsed_length(13) == 27185 );
		CHECK ( toc->parsed_length(14) == 28737 );
		CHECK ( toc->parsed_length(15) ==  3350 );
		CHECK ( toc->parsed_length(16) == 19665 );
		CHECK ( toc->parsed_length(17) == 28272 );
		CHECK ( toc->parsed_length(18) ==     0 ); //normalized
		CHECK_THROWS ( toc->parsed_length(19)   );

		CHECK_THROWS ( toc->filename( 0)       );
		CHECK        ( toc->filename( 1) == "" );
		CHECK        ( toc->filename(18) == "" );
		CHECK_THROWS ( toc->filename(19)       );

		CHECK ( toc->total_tracks() == 18 );
		CHECK ( not toc->complete() );
		CHECK ( toc->leadout() == 0 ); // unknown due to last length unknown

		auto mctx = arcstk::make_context(toc);
		auto audiosize = mctx->audio_size();
		auto arid = arcstk::ARId(18, 0x00307c78, 0x0281351d, 0x27114b12);


		CHECK ( mctx->skips_front() );
		CHECK ( mctx->skips_back() );
		CHECK ( mctx->is_multi_track() );

		CHECK ( mctx->filename() == "" );
		//CHECK ( mctx->id() == arid );  // undefined

		CHECK ( audiosize.leadout_frame()  == 0 );
		CHECK ( audiosize.total_samples()  == 0 );
		CHECK ( audiosize.total_pcm_bytes() == 0 );

		CHECK ( mctx->total_tracks() == 18 );

		CHECK ( mctx->offset( 0) ==      0 );
		CHECK ( mctx->offset( 1) ==  29042 );
		CHECK ( mctx->offset( 2) ==  53880 );
		CHECK ( mctx->offset( 3) ==  58227 );
		CHECK ( mctx->offset( 4) ==  84420 );
		CHECK ( mctx->offset( 5) ==  94192 );
		CHECK ( mctx->offset( 6) == 119165 );
		CHECK ( mctx->offset( 7) == 123030 );
		CHECK ( mctx->offset( 8) == 147500 );
		CHECK ( mctx->offset( 9) == 148267 );
		CHECK ( mctx->offset(10) == 174602 );
		CHECK ( mctx->offset(11) == 208125 );
		CHECK ( mctx->offset(12) == 212705 );
		CHECK ( mctx->offset(13) == 239890 );
		CHECK ( mctx->offset(14) == 268705 );
		CHECK ( mctx->offset(15) == 272055 );
		CHECK ( mctx->offset(16) == 291720 );
		CHECK ( mctx->offset(17) == 319992 );

		CHECK ( mctx->length( 0) == 29042 );
		CHECK ( mctx->length( 1) == 24838 );
		CHECK ( mctx->length( 2) ==  4347 );
		CHECK ( mctx->length( 3) == 26193 );
		CHECK ( mctx->length( 4) ==  9772 );
		CHECK ( mctx->length( 5) == 24973 );
		CHECK ( mctx->length( 6) ==  3865 );
		CHECK ( mctx->length( 7) == 24470 );
		CHECK ( mctx->length( 8) ==   767 );
		CHECK ( mctx->length( 9) == 26335 );
		CHECK ( mctx->length(10) == 33523 );
		CHECK ( mctx->length(11) ==  4580 );
		CHECK ( mctx->length(12) == 27185 );
		CHECK ( mctx->length(13) == 28815 );
		CHECK ( mctx->length(14) ==  3350 );
		CHECK ( mctx->length(15) == 19665 );
		CHECK ( mctx->length(16) == 28272 );
		CHECK ( mctx->length(17) ==     0 );

		CHECK ( mctx->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->first_relevant_sample(1)  ==      2939 ); // skipping!
		CHECK ( mctx->first_relevant_sample(2)  ==  17076696 );
		CHECK ( mctx->first_relevant_sample(3)  ==  31681440 );
		CHECK ( mctx->first_relevant_sample(4)  ==  34237476 );
		CHECK ( mctx->first_relevant_sample(5)  ==  49638960 );
		CHECK ( mctx->first_relevant_sample(6)  ==  55384896 );
		CHECK ( mctx->first_relevant_sample(7)  ==  70069020 );
		CHECK ( mctx->first_relevant_sample(8)  ==  72341640 );
		CHECK ( mctx->first_relevant_sample(9)  ==  86730000 );
		CHECK ( mctx->first_relevant_sample(10) ==  87180996 );
		CHECK ( mctx->first_relevant_sample(11) == 102665976 );
		CHECK ( mctx->first_relevant_sample(12) == 122377500 );
		CHECK ( mctx->first_relevant_sample(13) == 125070540 );
		CHECK ( mctx->first_relevant_sample(14) == 141055320 );
		CHECK ( mctx->first_relevant_sample(15) == 157998540 );
		CHECK ( mctx->first_relevant_sample(16) == 159968340 );
		CHECK ( mctx->first_relevant_sample(17) == 171531360 );
		CHECK ( mctx->first_relevant_sample(18) == 188155296 );

		CHECK ( mctx->first_relevant_sample(19) == 0 ); // not a track

		//CHECK ( mctx->last_relevant_sample()   == ); // undefined

		CHECK ( mctx->last_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( mctx->last_relevant_sample(1)  ==  17076695 );
		CHECK ( mctx->last_relevant_sample(2)  ==  31681439 );
		CHECK ( mctx->last_relevant_sample(3)  ==  34237475 );
		CHECK ( mctx->last_relevant_sample(4)  ==  49638959 );
		CHECK ( mctx->last_relevant_sample(5)  ==  55384895 );
		CHECK ( mctx->last_relevant_sample(6)  ==  70069019 );
		CHECK ( mctx->last_relevant_sample(7)  ==  72341639 );
		CHECK ( mctx->last_relevant_sample(8)  ==  86729999 );
		CHECK ( mctx->last_relevant_sample(9)  ==  87180995 );
		CHECK ( mctx->last_relevant_sample(10) == 102665975 );
		CHECK ( mctx->last_relevant_sample(11) == 122377499 );
		CHECK ( mctx->last_relevant_sample(12) == 125070539 );
		CHECK ( mctx->last_relevant_sample(13) == 141055319 );
		CHECK ( mctx->last_relevant_sample(14) == 157998539 );
		CHECK ( mctx->last_relevant_sample(15) == 159968339 );
		CHECK ( mctx->last_relevant_sample(16) == 171531359 );
		CHECK ( mctx->last_relevant_sample(17) == 188155295 );
		//CHECK ( mctx->last_relevant_sample(18) ==         0 ); // undefined

		//CHECK ( mctx->last_relevant_sample(19) == 195257159 ); // undefined

		// Test the bounds of each track

		CHECK ( mctx->track(0) == 0);
// FIXME Would be better to yield those values instead of 0
//		CHECK ( mctx->track(1) == 1);
//		CHECK ( mctx->track(2) == 1);
//
//		CHECK ( mctx->track(17076694) == 1);
//		CHECK ( mctx->track(17076695) == 1);
//		CHECK ( mctx->track(17076696) == 2);
//
//		CHECK ( mctx->track(31681438) == 2);
//		CHECK ( mctx->track(31681439) == 2);
//		CHECK ( mctx->track(31681440) == 3);
//
//		CHECK ( mctx->track(34237474) == 3);
//		CHECK ( mctx->track(34237475) == 3);
//		CHECK ( mctx->track(34237476) == 4);
//
//		CHECK ( mctx->track(49638958) == 4);
//		CHECK ( mctx->track(49638959) == 4);
//		CHECK ( mctx->track(49638960) == 5);
//
//		CHECK ( mctx->track(55384894) == 5);
//		CHECK ( mctx->track(55384895) == 5);
//		CHECK ( mctx->track(55384896) == 6);
//
//		CHECK ( mctx->track(70069018) == 6);
//		CHECK ( mctx->track(70069019) == 6);
//		CHECK ( mctx->track(70069020) == 7);
//
//		CHECK ( mctx->track(72341638) == 7);
//		CHECK ( mctx->track(72341639) == 7);
//		CHECK ( mctx->track(72341640) == 8);
//
//		CHECK ( mctx->track(86729998) == 8);
//		CHECK ( mctx->track(86729999) == 8);
//		CHECK ( mctx->track(86730000) == 9);
//
//		CHECK ( mctx->track(87180994) == 9);
//		CHECK ( mctx->track(87180995) == 9);
//		CHECK ( mctx->track(87180996) == 10);
//
//		CHECK ( mctx->track(102665974) == 10);
//		CHECK ( mctx->track(102665975) == 10);
//		CHECK ( mctx->track(102665976) == 11);
//
//		CHECK ( mctx->track(122377498) == 11);
//		CHECK ( mctx->track(122377499) == 11);
//		CHECK ( mctx->track(122377500) == 12);
//
//		CHECK ( mctx->track(125070538) == 12);
//		CHECK ( mctx->track(125070539) == 12);
//		CHECK ( mctx->track(125070540) == 13);
//
//		CHECK ( mctx->track(141055318) == 13);
//		CHECK ( mctx->track(141055319) == 13);
//		CHECK ( mctx->track(141055320) == 14);
//
//		CHECK ( mctx->track(157998538) == 14);
//		CHECK ( mctx->track(157998539) == 14);
//		CHECK ( mctx->track(157998540) == 15);
//
//		CHECK ( mctx->track(159968338) == 15);
//		CHECK ( mctx->track(159968339) == 15);
//		CHECK ( mctx->track(159968340) == 16);
//
//		CHECK ( mctx->track(171531358) == 16);
//		CHECK ( mctx->track(171531359) == 16);
//		CHECK ( mctx->track(171531360) == 17);
//
//		CHECK ( mctx->track(188155294) == 17);
//		CHECK ( mctx->track(188155295) == 17);
//		CHECK ( mctx->track(188155296) == 18);
//
//		CHECK ( mctx->track(195257158) == 18);
//		CHECK ( mctx->track(195257159) == 18);
//		CHECK ( mctx->track(195257160) > mctx->total_tracks());
	}
}


TEST_CASE ("MultitrackCalcContext::clone()",
		"[calculate] [calccontext] [multitrack]" )
{
	using arcstk::details::TOCBuilder;

	SECTION ( "clone()" )
	{
		// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

		auto toc = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
		);

		auto mctx = arcstk::make_context(toc);

		CHECK ( mctx->audio_size().total_pcm_bytes() == 595145376 );
		CHECK ( mctx->filename() == std::string() );

		CHECK ( mctx->total_tracks() == 15 );

		CHECK ( mctx->offset(0)  ==     33 );
		CHECK ( mctx->offset(1)  ==   5225 );
		CHECK ( mctx->offset(2)  ==   7390 );
		CHECK ( mctx->offset(3)  ==  23380 );
		CHECK ( mctx->offset(4)  ==  35608 );
		CHECK ( mctx->offset(5)  ==  49820 );
		CHECK ( mctx->offset(6)  ==  69508 );
		CHECK ( mctx->offset(7)  ==  87733 );
		CHECK ( mctx->offset(8)  == 106333 );
		CHECK ( mctx->offset(9)  == 139495 );
		CHECK ( mctx->offset(10) == 157863 );
		CHECK ( mctx->offset(11) == 198495 );
		CHECK ( mctx->offset(12) == 213368 );
		CHECK ( mctx->offset(13) == 225320 );
		CHECK ( mctx->offset(14) == 234103 );
		CHECK ( mctx->offset(15) ==      0 );

		// The lengths parsed from the CUEsheet differ from the lengths
		// computed by CalcContext. The cause is that for CalcContext the length
		// of track i is the difference offset(i+1) - offset(i). This accepts
		// the gaps as part of the track and appends each gap to the end of
		// the track. Libcue on the other hand seems to just ignore the gaps and
		// subtract them from the actual length.

		CHECK ( mctx->length(0)  ==  5192 );
		CHECK ( mctx->length(1)  ==  2165 );
		CHECK ( mctx->length(2)  == 15990 ); // TOC: 15885
		CHECK ( mctx->length(3)  == 12228 );
		CHECK ( mctx->length(4)  == 14212 ); // TOC: 13925
		CHECK ( mctx->length(5)  == 19688 ); // TOC: 19513
		CHECK ( mctx->length(6)  == 18225 ); // TOC: 18155
		CHECK ( mctx->length(7)  == 18600 ); // TOC: 18325
		CHECK ( mctx->length(8)  == 33162 ); // TOC: 33075
		CHECK ( mctx->length(9)  == 18368 );
		CHECK ( mctx->length(10) == 40632 ); // TOC: 40152
		CHECK ( mctx->length(11) == 14873 ); // TOC: 14798
		CHECK ( mctx->length(12) == 11952 );
		CHECK ( mctx->length(13) ==  8783 ); // TOC: 8463
		CHECK ( mctx->length(14) == 18935 ); // TOC: 18935
		CHECK ( mctx->length(15) ==     0 );


		auto ctx_copy = mctx->clone();

		// TODO Implement and test equality/inequality before this
		//CHECK ( ctx_copy == ctx0 );
		//CHECK ( not(ctx_copy != ctx0 ) );

		CHECK ( ctx_copy->audio_size().leadout_frame()  == 253038 );
		CHECK ( ctx_copy->audio_size().total_samples()  == 148786344 );
		CHECK ( ctx_copy->audio_size().total_pcm_bytes() == 595145376 );

		CHECK ( ctx_copy->total_tracks() == 15 );

		CHECK ( ctx_copy->offset(0)  ==     33 );
		CHECK ( ctx_copy->offset(1)  ==   5225 );
		CHECK ( ctx_copy->offset(2)  ==   7390 );
		CHECK ( ctx_copy->offset(3)  ==  23380 );
		CHECK ( ctx_copy->offset(4)  ==  35608 );
		CHECK ( ctx_copy->offset(5)  ==  49820 );
		CHECK ( ctx_copy->offset(6)  ==  69508 );
		CHECK ( ctx_copy->offset(7)  ==  87733 );
		CHECK ( ctx_copy->offset(8)  == 106333 );
		CHECK ( ctx_copy->offset(9)  == 139495 );
		CHECK ( ctx_copy->offset(10) == 157863 );
		CHECK ( ctx_copy->offset(11) == 198495 );
		CHECK ( ctx_copy->offset(12) == 213368 );
		CHECK ( ctx_copy->offset(13) == 225320 );
		CHECK ( ctx_copy->offset(14) == 234103 );
		CHECK ( ctx_copy->offset(15) ==      0 );

		CHECK ( ctx_copy->length(0)  ==  5192 );
		CHECK ( ctx_copy->length(1)  ==  2165 );
		CHECK ( ctx_copy->length(2)  == 15990 );
		CHECK ( ctx_copy->length(3)  == 12228 );
		CHECK ( ctx_copy->length(4)  == 14212 );
		CHECK ( ctx_copy->length(5)  == 19688 );
		CHECK ( ctx_copy->length(6)  == 18225 );
		CHECK ( ctx_copy->length(7)  == 18600 );
		CHECK ( ctx_copy->length(8)  == 33162 );
		CHECK ( ctx_copy->length(9)  == 18368 );
		CHECK ( ctx_copy->length(10) == 40632 );
		CHECK ( ctx_copy->length(11) == 14873 );
		CHECK ( ctx_copy->length(12) == 11952 );
		CHECK ( ctx_copy->length(13) ==  8783 );
		CHECK ( ctx_copy->length(14) == 18935 );
		CHECK ( ctx_copy->length(15) ==     0 );

		CHECK ( ctx_copy->filename() == std::string() );
		CHECK ( ctx_copy->id() ==
				arcstk::ARId(15, 0x001B9178, 0x014BE24E, 0xB40D2D0F) );

		CHECK ( ctx_copy->skips_front() );
		CHECK ( ctx_copy->skips_back() );
		CHECK ( ctx_copy->is_multi_track() );


		CHECK ( ctx_copy->first_relevant_sample(0)  ==         0 ); // not a track

		CHECK ( ctx_copy->first_relevant_sample(1)  ==     22343 ); // skipping
		CHECK ( ctx_copy->first_relevant_sample(2)  ==   3072300 );
		CHECK ( ctx_copy->first_relevant_sample(3)  ==   4345320 );
		CHECK ( ctx_copy->first_relevant_sample(4)  ==  13747440 );
		CHECK ( ctx_copy->first_relevant_sample(5)  ==  20937504 );
		CHECK ( ctx_copy->first_relevant_sample(6)  ==  29294160 );
		CHECK ( ctx_copy->first_relevant_sample(7)  ==  40870704 );
		CHECK ( ctx_copy->first_relevant_sample(8)  ==  51587004 );
		CHECK ( ctx_copy->first_relevant_sample(9)  ==  62523804 );
		CHECK ( ctx_copy->first_relevant_sample(10) ==  82023060 );
		CHECK ( ctx_copy->first_relevant_sample(11) ==  92823444 );
		CHECK ( ctx_copy->first_relevant_sample(12) == 116715060 );
		CHECK ( ctx_copy->first_relevant_sample(13) == 125460384 );
		CHECK ( ctx_copy->first_relevant_sample(14) == 132488160 );
		CHECK ( ctx_copy->first_relevant_sample(15) == 137652564 );
		CHECK ( ctx_copy->first_relevant_sample(16) ==         0 ); // not a track
		CHECK ( ctx_copy->first_relevant_sample(99) ==         0 ); // not a track

		CHECK ( ctx_copy->last_relevant_sample()    == 148783403 );

		CHECK ( ctx_copy->last_relevant_sample(0)   ==     19403 ); // not a track

		CHECK ( ctx_copy->last_relevant_sample(1)   ==   3072299 );
		CHECK ( ctx_copy->last_relevant_sample(2)   ==   4345319 );
		CHECK ( ctx_copy->last_relevant_sample(3)   ==  13747439 );
		CHECK ( ctx_copy->last_relevant_sample(4)   ==  20937503 );
		CHECK ( ctx_copy->last_relevant_sample(5)   ==  29294159 );
		CHECK ( ctx_copy->last_relevant_sample(6)   ==  40870703 );
		CHECK ( ctx_copy->last_relevant_sample(7)   ==  51587003 );
		CHECK ( ctx_copy->last_relevant_sample(8)   ==  62523803 );
		CHECK ( ctx_copy->last_relevant_sample(9)   ==  82023059 );
		CHECK ( ctx_copy->last_relevant_sample(10)  ==  92823443 );
		CHECK ( ctx_copy->last_relevant_sample(11)  == 116715059 );
		CHECK ( ctx_copy->last_relevant_sample(12)  == 125460383 );
		CHECK ( ctx_copy->last_relevant_sample(13)  == 132488159 );
		CHECK ( ctx_copy->last_relevant_sample(14)  == 137652563 );
		CHECK ( ctx_copy->last_relevant_sample(15)  == 148783403 ); // skipping

		CHECK ( ctx_copy->last_relevant_sample(16)  == 148783403 ); // not a track
		CHECK ( ctx_copy->last_relevant_sample(99)  == 148783403 ); // not a track

		CHECK ( ctx_copy->track(0) == 0);
		CHECK ( ctx_copy->track(1) == 0);

		CHECK ( ctx_copy->track(19402) == 0);
		CHECK ( ctx_copy->track(19403) == 0);
		CHECK ( ctx_copy->track(19404) == 1);

		CHECK ( ctx_copy->track(3072298) == 1);
		CHECK ( ctx_copy->track(3072299) == 1);
		CHECK ( ctx_copy->track(3072300) == 2);

		CHECK ( ctx_copy->track(4345318) == 2);
		CHECK ( ctx_copy->track(4345319) == 2);
		CHECK ( ctx_copy->track(4345320) == 3);

		CHECK ( ctx_copy->track(13747438) == 3);
		CHECK ( ctx_copy->track(13747439) == 3);
		CHECK ( ctx_copy->track(13747440) == 4);

		CHECK ( ctx_copy->track(20937502) == 4);
		CHECK ( ctx_copy->track(20937503) == 4);
		CHECK ( ctx_copy->track(20937504) == 5);

		CHECK ( ctx_copy->track(29294158) == 5);
		CHECK ( ctx_copy->track(29294159) == 5);
		CHECK ( ctx_copy->track(29294160) == 6);

		CHECK ( ctx_copy->track(40870702) == 6);
		CHECK ( ctx_copy->track(40870703) == 6);
		CHECK ( ctx_copy->track(40870704) == 7);

		CHECK ( ctx_copy->track(51587002) == 7);
		CHECK ( ctx_copy->track(51587003) == 7);
		CHECK ( ctx_copy->track(51587004) == 8);

		CHECK ( ctx_copy->track(62523802) == 8);
		CHECK ( ctx_copy->track(62523803) == 8);
		CHECK ( ctx_copy->track(62523804) == 9);

		CHECK ( ctx_copy->track(82023058) == 9);
		CHECK ( ctx_copy->track(82023059) == 9);
		CHECK ( ctx_copy->track(82023060) == 10);

		CHECK ( ctx_copy->track(92823442) == 10);
		CHECK ( ctx_copy->track(92823443) == 10);
		CHECK ( ctx_copy->track(92823444) == 11);

		CHECK ( ctx_copy->track(116715058) == 11);
		CHECK ( ctx_copy->track(116715059) == 11);
		CHECK ( ctx_copy->track(116715060) == 12);

		CHECK ( ctx_copy->track(125460382) == 12);
		CHECK ( ctx_copy->track(125460383) == 12);
		CHECK ( ctx_copy->track(125460384) == 13);

		CHECK ( ctx_copy->track(132488158) == 13);
		CHECK ( ctx_copy->track(132488159) == 13);
		CHECK ( ctx_copy->track(132488160) == 14);

		CHECK ( ctx_copy->track(137652562) == 14);
		CHECK ( ctx_copy->track(137652563) == 14);
		CHECK ( ctx_copy->track(137652564) == 15);

		CHECK ( ctx_copy->track(148783403) == 15);
		CHECK ( ctx_copy->track(148783404) > ctx_copy->total_tracks() );
	}
}


TEST_CASE ("MultitrackCalcContext Equality",
		"[calculate] [calccontext] [multitrack]" )
{
	using arcstk::details::MultitrackCalcContext;
	using arcstk::details::TOCBuilder;

	// "Bach: Organ Concertos", Simon Preston, DGG (with offset(1) > 0)

	auto mctx1 = MultitrackCalcContext(TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	));

	// same as mctx 1
	auto mctx2 = MultitrackCalcContext(TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	));

	// different from mctx 1 and 2
	auto mctx3 = MultitrackCalcContext(TOCBuilder::build(
		// track count
		16,
		// offsets
		{ 0, 2500, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
		139495, 157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	));

	CHECK ( mctx1 == mctx1 );
	CHECK ( mctx2 == mctx2 );
	CHECK ( mctx3 == mctx3 );

	CHECK ( mctx1 == mctx2 );
	CHECK ( mctx1 != mctx3 );
	CHECK ( mctx2 != mctx3 );
}



