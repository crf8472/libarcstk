#include "catch2/catch.hpp"

#include <cstdint>
#include <memory>

#ifndef __LIBARCS_CALCULATE_HPP__
#include "calculate.hpp"
#endif

#ifndef __LIBARCS_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcs
{

inline namespace v_1_0_0
{

#ifndef __LIBARCS_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif

} // namespace v_1_0_0

} // namespace arcs



/**
 * \file calculate.cpp Fixtures for classes in module calculate
 */


TEST_CASE ( "SampleBlock", "[calculate] [sampleblock]" )
{
	arcs::SampleBlock block(1000);

	REQUIRE ( block.size() == 1000 );
	REQUIRE ( block.capacity() == 1000 );


	SECTION ( "Iterator increment and decremtn is ok" )
	{
		// prefix increment
		int i = 0;
		for (auto it = block.begin(); it != block.end(); ++it, ++i)
		{
			// empty
		}

		REQUIRE ( i == 1000 );


		// postfix increment
		int j = 0;
		for (auto it = block.begin(); it != block.end(); it++, ++j)
		{
			// empty
		}

		REQUIRE ( j == 1000 );


		// prefix decrement
		int k = 0;
		for (auto it = block.end(); it != block.begin(); --it, ++k)
		{
			// empty
		}

		REQUIRE ( k == 1000 );


		// postfix decrement
		int m = 0;
		for (auto it = block.end(); it != block.begin(); it--, ++m)
		{
			// empty
		}

		REQUIRE ( m == 1000 );
	}


	SECTION ( "Assignment by iterator works and does not alter capacity" )
	{
		int i = 0;
		for (auto it = block.begin(); it != block.end(); ++it, ++i)
		{
			*it = (i % 2) ? i : 0; // keep odd numbers, otherwise set 0
		}

		REQUIRE ( i == 1000 );
		REQUIRE ( block.size() == 1000 );
		REQUIRE ( block.capacity() == 1000 );


		// verify odd numbers and zeros
		for (auto it = block.begin(); it != block.end(); ++it)
		{
			REQUIRE ( *it     % 2 == 0 );
			REQUIRE ( *(++it) % 2 == 1 );
		}

		REQUIRE ( block.size() == 1000 );
		REQUIRE ( block.capacity() == 1000 );


		int j = 0;
		for (auto it = block.begin(); it != block.end(); ++it, ++j)
		{
			*it = (j % 2) ? 0 : j; // keep even numbers, otherwise set 0
		}

		REQUIRE ( j == 1000 );
		REQUIRE ( block.size() == 1000 );
		REQUIRE ( block.capacity() == 1000 );


		// verify even numbers and zeros except for the first two positions
		// which are 0
		for (auto it = block.begin() + 2; it != block.end(); ++it)
		{
			REQUIRE ( *it         >  0 );
			REQUIRE ( *it     % 2 == 0 );
			REQUIRE ( *(++it)     == 0 );
		}

		REQUIRE ( block.size() == 1000 );
		REQUIRE ( block.capacity() == 1000 );
	}
}


TEST_CASE ( "Interval" "[calculate] [interval]" )
{
	arcs::Interval i(10, 20);

	REQUIRE ( i.contains(10) );
	REQUIRE ( i.contains(11) );
	REQUIRE ( i.contains(19) );
	REQUIRE ( i.contains(20) );
}

