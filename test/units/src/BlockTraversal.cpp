#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for BlockTraversal.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"
#endif
#ifndef LIBARCSTK_VERIFY_DETAILS_HPP_
#include "verify_details.hpp"     // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


// TEST_CASE ( "BlockTraversal", "[blocktraversal][verify]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::BlockTraversal;
//
// 	auto instance = BlockTraversal {};
//
//
// 	SECTION ("Parametized construction is correct")
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ("Copy construction is correct")
// 	{
// 		const BlockTraversal copied { Copy<BlockTraversal>::construct(instance) };
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy construction test is missing" );
// 	}
//
// 	SECTION ("Move construction is correct")
// 	{
// 		const BlockTraversal moved { Move<BlockTraversal>::construct(instance) };
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move construction test is missing" );
// 	}
//
// 	SECTION ("Copy assignment is correct")
// 	{
// 		auto copied = BlockTraversal {};
// 		Copy<BlockTraversal>::assign(copied, instance);
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy assignment test is missing" );
// 	}
//
// 	SECTION ("Move assignment is correct")
// 	{
// 		auto moved = BlockTraversal {};
// 		Move<BlockTraversal>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move assignment test is missing" );
// 	}
// }


TEST_CASE ( "BlockTraversal default constructed instance",
		"[blocktraversal][verify]" )
{
	const auto defaulted = arcstk::details::BlockTraversal {};

	SECTION ( "begin() throws")
	{
		CHECK_THROWS ( defaulted.begin(0) );
	}

	SECTION ( "end() throws")
	{
		CHECK_THROWS ( defaulted.end(0) );
	}

	SECTION ( "Creates a strict VerificationPolicy")
	{
		CHECK ( defaulted.get_policy()->is_strict() );
	}

	SECTION ( "Creates a Selector")
	{
		CHECK ( defaulted.get_selector() != nullptr );
	}

	// SECTION ( "current_block() is 0")
	// {
	// 	auto b = defaulted.begin(0);
	//
	// 	CHECK ( defaulted.current_block(b) == 0 );
	// }
	//
	// SECTION ( "current_track() is 0")
	// {
	// 	auto b = defaulted.begin(0);
	//
	// 	CHECK ( defaulted.current_track(b) == 0 );
	// }
	//
	// SECTION ( "end_current() is 0")
	// {
	// 	CHECK ( defaulted.end_current(0) == 0 );
	// }
	//
	// SECTION ( "end_counter() is 0")
	// {
	// 	CHECK ( defaulted.end_counter(0) == 0 );
	// }
}


// TEST_CASE ( "BlockTraversal property", "[blocktraversal][verify]" )
// {
// 	using arcstk::BlockTraversal;
//
// 	auto instance = BlockTraversal {};
//
//
// 	SECTION ("Equality operator == is correct")
// 	{
// 		FAIL ( "Equality operator test is missing" );
// 	}
//
// 	SECTION ("Stream-in operator << is correct")
// 	{
// 		FAIL ( "Stream-in operator << test is missing" );
// 	}
//
// 	SECTION ("operator bool() is correct")
// 	{
// 		FAIL ( "operator bool() test is missing" );
// 	}
//
// 	SECTION ("swap() is correct")
// 	{
// 		FAIL ( "swap() test is missing" );
// 	}
//
// 	SECTION ("to_string() is correct")
// 	{
// 		FAIL ( "to_string() test is missing" );
// 	}
//
// 	SECTION ("clone() is correct")
// 	{
// 		FAIL ( "clone() test is missing" );
// 	}
//
// 	SECTION ("size() is correct")
// 	{
// 		FAIL ( "size() test is missing" );
// 	}
//
// 	SECTION ("empty() is correct")
// 	{
// 		FAIL ( "empty() test is missing" );
// 	}
// }


// TEST_CASE ( "BlockTraversal functions", "[blocktraversal][verify]" )
// {
// 	using arcstk::BlockTraversal;
//
// 	auto instance = BlockTraversal {};
//
// 	// TODO Test from TraversalPolicy:
//
// 	SECTION ("set_source() is correct")
// 	{
// 		FAIL ("set_source() test is missing");
// 	}
//
// 	SECTION ("current() is correct")
// 	{
// 		FAIL ("current() test is missing");
// 	}
//
// 	SECTION ("set_current() is correct")
// 	{
// 		FAIL ("set_current() test is missing");
// 	}
//
// 	SECTION ("end_current() is correct")
// 	{
// 		FAIL ("end_current() test is missing");
// 	}
//
// 	SECTION ("end_counter() is correct")
// 	{
// 		FAIL ("end_counter() test is missing");
// 	}
//
// 	SECTION ("get_policy() is correct")
// 	{
// 		FAIL ("get_policy() test is missing");
// 	}
//
// 	SECTION ("get_selector() is correct")
// 	{
// 		FAIL ("get_selector() test is missing");
// 	}
//
// 	// begin()
// 	// end()
//
// 	SECTION ("create_track_policy() is correct")
// 	{
// 		FAIL ("create_track_policy() test is missing");
// 	}
//
// 	SECTION ("create_selector() is correct")
// 	{
// 		FAIL ("create_selector() test is missing");
// 	}
//
// 	SECTION ("current_block() is correct")
// 	{
// 		FAIL ("do_current_block() test is missing");
// 	}
//
// 	SECTION ("current_track() is correct")
// 	{
// 		FAIL ("do_current_track() test is missing");
// 	}
//
// 	SECTION ("end_current() is correct")
// 	{
// 		FAIL ("do_end_current() test is missing");
// 	}
//
// 	SECTION ("end_counter() is correct")
// 	{
// 		FAIL ("do_end_counter() test is missing");
// 	}
// }

