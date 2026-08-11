#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for SampleSequence.
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


TEST_CASE ( "SampleSequence", "[samplesequence] [calc] [samples]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::SampleSequence;

	auto instance = SampleSequence {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const SampleSequence copied { Copy<SampleSequence>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const SampleSequence moved { Move<SampleSequence>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = SampleSequence {};
		Copy<SampleSequence>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = SampleSequence {};
		Move<SampleSequence>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "SampleSequence default constructed instance",
		"[samplesequence] [calc] [samples]" )
{
	const auto defaulted = arcstk::SampleSequence{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "SampleSequence property", "[samplesequence] [calc] [samples]" )
{
	using arcstk::SampleSequence;

	auto instance = SampleSequence {};


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


TEST_CASE ( "SampleSequence functions", "[samplesequence] [calc] [samples]" )
{
	using arcstk::SampleSequence;

	auto instance = SampleSequence {};

}

