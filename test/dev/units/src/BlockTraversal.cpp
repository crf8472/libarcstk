#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for BlockTraversal.
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


TEST_CASE ( "BlockTraversal", "[blocktraversal] [verify] [verify]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::BlockTraversal;

	auto instance = BlockTraversal {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const BlockTraversal copied { Copy<BlockTraversal>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const BlockTraversal moved { Move<BlockTraversal>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = BlockTraversal {};
		Copy<BlockTraversal>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = BlockTraversal {};
		Move<BlockTraversal>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "BlockTraversal default constructed instance",
		"[blocktraversal] [verify] [verify]" )
{
	const auto defaulted = arcstk::BlockTraversal{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "BlockTraversal property", "[blocktraversal] [verify] [verify]" )
{
	using arcstk::BlockTraversal;

	auto instance = BlockTraversal {};


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


TEST_CASE ( "BlockTraversal functions", "[blocktraversal] [verify] [verify]" )
{
	using arcstk::BlockTraversal;

	auto instance = BlockTraversal {};

	SECTION ("create_track_policy() is correct")
	{
		FAIL ("create_track_policy() test is missing");
	}

	SECTION ("create_selector() is correct")
	{
		FAIL ("create_selector() test is missing");
	}

	SECTION ("do_current_block() is correct")
	{
		FAIL ("do_current_block() test is missing");
	}

	SECTION ("do_current_track() is correct")
	{
		FAIL ("do_current_track() test is missing");
	}

	SECTION ("do_end_current() is correct")
	{
		FAIL ("do_end_current() test is missing");
	}

	SECTION ("do_end_counter() is correct")
	{
		FAIL ("do_end_counter() test is missing");
	}

	SECTION ("BlockTraversal() is correct")
	{
		FAIL ("BlockTraversal() test is missing");
	}

}

