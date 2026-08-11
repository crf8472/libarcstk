#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for InvalidMetadataException.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "InvalidMetadataException", "[invalidmetadataexception] [metadata] [metadata]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::InvalidMetadataException;

	auto instance = InvalidMetadataException {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const InvalidMetadataException copied { Copy<InvalidMetadataException>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const InvalidMetadataException moved { Move<InvalidMetadataException>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = InvalidMetadataException {};
		Copy<InvalidMetadataException>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = InvalidMetadataException {};
		Move<InvalidMetadataException>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "InvalidMetadataException default constructed instance",
		"[invalidmetadataexception] [metadata] [metadata]" )
{
	const auto defaulted = arcstk::InvalidMetadataException{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "InvalidMetadataException property", "[invalidmetadataexception] [metadata] [metadata]" )
{
	using arcstk::InvalidMetadataException;

	auto instance = InvalidMetadataException {};


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


TEST_CASE ( "InvalidMetadataException functions", "[invalidmetadataexception] [metadata] [metadata]" )
{
	using arcstk::InvalidMetadataException;

	auto instance = InvalidMetadataException {};

	SECTION ("InvalidMetadataException() is correct")
	{
		FAIL ("InvalidMetadataException() test is missing");
	}

	SECTION ("requirement() is correct")
	{
		FAIL ("requirement() test is missing");
	}

	SECTION ("violating_value() is correct")
	{
		FAIL ("violating_value() test is missing");
	}

	SECTION ("pos() is correct")
	{
		FAIL ("pos() test is missing");
	}

}

