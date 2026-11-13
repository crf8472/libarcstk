#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for SourceIterator.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"
#endif
#ifndef LIBARCSTK_VERIFY_DETAILS_HPP_
#include "verify_details.hpp"      // for SourceIterator
#endif

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"                // for DBAR
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"        // for Copy, Move
#endif


// TEST_CASE ( "SourceIterator", "[sourceiterator][verify][details]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::SourceIterator;
//
// 	auto instance = SourceIterator {};
//
//
// 	SECTION ( "Parametized construction is correct" )
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ( "Copy construction is correct" )
// 	{
// 		const SourceIterator copied { Copy<SourceIterator>::construct(instance) };
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy construction test is missing" );
// 	}
//
// 	SECTION ( "Move construction is correct" )
// 	{
// 		const SourceIterator moved { Move<SourceIterator>::construct(instance) };
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move construction test is missing" );
// 	}
//
// 	SECTION ( "Copy assignment is correct" )
// 	{
// 		auto copied = SourceIterator {};
// 		Copy<SourceIterator>::assign(copied, instance);
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy assignment test is missing" );
// 	}
//
// 	SECTION ( "Move assignment is correct" )
// 	{
// 		auto moved = SourceIterator {};
// 		Move<SourceIterator>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move assignment test is missing" );
// 	}
// }


// TEST_CASE ( "SourceIterator default constructed instance",
// 		"[sourceiterator][verify][details]" )
// {
// 	SECTION ( "converts to FALSE" )
// 	{
// 		CHECK ( !defaulted );
// 	}
// }


TEST_CASE ( "SourceIterator property", "[sourceiterator][verify][details]" )
{
	using arcstk::DBAR;
	using arcstk::DBARSource;

	using arcstk::details::BlockSelector;

	const auto dbar = DBAR {
		{ // pair
			{ 2, 5, 6, 7 }, // tuple
			{
				{ 1, 1, 5 },
				{ 2, 2, 6 }
			}
		},
		{ // pair
			{ 2, 5, 6, 7 }, // tuple
			{
				{ 3, 3, 7 },
				{ 4, 4, 8 }
			}
		}
	};

	const auto source = DBARSource { &dbar };
	auto selector = BlockSelector {};

	auto instance = arcstk::details::SourceIterator { source, 0, 0, selector };


	// SECTION ( "Equality operator == is correct" )
	// {
	// 	FAIL ( "Equality operator test is missing" );
	// }
	//
	// SECTION ( "Stream-in operator << is correct" )
	// {
	// 	FAIL ( "Stream-in operator << test is missing" );
	// }
	//
	// SECTION ( "operator bool() is correct" )
	// {
	// 	FAIL ( "operator bool() test is missing" );
	// }

	SECTION ( "swap() is correct" )
	{
		using arcstk::details::TrackSelector;
		auto selector2 = TrackSelector {};

		auto instance2 = arcstk::details::SourceIterator
			{ source, 1, 1, selector2 };

		CHECK ( instance.counter()  == 0 );
		CHECK ( instance.current()  == 0 );
		CHECK ( instance2.counter() == 1 );
		CHECK ( instance2.current() == 1 );

		swap ( instance, instance2 );

		CHECK ( instance.counter()  == 1 );
		CHECK ( instance.current()  == 1 );
		CHECK ( instance2.counter() == 0 );
		CHECK ( instance2.current() == 0 );
	}

	// SECTION ( "to_string() is correct" )
	// {
	// 	FAIL ( "to_string() test is missing" );
	// }
	//
	// SECTION ( "clone() is correct" )
	// {
	// 	FAIL ( "clone() test is missing" );
	// }

	// SECTION ( "size() is correct" )
	// {
	// 	FAIL ( "size() test is missing" );
	// }
	//
	// SECTION ( "empty() is correct" )
	// {
	// 	FAIL ( "empty() test is missing" );
	// }
}


// TEST_CASE ( "SourceIterator functions", "[sourceiterator][verify][details]" )
// {
// 	using arcstk::details::SourceIterator;
//
// 	auto instance = SourceIterator {};
//
// 	SECTION ( "SourceIterator() is correct" )
// 	{
// 		FAIL ("SourceIterator() test is missing");
// 	}
//
// 	SECTION ( "counter() is correct" )
// 	{
// 		FAIL ("counter() test is missing");
// 	}
//
// 	SECTION ( "current() is correct" )
// 	{
// 		FAIL ("current() test is missing");
// 	}
//
// 	SECTION ( "operator*() is correct" )
// 	{
// 		FAIL ("operator*() test is missing");
// 	}
//
// 	SECTION ( "operator->() is correct" )
// 	{
// 		FAIL ("operator->() test is missing");
// 	}
//
// 	SECTION ( "operator++() is correct" )
// 	{
// 		FAIL ("operator++() test is missing");
// 	}
//
// }

