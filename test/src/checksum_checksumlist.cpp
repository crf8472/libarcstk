#include "catch2/catch.hpp"

#include <array>

#ifndef __LIBARCS_CHECKSUM_HPP__
#include "checksum.hpp"
#endif


/**
 * \file checksum_checksumlist.cpp Fixtures for class \ref ChecksumList
 */


TEST_CASE ( "ChecksumList<ChecksumType>", "[checksum] [checksumset]" )
{
	using type = arcs::checksum::type;

	arcs::ChecksumSet track01(0);

	REQUIRE ( track01.empty() );
	REQUIRE ( 0 == track01.size() );
	REQUIRE ( track01.begin()  == track01.end() );
	REQUIRE ( track01.cbegin() == track01.cend() );

	track01.insert(type::ARCS2, arcs::Checksum(0xB89992E5));
	track01.insert(type::ARCS1, arcs::Checksum(0x98B10E0F));

	REQUIRE ( not track01.empty() );
	REQUIRE ( 2 == track01.size() );

// Commented out: just constructed all 15 tracks

//	arcs::ChecksumList<arcs::ChecksumType> track02;
//	track02.insert(type::ARCS2, arcs::Checksum(0x4F77EB03));
//	track02.insert(type::ARCS1, arcs::Checksum(0x475F57E9));
//
//	arcs::ChecksumList<arcs::ChecksumType> track03;
//	track03.insert(type::ARCS2, arcs::Checksum(0x56582282));
//	track03.insert(type::ARCS1, arcs::Checksum(0x7304F1C4));
//
//	arcs::ChecksumList<arcs::ChecksumType> track04;
//	track04.insert(type::ARCS2, arcs::Checksum(0x9E2187F9));
//	track04.insert(type::ARCS1, arcs::Checksum(0xF2472287));
//
//	arcs::ChecksumList<arcs::ChecksumType> track05;
//	track05.insert(type::ARCS2, arcs::Checksum(0x6BE71E50));
//	track05.insert(type::ARCS1, arcs::Checksum(0x881BC504));
//
//	arcs::ChecksumList<arcs::ChecksumType> track06;
//	track06.insert(type::ARCS2, arcs::Checksum(0x01E7235F));
//	track06.insert(type::ARCS1, arcs::Checksum(0xBB94BFD4));
//
//	arcs::ChecksumList<arcs::ChecksumType> track07;
//	track07.insert(type::ARCS2, arcs::Checksum(0xD8F7763C));
//	track07.insert(type::ARCS1, arcs::Checksum(0xF9CAEE76));
//
//	arcs::ChecksumList<arcs::ChecksumType> track08;
//	track08.insert(type::ARCS2, arcs::Checksum(0x8480223E));
//	track08.insert(type::ARCS1, arcs::Checksum(0xF9F60BC1));
//
//	arcs::ChecksumList<arcs::ChecksumType> track09;
//	track09.insert(type::ARCS2, arcs::Checksum(0x42C5061C));
//	track09.insert(type::ARCS1, arcs::Checksum(0x2C736302));
//
//	arcs::ChecksumList<arcs::ChecksumType> track10;
//	track10.insert(type::ARCS2, arcs::Checksum(0x47A70F02));
//	track10.insert(type::ARCS1, arcs::Checksum(0x1C955978));
//
//	arcs::ChecksumList<arcs::ChecksumType> track11;
//	track11.insert(type::ARCS2, arcs::Checksum(0xBABF08CC));
//	track11.insert(type::ARCS1, arcs::Checksum(0xFDA6D833));
//
//	arcs::ChecksumList<arcs::ChecksumType> track12;
//	track12.insert(type::ARCS2, arcs::Checksum(0x563EDCCB));
//	track12.insert(type::ARCS1, arcs::Checksum(0x3A57E5D1));
//
//	arcs::ChecksumList<arcs::ChecksumType> track13;
//	track13.insert(type::ARCS2, arcs::Checksum(0xAB123C7C));
//	track13.insert(type::ARCS1, arcs::Checksum(0x6ED5F3E7));
//
//	arcs::ChecksumList<arcs::ChecksumType> track14;
//	track14.insert(type::ARCS2, arcs::Checksum(0xC65C20E4));
//	track14.insert(type::ARCS1, arcs::Checksum(0x4A5C3872));
//
//	arcs::ChecksumList<arcs::ChecksumType> track15;
//	track15.insert(type::ARCS2, arcs::Checksum(0x58FC3C3E));
//	track15.insert(type::ARCS1, arcs::Checksum(0x5FE8B032));


	SECTION ( "Equality and Inequality" )
	{
		arcs::ChecksumSet track02(0);
		track02.insert(type::ARCS2, arcs::Checksum(0xB89992E5));
		track02.insert(type::ARCS1, arcs::Checksum(0x98B10E0F));

		REQUIRE ( track02 == track01 );
		REQUIRE ( track01 == track02 );
		REQUIRE ( not (track02 != track01) );
		REQUIRE ( not (track01 != track02) );
	}


	SECTION ( "copy assignment" )
	{
		auto track02 = track01;

		REQUIRE ( track02 == track01 );
		REQUIRE ( track01 == track02 );
	}


	SECTION ( "copy construction" )
	{
		auto track02(track01);

		REQUIRE ( track02 == track01 );
		REQUIRE ( track01 == track02 );
	}


	SECTION ( "contains()" )
	{
		REQUIRE ( track01.contains(type::ARCS2) );
		REQUIRE ( track01.contains(type::ARCS1) );
	}


	SECTION ( "find()" ) /* find() of each position is tested with insert() */
	{
		REQUIRE ( *track01.find(type::ARCS2) == arcs::Checksum(0xB89992E5) );
		REQUIRE ( *track01.find(type::ARCS1) == arcs::Checksum(0x98B10E0F) );
	}


	SECTION ( "erase()" )
	{
		track01.erase(type::ARCS1);

		REQUIRE ( 1 == track01.size() );
		REQUIRE ( not track01.contains(type::ARCS1) );
		REQUIRE ( track01.contains(type::ARCS2) );
	}


	SECTION ( "clear()" )
	{
		track01.clear();

		REQUIRE ( 0 == track01.size() );
		REQUIRE ( track01.empty() );

		REQUIRE ( not track01.contains(type::ARCS2) );
		REQUIRE ( not track01.contains(type::ARCS1) );
	}
}


//TEST_CASE ( "ChecksumList<TrackNo>", "[checksum] [tracklist]" )
//{
//	arcs::ChecksumList<arcs::TrackNo> track_list1;
//
//	track_list1.insert( 1, arcs::Checksum(0xB89992E5));
//	track_list1.insert( 2, arcs::Checksum(0x4F77EB03));
//	track_list1.insert( 3, arcs::Checksum(0x56582282));
//	track_list1.insert( 4, arcs::Checksum(0x9E2187F9));
//	track_list1.insert( 5, arcs::Checksum(0x6BE71E50));
//	track_list1.insert( 6, arcs::Checksum(0x01E7235F));
//	track_list1.insert( 7, arcs::Checksum(0xD8F7763C));
//	track_list1.insert( 8, arcs::Checksum(0x8480223E));
//	track_list1.insert( 9, arcs::Checksum(0x42C5061C));
//	track_list1.insert(10, arcs::Checksum(0x47A70F02));
//	track_list1.insert(11, arcs::Checksum(0xBABF08CC));
//	track_list1.insert(12, arcs::Checksum(0x563EDCCB));
//	track_list1.insert(13, arcs::Checksum(0xAB123C7C));
//	track_list1.insert(14, arcs::Checksum(0xC65C20E4));
//	track_list1.insert(15, arcs::Checksum(0x58FC3C3E));
//
//	REQUIRE ( 15 == track_list1.size() );
//	REQUIRE ( not track_list1.empty() );
//
//
//	SECTION ( "contains()" )
//	{
//		REQUIRE ( track_list1.contains( 1) );
//		REQUIRE ( track_list1.contains( 2) );
//		REQUIRE ( track_list1.contains( 3) );
//		REQUIRE ( track_list1.contains( 4) );
//		REQUIRE ( track_list1.contains( 5) );
//		REQUIRE ( track_list1.contains( 6) );
//		REQUIRE ( track_list1.contains( 7) );
//		REQUIRE ( track_list1.contains( 8) );
//		REQUIRE ( track_list1.contains( 9) );
//		REQUIRE ( track_list1.contains(10) );
//		REQUIRE ( track_list1.contains(11) );
//		REQUIRE ( track_list1.contains(12) );
//		REQUIRE ( track_list1.contains(13) );
//		REQUIRE ( track_list1.contains(14) );
//		REQUIRE ( track_list1.contains(15) );
//
//		REQUIRE ( not track_list1.contains( 0) );
//		REQUIRE ( not track_list1.contains(16) );
//		REQUIRE ( not track_list1.contains(255) );
//	}
//
//
//	SECTION ( "copy assignment" )
//	{
//		auto track_list2 = track_list1;
//
//		REQUIRE ( track_list2 == track_list1 );
//		REQUIRE ( track_list1 == track_list2 );
//	}
//
//
//	SECTION ( "copy construction" )
//	{
//		auto track_list2(track_list1);
//
//		REQUIRE ( track_list2 == track_list1 );
//		REQUIRE ( track_list1 == track_list2 );
//	}
//
//
//	SECTION ( "iterators point to end() after last entry" )
//	{
//		REQUIRE ( not track_list1.empty() );
//		REQUIRE ( 15 == track_list1.size() );
//
//		auto it  = track_list1.begin();
//
//		// it + 15
//		for (auto i = track_list1.size(); i > 0; --i) { ++it; }
//
//		REQUIRE ( it == track_list1.end() );
//		// TODO: REQUIRE ( it == track_list1.cend() );
//
//		auto cit = track_list1.cbegin();
//
//		// cit + 15
//		for (auto i = track_list1.size(); i > 0; --i) { ++cit; }
//
//		REQUIRE ( cit == track_list1.cend() );
//		// TODO: REQUIRE ( cit == track_list1.end() );
//	}
//
//
//	SECTION ( "find()" )
//	{
//		REQUIRE ( *track_list1.find( 1) == arcs::Checksum(0xB89992E5) );
//		REQUIRE ( *track_list1.find( 2) == arcs::Checksum(0x4F77EB03) );
//		REQUIRE ( *track_list1.find( 3) == arcs::Checksum(0x56582282) );
//		REQUIRE ( *track_list1.find( 4) == arcs::Checksum(0x9E2187F9) );
//		REQUIRE ( *track_list1.find( 5) == arcs::Checksum(0x6BE71E50) );
//		REQUIRE ( *track_list1.find( 6) == arcs::Checksum(0x01E7235F) );
//		REQUIRE ( *track_list1.find( 7) == arcs::Checksum(0xD8F7763C) );
//		REQUIRE ( *track_list1.find( 8) == arcs::Checksum(0x8480223E) );
//		REQUIRE ( *track_list1.find( 9) == arcs::Checksum(0x42C5061C) );
//		REQUIRE ( *track_list1.find(10) == arcs::Checksum(0x47A70F02) );
//		REQUIRE ( *track_list1.find(11) == arcs::Checksum(0xBABF08CC) );
//		REQUIRE ( *track_list1.find(12) == arcs::Checksum(0x563EDCCB) );
//		REQUIRE ( *track_list1.find(13) == arcs::Checksum(0xAB123C7C) );
//		REQUIRE ( *track_list1.find(14) == arcs::Checksum(0xC65C20E4) );
//		REQUIRE ( *track_list1.find(15) == arcs::Checksum(0x58FC3C3E) );
//
//		REQUIRE ( track_list1.find(0) == track_list1.end() );
//		REQUIRE ( track_list1.end()   == track_list1.find(0));
//
//		REQUIRE ( track_list1.find(16) == track_list1.end() );
//		REQUIRE ( track_list1.end()    == track_list1.find(16));
//	}
//
//
//	SECTION ( "erase()" )
//	{
//		track_list1.erase(7);
//
//		REQUIRE ( 14 == track_list1.size() );
//		REQUIRE ( not track_list1.contains(7) );
//	}
//
//
//	SECTION ( "clear()" )
//	{
//		track_list1.clear();
//
//		REQUIRE ( 0 == track_list1.size() );
//		REQUIRE ( track_list1.empty() );
//
//		REQUIRE ( not track_list1.contains( 1) );
//		REQUIRE ( not track_list1.contains( 2) );
//		REQUIRE ( not track_list1.contains( 3) );
//		REQUIRE ( not track_list1.contains( 4) );
//		REQUIRE ( not track_list1.contains( 5) );
//		REQUIRE ( not track_list1.contains( 6) );
//		REQUIRE ( not track_list1.contains( 7) );
//		REQUIRE ( not track_list1.contains( 8) );
//		REQUIRE ( not track_list1.contains( 9) );
//		REQUIRE ( not track_list1.contains(10) );
//		REQUIRE ( not track_list1.contains(11) );
//		REQUIRE ( not track_list1.contains(12) );
//		REQUIRE ( not track_list1.contains(13) );
//		REQUIRE ( not track_list1.contains(14) );
//		REQUIRE ( not track_list1.contains(15) );
//	}
//}

