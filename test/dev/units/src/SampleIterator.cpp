#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for SampleIterator.
 */

#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "SampleIterator", "[sampleiterator] [calc] [samples]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::SampleIterator;

	auto instance = SampleIterator {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const SampleIterator copied { Copy<SampleIterator>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const SampleIterator moved { Move<SampleIterator>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = SampleIterator {};
		Copy<SampleIterator>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = SampleIterator {};
		Move<SampleIterator>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "SampleIterator default constructed instance",
		"[sampleiterator] [calc] [samples]" )
{
	const auto defaulted = arcstk::SampleIterator{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "SampleIterator property", "[sampleiterator] [calc] [samples]" )
{
	using arcstk::SampleIterator;

	auto instance = SampleIterator {};


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


TEST_CASE ( "SampleIterator functions", "[sampleiterator] [calc] [samples]" )
{
	using arcstk::SampleIterator;

	auto instance = SampleIterator {};

	SECTION ("sequence() is correct")
	{
		FAIL ("sequence() test is missing");
	}

	SECTION ("pos() is correct")
	{
		FAIL ("pos() test is missing");
	}

	SECTION ("operator*() is correct")
	{
		FAIL ("operator*() test is missing");
	}

	SECTION ("operator++() is correct")
	{
		FAIL ("operator++() test is missing");
	}

	SECTION ("operator+=() is correct")
	{
		FAIL ("operator+=() test is missing");
	}

	SECTION ("SampleIterator() is correct")
	{
		FAIL ("SampleIterator() test is missing");
	}

	SECTION ("out_of_bounds() is correct")
	{
		FAIL ("out_of_bounds() test is missing");
	}

}

