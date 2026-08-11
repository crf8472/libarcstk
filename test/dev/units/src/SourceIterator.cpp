#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for SourceIterator.
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


TEST_CASE ( "SourceIterator", "[sourceiterator] [verify] [verify]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::SourceIterator;

	auto instance = SourceIterator {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const SourceIterator copied { Copy<SourceIterator>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const SourceIterator moved { Move<SourceIterator>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = SourceIterator {};
		Copy<SourceIterator>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = SourceIterator {};
		Move<SourceIterator>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "SourceIterator default constructed instance",
		"[sourceiterator] [verify] [verify]" )
{
	const auto defaulted = arcstk::SourceIterator{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "SourceIterator property", "[sourceiterator] [verify] [verify]" )
{
	using arcstk::SourceIterator;

	auto instance = SourceIterator {};


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


TEST_CASE ( "SourceIterator functions", "[sourceiterator] [verify] [verify]" )
{
	using arcstk::SourceIterator;

	auto instance = SourceIterator {};

	SECTION ("SourceIterator() is correct")
	{
		FAIL ("SourceIterator() test is missing");
	}

	SECTION ("counter() is correct")
	{
		FAIL ("counter() test is missing");
	}

	SECTION ("current() is correct")
	{
		FAIL ("current() test is missing");
	}

	SECTION ("operator*() is correct")
	{
		FAIL ("operator*() test is missing");
	}

	SECTION ("operator->() is correct")
	{
		FAIL ("operator->() test is missing");
	}

	SECTION ("operator++() is correct")
	{
		FAIL ("operator++() test is missing");
	}

}

