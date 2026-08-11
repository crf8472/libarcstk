#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for istream_wrapper.
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


TEST_CASE ( "istream_wrapper", "[istream_wrapper] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::istream_wrapper;

	auto instance = istream_wrapper {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const istream_wrapper copied { Copy<istream_wrapper>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const istream_wrapper moved { Move<istream_wrapper>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = istream_wrapper {};
		Copy<istream_wrapper>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = istream_wrapper {};
		Move<istream_wrapper>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "istream_wrapper default constructed instance",
		"[istream_wrapper] [dbar] [dbar]" )
{
	const auto defaulted = arcstk::istream_wrapper{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "istream_wrapper property", "[istream_wrapper] [dbar] [dbar]" )
{
	using arcstk::istream_wrapper;

	auto instance = istream_wrapper {};


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


TEST_CASE ( "istream_wrapper functions", "[istream_wrapper] [dbar] [dbar]" )
{
	using arcstk::istream_wrapper;

	auto instance = istream_wrapper {};

}

