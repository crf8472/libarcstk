#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for StreamParseException.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "StreamParseException", "[streamparseexception] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::StreamParseException;

	auto instance = StreamParseException {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const StreamParseException copied { Copy<StreamParseException>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const StreamParseException moved { Move<StreamParseException>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = StreamParseException {};
		Copy<StreamParseException>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = StreamParseException {};
		Move<StreamParseException>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "StreamParseException default constructed instance",
		"[streamparseexception] [dbar] [dbar]" )
{
	const auto defaulted = arcstk::StreamParseException{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "StreamParseException property", "[streamparseexception] [dbar] [dbar]" )
{
	using arcstk::StreamParseException;

	auto instance = StreamParseException {};


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


TEST_CASE ( "StreamParseException functions", "[streamparseexception] [dbar] [dbar]" )
{
	using arcstk::StreamParseException;

	auto instance = StreamParseException {};

	SECTION ("StreamParseException() is correct")
	{
		FAIL ("StreamParseException() test is missing");
	}

	SECTION ("byte_position() is correct")
	{
		FAIL ("byte_position() test is missing");
	}

	SECTION ("block() is correct")
	{
		FAIL ("block() test is missing");
	}

	SECTION ("block_byte_position() is correct")
	{
		FAIL ("block_byte_position() test is missing");
	}

}

