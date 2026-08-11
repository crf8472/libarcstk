#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Updateable.
 */

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Updateable", "[updateable] [calc] [algorithm]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Updateable;

	auto instance = Updateable {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const Updateable copied { Copy<Updateable>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const Updateable moved { Move<Updateable>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Updateable {};
		Copy<Updateable>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Updateable {};
		Move<Updateable>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "Updateable default constructed instance",
		"[updateable] [calc] [algorithm]" )
{
	const auto defaulted = arcstk::Updateable{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "Updateable property", "[updateable] [calc] [algorithm]" )
{
	using arcstk::Updateable;

	auto instance = Updateable {};


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


TEST_CASE ( "Updateable functions", "[updateable] [calc] [algorithm]" )
{
	using arcstk::Updateable;

	auto instance = Updateable {};

	SECTION ("Updateable() is correct")
	{
		FAIL ("Updateable() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

	SECTION ("~Updateable() is correct")
	{
		FAIL ("~Updateable() test is missing");
	}

	SECTION ("algorithm() is correct")
	{
		FAIL ("algorithm() test is missing");
	}

	SECTION ("update() is correct")
	{
		FAIL ("update() test is missing");
	}

	SECTION ("finish_track() is correct")
	{
		FAIL ("finish_track() test is missing");
	}

	SECTION ("name() is correct")
	{
		FAIL ("name() test is missing");
	}

	SECTION ("types() is correct")
	{
		FAIL ("types() test is missing");
	}

	SECTION ("context() is correct")
	{
		FAIL ("context() test is missing");
	}

	SECTION ("range() is correct")
	{
		FAIL ("range() test is missing");
	}

	SECTION ("result() is correct")
	{
		FAIL ("result() test is missing");
	}

}

