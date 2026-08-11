#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for IteratorElement.
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


TEST_CASE ( "IteratorElement", "[iteratorelement] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::IteratorElement;

	auto instance = IteratorElement {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const IteratorElement copied { Copy<IteratorElement>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const IteratorElement moved { Move<IteratorElement>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = IteratorElement {};
		Copy<IteratorElement>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = IteratorElement {};
		Move<IteratorElement>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "IteratorElement default constructed instance",
		"[iteratorelement] [dbar] [dbar]" )
{
	const auto defaulted = arcstk::IteratorElement{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "IteratorElement property", "[iteratorelement] [dbar] [dbar]" )
{
	using arcstk::IteratorElement;

	auto instance = IteratorElement {};


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


TEST_CASE ( "IteratorElement functions", "[iteratorelement] [dbar] [dbar]" )
{
	using arcstk::IteratorElement;

	auto instance = IteratorElement {};

	SECTION ("IteratorElement() is correct")
	{
		FAIL ("IteratorElement() test is missing");
	}

	SECTION ("index() is correct")
	{
		FAIL ("index() test is missing");
	}

	SECTION ("element() is correct")
	{
		FAIL ("element() test is missing");
	}

	SECTION ("operator value_type() is correct")
	{
		FAIL ("operator value_type() test is missing");
	}

	SECTION ("operator*() is correct")
	{
		FAIL ("operator*() test is missing");
	}

	SECTION ("operator->() is correct")
	{
		FAIL ("operator->() test is missing");
	}

}

