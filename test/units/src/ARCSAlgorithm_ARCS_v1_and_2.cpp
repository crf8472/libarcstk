#include "algorithm.hpp"
#include "catch2/catch_test_macros.hpp"
#include "checksum.hpp"

/**
 * \file
 *
 * \brief Unit tests for ARCSAlgorithm<ARCS1,ARCS2>.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"        // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "ARCSAlgorithm<ARCS1,ARCS2>",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::checksum::type;
	using arcstk::ChecksumtypeSet;
	using arcstk::Context;

	using ARv12 = arcstk::accuraterip::algorithm::Versions1and2;

	auto instance = ARv12 {};

	REQUIRE ( instance.context() == Context::ALBUM );
	REQUIRE ( instance.types()   ==
					ChecksumtypeSet { type::ARCS1, type::ARCS2 } );


	// SECTION ("Parametized construction is correct")
	// {
	// 	FAIL ( "Parametized construction test is missing" );
	// }

	SECTION ("Copy construction is correct")
	{
		// Change Context to have a different property
		instance.set_context(Context::FIRST_TRACK);
		REQUIRE ( instance.context() == Context::FIRST_TRACK );

		const ARv12 copied { Copy<ARv12>::construct(instance) };

		// --

        CHECK ( copied.context() == Context::FIRST_TRACK );
	}

	SECTION ("Move construction is correct")
	{
		// Change Context to have a different property
		instance.set_context(Context::TRACK);
		REQUIRE ( instance.context() == Context::TRACK );

		const ARv12 moved { Move<ARv12>::construct(instance) };

		// --

        CHECK ( moved.context() == Context::TRACK );
	}

	SECTION ("Copy assignment is correct")
	{
		// Change Context to have a different property
		instance.set_context(Context::FIRST_TRACK);
		REQUIRE ( instance.context() == Context::FIRST_TRACK );

		auto copied = ARv12 {};
		Copy<ARv12>::assign(copied, instance);

		// --

        CHECK ( copied.context() == Context::FIRST_TRACK );
	}

	SECTION ("Move assignment is correct")
	{
		// Change Context to have a different property
		instance.set_context(Context::TRACK);
		REQUIRE ( instance.context() == Context::TRACK );

		auto moved = ARv12 {};
		Move<ARv12>::assign(moved, instance);

		// --

        CHECK ( moved.context() == Context::TRACK );
	}
}


TEST_CASE ( "ARCSAlgorithm<ARCS1,ARCS2> default constructed instance",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	using ARv12 = arcstk::accuraterip::algorithm::Versions1and2;

	const auto instance = ARv12 {};

	SECTION ( "is ok")
	{
		// just to call sth
		CHECK ( instance.name() == "AccurateRip v1+2" );
	}

	// SECTION ( "is empty()")
	// {
	// 	CHECK ( instance.empty() );
	// }
	//
	// SECTION ( "converts to FALSE")
	// {
	// 	CHECK ( !instance );
	// }
}


TEST_CASE ( "ARCSAlgorithm<ARCS1,ARCS2> property",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	using ARv12 = arcstk::accuraterip::algorithm::Versions1and2;

	auto defaulted = ARv12 {};


	// SECTION ("Equality operator == is correct")
	// {
	// 	FAIL ( "Equality operator test is missing" );
	// }
	//
	// SECTION ("Stream-in operator << is correct")
	// {
	// 	FAIL ( "Stream-in operator << test is missing" );
	// }
	//
	// SECTION ("operator bool() is correct")
	// {
	// 	FAIL ( "operator bool() test is missing" );
	// }

	// SECTION ("swap() is correct")
	// {
	// 	FAIL ( "swap() test is missing" );
	// }

	// SECTION ("to_string() is correct")
	// {
	// 	FAIL ( "to_string() test is missing" );
	// }

	// SECTION ("clone() is correct")
	// {
	// 	FAIL ( "clone() test is missing" );
	// }

	// SECTION ("size() is correct")
	// {
	// 	FAIL ( "size() test is missing" );
	// }
	//
	// SECTION ("empty() is correct")
	// {
	// 	FAIL ( "empty() test is missing" );
	// }
}


TEST_CASE ( "ARCSAlgorithm<ARCS1,ARCS2> functions",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	using arcstk::checksum::type;
	using arcstk::ChecksumtypeSet;
	using arcstk::Context;

	using ARv12 = arcstk::accuraterip::algorithm::Versions1and2;

	auto instance = ARv12 {};

	REQUIRE ( instance.context() == Context::ALBUM );


	SECTION ("set_context() is correct")
	{
		instance.set_context(Context::LAST_TRACK);

		CHECK ( instance.context() == Context::LAST_TRACK );
	}

	SECTION ("name() is correct")
	{
		CHECK ( instance.name() == "AccurateRip v1+2" );
	}

	SECTION ("types() is correct")
	{
		CHECK ( instance.types()   ==
					ChecksumtypeSet { type::ARCS1, type::ARCS2 } );
	}

	// SECTION ("range() is correct")
	// {
	// 	FAIL ("do_range test is missing");
	// }
	//
	// SECTION ("track_finished() is correct")
	// {
	// 	FAIL ("do_track_finished test is missing");
	// }
	//
	// SECTION ("result() is correct")
	// {
	// 	FAIL ("do_result test is missing");
	// }
	//
	// SECTION ("perform_update() is correct")
	// {
	// 	FAIL ("perform_update test is missing");
	// }
}

