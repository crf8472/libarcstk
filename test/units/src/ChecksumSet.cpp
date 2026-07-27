#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ChecksumSet.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "ChecksumSet", "[checksumset] [calc] [checksum]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;

	auto instance = ChecksumSet {};

	REQUIRE ( instance.empty() );
	REQUIRE ( instance.size() == 0 );
	REQUIRE ( instance.begin()  == instance.end() );
	REQUIRE ( instance.cbegin() == instance.cend() );

	instance.insert(type::ARCS2, Checksum { 0xB89992E5 });
	instance.insert(type::ARCS1, Checksum { 0x98B10E0F });

	REQUIRE ( not instance.empty() );
	REQUIRE ( instance.size() == 2 );


	// SECTION ("Parametized construction is correct")
	// {
	// 	FAIL ( "Parametized construction test is missing" );
	// }

	SECTION ("Copy construction is correct")
	{
		const ChecksumSet copied { Copy<ChecksumSet>::construct(instance) };

		// --

		CHECK ( copied == instance );
		CHECK ( instance == copied );

		CHECK ( copied.get(type::ARCS2).first == Checksum { 0xB89992E5 }  );
		CHECK ( copied.get(type::ARCS1).first == Checksum { 0x98B10E0F }  );
	}

	SECTION ("Move construction is correct")
	{
		const ChecksumSet moved { Move<ChecksumSet>::construct(instance) };

		// --

		CHECK ( moved.get(type::ARCS2).first == Checksum { 0xB89992E5 }  );
		CHECK ( moved.get(type::ARCS1).first == Checksum { 0x98B10E0F }  );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = ChecksumSet {};
		Copy<ChecksumSet>::assign(copied, instance);

		// --

		CHECK ( copied == instance );
		CHECK ( instance == copied );

		CHECK ( copied.get(type::ARCS2).first == Checksum { 0xB89992E5 }  );
		CHECK ( copied.get(type::ARCS1).first == Checksum { 0x98B10E0F }  );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = ChecksumSet {};
		Move<ChecksumSet>::assign(moved, instance);

		// --

		CHECK ( moved.get(type::ARCS2).first == Checksum { 0xB89992E5 }  );
		CHECK ( moved.get(type::ARCS1).first == Checksum { 0x98B10E0F }  );
	}
}


TEST_CASE ( "ChecksumSet default constructed instance",
		"[checksumset] [calc] [checksum]" )
{
	const auto instance = arcstk::ChecksumSet{};

	SECTION ( "is empty()")
	{
		CHECK ( instance.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !instance );
	}
}


TEST_CASE ( "ChecksumSet property", "[checksumset] [calc] [checksum]" )
{
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;

	auto instance = ChecksumSet {};

	REQUIRE ( instance.empty() );
	REQUIRE ( instance.size() == 0 );
	REQUIRE ( instance.begin()  == instance.end() );
	REQUIRE ( instance.cbegin() == instance.cend() );

	instance.insert(type::ARCS2, Checksum { 0xB89992E5 });
	instance.insert(type::ARCS1, Checksum { 0x98B10E0F });

	REQUIRE ( not instance.empty() );
	REQUIRE ( instance.size() == 2 );


	SECTION ("Equality operator == is correct")
	{
		ChecksumSet track02 {};

		track02.insert(type::ARCS2, Checksum { 0xB89992E5 });
		track02.insert(type::ARCS1, Checksum { 0x98B10E0F });

		CHECK ( track02 == instance );
		CHECK ( instance == track02 );

		CHECK ( not (track02 != instance) );
		CHECK ( not (instance != track02) );
	}
/*
	SECTION ("Stream-in operator << is correct")
	{
		FAIL ( "Stream-in operator << test is missing" );
	}

	SECTION ("operator bool() is correct")
	{
		FAIL ( "operator bool() test is missing" );
	}

	SECTION ("swap() is correct")
	{
		FAIL ( "swap() test is missing" );
	}

	SECTION ("to_string() is correct")
	{
		FAIL ( "to_string() test is missing" );
	}

	SECTION ("clone() is correct")
	{
		FAIL ( "clone() test is missing" );
	}

	SECTION ("size() is correct")
	{
		FAIL ( "size() test is missing" );
	}

	SECTION ("empty() is correct")
	{
		FAIL ( "empty() test is missing" );
	}
*/
}


TEST_CASE ( "ChecksumSet functions", "[checksumset] [calc] [checksum]" )
{
	using arcstk::checksum::type;
	using arcstk::Checksum;
	using arcstk::ChecksumSet;

	auto instance = ChecksumSet {};

	REQUIRE ( instance.empty() );
	REQUIRE ( instance.size() == 0 );
	REQUIRE ( instance.begin()  == instance.end() );
	REQUIRE ( instance.cbegin() == instance.cend() );

	instance.insert(type::ARCS2, Checksum { 0xB89992E5 });
	instance.insert(type::ARCS1, Checksum { 0x98B10E0F });

	REQUIRE ( not instance.empty() );
	REQUIRE ( instance.size() == 2 );

/*
	SECTION ("length is correct")
	{
		FAIL ("length test is missing");
	}

	SECTION ("set_length is correct")
	{
		FAIL ("set_length test is missing");
	}
*/
	SECTION ("contains is correct")
	{
		CHECK ( instance.contains(type::ARCS2) );
		CHECK ( instance.contains(type::ARCS1) );
	}

	SECTION ("get is correct")
	{
		CHECK ( instance.get(type::ARCS2).first == Checksum { 0xB89992E5 }  );
		CHECK ( instance.get(type::ARCS1).first == Checksum { 0x98B10E0F }  );
	}
/*
	SECTION ("types is correct")
	{
		FAIL ("types test is missing");
	}
*/
	SECTION ("insert is correct")
	{
		CHECK ( instance.get(type::ARCS2).first == Checksum { 0xB89992E5 });
		CHECK ( instance.get(type::ARCS1).first == Checksum { 0x98B10E0F });
	}

	SECTION ( "insert()ing same type again has no effect" )
	{
		instance.insert(type::ARCS2, Checksum { 0x4F77EB03 });
		instance.insert(type::ARCS1, Checksum { 0x475F57E9 });

		// --

		CHECK ( instance.get(type::ARCS2).first == Checksum { 0xB89992E5 });
		CHECK ( instance.get(type::ARCS1).first == Checksum { 0x98B10E0F });
	}

	SECTION ("merge() new elements is correct")
	{
		ChecksumSet track02;

		track02.insert(type::ARCS1, Checksum { 0x475F57E9 } );
		track02.insert(type::ARCS2, Checksum { 0x4F77EB03 } );

		ChecksumSet track03;

		track03.insert(type::ARCS1, Checksum { 0xB89992E5 } );

		track03.merge(track02); // Inserts ARCSv2 but leaves ARCSv1 untouched

		CHECK ( track03.size() == 2 );
		CHECK ( track03.get(type::ARCS1).first == Checksum { 0xB89992E5 }  );
		CHECK ( track03.get(type::ARCS2).first == Checksum { 0x4F77EB03 }  );
	}

	SECTION ("merge() existing elements does nothing")
	{
		ChecksumSet track02;

		track02.insert(type::ARCS1, Checksum { 0x475F57E9 } );
		track02.insert(type::ARCS2, Checksum { 0x4F77EB03 } );

		instance.merge(track02); // does nothing, since both types are present

		CHECK ( instance.size() == 2 );
		CHECK ( instance.get(type::ARCS2).first == Checksum { 0xB89992E5 }  );
		CHECK ( instance.get(type::ARCS1).first == Checksum { 0x98B10E0F }  );
	}

	SECTION ("erase is correct")
	{
		instance.erase(type::ARCS1);

		CHECK ( instance.size() == 1 );
		CHECK ( not instance.contains(type::ARCS1) );
		CHECK ( instance.contains(type::ARCS2) );
	}

	SECTION ("clear is correct")
	{
		instance.clear();

		CHECK ( instance.size() == 0 );
		CHECK ( instance.empty() );

		CHECK ( not instance.contains(type::ARCS2) );
		CHECK ( not instance.contains(type::ARCS1) );
	}

	SECTION ("begin is correct")
	{
		auto it { instance.begin()  };

		CHECK ( it != instance.end() );

		CHECK ( not (*it).second.zero() );
		CHECK ( not it->second.zero() );
	}

	SECTION ("end is correct")
	{
		auto it { instance.begin()  };

		// begin() + size()
		for (auto i = instance.size(); i > 0; --i) { ++it; }

		CHECK ( it == instance.end() );
	}

	SECTION ("cbegin is correct")
	{
		auto cit { instance.cbegin() };

		CHECK ( cit != instance.cend() );

		CHECK ( not (*cit).second.zero() );
		CHECK ( not cit->second.zero() );
	}

	SECTION ("cend is correct")
	{
		auto cit { instance.cbegin() };

		// begin() + size()
		for (auto i = instance.size(); i > 0; --i) { ++cit; }

		CHECK ( cit == instance.cend() );
	}
/*
	SECTION ("size is correct")
	{
		FAIL ("size test is missing");
	}

	SECTION ("empty is correct")
	{
		FAIL ("empty test is missing");
	}

	SECTION ("operator bool is correct")
	{
		FAIL ("operator bool test is missing");
	}

	SECTION ("swap is correct")
	{
		FAIL ("swap test is missing");
	}

	SECTION ("equals is correct")
	{
		FAIL ("equals test is missing");
	}

	SECTION ("to_string is correct")
	{
		FAIL ("to_string test is missing");
	}
*/
}

