#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ${CLASS}.
 */

#ifndef LIBARCSTK_${HEADER:u}_HPP_
#include "${HEADER}.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_${HEADER:u}_HPP_
#include "${HEADER}_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "${CLASS}", "[${CLASS:l}] [${MODULE}] [${HEADER}]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::${CLASS};

	auto instance = ${CLASS} {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const ${CLASS} copied { Copy<${CLASS}>::construct(instance) };

		// --

        //CHECK(copied.value() == instance.value());
        //CHECK(copied.name()  == instance.name());
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const ${CLASS} moved { Move<${CLASS}>::construct(instance) };

		// --

        //CHECK(moved.value() == instance.value());
        //CHECK(moved.name()  == instance.name());
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied ${CLASS} {};
		Copy<${CLASS}>::assign(copied, instance);

		// --

        //CHECK(copied.value() == instance.value());
        //CHECK(copied.name()  == instance.name());
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved ${CLASS} {};
		Move<${CLASS}>::assign(moved, instance);

		// --

        //CHECK(moved.value() == instance.value());
        //CHECK(moved.name()  == instance.name());
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "${CLASS} default constructed instance",
		"[${CLASS:l}] [${MODULE}] [${HEADER:l}]" )
{
	const auto instance = arcstk::${CLASS}{};

	SECTION ( "is empty()")
	{
		CHECK ( instance.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !instance );
	}
}


TEST_CASE ( "${CLASS} property", "[${CLASS:l}] [${MODULE}] [${HEADER}]" )
{
	using arcstk::${CLASS};

	auto defaulted = ${CLASS} {};


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


TEST_CASE ( "${CLASS} functions", "[${CLASS:l}] [${MODULE}] [${HEADER}]" )
{
	using arcstk::${CLASS};

	auto instance = ${CLASS} {};

	$<SECTIONS>
}

