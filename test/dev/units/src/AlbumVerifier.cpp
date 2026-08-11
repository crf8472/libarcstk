#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for AlbumVerifier.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "AlbumVerifier", "[albumverifier] [verify] [verify]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::AlbumVerifier;

	auto instance = AlbumVerifier {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const AlbumVerifier copied { Copy<AlbumVerifier>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const AlbumVerifier moved { Move<AlbumVerifier>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = AlbumVerifier {};
		Copy<AlbumVerifier>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = AlbumVerifier {};
		Move<AlbumVerifier>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "AlbumVerifier default constructed instance",
		"[albumverifier] [verify] [verify]" )
{
	const auto defaulted = arcstk::AlbumVerifier{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "AlbumVerifier property", "[albumverifier] [verify] [verify]" )
{
	using arcstk::AlbumVerifier;

	auto instance = AlbumVerifier {};


	SECTION ("Equality operator == is correct")
	{
		FAIL ( "Equality operator test is missing" );
	}

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
}


TEST_CASE ( "AlbumVerifier functions", "[albumverifier] [verify] [verify]" )
{
	using arcstk::AlbumVerifier;

	auto instance = AlbumVerifier {};

	SECTION ("do_actual_id() is correct")
	{
		FAIL ("do_actual_id() test is missing");
	}

	SECTION ("do_actual_checksums() is correct")
	{
		FAIL ("do_actual_checksums() test is missing");
	}

	SECTION ("do_strict() is correct")
	{
		FAIL ("do_strict() test is missing");
	}

	SECTION ("do_set_strict() is correct")
	{
		FAIL ("do_set_strict() test is missing");
	}

	SECTION ("do_perform() is correct")
	{
		FAIL ("do_perform() test is missing");
	}

	SECTION ("do_clone() is correct")
	{
		FAIL ("do_clone() test is missing");
	}

	SECTION ("AlbumVerifier() is correct")
	{
		FAIL ("AlbumVerifier() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

	SECTION ("~AlbumVerifier() is correct")
	{
		FAIL ("~AlbumVerifier() test is missing");
	}

}

