#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ${CLASS}.
 */

#ifndef LIBARCSTK_${HEADER:u}_HPP_
#include "${HEADER}.hpp"           // TO BE TESTED
#endif
#ifndef LIBARCSTK_${HEADER:u}_DETAILS_HPP_
#include "${HEADER}_details.hpp"   // TO BE TESTED
#endif

// testing

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"        // for Copy, Move
#endif

#include <sstream>                 // for ostringstream


// get an instance of this class (== how this class can be constructed)
TEST_CASE ( "${CLASS}", "[${CLASS:l}][${MODULE}][${HEADER}]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::${CLASS};

	auto instance = ${CLASS} {};


	SECTION ( "can be constructed with  parameters correctly" )
	{
		//auto object = ${CLASS} { };

		//CHECK ( object == );

		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ( "can be copy-constructied correctly" )
	{
		const ${CLASS} copied { Copy<${CLASS}>::construct(instance) };

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );

		FAIL ( "Copy construction test is missing" );
	}

	SECTION ( "can be move-constructied correctly" )
	{
		const ${CLASS} moved { Move<${CLASS}>::construct(instance) };

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );

		FAIL ( "Move construction test is missing" );
	}

	SECTION ( "can be copy-assigned correctly" )
	{
		auto copied = ${CLASS} {};
		Copy<${CLASS}>::assign(copied, instance);

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );

		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ( "can be move-assigned correctly" )
	{
		auto moved = ${CLASS} {};
		Move<${CLASS}>::assign(moved, instance);

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );

		FAIL ( "Move assignment test is missing" );
	}
}


// guarantees on a default constructed instance (== state)
TEST_CASE ( "${CLASS} default", "[${CLASS:l}][${MODULE}][${HEADER:l}]" )
{
	const auto defaulted = arcstk::${CLASS} {};

	SECTION ( "is empty()" )
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE" )
	{
		CHECK ( !defaulted );
	}
}


// capabilities of this class as a value (== what every class like this does)
TEST_CASE ( "${CLASS} value", "[${CLASS:l}][${MODULE}][${HEADER}]" )
{
	using arcstk::${CLASS};

	auto instance = ${CLASS} {};


	SECTION ( "swap()s correctly" )
	{
		auto rhs = ${CLASS} {};

		using std::swap;
		swap(instance, rhs);

		//CHECK ( instance == );

		FAIL ( "swap() test is missing" );
	}

	SECTION ( "converts to bool() correctly" )
	{
		//CHECK ( instance == true );

		FAIL ( "operator bool() test is missing" );
	}

	SECTION ( "can be compared for equality correctly" )
	{
		auto rhs = ${CLASS} {};

		//CHECK ( instance.equals(rhs) );
		//CHECK ( instance == rhs );

		FAIL ( "equals() test is missing" );
	}

	SECTION ( "reports its size() correctly" )
	{
		//CHECK ( instance.size() == 23 );

		FAIL ( "size() test is missing" );
	}

	SECTION ( "knows correctly whether it is empty()" )
	{
		//CHECK ( instance.empty() );

		FAIL ( "empty() test is missing" );
	}

	SECTION ( "converts to_string() correctly" )
	{
		//CHECK ( instance.to_string() == "instance" );

		FAIL ( "to_string() test is missing" );
	}

	SECTION ( "can be written to a stream correctly" )
	{
		auto stream = std::ostringstream {};
		stream << instance;

		//CHECK ( stream.str() == "instance" );

		FAIL ( "Stream-in operator << test is missing" );
	}

	SECTION ( "can be clone()d correctly" )
	{
		const auto clone = instance.clone();

		//CHECK ( clone->foo() == bar );

		FAIL ( "clone() test is missing" );
	}
}


// public members specific for this class (== what this class does)
TEST_CASE ( "${CLASS} behavior", "[${CLASS:l}][${MODULE}][${HEADER}]" )
{
	using arcstk::${CLASS};

	auto instance = ${CLASS} {};

	$<SECTIONS>
}

