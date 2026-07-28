#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for DBARBlock.
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


// TEST_CASE ( "DBARBlock", "[dbarblock] [dbar] [dbar]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::DBARBlock;
//
// 	auto instance = DBARBlock {};
//
//
// 	SECTION ("Parametized construction is correct")
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ("Copy construction is correct")
// 	{
// 		const DBARBlock copied { Copy<DBARBlock>::construct(instance) };
//
// 		// --
//
//         //CHECK(copied.value() == instance.value());
//         //CHECK(copied.name()  == instance.name());
// 		FAIL ( "Copy construction test is missing" );
// 	}
//
// 	SECTION ("Move construction is correct")
// 	{
// 		const DBARBlock moved { Move<DBARBlock>::construct(instance) };
//
// 		// --
//
//         //CHECK(moved.value() == instance.value());
//         //CHECK(moved.name()  == instance.name());
// 		FAIL ( "Move construction test is missing" );
// 	}
//
// 	SECTION ("Copy assignment is correct")
// 	{
// 		auto copied = DBARBlock {};
// 		Copy<DBARBlock>::assign(copied, instance);
//
// 		// --
//
//         //CHECK(copied.value() == instance.value());
//         //CHECK(copied.name()  == instance.name());
// 		FAIL ( "Copy assignment test is missing" );
// 	}
//
// 	SECTION ("Move assignment is correct")
// 	{
// 		auto moved = DBARBlock {};
// 		Move<DBARBlock>::assign(moved, instance);
//
// 		// --
//
//         //CHECK(moved.value() == instance.value());
//         //CHECK(moved.name()  == instance.name());
// 		FAIL ( "Move assignment test is missing" );
// 	}
// }


TEST_CASE ( "DBARBlock default constructed instance",
		"[dbarblock] [dbar] [dbar]" )
{
	const auto instance = arcstk::DBARBlock{};

	// SECTION ( "is empty()")
	// {
	// 	CHECK ( instance.empty() );
	// }
}


// TEST_CASE ( "DBARBlock property", "[dbarblock] [dbar] [dbar]" )
// {
// 	using arcstk::DBARBlock;
//
// 	auto defaulted = DBARBlock {};
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


TEST_CASE ( "DBARBlock functions", "[dbarblock] [dbar] [dbar]" )
{
	using arcstk::DBARBlock;

	using arcstk::DBAR;
	using arcstk::details::get_element;
	using arcstk::is_valid;
	using arcstk::is_uniform;
	using arcstk::is_regular;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0xB89992E5,  6, 0x0BB019EF },
			{ 0x4F77EB03,  8, 0x4A28F1DA },
			{ 0x56582282,  7, 0x3240255C },
			{ 0x9E2187F9,  9, 0x791B0AF9 },
			{ 0x6BE71E50,  2, 0x703FCAEA },
			{ 0x01E7235F,  1, 0x82F7AC32 },
			{ 0xD8F7763C,  0, 0xDD62086B },
			{ 0x8480223E, 13, 0x2E80AD26 },
			{ 0x42C5061C, 16, 0xABF96919 },
			{ 0x47A70F02, 17, 0xDAED909B },
			{ 0xBABF08CC, 18, 0x39D3C0FC },
			{ 0x563EDCCB, 21, 0x8ED8FEBB },
			{ 0xAB123C7C, 14, 0xD03B6267 },
			{ 0xC65C20E4, 26, 0x92349543 },
			{ 0x58FC3C3E, 28, 0x6F309B40 }  //   |  different!
		} },                                //   v
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0E },
		{ /* triplets */
			{ 0x98B10E0F,  3, 0xC19172F9 },
			{ 0x475F57E9,  4, 0x6F71EA01 },
			{ 0x7304F1C4,  5, 0xA8712982 },
			{ 0xF2472287,  6, 0x27FB8E11 },
			{ 0x881BC504,  7, 0xA8D55C13 },
			{ 0xBB94BFD4,  9, 0x72856BF7 },
			{ 0xF9CAEE76, 10, 0x58FF4378 },
			{ 0xF9F60BC1, 11, 0x7520F060 },
			{ 0x2C736302, 12, 0xBC4A5809 },
			{ 0x1C955978, 13, 0x9A349EF3 },
			{ 0xFDA6D833, 16, 0x0CBD69CD },
			{ 0x3A57E5D1, 17, 0xAB9DE2CD },
			{ 0x6ED5F3E7, 18, 0x5D38CD80 },
			{ 0x4A5C3872, 21, 0x000353C6 },
			{ 0x5FE8B032, 24, 0x38179D44 }
		} }
	};

	const auto instance = DBARBlock { dBAR, 1 };

	REQUIRE ( instance.size() == 15 );

	// SECTION ("set is correct")
	// {
	// 	FAIL ("set test is missing");
	// }
	//
	// SECTION ("index is correct")
	// {
	// 	FAIL ("index test is missing");
	// }
	//
	// SECTION ("size is correct")
	// {
	// 	FAIL ("size test is missing");
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
	// SECTION ("id is correct")
	// {
	// 	FAIL ("id test is missing");
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

		CHECK ( it->arcs()          == 0x98B10E0F );
		CHECK ( it->confidence()    == 3 );
		CHECK ( it->frame450_arcs() == 0xC19172F9 );

		++it;

		CHECK ( it->arcs()          == 0x475F57E9 );
		CHECK ( it->confidence()    == 4 );
		CHECK ( it->frame450_arcs() == 0x6F71EA01 );
	}

	SECTION ( "Can be reverse forward-iterated correctly" )
	{
		using std::crbegin;
		using std::crend;

		auto it { crbegin(instance) };

		CHECK ( it->arcs()          == 0x5FE8B032 );
		CHECK ( it->confidence()    == 24 );
		CHECK ( it->frame450_arcs() == 0x38179D44 );

		++it;

		CHECK ( it->arcs()          == 0x4A5C3872 );
		CHECK ( it->confidence()    == 21 );
		CHECK ( it->frame450_arcs() == 0x000353C6 );

		++it;

		for (auto i = DBARBlock::size_type { 2 }; i < instance.size(); ++i)
		{
			it++;
		}

		CHECK ( it == crend(instance) );
	}

	SECTION ( "Reverse end iterators work correctly" )
	{
		using std::crbegin;
		using std::crend;

		using std::rbegin;
		using std::rend;

		auto block_cend { crend(instance)  };

		CHECK ( rend(instance) == block_cend );

		auto block_pos  { crbegin(instance) };

		// Add complete size to crbegin
		for (auto i = DBARBlock::size_type {0}; i < instance.size(); ++i)
		{
			block_pos++;
		}

		CHECK ( block_pos == block_cend );
		CHECK ( block_pos.index() == block_cend.index() );
	}

	SECTION ( "Can be accessed correctly by get_element()" )
	{
		auto mytriplet = get_element(instance, 0);

		CHECK ( mytriplet.arcs()          == 0x98B10E0F );
		CHECK ( mytriplet.confidence()    == 3 );
		CHECK ( mytriplet.frame450_arcs() == 0xC19172F9 );
	}
}

