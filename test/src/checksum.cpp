#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for checksum.hpp
 */

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"           // TO BE TESTED
#endif

#include <utility>                // for move


TEST_CASE ( "checksum::type_name provides correct names",
		"[checksum::type_name] [calc]" )
{
	using arcstk::checksum::type;
	using arcstk::checksum::type_name;

	CHECK ( type_name(type::ARCS1) == "ARCSv1" );
	CHECK ( type_name(type::ARCS2) == "ARCSv2" );
	//CHECK ( type_name(type::CRC32)   == "CRC32" );
	//CHECK ( type_name(type::CRC32ns) == "CRC32ns" );
}


TEST_CASE ( "Checksum", "[checksum] [calc]" )
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


TEST_CASE ( "ChecksumSet", "[checksumset] [calc]" )
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


	SECTION ( "get(type)" )
	{
		CHECK ( track01.get(type::ARCS2) == Checksum(0xB89992E5) );
		CHECK ( track01.get(type::ARCS1) == Checksum(0x98B10E0F) );
	}


	SECTION ( "merge(rhs) present does nothing" )
	{
		ChecksumSet track02;
		track02.insert(type::ARCS1, Checksum(0x475F57E9));
		track02.insert(type::ARCS2, Checksum(0x4F77EB03));

		track01.merge(track02); // does nothing, since both types are present

		CHECK ( track01.size() == 2 );
		CHECK ( track01.get(type::ARCS2) == Checksum(0xB89992E5) );
		CHECK ( track01.get(type::ARCS1) == Checksum(0x98B10E0F) );
	}


	SECTION ( "merge(rhs) new elements works" )
	{
		ChecksumSet track02;
		track02.insert(type::ARCS1, Checksum(0x475F57E9));
		track02.insert(type::ARCS2, Checksum(0x4F77EB03));

		ChecksumSet track03;
		track03.insert(type::ARCS1, Checksum(0xB89992E5));

		track03.merge(track02); // Inserts ARCSv2 but leaves ARCSv1 untouched

		CHECK ( track03.size() == 2 );
		CHECK ( track03.get(type::ARCS1) == Checksum(0xB89992E5) );
		CHECK ( track03.get(type::ARCS2) == Checksum(0x4F77EB03) );
	}


	SECTION ( "iterator begin() points to first entry" )
	{
		auto it { track01.begin()  };

		CHECK ( it != track01.end() );

		CHECK ( not (*it).second.empty() );
		CHECK ( not it->second.empty() );
	}


	SECTION ( "const_iterator cbegin() points to first entry" )
	{
		auto cit { track01.cbegin() };

		CHECK ( cit != track01.cend() );

		CHECK ( not (*cit).second.empty() );
		CHECK ( not cit->second.empty() );
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


TEST_CASE ( "Checksums", "[checksums] [calc]" )
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


	SECTION ( "at() read succeeds on all legal positions" )
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
	}


	SECTION ( "at() throws accessing illegal positions" )
	{
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

