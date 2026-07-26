#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for UpdateableSubtotals.
 */

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#define LIBARCSTK_ALGORITHMS_HPP_ // allow accuraterip.hpp
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"        // TO BE TESTED
#endif

#include <vector>                 // for vector

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "UpdateableSubtotals",
		"[updateablesubtotals] [calc] [accuraterip]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::accuraterip::details::UpdateableSubtotals;
	using arcstk::checksum::type;

	using UpdateableSubtotalsForV1andV2 =
		UpdateableSubtotals<type::ARCS1,type::ARCS2>;

	auto instance = UpdateableSubtotalsForV1andV2 {};


	// SECTION ("Parametized construction is correct")
	// {
	// 	FAIL ( "Parametized construction test is missing" );
	// }

	SECTION ("Copy construction is correct")
	{
		instance.set_multiplier(49);
		REQUIRE (instance.multiplier() == 49);

		const UpdateableSubtotals copied {
			Copy<UpdateableSubtotalsForV1andV2>::construct(instance) };

		// --

        CHECK(copied.multiplier() == instance.multiplier());
	}

	SECTION ("Move construction is correct")
	{
		instance.set_multiplier(127);
		REQUIRE (instance.multiplier() == 127);

		const UpdateableSubtotals moved {
			Move<UpdateableSubtotalsForV1andV2>::construct(instance) };

		// --

        CHECK(moved.multiplier() == 127);
	}

	SECTION ("Copy assignment is correct")
	{
		instance.set_multiplier(91);
		REQUIRE (instance.multiplier() == 91);

		auto copied = UpdateableSubtotalsForV1andV2 {};
		Copy<UpdateableSubtotalsForV1andV2>::assign(copied, instance);

		// --

        CHECK(copied.multiplier() == instance.multiplier());
	}

	SECTION ("Move assignment is correct")
	{
		instance.set_multiplier(23);
		REQUIRE (instance.multiplier() == 23);

		auto moved = UpdateableSubtotalsForV1andV2 {};
		Move<UpdateableSubtotalsForV1andV2>::assign(moved, instance);

		// --

        CHECK(moved.multiplier() == 23);
	}
}


TEST_CASE ( "UpdateableSubtotals property",
		"[updateablesubtotals] [calc] [accuraterip]" )
{
	using arcstk::accuraterip::details::UpdateableSubtotals;
	using arcstk::checksum::type;

	using std::cbegin;
	using std::cend;

	const std::vector<uint32_t> data { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

	auto u1 = UpdateableSubtotals<type::ARCS1,type::ARCS2> {};
	REQUIRE (u1.multiplier() == 1);

	u1.update(cbegin(data), cend(data));

	auto u2 = UpdateableSubtotals<type::ARCS1,type::ARCS2> {};

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

	SECTION ("swap() is correct")
	{
		REQUIRE ( u1.value().get(type::ARCS2).first.value() == 650 );
		REQUIRE ( u2.value().get(type::ARCS2).first.value() ==   0 );

		using std::swap;
		swap(u1, u2);

		// --

		// TODO More CHECKs
		CHECK ( u1.value().get(type::ARCS2).first.value() ==   0 );
		CHECK ( u2.value().get(type::ARCS2).first.value() == 650 );
	}

	// SECTION ("to_string() is correct")
	// {
	// 	FAIL ( "to_string() test is missing" );
	// }
	//
	// SECTION ("clone() is correct")
	// {
	// 	FAIL ( "clone() test is missing" );
	// }
	//
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


// TODO Implement tests for functions
// TEST_CASE ( "UpdateableSubtotals functions",
// 		"[updateablesubtotals] [calc] [accuraterip]" )
// {
// 	using arcstk::accuraterip::details::UpdateableSubtotals;
// 	using arcstk::checksum::type;
//
// 	auto instance = UpdateableSubtotals<type::ARCS1,type::ARCS2> {};
//
//
// 	SECTION ("multiplier() is correct")
// 	{
// 		FAIL ( "multiplier() test is missing" );
// 	}
//
// 	SECTION ("set_multiplier() is correct")
// 	{
// 		FAIL ( "set_multiplier() test is missing" );
// 	}
//
// 	SECTION ("update<B,E>() is correct")
// 	{
// 		FAIL ( "update<B,E>() test is missing" );
// 	}
//
// 	SECTION ("value() is correct")
// 	{
// 		FAIL ( "value() test is missing" );
// 	}
//
// 	SECTION ("reset() is correct")
// 	{
// 		FAIL ( "reset() test is missing" );
// 	}
//
// 	SECTION ("id_string() is correct")
// 	{
// 		FAIL ( "id_string() test is missing" );
// 	}
//
// 	SECTION ("types() is correct")
// 	{
// 		FAIL ( "types() test is missing" );
// 	}
// }

