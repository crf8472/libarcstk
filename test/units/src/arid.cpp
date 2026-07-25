#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ARId.
 */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"        // for Copy, Move
#endif


TEST_CASE ( "ARId", "[arid] [id] [identifier]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::ARId;

	const auto instance = ARId {};

	auto id = ARId { 10, 0x02c34fd0, 0x01f880cc, 0xbc55023f };


	SECTION ("Parametized construction is correct")
	{
		CHECK ( id.total_tracks() == 10 );
		CHECK ( id.disc_id_1()    == 0x02c34fd0 );
		CHECK ( id.disc_id_2()    == 0x01f880cc );
		CHECK ( id.cddb_id()      == 0xbc55023f );

		CHECK ( id.url() ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( id.filename() ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( not id.empty() );
	}

	SECTION ("Copy construction is correct")
	{
		const ARId copied { Copy<ARId>::construct(id) };

		// --

		CHECK ( copied.total_tracks() == 10 );
		CHECK ( copied.disc_id_1()    == 0x02c34fd0 );
		CHECK ( copied.disc_id_2()    == 0x01f880cc );
		CHECK ( copied.cddb_id()      == 0xbc55023f );

		CHECK ( copied.url() ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( copied.filename() ==
				"dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( not copied.empty() );
	}

	SECTION ("Move construction is correct")
	{
		auto moved = ARId {};
		Copy<ARId>::assign(moved, id);

		// --

		CHECK ( moved.total_tracks() == 10 );
		CHECK ( moved.disc_id_1()    == 0x02c34fd0 );
		CHECK ( moved.disc_id_2()    == 0x01f880cc );
		CHECK ( moved.cddb_id()      == 0xbc55023f );

		// --

		const auto id2 = ARId { 11, 0x02c34fd0, 0x04e880bb, 0xbc55023f };

		REQUIRE ( moved != id2 );

		moved = id2;

		// --

		CHECK ( moved == id2 );

		CHECK ( moved.total_tracks() == 11 );
		CHECK ( moved.disc_id_1()    == 0x02c34fd0 );
		CHECK ( moved.disc_id_2()    == 0x04e880bb );
		CHECK ( moved.cddb_id()      == 0xbc55023f );

		CHECK ( moved.url() ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( moved.filename() ==
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( not moved.empty() );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = ARId {};
		Copy<ARId>::assign(copied, id);

		// --

		CHECK ( copied.total_tracks() == 10 );
		CHECK ( copied.disc_id_1()    == 0x02c34fd0 );
		CHECK ( copied.disc_id_2()    == 0x01f880cc );
		CHECK ( copied.cddb_id()      == 0xbc55023f );

		// --

		const auto id2 = ARId { 11, 0x02c34fd0, 0x04e880bb, 0xbc55023f };

		REQUIRE ( copied != id2 );

		copied = id2;

		// --

		CHECK ( copied == id2 );

		CHECK ( copied.total_tracks() == 11 );
		CHECK ( copied.disc_id_1()    == 0x02c34fd0 );
		CHECK ( copied.disc_id_2()    == 0x04e880bb );
		CHECK ( copied.cddb_id()      == 0xbc55023f );

		CHECK ( copied.url() ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( copied.filename() ==
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( not copied.empty() );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = ARId {};
		Move<ARId>::assign(moved, id);

		// --

		CHECK ( moved.total_tracks() == 10 );
		CHECK ( moved.disc_id_1()    == 0x02c34fd0 );
		CHECK ( moved.disc_id_2()    == 0x01f880cc );
		CHECK ( moved.cddb_id()      == 0xbc55023f );


		const auto id2 = ARId { 11, 0x02c34fd0, 0x04e880bb, 0xbc55023f };

		REQUIRE ( moved != id2 );

		moved = id2;

		// --

		CHECK ( moved == id2 );

		CHECK ( moved.total_tracks() == 11 );
		CHECK ( moved.disc_id_1()    == 0x02c34fd0 );
		CHECK ( moved.disc_id_2()    == 0x04e880bb );
		CHECK ( moved.cddb_id()      == 0xbc55023f );

		CHECK ( moved.url() ==
				"http://www.accuraterip.com/accuraterip"
				"/0/d/f/"
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( moved.filename() ==
				"dBAR-011-02c34fd0-04e880bb-bc55023f.bin" );

		CHECK ( not moved.empty() );
	}
}


TEST_CASE ( "ARId default constructed instance", "[arid] [id] [identifier]" )
{
	const auto instance = arcstk::ARId {};

	SECTION ( "is empty()")
	{
		CHECK ( instance.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !instance );
	}
}


TEST_CASE ( "ARId property", "[arid] [id] [identifier]" )
{
	using arcstk::ARId;

	auto defaulted = ARId {};


	SECTION ("Equality operator == is correct")
	{
		SECTION ("for equal objects")
		{
			const auto id1 = ARId { 10, 0x02c34fd0, 0x01f880cc, 0xbc55023f };
			const auto id2 = ARId { 10, 0x02c34fd0, 0x01f880cc, 0xbc55023f };

			// --

			CHECK ( id1 == id1 );
			CHECK ( not (id1 != id1) );

			CHECK ( id1 == id2 );
			CHECK ( not (id1 != id2) );
		}

		SECTION ("for non-equal objects")
		{
			const auto id1 = ARId { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F } ;

			// different track
			const auto id2 = ARId { 16, 0x001B9178, 0x014BE24E, 0xB40D2D0F } ;
			// different id 1
			const auto id3 = ARId { 15, 0x001B9179, 0x014BE24E, 0xB40D2D0F } ;
			// different id 2
			const auto id4 = ARId { 15, 0x001B9178, 0x014BE24D, 0xB40D2D0F } ;
			// different cddb id
			const auto id5 = ARId { 15, 0x001B9178, 0x014BE24E, 0xC40D2D0F } ;

			// --

			CHECK ( id1 != id2 );
			CHECK ( not (id1 == id2) );

			CHECK ( id1 != id3 );
			CHECK ( not (id1 == id3) );

			CHECK ( id1 != id4 );
			CHECK ( not (id1 == id4) );

			CHECK ( id1 != id5 );
			CHECK ( not (id1 == id5) );


			CHECK ( id2 != id3 );
			CHECK ( not (id2 == id3) );

			CHECK ( id2 != id4 );
			CHECK ( not (id2 == id4) );

			CHECK ( id2 != id5 );
			CHECK ( not (id2 == id5) );


			CHECK ( id3 != id4 );
			CHECK ( not (id3 == id4) );

			CHECK ( id3 != id5 );
			CHECK ( not (id3 == id5) );


			CHECK ( id4 != id5 );
			CHECK ( not (id4 == id5) );
		}
	}

	// TODO Implement ARId property tests

	// SECTION ("Stream-in operator << is correct")
	// {
	// 	FAIL ( "Stream-in operator << test is missing" );
	// }
	//
	// SECTION ("operator bool() is correct")
	// {
	// 	FAIL ( "operator bool() test is missing" );
	// }
	//
	// SECTION ("swap() is correct")
	// {
	// 	FAIL ( "swap() test is missing" );
	// }
	//
	// SECTION ("to_string() is correct")
	// {
	// 	FAIL ( "to_string() test is missing" );
	// }
	//
	// SECTION ("clone() is correct")
	// {
	// 	FAIL ( "clone() test is missing" );
	// }
	//
	// SECTION ("size() is correct")
	// {
	// 	FAIL ( "size() test is missing" );
	// }
	//
	// SECTION ("empty() is correct")
	// {
	// 	FAIL ( "empty() test is missing" );
	// }
}

// TODO Implement ARId property tests

// TEST_CASE ( "ARId functions", "[arid] [id] [identifier]" )
// {
// 	using arcstk::ARId;
//
// 	auto instance = ARId {};
//
//
// 	SECTION ("ARId is correct")
// 	{
// 		FAIL ("ARId test is missing");
// 	}
//
// 	SECTION ("operator= is correct")
// 	{
// 		FAIL ("operator= test is missing");
// 	}
//
// 	SECTION ("~ARId is correct")
// 	{
// 		FAIL ("~ARId test is missing");
// 	}
//
// 	SECTION ("url is correct")
// 	{
// 		FAIL ("url test is missing");
// 	}
//
// 	SECTION ("filename is correct")
// 	{
// 		FAIL ("filename test is missing");
// 	}
//
// 	SECTION ("total_tracks is correct")
// 	{
// 		FAIL ("total_tracks test is missing");
// 	}
//
// 	SECTION ("disc_id_1 is correct")
// 	{
// 		FAIL ("disc_id_1 test is missing");
// 	}
//
// 	SECTION ("disc_id_2 is correct")
// 	{
// 		FAIL ("disc_id_2 test is missing");
// 	}
//
// 	SECTION ("cddb_id is correct")
// 	{
// 		FAIL ("cddb_id test is missing");
// 	}
//
// 	SECTION ("prefix is correct")
// 	{
// 		FAIL ("prefix test is missing");
// 	}
//
// 	SECTION ("empty is correct")
// 	{
// 		FAIL ("empty test is missing");
// 	}
//
// 	SECTION ("operator bool is correct")
// 	{
// 		FAIL ("operator bool test is missing");
// 	}
//
// 	SECTION ("swap is correct")
// 	{
// 		FAIL ("swap test is missing");
// 	}
//
// 	SECTION ("equals is correct")
// 	{
// 		FAIL ("equals test is missing");
// 	}
//
// 	SECTION ("to_string is correct")
// 	{
// 		FAIL ("to_string test is missing");
// 	}
// }


// ARId specific


TEST_CASE ( "Empty ARIds", "[arid] [id] [identifier]" )
{
	const auto id { arcstk::ARId{} };

	REQUIRE ( id.empty() );

	SECTION ( "Empty ARIds do not throw when url() is called" )
	{
		CHECK_NOTHROW( id.url() );
	}

	SECTION ( "Empty ARIds do not throw when filename() is called" )
	{
		CHECK_NOTHROW( id.filename() );
	}

	SECTION ( "Empty ARIds do not throw when to_string() is called" )
	{
		CHECK_NOTHROW( id.to_string() );
	}

	SECTION ( "Empty ARIds produce empty URLs" )
	{
		CHECK( id.url() == "" );
	}

	SECTION ( "Empty ARIds produce empty filenames" )
	{
		CHECK( id.filename() == "" );
	}

	SECTION ( "Empty ARIds produce empty strings" )
	{
		CHECK( id.to_string() == "" );
	}
}

