#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for LenientPolicy.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"
#endif
#ifndef LIBARCSTK_VERIFY_DETAILS_HPP_
#include "verify_details.hpp"      // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"        // for Copy, Move
#endif


// TEST_CASE ( "LenientPolicy", "[lenientpolicy][verify][details]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::LenientPolicy;
//
// 	auto instance = LenientPolicy {};
//
//
// 	SECTION ( "Parametized construction is correct" )
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ( "Copy construction is correct" )
// 	{
// 		const LenientPolicy copied { Copy<LenientPolicy>::construct(instance) };
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
// 		const LenientPolicy moved { Move<LenientPolicy>::construct(instance) };
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
// 		auto copied = LenientPolicy {};
// 		Copy<LenientPolicy>::assign(copied, instance);
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
// 		auto moved = LenientPolicy {};
// 		Move<LenientPolicy>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move assignment test is missing" );
// 	}
// }


TEST_CASE ( "LenientPolicy default constructed instance",
	"[lenientpolicy][verify][details]" )
{
	const auto defaulted  = arcstk::details::LenientPolicy {};

	SECTION ( "not is_strict()" )
	{
		CHECK ( ! defaulted.is_strict() );
	}

	SECTION ( "is a VerificationPolicy" )
	{
		const arcstk::details::VerificationPolicy* v = &defaulted;

		CHECK ( v );
	}
}


TEST_CASE ( "LenientPolicy property", "[lenientpolicy][verify][details]" )
{
	auto instance = arcstk::details::LenientPolicy {};


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
	//
	// SECTION ( "swap() is correct" )
	// {
	// 	FAIL ( "swap() test is missing" );
	// }
	//
	// SECTION ( "to_string() is correct" )
	// {
	// 	FAIL ( "to_string() test is missing" );
	// }

	SECTION ( "clone() is correct" )
	{
		const auto cloned = instance.clone();

		CHECK ( ! cloned->is_strict() );
	}

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


TEST_CASE ( "LenientPolicy functions", "[lenientpolicy][verify][details]" )
{
	auto instance = arcstk::details::LenientPolicy {};

// 	SECTION ( "is_verified() is correct" )
// 	{
// 		FAIL ("is_verified() test is missing");
// 	}
//
// 	SECTION ( "total_unverified_tracks() is correct" )
// 	{
// 		FAIL ("total_unverified_tracks() test is missing");
// 	}

	SECTION ( "is_strict() is correct" )
	{
		CHECK ( ! instance.is_strict() );
	}
}

