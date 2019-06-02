#include "catch2/catch.hpp"

#include <array>

#ifndef __LIBARCSTK_CHECKSUM_HPP__
#include "checksum.hpp"
#endif


/**
 * \file checksum.cpp Fixtures for classes in submodule 'checksum'
 */


// checksum::type


TEST_CASE ( "checksum::type_name provides correct names",
		"[checksum] [checksum::type_name]" )
{
	using type      = arcstk::checksum::type;

	CHECK ( arcstk::checksum::type_name(type::ARCS1)   == "ARCSv1" );
	CHECK ( arcstk::checksum::type_name(type::ARCS2)   == "ARCSv2" );
	//CHECK ( arcstk::checksum::type_name(type::CRC32)   == "CRC32" );
	//CHECK ( arcstk::checksum::type_name(type::CRC32ns) == "CRC32ns" );
}


// ChecksumList


TEST_CASE ( "ChecksumList<ChecksumType>", "[checksum] [checksumset]" )
{
	using type = arcstk::checksum::type;

	arcstk::ChecksumSet track01(0);

	CHECK ( track01.empty() );
	CHECK ( 0 == track01.size() );
	CHECK ( track01.begin()  == track01.end() );
	CHECK ( track01.cbegin() == track01.cend() );

	track01.insert(type::ARCS2, arcstk::Checksum(0xB89992E5));
	track01.insert(type::ARCS1, arcstk::Checksum(0x98B10E0F));

	CHECK ( not track01.empty() );
	CHECK ( 2 == track01.size() );

// Commented out: just constructed all 15 tracks

//	arcstk::ChecksumList<arcstk::ChecksumType> track02;
//	track02.insert(type::ARCS2, arcstk::Checksum(0x4F77EB03));
//	track02.insert(type::ARCS1, arcstk::Checksum(0x475F57E9));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track03;
//	track03.insert(type::ARCS2, arcstk::Checksum(0x56582282));
//	track03.insert(type::ARCS1, arcstk::Checksum(0x7304F1C4));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track04;
//	track04.insert(type::ARCS2, arcstk::Checksum(0x9E2187F9));
//	track04.insert(type::ARCS1, arcstk::Checksum(0xF2472287));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track05;
//	track05.insert(type::ARCS2, arcstk::Checksum(0x6BE71E50));
//	track05.insert(type::ARCS1, arcstk::Checksum(0x881BC504));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track06;
//	track06.insert(type::ARCS2, arcstk::Checksum(0x01E7235F));
//	track06.insert(type::ARCS1, arcstk::Checksum(0xBB94BFD4));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track07;
//	track07.insert(type::ARCS2, arcstk::Checksum(0xD8F7763C));
//	track07.insert(type::ARCS1, arcstk::Checksum(0xF9CAEE76));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track08;
//	track08.insert(type::ARCS2, arcstk::Checksum(0x8480223E));
//	track08.insert(type::ARCS1, arcstk::Checksum(0xF9F60BC1));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track09;
//	track09.insert(type::ARCS2, arcstk::Checksum(0x42C5061C));
//	track09.insert(type::ARCS1, arcstk::Checksum(0x2C736302));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track10;
//	track10.insert(type::ARCS2, arcstk::Checksum(0x47A70F02));
//	track10.insert(type::ARCS1, arcstk::Checksum(0x1C955978));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track11;
//	track11.insert(type::ARCS2, arcstk::Checksum(0xBABF08CC));
//	track11.insert(type::ARCS1, arcstk::Checksum(0xFDA6D833));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track12;
//	track12.insert(type::ARCS2, arcstk::Checksum(0x563EDCCB));
//	track12.insert(type::ARCS1, arcstk::Checksum(0x3A57E5D1));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track13;
//	track13.insert(type::ARCS2, arcstk::Checksum(0xAB123C7C));
//	track13.insert(type::ARCS1, arcstk::Checksum(0x6ED5F3E7));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track14;
//	track14.insert(type::ARCS2, arcstk::Checksum(0xC65C20E4));
//	track14.insert(type::ARCS1, arcstk::Checksum(0x4A5C3872));
//
//	arcstk::ChecksumList<arcstk::ChecksumType> track15;
//	track15.insert(type::ARCS2, arcstk::Checksum(0x58FC3C3E));
//	track15.insert(type::ARCS1, arcstk::Checksum(0x5FE8B032));


	SECTION ( "Equality and Inequality" )
	{
		arcstk::ChecksumSet track02(0);
		track02.insert(type::ARCS2, arcstk::Checksum(0xB89992E5));
		track02.insert(type::ARCS1, arcstk::Checksum(0x98B10E0F));

		CHECK ( track02 == track01 );
		CHECK ( track01 == track02 );
		CHECK ( not (track02 != track01) );
		CHECK ( not (track01 != track02) );
	}


	SECTION ( "copy assignment" )
	{
		auto track02 = track01;

		CHECK ( track02 == track01 );
		CHECK ( track01 == track02 );
	}


	SECTION ( "copy construction" )
	{
		auto track02(track01);

		CHECK ( track02 == track01 );
		CHECK ( track01 == track02 );
	}


	SECTION ( "contains()" )
	{
		CHECK ( track01.contains(type::ARCS2) );
		CHECK ( track01.contains(type::ARCS1) );
	}


	SECTION ( "find()" ) /* find() of each position is tested with insert() */
	{
		CHECK ( *track01.find(type::ARCS2) == arcstk::Checksum(0xB89992E5) );
		CHECK ( *track01.find(type::ARCS1) == arcstk::Checksum(0x98B10E0F) );
	}


	SECTION ( "erase()" )
	{
		track01.erase(type::ARCS1);

		CHECK ( 1 == track01.size() );
		CHECK ( not track01.contains(type::ARCS1) );
		CHECK ( track01.contains(type::ARCS2) );
	}


	SECTION ( "clear()" )
	{
		track01.clear();

		CHECK ( 0 == track01.size() );
		CHECK ( track01.empty() );

		CHECK ( not track01.contains(type::ARCS2) );
		CHECK ( not track01.contains(type::ARCS1) );
	}
}


//TEST_CASE ( "ChecksumList<TrackNo>", "[checksum] [tracklist]" )
//{
//	arcstk::ChecksumList<arcstk::TrackNo> track_list1;
//
//	track_list1.insert( 1, arcstk::Checksum(0xB89992E5));
//	track_list1.insert( 2, arcstk::Checksum(0x4F77EB03));
//	track_list1.insert( 3, arcstk::Checksum(0x56582282));
//	track_list1.insert( 4, arcstk::Checksum(0x9E2187F9));
//	track_list1.insert( 5, arcstk::Checksum(0x6BE71E50));
//	track_list1.insert( 6, arcstk::Checksum(0x01E7235F));
//	track_list1.insert( 7, arcstk::Checksum(0xD8F7763C));
//	track_list1.insert( 8, arcstk::Checksum(0x8480223E));
//	track_list1.insert( 9, arcstk::Checksum(0x42C5061C));
//	track_list1.insert(10, arcstk::Checksum(0x47A70F02));
//	track_list1.insert(11, arcstk::Checksum(0xBABF08CC));
//	track_list1.insert(12, arcstk::Checksum(0x563EDCCB));
//	track_list1.insert(13, arcstk::Checksum(0xAB123C7C));
//	track_list1.insert(14, arcstk::Checksum(0xC65C20E4));
//	track_list1.insert(15, arcstk::Checksum(0x58FC3C3E));
//
//	CHECK ( 15 == track_list1.size() );
//	CHECK ( not track_list1.empty() );
//
//
//	SECTION ( "contains()" )
//	{
//		CHECK ( track_list1.contains( 1) );
//		CHECK ( track_list1.contains( 2) );
//		CHECK ( track_list1.contains( 3) );
//		CHECK ( track_list1.contains( 4) );
//		CHECK ( track_list1.contains( 5) );
//		CHECK ( track_list1.contains( 6) );
//		CHECK ( track_list1.contains( 7) );
//		CHECK ( track_list1.contains( 8) );
//		CHECK ( track_list1.contains( 9) );
//		CHECK ( track_list1.contains(10) );
//		CHECK ( track_list1.contains(11) );
//		CHECK ( track_list1.contains(12) );
//		CHECK ( track_list1.contains(13) );
//		CHECK ( track_list1.contains(14) );
//		CHECK ( track_list1.contains(15) );
//
//		CHECK ( not track_list1.contains( 0) );
//		CHECK ( not track_list1.contains(16) );
//		CHECK ( not track_list1.contains(255) );
//	}
//
//
//	SECTION ( "copy assignment" )
//	{
//		auto track_list2 = track_list1;
//
//		CHECK ( track_list2 == track_list1 );
//		CHECK ( track_list1 == track_list2 );
//	}
//
//
//	SECTION ( "copy construction" )
//	{
//		auto track_list2(track_list1);
//
//		CHECK ( track_list2 == track_list1 );
//		CHECK ( track_list1 == track_list2 );
//	}
//
//
//	SECTION ( "iterators point to end() after last entry" )
//	{
//		CHECK ( not track_list1.empty() );
//		CHECK ( 15 == track_list1.size() );
//
//		auto it  = track_list1.begin();
//
//		// it + 15
//		for (auto i = track_list1.size(); i > 0; --i) { ++it; }
//
//		CHECK ( it == track_list1.end() );
//		// TODO: CHECK ( it == track_list1.cend() );
//
//		auto cit = track_list1.cbegin();
//
//		// cit + 15
//		for (auto i = track_list1.size(); i > 0; --i) { ++cit; }
//
//		CHECK ( cit == track_list1.cend() );
//		// TODO: CHECK ( cit == track_list1.end() );
//	}
//
//
//	SECTION ( "find()" )
//	{
//		CHECK ( *track_list1.find( 1) == arcstk::Checksum(0xB89992E5) );
//		CHECK ( *track_list1.find( 2) == arcstk::Checksum(0x4F77EB03) );
//		CHECK ( *track_list1.find( 3) == arcstk::Checksum(0x56582282) );
//		CHECK ( *track_list1.find( 4) == arcstk::Checksum(0x9E2187F9) );
//		CHECK ( *track_list1.find( 5) == arcstk::Checksum(0x6BE71E50) );
//		CHECK ( *track_list1.find( 6) == arcstk::Checksum(0x01E7235F) );
//		CHECK ( *track_list1.find( 7) == arcstk::Checksum(0xD8F7763C) );
//		CHECK ( *track_list1.find( 8) == arcstk::Checksum(0x8480223E) );
//		CHECK ( *track_list1.find( 9) == arcstk::Checksum(0x42C5061C) );
//		CHECK ( *track_list1.find(10) == arcstk::Checksum(0x47A70F02) );
//		CHECK ( *track_list1.find(11) == arcstk::Checksum(0xBABF08CC) );
//		CHECK ( *track_list1.find(12) == arcstk::Checksum(0x563EDCCB) );
//		CHECK ( *track_list1.find(13) == arcstk::Checksum(0xAB123C7C) );
//		CHECK ( *track_list1.find(14) == arcstk::Checksum(0xC65C20E4) );
//		CHECK ( *track_list1.find(15) == arcstk::Checksum(0x58FC3C3E) );
//
//		CHECK ( track_list1.find(0) == track_list1.end() );
//		CHECK ( track_list1.end()   == track_list1.find(0));
//
//		CHECK ( track_list1.find(16) == track_list1.end() );
//		CHECK ( track_list1.end()    == track_list1.find(16));
//	}
//
//
//	SECTION ( "erase()" )
//	{
//		track_list1.erase(7);
//
//		CHECK ( 14 == track_list1.size() );
//		CHECK ( not track_list1.contains(7) );
//	}
//
//
//	SECTION ( "clear()" )
//	{
//		track_list1.clear();
//
//		CHECK ( 0 == track_list1.size() );
//		CHECK ( track_list1.empty() );
//
//		CHECK ( not track_list1.contains( 1) );
//		CHECK ( not track_list1.contains( 2) );
//		CHECK ( not track_list1.contains( 3) );
//		CHECK ( not track_list1.contains( 4) );
//		CHECK ( not track_list1.contains( 5) );
//		CHECK ( not track_list1.contains( 6) );
//		CHECK ( not track_list1.contains( 7) );
//		CHECK ( not track_list1.contains( 8) );
//		CHECK ( not track_list1.contains( 9) );
//		CHECK ( not track_list1.contains(10) );
//		CHECK ( not track_list1.contains(11) );
//		CHECK ( not track_list1.contains(12) );
//		CHECK ( not track_list1.contains(13) );
//		CHECK ( not track_list1.contains(14) );
//		CHECK ( not track_list1.contains(15) );
//	}
//}

