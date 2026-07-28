#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for DBAR.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "DBAR", "[dbar] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::DBAR;

	auto instance = DBAR {
		// 0:
		{
			/* id */
			{ 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			/* triplets */
			{
				{ 0xB89992E5,  6, 0xC89192E5 },
				{ 0x4F77EB03,  8, 0xB9653BEA },
				{ 0x56582282,  7, 0x6952EC4A },
				{ 0x9E2187F9,  9, 0x1B113449 },
				{ 0x6BE71E50,  2, 0xC5567E10 },
				{ 0x01E7235F,  1, 0x86ECAA48 },
				{ 0xD8F7763C,  0, 0xC3F25BE1 },
				{ 0x8480223E, 13, 0xEBDB4CE2 },
				{ 0x42C5061C, 16, 0xE4BA8570 },
				{ 0x47A70F02, 17, 0x259A8119 },
				{ 0xBABF08CC, 18, 0x527A81E7 },
				{ 0x563EDCCB, 21, 0x0D2496E5 },
				{ 0xAB123C7C, 14, 0xACD78A48 },
				{ 0xC65C20E4, 26, 0x6BD26848 },
				{ 0x58FC3C3E, 28, 0x45A9F67E }
			}
		},
		// 1:
		{
			/* id */
			{ 15, 0x101B9178, 0xB14BE24E, 0x540C2D0A },
			/* triplets */
			{
				{ 0x98B10E0F,  2, 0x1E46272D },
				{ 0x475F57E9,  4, 0x69E4CC7F },
				{ 0x7304F1C4,  5, 0x30D6EF87 },
				{ 0xF2472287,  3, 0x5ACE649C },
				{ 0x881BC504,  7, 0x33FB14FF },
				{ 0xBB94BFD4,  9, 0x19FB154A },
				{ 0xF9CAEE76, 10, 0x61C45BE8 },
				{ 0xF9F60BC1, 11, 0xC19B218F },
				{ 0x2C736302, 12, 0x86A4648E },
				{ 0x1C955978, 13, 0x32704DE8 },
				{ 0xFDA6D833, 16, 0x751D3E13 },
				{ 0x3A57E5D1, 17, 0xB99D9A71 },
				{ 0x6ED5F3E7, 18, 0x342DEBEF },
				{ 0x4A5C3872, 21, 0x783537F2 },
				{ 0x5FE8B032, 24, 0x765EE0C8 }
			}
		}
	};

	SECTION ("Parametized construction is correct")
	{
		CHECK ( instance.size() == 2 );

		const auto block0 = instance.block(0);

		CHECK ( block0.header().total_tracks() == 15 );
		CHECK ( block0.header().id1() == 0x001B9178 );
		CHECK ( block0.header().id2() == 0x014BE24E );
		CHECK ( block0.header().cddb_id() == 0xB40D2D0F );
		CHECK ( block0.size() == 15 );

		const auto block1 = instance.block(1);

		CHECK ( block1.header().total_tracks() == 15 );
		CHECK ( block1.header().id1() == 0x101B9178 );
		CHECK ( block1.header().id2() == 0xB14BE24E );
		CHECK ( block1.header().cddb_id() == 0x540C2D0A );
		CHECK ( block1.size() == 15 );
	}

	SECTION ("Copy construction is correct")
	{
		const DBAR copied { Copy<DBAR>::construct(instance) };

		// --

		CHECK ( copied.size() == 2 );

		const auto block0 = copied.block(0);

		CHECK ( block0.header().total_tracks() == 15 );
		CHECK ( block0.header().id1() == 0x001B9178 );
		CHECK ( block0.header().id2() == 0x014BE24E );
		CHECK ( block0.header().cddb_id() == 0xB40D2D0F );
		CHECK ( block0.size() == 15 );

		CHECK ( block0.triplet(0).arcs()           == 0xB89992E5 );
		CHECK ( block0.triplet(0).confidence()     == 6 );
		CHECK ( block0.triplet(0).frame450_arcs()  == 0xC89192E5 );

		// TODO ...

		CHECK ( block0.triplet(14).arcs()          == 0x58FC3C3E );
		CHECK ( block0.triplet(14).confidence()    == 28 );
		CHECK ( block0.triplet(14).frame450_arcs() == 0x45A9F67E );

		const auto block1 = copied.block(1);

		CHECK ( block1.header().total_tracks() == 15 );
		CHECK ( block1.header().id1() == 0x101B9178 );
		CHECK ( block1.header().id2() == 0xB14BE24E );
		CHECK ( block1.header().cddb_id() == 0x540C2D0A );
		CHECK ( block1.size() == 15 );

		CHECK ( block1.triplet(0).arcs()           == 0x98B10E0F );
		CHECK ( block1.triplet(0).confidence()     == 2 );
		CHECK ( block1.triplet(0).frame450_arcs()  == 0x1E46272D );

		// TODO ...

		CHECK ( block1.triplet(14).arcs()          == 0x5FE8B032 );
		CHECK ( block1.triplet(14).confidence()    == 24 );
		CHECK ( block1.triplet(14).frame450_arcs() == 0x765EE0C8 );
	}

	SECTION ("Move construction is correct")
	{
		const DBAR moved { Move<DBAR>::construct(instance) };

		// --

		CHECK ( moved.size() == 2 );

		const auto block0 = moved.block(0);

		CHECK ( block0.header().total_tracks() == 15 );
		CHECK ( block0.header().id1() == 0x001B9178 );
		CHECK ( block0.header().id2() == 0x014BE24E );
		CHECK ( block0.header().cddb_id() == 0xB40D2D0F );
		CHECK ( block0.size() == 15 );

		CHECK ( block0.triplet(0).arcs()           == 0xB89992E5 );
		CHECK ( block0.triplet(0).confidence()     == 6 );
		CHECK ( block0.triplet(0).frame450_arcs()  == 0xC89192E5 );

		// TODO ...

		CHECK ( block0.triplet(14).arcs()          == 0x58FC3C3E );
		CHECK ( block0.triplet(14).confidence()    == 28 );
		CHECK ( block0.triplet(14).frame450_arcs() == 0x45A9F67E );

		const auto block1 = moved.block(1);

		CHECK ( block1.header().total_tracks() == 15 );
		CHECK ( block1.header().id1() == 0x101B9178 );
		CHECK ( block1.header().id2() == 0xB14BE24E );
		CHECK ( block1.header().cddb_id() == 0x540C2D0A );
		CHECK ( block1.size() == 15 );

		CHECK ( block1.triplet(0).arcs()           == 0x98B10E0F );
		CHECK ( block1.triplet(0).confidence()     == 2 );
		CHECK ( block1.triplet(0).frame450_arcs()  == 0x1E46272D );

		// TODO ...

		CHECK ( block1.triplet(14).arcs()          == 0x5FE8B032 );
		CHECK ( block1.triplet(14).confidence()    == 24 );
		CHECK ( block1.triplet(14).frame450_arcs() == 0x765EE0C8 );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = DBAR {};
		Copy<DBAR>::assign(copied, instance);

		// --

		CHECK ( copied.size() == 2 );

		const auto block0 = copied.block(0);

		CHECK ( block0.header().total_tracks() == 15 );
		CHECK ( block0.header().id1() == 0x001B9178 );
		CHECK ( block0.header().id2() == 0x014BE24E );
		CHECK ( block0.header().cddb_id() == 0xB40D2D0F );
		CHECK ( block0.size() == 15 );

		CHECK ( block0.triplet(0).arcs()           == 0xB89992E5 );
		CHECK ( block0.triplet(0).confidence()     == 6 );
		CHECK ( block0.triplet(0).frame450_arcs()  == 0xC89192E5 );

		// TODO ...

		CHECK ( block0.triplet(14).arcs()          == 0x58FC3C3E );
		CHECK ( block0.triplet(14).confidence()    == 28 );
		CHECK ( block0.triplet(14).frame450_arcs() == 0x45A9F67E );

		const auto block1 = copied.block(1);

		CHECK ( block1.header().total_tracks() == 15 );
		CHECK ( block1.header().id1() == 0x101B9178 );
		CHECK ( block1.header().id2() == 0xB14BE24E );
		CHECK ( block1.header().cddb_id() == 0x540C2D0A );
		CHECK ( block1.size() == 15 );

		CHECK ( block1.triplet(0).arcs()           == 0x98B10E0F );
		CHECK ( block1.triplet(0).confidence()     == 2 );
		CHECK ( block1.triplet(0).frame450_arcs()  == 0x1E46272D );

		// TODO ...

		CHECK ( block1.triplet(14).arcs()          == 0x5FE8B032 );
		CHECK ( block1.triplet(14).confidence()    == 24 );
		CHECK ( block1.triplet(14).frame450_arcs() == 0x765EE0C8 );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = DBAR {};
		Move<DBAR>::assign(moved, instance);

		// --

		CHECK ( moved.size() == 2 );

		const auto block0 = moved.block(0);

		CHECK ( block0.header().total_tracks() == 15 );
		CHECK ( block0.header().id1() == 0x001B9178 );
		CHECK ( block0.header().id2() == 0x014BE24E );
		CHECK ( block0.header().cddb_id() == 0xB40D2D0F );
		CHECK ( block0.size() == 15 );

		CHECK ( block0.triplet(0).arcs()           == 0xB89992E5 );
		CHECK ( block0.triplet(0).confidence()     == 6 );
		CHECK ( block0.triplet(0).frame450_arcs()  == 0xC89192E5 );

		// TODO ...

		CHECK ( block0.triplet(14).arcs()          == 0x58FC3C3E );
		CHECK ( block0.triplet(14).confidence()    == 28 );
		CHECK ( block0.triplet(14).frame450_arcs() == 0x45A9F67E );

		const auto block1 = moved.block(1);

		CHECK ( block1.header().total_tracks() == 15 );
		CHECK ( block1.header().id1() == 0x101B9178 );
		CHECK ( block1.header().id2() == 0xB14BE24E );
		CHECK ( block1.header().cddb_id() == 0x540C2D0A );
		CHECK ( block1.size() == 15 );

		CHECK ( block1.triplet(0).arcs()           == 0x98B10E0F );
		CHECK ( block1.triplet(0).confidence()     == 2 );
		CHECK ( block1.triplet(0).frame450_arcs()  == 0x1E46272D );

		// TODO ...

		CHECK ( block1.triplet(14).arcs()          == 0x5FE8B032 );
		CHECK ( block1.triplet(14).confidence()    == 24 );
		CHECK ( block1.triplet(14).frame450_arcs() == 0x765EE0C8 );
	}
}


TEST_CASE ( "DBAR default constructed instance",
		"[dbar] [dbar] [dbar]" )
{
	const auto instance = arcstk::DBAR{};

	SECTION ( "is empty()")
	{
		CHECK ( instance.empty() );
	}

	SECTION ( "has size() of 0")
	{
		CHECK ( instance.size() == 0 );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !instance );
	}
}


TEST_CASE ( "DBAR property", "[dbar] [dbar] [dbar]" )
{
	using arcstk::DBAR;

	auto defaulted = DBAR {};


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
	//
	// SECTION ("swap() is correct")
	// {
	// 	FAIL ( "swap() test is missing" );
	// }
	//
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


TEST_CASE ( "DBAR functions", "[dbar] [dbar] [dbar]" )
{
	using arcstk::DBAR;

	const auto instance = DBAR {
		// 0:
		{
			/* id */
			{ 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			/* triplets */
			{
				{ 0xB89992E5,  6, 0xC89192E5 },
				{ 0x4F77EB03,  8, 0xB9653BEA },
				{ 0x56582282,  7, 0x6952EC4A },
				{ 0x9E2187F9,  9, 0x1B113449 },
				{ 0x6BE71E50,  2, 0xC5567E10 },
				{ 0x01E7235F,  1, 0x86ECAA48 },
				{ 0xD8F7763C,  0, 0xC3F25BE1 },
				{ 0x8480223E, 13, 0xEBDB4CE2 },
				{ 0x42C5061C, 16, 0xE4BA8570 },
				{ 0x47A70F02, 17, 0x259A8119 },
				{ 0xBABF08CC, 18, 0x527A81E7 },
				{ 0x563EDCCB, 21, 0x0D2496E5 },
				{ 0xAB123C7C, 14, 0xACD78A48 },
				{ 0xC65C20E4, 26, 0x6BD26848 },
				{ 0x58FC3C3E, 28, 0x45A9F67E }
			}
		},
		// 1:
		{
			/* id */
			{ 15, 0x101B9178, 0xB14BE24E, 0x540C2D0A },
			/* triplets */
			{
				{ 0x98B10E0F,  2, 0x1E46272D },
				{ 0x475F57E9,  4, 0x69E4CC7F },
				{ 0x7304F1C4,  5, 0x30D6EF87 },
				{ 0xF2472287,  3, 0x5ACE649C },
				{ 0x881BC504,  7, 0x33FB14FF },
				{ 0xBB94BFD4,  9, 0x19FB154A },
				{ 0xF9CAEE76, 10, 0x61C45BE8 },
				{ 0xF9F60BC1, 11, 0xC19B218F },
				{ 0x2C736302, 12, 0x86A4648E },
				{ 0x1C955978, 13, 0x32704DE8 },
				{ 0xFDA6D833, 16, 0x751D3E13 },
				{ 0x3A57E5D1, 17, 0xB99D9A71 },
				{ 0x6ED5F3E7, 18, 0x342DEBEF },
				{ 0x4A5C3872, 21, 0x783537F2 },
				{ 0x5FE8B032, 24, 0x765EE0C8 }
			}
		}
	};

	REQUIRE ( instance.size() == 2 );


	// SECTION ("size is correct")
	// {
	// 	FAIL ("size test is missing");
	// }
	//
	// SECTION ("arcs_value is correct")
	// {
	// 	FAIL ("arcs_value test is missing");
	// }
	//
	// SECTION ("confidence_value is correct")
	// {
	// 	FAIL ("confidence_value test is missing");
	// }
	//
	// SECTION ("frame450_arcs_value is correct")
	// {
	// 	FAIL ("frame450_arcs_value test is missing");
	// }
	//
	// SECTION ("total_tracks is correct")
	// {
	// 	FAIL ("total_tracks test is missing");
	// }
	//
	// SECTION ("header is correct")
	// {
	// 	FAIL ("header test is missing");
	// }
	//
	// SECTION ("triplet is correct")
	// {
	// 	FAIL ("triplet test is missing");
	// }
	//
	// SECTION ("block is correct")
	// {
	// 	FAIL ("block test is missing");
	// }
	//
	// SECTION ("begin is correct")
	// {
	// 	FAIL ("begin test is missing");
	// }
	//
	// SECTION ("end is correct")
	// {
	// 	FAIL ("end test is missing");
	// }
	//
	// SECTION ("cbegin is correct")
	// {
	// 	FAIL ("cbegin test is missing");
	// }
	//
	// SECTION ("cend is correct")
	// {
	// 	FAIL ("cend test is missing");
	// }
	//
	// SECTION ("rbegin is correct")
	// {
	// 	FAIL ("rbegin test is missing");
	// }
	//
	// SECTION ("rend is correct")
	// {
	// 	FAIL ("rend test is missing");
	// }
	//
	// SECTION ("crbegin is correct")
	// {
	// 	FAIL ("crbegin test is missing");
	// }
	//
	// SECTION ("crend is correct")
	// {
	// 	FAIL ("crend test is missing");
	// }
	//
	// SECTION ("empty is correct")
	// {
	// 	FAIL ("empty test is missing");
	// }
	//
	// SECTION ("operator bool is correct")
	// {
	// 	FAIL ("operator bool test is missing");
	// }
	//
	// SECTION ("swap is correct")
	// {
	// 	FAIL ("swap test is missing");
	// }
	//
	// SECTION ("equals is correct")
	// {
	// 	FAIL ("equals test is missing");
	// }

	SECTION ( "Can be forward-iterated correctly" )
	{
		using std::cbegin;

		auto it { cbegin(instance) };

		CHECK ( it->header().total_tracks() == 15 );
		CHECK ( it->header().id1() == 0x001B9178 );
		CHECK ( it->header().id2() == 0x014BE24E );
		CHECK ( it->header().cddb_id() == 0xB40D2D0F );

		CHECK ( it->size() == 15 );
		CHECK ( it->triplet(0).arcs() == 0xB89992E5 );
		CHECK ( it->triplet(0).confidence() == 6 );
		CHECK ( it->triplet(0).frame450_arcs() == 0xC89192E5 );

		++it;

		CHECK ( it->header().total_tracks() == 15 );
		CHECK ( it->header().id1() == 0x101B9178 );
		CHECK ( it->header().id2() == 0xB14BE24E );
		CHECK ( it->header().cddb_id() == 0x540C2D0A );

		CHECK ( it->triplet(0).arcs() == 0x98B10E0F );
		CHECK ( it->triplet(0).confidence() == 2 );
		CHECK ( it->triplet(0).frame450_arcs() == 0x1E46272D );

		CHECK ( (*it).header().total_tracks() == 15 );
	}

	SECTION ( "End iterators work correctly" )
	{
		using std::cbegin;
		using std::cend;

		using std::begin;
		using std::end;

		auto dbar_cend { cend(instance)   };
		auto dbar_pos  { cbegin(instance) };

		CHECK ( end(instance) == dbar_cend );

		// Add complete size to begin
		for (auto i = DBAR::size_type {0}; i < instance.size(); ++i) { ++dbar_pos; }

		CHECK ( dbar_pos == dbar_cend );
	}

	SECTION ( "Can be reverse forward-iterated correctly" )
	{
		using std::crbegin;
		using std::crend;

		auto it { crbegin(instance) };

		CHECK ( it->header().total_tracks() == 15 );
		CHECK ( it->header().id1() == 0x101B9178 );
		CHECK ( it->header().id2() == 0xB14BE24E );
		CHECK ( it->header().cddb_id() == 0x540C2D0A );

		CHECK ( it->triplet(0).arcs() == 0x98B10E0F );
		CHECK ( it->triplet(0).confidence() == 2 );
		CHECK ( it->triplet(0).frame450_arcs() == 0x1E46272D );

		CHECK ( (*it).header().total_tracks() == 15 );

		++it;

		CHECK ( it->header().total_tracks() == 15 );
		CHECK ( it->header().id1() == 0x001B9178 );
		CHECK ( it->header().id2() == 0x014BE24E );
		CHECK ( it->header().cddb_id() == 0xB40D2D0F );

		CHECK ( it->size() == 15 );
		CHECK ( it->triplet(0).arcs() == 0xB89992E5 );
		CHECK ( it->triplet(0).confidence() == 6 );
		CHECK ( it->triplet(0).frame450_arcs() == 0xC89192E5 );

		++it;

		CHECK ( it == crend(instance) );
	}

	SECTION ( "Reverse end iterators work correctly" )
	{
		using std::crbegin;
		using std::crend;

		using std::rbegin;
		using std::rend;

		auto dbar_cend { crend(instance)  };

		CHECK ( rend(instance) == dbar_cend );

		auto dbar_pos  { crbegin(instance) };

		// Add complete size to crbegin
		for (auto i = DBAR::size_type {0}; i < instance.size(); ++i)
		{
			dbar_pos++;
		}

		CHECK ( dbar_pos == dbar_cend );
	}

	SECTION (
		"Range-based for loop on initalizer_list constructed DBAR works correctly" )
	{
		for (const auto& block : instance)
		{
			CHECK ( block.header().total_tracks() > 0 );

			for (const auto& triplet : block)
			{
				CHECK ( triplet.arcs() > 0 );
			}
		}
	}

	SECTION ( "Range-based for loop on copy-constructed DBAR works correctly" )
	{
		for (const auto& block : instance)
		{
			CHECK ( block.header().total_tracks() > 0 );

			for (const auto& triplet : block)
			{
				CHECK ( triplet.arcs() > 0 );
			}
		}
	}

	SECTION ( "is_valid()" )
	{
		CHECK ( is_valid(instance) );
	}

	SECTION ( "is_uniform()" )
	{
		CHECK ( not is_uniform(instance) );
	}

	SECTION ( "is_uniform()" )
	{
		CHECK ( not is_regular(instance) );
	}
}

