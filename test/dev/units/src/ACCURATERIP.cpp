#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ACCURATERIP.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip_func.hpp"     // for
#endif

// #ifndef LIBARCSTK_CONSTRUCTION_HPP_
// #include "construction.hpp"       // for Copy, Move
// #endif


// TEST_CASE ( "ACCURATERIP", "[accuraterip] [calc] [accuraterip]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::ACCURATERIP;
//
// 	auto instance = ACCURATERIP {};
//
//
// 	SECTION ("Parametized construction is correct")
// 	{
// 	}
//
// 	SECTION ("Copy construction is correct")
// 	{
// 		const ACCURATERIP copied { Copy<ACCURATERIP>::construct(instance) };
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 	}
//
// 	SECTION ("Move construction is correct")
// 	{
// 		const ACCURATERIP moved { Move<ACCURATERIP>::construct(instance) };
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 	}
//
// 	SECTION ("Copy assignment is correct")
// 	{
// 		auto copied = ACCURATERIP {};
// 		Copy<ACCURATERIP>::assign(copied, instance);
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 	}
//
// 	SECTION ("Move assignment is correct")
// 	{
// 		auto moved = ACCURATERIP {};
// 		Move<ACCURATERIP>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 	}
// }


TEST_CASE ( "ACCURATERIP default constructed instance",
		"[accuraterip] [calc] [accuraterip]" )
{
	const auto defaulted = arcstk::ACCURATERIP{};

	SECTION ("url prefix has default value")
	{
		CHECK( defaulted.request_url_prefix() ==
				defaulted.default_request_url_prefix() );
	}
}


// TEST_CASE ( "ACCURATERIP property", "[accuraterip] [calc] [accuraterip]" )
// {
// 	using arcstk::ACCURATERIP;
//
// 	auto instance = ACCURATERIP {};
//
//
// 	SECTION ("Equality operator == is correct")
// 	{
// 	}
//
// 	SECTION ("Stream-in operator << is correct")
// 	{
// 	}
//
// 	SECTION ("operator bool() is correct")
// 	{
// 	}
//
// 	SECTION ("swap() is correct")
// 	{
// 	}
//
// 	SECTION ("to_string() is correct")
// 	{
// 	}
//
// 	SECTION ("clone() is correct")
// 	{
// 	}
// }


TEST_CASE ( "ACCURATERIP functions", "[accuraterip] [calc] [accuraterip]" )
{
	using arcstk::ACCURATERIP;

	// auto instance = ACCURATERIP {};

	SECTION ("request_url_prefix() has default value")
	{
		CHECK( ACCURATERIP::request_url_prefix() ==
				ACCURATERIP::default_request_url_prefix() );
	}

	SECTION ("default_request_url_prefix() is correct")
	{
		CHECK( "http://www.accuraterip.com/accuraterip/" ==
				ACCURATERIP::default_request_url_prefix() );
	}

	// SECTION ("set_request_url_prefix() is correct")
	// {
	// }
	//
	// SECTION ("reset_request_url_prefix() is correct")
	// {
	// }

	SECTION ("default_arcs_format() is correct")
	{
		CHECK( ACCURATERIP::default_arcs_format( 0) == "00000000" );
		CHECK( ACCURATERIP::default_arcs_format(16) == "00000010" );
		CHECK( ACCURATERIP::default_arcs_format(749321) == "000B6F09" );
		CHECK( ACCURATERIP::default_arcs_format(2059706353) == "7AC49FF1" );
	}
}

