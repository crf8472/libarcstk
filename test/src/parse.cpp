#include "catch2/catch.hpp"

#include <fstream>           // for ifstream
#include <memory>            // for make_unique, unique_ptr
#include <string>            // for basic_string, operator==, string
#include <type_traits>       // for integral_constant<>::value, is_copy_assi...
#include <utility>           // for move

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_PARSE_HPP__
#include "parse.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif


/**
 * \file parse.cpp Fixtures for \ref ARTriplet, \ref ARBlock, \ref ARParser and handlers
 */


TEST_CASE ( "ARTriplet", "[parse] [artriplet]" )
{
	using arcstk::ARTriplet;

	ARTriplet empty_triplet;

	REQUIRE ( empty_triplet.empty() );

	ARTriplet triplet0{ 0xABCDEF00, 100, 0x0023BFCC, true,  true,  true  };
	ARTriplet triplet1{ 0xABCDEF01, 101, 0x0023BFCC, true,  false, true  };
	ARTriplet triplet2{ 0xABCDEF02, 102, 0x0023BFCC, false, true,  true  };
	ARTriplet triplet3{ 0xABCDEF03, 103, 0x0023BFCC, true,  true,  false };
	ARTriplet triplet4{ 0xABCDEF04, 104, 0x0023BFCC, true,  false, false };
	ARTriplet triplet5{ 0xABCDEF05, 105, 0x0023BFCC, false, true,  false };
	ARTriplet triplet6{ 0xABCDEF06, 106, 0x0023BFCC, false, false, true  };
	ARTriplet triplet7{ 0xABCDEF07, 107, 0x0023BFCC, false, false, false };


	// TODO REQUIRE ...


	SECTION ( "Construct complete ARTriplet" )
	{
		ARTriplet triplet { 0xABCDEF00, 100, 0x0023BFCC };

		CHECK ( triplet.arcs()          == 0xABCDEF00 );
		CHECK ( triplet.confidence()    == 100 );
		CHECK ( triplet.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet.arcs_valid() );
		CHECK ( triplet.confidence_valid() );
		CHECK ( triplet.frame450_arcs_valid() );

		CHECK ( not triplet.empty() );
	}


	SECTION ( "Construct incomplete ARTriplet" )
	{
		CHECK ( triplet0.arcs()          == 0xABCDEF00 );
		CHECK ( triplet0.confidence()    == 100 );
		CHECK ( triplet0.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet0.arcs_valid() );
		CHECK ( triplet0.confidence_valid() );
		CHECK ( triplet0.frame450_arcs_valid() );

		CHECK ( not triplet0.empty() );


		CHECK ( triplet1.arcs()          == 0xABCDEF01 );
		CHECK ( triplet1.confidence()    == 101 );
		CHECK ( triplet1.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet1.arcs_valid() );
		CHECK ( not triplet1.confidence_valid() );
		CHECK ( triplet1.frame450_arcs_valid() );

		CHECK ( not triplet1.empty() );


		CHECK ( triplet2.arcs()          == 0xABCDEF02 );
		CHECK ( triplet2.confidence()    == 102 );
		CHECK ( triplet2.frame450_arcs() == 0x0023BFCC );

		CHECK ( not triplet2.arcs_valid() );
		CHECK ( triplet2.confidence_valid() );
		CHECK ( triplet2.frame450_arcs_valid() );

		CHECK ( not triplet2.empty() );


		CHECK ( triplet3.arcs()          == 0xABCDEF03 );
		CHECK ( triplet3.confidence()    == 103 );
		CHECK ( triplet3.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet3.arcs_valid() );
		CHECK ( triplet3.confidence_valid() );
		CHECK ( not triplet3.frame450_arcs_valid() );

		CHECK ( not triplet3.empty() );


		CHECK ( triplet4.arcs()          == 0xABCDEF04 );
		CHECK ( triplet4.confidence()    == 104 );
		CHECK ( triplet4.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet4.arcs_valid() );
		CHECK ( not triplet4.confidence_valid() );
		CHECK ( not triplet4.frame450_arcs_valid() );

		CHECK ( not triplet4.empty() );


		CHECK ( triplet5.arcs()          == 0xABCDEF05 );
		CHECK ( triplet5.confidence()    == 105 );
		CHECK ( triplet5.frame450_arcs() == 0x0023BFCC );

		CHECK ( not triplet5.arcs_valid() );
		CHECK ( triplet5.confidence_valid() );
		CHECK ( not triplet5.frame450_arcs_valid() );

		CHECK ( not triplet5.empty() );


		CHECK ( triplet6.arcs()          == 0xABCDEF06 );
		CHECK ( triplet6.confidence()    == 106 );
		CHECK ( triplet6.frame450_arcs() == 0x0023BFCC );

		CHECK ( not triplet6.arcs_valid() );
		CHECK ( not triplet6.confidence_valid() );
		CHECK ( triplet6.frame450_arcs_valid() );

		CHECK ( not triplet6.empty() );


		CHECK ( triplet7.arcs()          == 0xABCDEF07 );
		CHECK ( triplet7.confidence()    == 107 );
		CHECK ( triplet7.frame450_arcs() == 0x0023BFCC );

		CHECK ( not triplet7.arcs_valid() );
		CHECK ( not triplet7.confidence_valid() );
		CHECK ( not triplet7.frame450_arcs_valid() );

		CHECK ( not triplet7.empty() );
	}


	SECTION ( "Construct empty ARTriplet" )
	{
		ARTriplet triplet;

		CHECK ( triplet.empty() );
	}


	SECTION ( "Copy construct complete ARTriplet" )
	{
		ARTriplet triplet { 0xABCDEF00, 100, 0x0023BFCC };
		ARTriplet triplet_copy(triplet);

		CHECK ( triplet_copy.arcs()          == 0xABCDEF00 );
		CHECK ( triplet_copy.confidence()    == 100 );
		CHECK ( triplet_copy.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet_copy.arcs_valid() );
		CHECK ( triplet_copy.confidence_valid() );
		CHECK ( triplet_copy.frame450_arcs_valid() );

		CHECK ( not triplet_copy.empty() );

		CHECK ( triplet == triplet_copy );
	}


	SECTION ( "Copy construct incomplete ARTriplet" )
	{
		ARTriplet triplet_copy(triplet1);

		CHECK ( triplet_copy.arcs()          == 0xABCDEF01 );
		CHECK ( triplet_copy.confidence()    == 101 );
		CHECK ( triplet_copy.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet_copy.arcs_valid() );
		CHECK ( not triplet_copy.confidence_valid() );
		CHECK ( triplet_copy.frame450_arcs_valid() );

		CHECK ( not triplet_copy.empty() );

		CHECK ( triplet1 == triplet_copy );
	}


	SECTION ( "Copy construct empty ARTriplet" )
	{
		ARTriplet triplet { empty_triplet };

		CHECK ( triplet.empty() );
	}


	SECTION ( "Move construct complete ARTriplet" )
	{
		ARTriplet triplet { 0xABCDEF00, 100, 0x0023BFCC };
		ARTriplet triplet_moved( std::move(triplet) );

		CHECK ( triplet_moved.arcs()          == 0xABCDEF00 );
		CHECK ( triplet_moved.confidence()    == 100 );
		CHECK ( triplet_moved.frame450_arcs() == 0x0023BFCC );

		CHECK ( triplet_moved.arcs_valid() );
		CHECK ( triplet_moved.confidence_valid() );
		CHECK ( triplet_moved.frame450_arcs_valid() );

		CHECK ( not triplet_moved.empty() );
	}


	SECTION ( "Move construct incomplete ARTriplet" )
	{
		ARTriplet triplet { 0xABCDEF01, 101, 0x0023BFCC, false, true, false };
		ARTriplet triplet_moved( std::move(triplet) );

		CHECK ( triplet_moved.arcs()          == 0xABCDEF01 );
		CHECK ( triplet_moved.confidence()    == 101 );
		CHECK ( triplet_moved.frame450_arcs() == 0x0023BFCC );

		CHECK ( not triplet_moved.arcs_valid() );
		CHECK ( triplet_moved.confidence_valid() );
		CHECK ( not triplet_moved.frame450_arcs_valid() );

		CHECK ( not triplet_moved.empty() );
	}


	SECTION ( "Move construct empty ARTriplet" )
	{
		ARTriplet triplet;

		CHECK ( triplet.empty() );

		ARTriplet triplet_moved(std::move(triplet));

		CHECK ( triplet_moved.empty() );
	}


	SECTION ( "Equality of two complete ARTriplets" )
	{
		ARTriplet tripletA { 0xABCDEF00, 100, 0x0023BFCC };
		ARTriplet tripletB { 0xABCDEF00, 100, 0x0023BFCC };

		CHECK ( tripletA == tripletB );

		ARTriplet tripletC(0xABCDEF00, 101, 0x0023BFCC);

		CHECK ( not (tripletA == tripletC) );
		CHECK ( not (tripletB == tripletC) );
	}


	SECTION ( "Equality of two incomplete ARTriplets" )
	{
		ARTriplet tripletA { 0xABCDEF00, 100, 0x0023BFCC, true,  true, true };
		ARTriplet tripletB { 0xABCDEF00, 100, 0x0023BFCC, true,  true, true };

		CHECK ( tripletA == tripletB );

		ARTriplet tripletC { 0xABCDEF00, 101, 0x0023BFCC, true,  false, true };

		CHECK ( not (tripletA == tripletC) );
		CHECK ( not (tripletB == tripletC) );
	}


	SECTION ( "Equality of empty and non-empty ARTriplet" )
	{
		ARTriplet tripletA;

		REQUIRE (tripletA == empty_triplet);


		ARTriplet tripletB { 0, 0, 0, false, false, false };

		CHECK ( not (tripletB == empty_triplet) );

		ARTriplet tripletC { 0, 0, 0, true, true, true };

		CHECK ( not (tripletC == empty_triplet) );
	}


	SECTION ( "Swap two complete ARTriplets" )
	{
		ARTriplet tripletA { 0xABCDEF01, 101, 0x0023BFCC };
		ARTriplet tripletB { 0xABCDEF02, 102, 0x0023BFDD };

		swap(tripletA, tripletB);

		CHECK ( tripletA.arcs()          == 0xABCDEF02 );
		CHECK ( tripletA.confidence()    == 102 );
		CHECK ( tripletA.frame450_arcs() == 0x0023BFDD );
		CHECK ( tripletA.arcs_valid() );
		CHECK ( tripletA.confidence_valid() );
		CHECK ( tripletA.frame450_arcs_valid() );

		CHECK ( tripletB.arcs()          == 0xABCDEF01 );
		CHECK ( tripletB.confidence()    == 101 );
		CHECK ( tripletB.frame450_arcs() == 0x0023BFCC );
		CHECK ( tripletB.arcs_valid() );
		CHECK ( tripletB.confidence_valid() );
		CHECK ( tripletB.frame450_arcs_valid() );
	}


	SECTION ( "Swap two incomplete ARTriplets" )
	{
		ARTriplet tripletA { 0xABCDEF01, 101, 0x0023BFCC, false, false, false };
		ARTriplet tripletB { 0xABCDEF02, 102, 0x0023BFDD, true,  true,  true  };

		swap(tripletA, tripletB);

		CHECK ( tripletA.arcs()          == 0xABCDEF02 );
		CHECK ( tripletA.confidence()    == 102 );
		CHECK ( tripletA.frame450_arcs() == 0x0023BFDD );
		CHECK ( tripletA.arcs_valid() );
		CHECK ( tripletA.confidence_valid() );
		CHECK ( tripletA.frame450_arcs_valid() );

		CHECK ( tripletB.arcs()          == 0xABCDEF01 );
		CHECK ( tripletB.confidence()    == 101 );
		CHECK ( tripletB.frame450_arcs() == 0x0023BFCC );
		CHECK ( not tripletB.arcs_valid() );
		CHECK ( not tripletB.confidence_valid() );
		CHECK ( not tripletB.frame450_arcs_valid() );
	}


	SECTION ( "Swap two empty ARTriplets" )
	{
		ARTriplet tripletA;
		ARTriplet tripletB;

		REQUIRE ( tripletA == empty_triplet );
		REQUIRE ( tripletB == empty_triplet );
		REQUIRE ( tripletA == tripletB );

		swap(tripletA, tripletB);

		CHECK ( tripletA == empty_triplet );
		CHECK ( tripletB == empty_triplet );
		CHECK ( tripletA == tripletB );
	}
}


TEST_CASE ( "ARBlock", "[parse] [arblock]" )
{
	using arcstk::ARTriplet;
	using arcstk::ARBlock;

	ARBlock block( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F, 24, 0 },
			{ 0x475F57E9, 24, 0 },
			{ 0x7304F1C4, 24, 0 },
			{ 0xF2472287, 24, 0 },
			{ 0x881BC504, 24, 0 },
			{ 0xBB94BFD4, 24, 0 },
			{ 0xF9CAEE76, 24, 0 },
			{ 0xF9F60BC1, 24, 0 },
			{ 0x2C736302, 24, 0 },
			{ 0x1C955978, 24, 0 },
			{ 0xFDA6D833, 24, 0 },
			{ 0x3A57E5D1, 24, 0 },
			{ 0x6ED5F3E7, 24, 0 },
			{ 0x4A5C3872, 24, 0 },
			{ 0x5FE8B032, 24, 0 }
		}
	);

	REQUIRE ( block.id().track_count() == 15 );
	REQUIRE ( block.id().disc_id_1()   == 0x001b9178 );
	REQUIRE ( block.id().disc_id_2()   == 0x014be24e );
	REQUIRE ( block.id().cddb_id()     == 0xb40d2d0f );

	REQUIRE ( block.size()     == 15 );

	REQUIRE ( block[ 0].arcs() == 0x98B10E0F );
	REQUIRE ( block[ 1].arcs() == 0x475F57E9 );
	REQUIRE ( block[ 2].arcs() == 0x7304F1C4 );
	REQUIRE ( block[ 3].arcs() == 0xF2472287 );
	REQUIRE ( block[ 4].arcs() == 0x881BC504 );
	REQUIRE ( block[ 5].arcs() == 0xBB94BFD4 );
	REQUIRE ( block[ 6].arcs() == 0xF9CAEE76 );
	REQUIRE ( block[ 7].arcs() == 0xF9F60BC1 );
	REQUIRE ( block[ 8].arcs() == 0x2C736302 );
	REQUIRE ( block[ 9].arcs() == 0x1C955978 );
	REQUIRE ( block[10].arcs() == 0xFDA6D833 );
	REQUIRE ( block[11].arcs() == 0x3A57E5D1 );
	REQUIRE ( block[12].arcs() == 0x6ED5F3E7 );
	REQUIRE ( block[13].arcs() == 0x4A5C3872 );
	REQUIRE ( block[14].arcs() == 0x5FE8B032 );


	SECTION ( "Copy construct ARBlock" )
	{
		ARBlock block_copy { block };

		CHECK ( block_copy.id().track_count() == 15 );
		CHECK ( block_copy.id().disc_id_1()   == 0x001b9178 );
		CHECK ( block_copy.id().disc_id_2()   == 0x014be24e );
		CHECK ( block_copy.id().cddb_id()     == 0xb40d2d0f );

		CHECK ( block_copy.size()     == 15 );

		CHECK ( block_copy[ 0].arcs() == 0x98B10E0F );
		CHECK ( block_copy[ 1].arcs() == 0x475F57E9 );
		CHECK ( block_copy[ 2].arcs() == 0x7304F1C4 );
		CHECK ( block_copy[ 3].arcs() == 0xF2472287 );
		CHECK ( block_copy[ 4].arcs() == 0x881BC504 );
		CHECK ( block_copy[ 5].arcs() == 0xBB94BFD4 );
		CHECK ( block_copy[ 6].arcs() == 0xF9CAEE76 );
		CHECK ( block_copy[ 7].arcs() == 0xF9F60BC1 );
		CHECK ( block_copy[ 8].arcs() == 0x2C736302 );
		CHECK ( block_copy[ 9].arcs() == 0x1C955978 );
		CHECK ( block_copy[10].arcs() == 0xFDA6D833 );
		CHECK ( block_copy[11].arcs() == 0x3A57E5D1 );
		CHECK ( block_copy[12].arcs() == 0x6ED5F3E7 );
		CHECK ( block_copy[13].arcs() == 0x4A5C3872 );
		CHECK ( block_copy[14].arcs() == 0x5FE8B032 );
	}


	SECTION ( "Move construct ARBlock" )
	{
		ARBlock block_moved { std::move(block) };

		CHECK ( block_moved.id().track_count() == 15 );
		CHECK ( block_moved.id().disc_id_1()   == 0x001b9178 );
		CHECK ( block_moved.id().disc_id_2()   == 0x014be24e );
		CHECK ( block_moved.id().cddb_id()     == 0xb40d2d0f );

		CHECK ( block_moved.size()     == 15 );

		CHECK ( block_moved[ 0].arcs() == 0x98B10E0F );
		CHECK ( block_moved[ 1].arcs() == 0x475F57E9 );
		CHECK ( block_moved[ 2].arcs() == 0x7304F1C4 );
		CHECK ( block_moved[ 3].arcs() == 0xF2472287 );
		CHECK ( block_moved[ 4].arcs() == 0x881BC504 );
		CHECK ( block_moved[ 5].arcs() == 0xBB94BFD4 );
		CHECK ( block_moved[ 6].arcs() == 0xF9CAEE76 );
		CHECK ( block_moved[ 7].arcs() == 0xF9F60BC1 );
		CHECK ( block_moved[ 8].arcs() == 0x2C736302 );
		CHECK ( block_moved[ 9].arcs() == 0x1C955978 );
		CHECK ( block_moved[10].arcs() == 0xFDA6D833 );
		CHECK ( block_moved[11].arcs() == 0x3A57E5D1 );
		CHECK ( block_moved[12].arcs() == 0x6ED5F3E7 );
		CHECK ( block_moved[13].arcs() == 0x4A5C3872 );
		CHECK ( block_moved[14].arcs() == 0x5FE8B032 );
	}


	SECTION ( "Equality of two ARBlocks" )
	{
		// equal to block
		ARBlock block2( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0x98B10E0F, 24, 0 },
				{ 0x475F57E9, 24, 0 },
				{ 0x7304F1C4, 24, 0 },
				{ 0xF2472287, 24, 0 },
				{ 0x881BC504, 24, 0 },
				{ 0xBB94BFD4, 24, 0 },
				{ 0xF9CAEE76, 24, 0 },
				{ 0xF9F60BC1, 24, 0 },
				{ 0x2C736302, 24, 0 },
				{ 0x1C955978, 24, 0 },
				{ 0xFDA6D833, 24, 0 },
				{ 0x3A57E5D1, 24, 0 },
				{ 0x6ED5F3E7, 24, 0 },
				{ 0x4A5C3872, 24, 0 },
				{ 0x5FE8B032, 24, 0 }
			}
		);

		// not equal to block
		ARBlock block3( /* id */ { 14, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0xA8B10E0F, 23, 0 },
				{ 0x575F57E9, 23, 0 },
				{ 0x8304F1C4, 23, 0 },
				{ 0x02472287, 23, 0 },
				{ 0x981BC504, 23, 0 },
				{ 0xCB94BFD4, 23, 0 },
				{ 0x09CAEE76, 23, 0 },
				{ 0x09F60BC1, 23, 0 },
				{ 0x3C736302, 23, 0 },
				{ 0x2C955978, 23, 0 },
				{ 0x0DA6D833, 23, 0 },
				{ 0x4A57E5D1, 23, 0 },
				{ 0x7ED5F3E7, 23, 0 },
				{ 0x5A5C3872, 23, 0 }
			}
		);

		CHECK ( block  == block2 );
		CHECK ( not (block  == block3) );
		CHECK ( not (block2 == block3) );
	}


	SECTION ( "Swap two ARBlocks" )
	{
		// equal to block
		ARBlock block2( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0x98B10E0F, 24, 0 },
				{ 0x475F57E9, 24, 0 },
				{ 0x7304F1C4, 24, 0 },
				{ 0xF2472287, 24, 0 },
				{ 0x881BC504, 24, 0 },
				{ 0xBB94BFD4, 24, 0 },
				{ 0xF9CAEE76, 24, 0 },
				{ 0xF9F60BC1, 24, 0 },
				{ 0x2C736302, 24, 0 },
				{ 0x1C955978, 24, 0 },
				{ 0xFDA6D833, 24, 0 },
				{ 0x3A57E5D1, 24, 0 },
				{ 0x6ED5F3E7, 24, 0 },
				{ 0x4A5C3872, 24, 0 },
				{ 0x5FE8B032, 24, 0 }
			}
		);

		// not equal to block
		ARBlock block3( /* id */ { 14, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0xA8B10E0F, 23, 0 },
				{ 0x575F57E9, 23, 0 },
				{ 0x8304F1C4, 23, 0 },
				{ 0x02472287, 23, 0 },
				{ 0x981BC504, 23, 0 },
				{ 0xCB94BFD4, 23, 0 },
				{ 0x09CAEE76, 23, 0 },
				{ 0x09F60BC1, 23, 0 },
				{ 0x3C736302, 23, 0 },
				{ 0x2C955978, 23, 0 },
				{ 0x0DA6D833, 23, 0 },
				{ 0x4A57E5D1, 23, 0 },
				{ 0x7ED5F3E7, 23, 0 },
				{ 0x5A5C3872, 23, 0 }
			}
		);

		REQUIRE ( block  == block2 );
		REQUIRE ( not (block  == block3) );
		REQUIRE ( not (block2 == block3) );

		swap(block2, block3);

		CHECK ( block  == block3 );
		CHECK ( not (block  == block2) );
		CHECK ( not (block2 == block3) );
	}


	SECTION ( "Iteration" )
	{
		// const begin+end

		int i = 0;

		for (auto &triplet : block)
		{
			if ( i == 0)
			{
				CHECK ( triplet.arcs() == 0x98B10E0F );
			} else
			if ( i == 1)
			{
				CHECK ( triplet.arcs() == 0x475F57E9 );
			} else
			if ( i == 2)
			{
				CHECK ( triplet.arcs() == 0x7304F1C4 );
			} else
			if ( i == 3)
			{
				CHECK ( triplet.arcs() == 0xF2472287 );
			} else
			if ( i == 4)
			{
				CHECK ( triplet.arcs() == 0x881BC504 );
			} else
			if ( i == 5)
			{
				CHECK ( triplet.arcs() == 0xBB94BFD4 );
			} else
			if ( i == 6)
			{
				CHECK ( triplet.arcs() == 0xF9CAEE76 );
			} else
			if ( i == 7)
			{
				CHECK ( triplet.arcs() == 0xF9F60BC1 );
			} else
			if ( i == 8)
			{
				CHECK ( triplet.arcs() == 0x2C736302 );
			} else
			if ( i == 9)
			{
				CHECK ( triplet.arcs() == 0x1C955978 );
			} else
			if ( i == 10)
			{
				CHECK ( triplet.arcs() == 0xFDA6D833 );
			} else
			if ( i == 11)
			{
				CHECK ( triplet.arcs() == 0x3A57E5D1 );
			} else
			if ( i == 12)
			{
				CHECK ( triplet.arcs() == 0x6ED5F3E7 );
			} else
			if ( i == 13)
			{
				CHECK ( triplet.arcs() == 0x4A5C3872 );
			} else
			if ( i == 14)
			{
				CHECK ( triplet.arcs() == 0x5FE8B032 );
			}

			++i;
		}
		CHECK ( i == 15 );


		// cbegin + cend

		int j = 0;
		ARTriplet triplet;

		for (auto it = block.cbegin(); it != block.cend(); ++it)
		{
			triplet = *it;

			if ( j == 0)
			{
				CHECK ( triplet.arcs() == 0x98B10E0F );
			} else
			if ( j == 1)
			{
				CHECK ( triplet.arcs() == 0x475F57E9 );
			} else
			if ( j == 2)
			{
				CHECK ( triplet.arcs() == 0x7304F1C4 );
			} else
			if ( j == 3)
			{
				CHECK ( triplet.arcs() == 0xF2472287 );
			} else
			if ( j == 4)
			{
				CHECK ( triplet.arcs() == 0x881BC504 );
			} else
			if ( j == 5)
			{
				CHECK ( triplet.arcs() == 0xBB94BFD4 );
			} else
			if ( j == 6)
			{
				CHECK ( triplet.arcs() == 0xF9CAEE76 );
			} else
			if ( j == 7)
			{
				CHECK ( triplet.arcs() == 0xF9F60BC1 );
			} else
			if ( j == 8)
			{
				CHECK ( triplet.arcs() == 0x2C736302 );
			} else
			if ( j == 9)
			{
				CHECK ( triplet.arcs() == 0x1C955978 );
			} else
			if ( j == 10)
			{
				CHECK ( triplet.arcs() == 0xFDA6D833 );
			} else
			if ( j == 11)
			{
				CHECK ( triplet.arcs() == 0x3A57E5D1 );
			} else
			if ( j == 12)
			{
				CHECK ( triplet.arcs() == 0x6ED5F3E7 );
			} else
			if ( j == 13)
			{
				CHECK ( triplet.arcs() == 0x4A5C3872 );
			} else
			if ( j == 14)
			{
				CHECK ( triplet.arcs() == 0x5FE8B032 );
			}

			++j;
		}
	}

}


TEST_CASE ( "ARResponse", "[parse] [arresponse]" )
{
	using arcstk::ARTriplet;
	using arcstk::ARBlock;
	using arcstk::ARResponse;

	ARBlock block1( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x98B10E0F, 24, 0 },
			{ 0x475F57E9, 24, 0 },
			{ 0x7304F1C4, 24, 0 },
			{ 0xF2472287, 24, 0 },
			{ 0x881BC504, 24, 0 },
			{ 0xBB94BFD4, 24, 0 },
			{ 0xF9CAEE76, 24, 0 },
			{ 0xF9F60BC1, 24, 0 },
			{ 0x2C736302, 24, 0 },
			{ 0x1C955978, 24, 0 },
			{ 0xFDA6D833, 24, 0 },
			{ 0x3A57E5D1, 24, 0 },
			{ 0x6ED5F3E7, 24, 0 },
			{ 0x4A5C3872, 24, 0 },
			{ 0x5FE8B032, 24, 0 }
		}
	);

	REQUIRE ( block1.id().track_count() == 15 );
	REQUIRE ( block1.id().disc_id_1()   == 0x001b9178 );
	REQUIRE ( block1.id().disc_id_2()   == 0x014be24e );
	REQUIRE ( block1.id().cddb_id()     == 0xb40d2d0f );

	REQUIRE ( block1.size()     == 15 );

	REQUIRE ( block1[ 0].arcs() == 0x98B10E0F );
	REQUIRE ( block1[ 1].arcs() == 0x475F57E9 );
	REQUIRE ( block1[ 2].arcs() == 0x7304F1C4 );
	REQUIRE ( block1[ 3].arcs() == 0xF2472287 );
	REQUIRE ( block1[ 4].arcs() == 0x881BC504 );
	REQUIRE ( block1[ 5].arcs() == 0xBB94BFD4 );
	REQUIRE ( block1[ 6].arcs() == 0xF9CAEE76 );
	REQUIRE ( block1[ 7].arcs() == 0xF9F60BC1 );
	REQUIRE ( block1[ 8].arcs() == 0x2C736302 );
	REQUIRE ( block1[ 9].arcs() == 0x1C955978 );
	REQUIRE ( block1[10].arcs() == 0xFDA6D833 );
	REQUIRE ( block1[11].arcs() == 0x3A57E5D1 );
	REQUIRE ( block1[12].arcs() == 0x6ED5F3E7 );
	REQUIRE ( block1[13].arcs() == 0x4A5C3872 );
	REQUIRE ( block1[14].arcs() == 0x5FE8B032 );

	ARBlock block2( /* id */ { 13, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
			{ 0x08B10E0F, 23, 0 },
			{ 0x575F57E9, 23, 0 },
			{ 0x8304F1C4, 23, 0 },
			{ 0x02472287, 23, 0 },
			{ 0x981BC504, 23, 0 },
			{ 0xCB94BFD4, 23, 0 },
			{ 0x09CAEE76, 23, 0 },
			{ 0x09F60BC1, 23, 0 },
			{ 0x3C736302, 23, 0 },
			{ 0x2C955978, 23, 0 },
			{ 0x0DA6D833, 23, 0 },
			{ 0x4A57E5D1, 23, 0 },
			{ 0x7ED5F3E7, 23, 0 },
		}
	);

	ARResponse response1 { block1, block2 };

	REQUIRE( response1.size() == 2 );


	SECTION ( "Copy construct ARResponse" )
	{
		ARResponse response_copy { response1 };

		CHECK ( response_copy.size() == 2 );

		CHECK ( response_copy[0] == block1 );
		CHECK ( response_copy[1] == block2 );
	}


	SECTION ( "Move construct ARResponse" )
	{
		ARResponse response_moved { std::move(response1) };

		CHECK ( response_moved.size() == 2 );

		CHECK ( response_moved[0] == block1 );
		CHECK ( response_moved[1] == block2 );
	}


	SECTION ( "Equality of two ARResponses" )
	{
		// equal to block
		ARBlock block3( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0x98B10E0F, 24, 0 },
				{ 0x475F57E9, 24, 0 },
				{ 0x7304F1C4, 24, 0 },
				{ 0xF2472287, 24, 0 },
				{ 0x881BC504, 24, 0 },
				{ 0xBB94BFD4, 24, 0 },
				{ 0xF9CAEE76, 24, 0 },
				{ 0xF9F60BC1, 24, 0 },
				{ 0x2C736302, 24, 0 },
				{ 0x1C955978, 24, 0 },
				{ 0xFDA6D833, 24, 0 },
				{ 0x3A57E5D1, 24, 0 },
				{ 0x6ED5F3E7, 24, 0 },
				{ 0x4A5C3872, 24, 0 },
				{ 0x5FE8B032, 24, 0 }
			}
		);

		// not equal to block
		ARBlock block4( /* id */ { 14, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0xA8B10E0F, 23, 0 },
				{ 0x575F57E9, 23, 0 },
				{ 0x8304F1C4, 23, 0 },
				{ 0x02472287, 23, 0 },
				{ 0x981BC504, 23, 0 },
				{ 0xCB94BFD4, 23, 0 },
				{ 0x09CAEE76, 23, 0 },
				{ 0x09F60BC1, 23, 0 },
				{ 0x3C736302, 23, 0 },
				{ 0x2C955978, 23, 0 },
				{ 0x0DA6D833, 23, 0 },
				{ 0x4A57E5D1, 23, 0 },
				{ 0x7ED5F3E7, 23, 0 },
				{ 0x5A5C3872, 23, 0 }
			}
		);

		ARResponse response2 { block1, block2 };
		ARResponse response3 { block3, block4 };

		CHECK ( response1 == response2 );
		CHECK ( not (response1 == response3 ) );
		CHECK ( not (response2 == response3 ) );
	}


	SECTION ( "Swap two ARResponses" )
	{
		// equal to block
		ARBlock block3( /* id */ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0x98B10E0F, 24, 0 },
				{ 0x475F57E9, 24, 0 },
				{ 0x7304F1C4, 24, 0 },
				{ 0xF2472287, 24, 0 },
				{ 0x881BC504, 24, 0 },
				{ 0xBB94BFD4, 24, 0 },
				{ 0xF9CAEE76, 24, 0 },
				{ 0xF9F60BC1, 24, 0 },
				{ 0x2C736302, 24, 0 },
				{ 0x1C955978, 24, 0 },
				{ 0xFDA6D833, 24, 0 },
				{ 0x3A57E5D1, 24, 0 },
				{ 0x6ED5F3E7, 24, 0 },
				{ 0x4A5C3872, 24, 0 },
				{ 0x5FE8B032, 24, 0 }
			}
		);

		// not equal to block
		ARBlock block4( /* id */ { 14, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			{ /* triplets */
				{ 0xA8B10E0F, 23, 0 },
				{ 0x575F57E9, 23, 0 },
				{ 0x8304F1C4, 23, 0 },
				{ 0x02472287, 23, 0 },
				{ 0x981BC504, 23, 0 },
				{ 0xCB94BFD4, 23, 0 },
				{ 0x09CAEE76, 23, 0 },
				{ 0x09F60BC1, 23, 0 },
				{ 0x3C736302, 23, 0 },
				{ 0x2C955978, 23, 0 },
				{ 0x0DA6D833, 23, 0 },
				{ 0x4A57E5D1, 23, 0 },
				{ 0x7ED5F3E7, 23, 0 },
				{ 0x5A5C3872, 23, 0 }
			}
		);

		ARResponse response2 { block1, block2 };
		ARResponse response3 { block3, block4 };

		swap(response2, response3);

		CHECK ( response1 == response3 );
		CHECK ( not (response1 == response2 ) );
		CHECK ( not (response2 == response3 ) );
	}


	SECTION ( "Iteration" )
	{
		// const begin+end

		int i = 0;

		for (auto &block : response1)
		{
			if ( i == 0)
			{
				CHECK ( block == block1 );
			} else
			if ( i == 1)
			{
				CHECK ( block == block2 );
			}

			++i;
		}
		CHECK ( i == 2 );


		// cbegin + cend

		int j = 0;

		for (auto it = response1.cbegin(); it != response1.cend(); ++it)
		{
			if ( j == 0)
			{
				CHECK ( *it == block1 );
			} else
			if ( j == 1)
			{
				CHECK ( *it == block2 );
			}

			++j;
		}
	}
}


TEST_CASE ( "DefaultContentHandler", "[parse] [defaulthandler]" )
{
	using arcstk::ARId;
	using arcstk::DefaultContentHandler;
	using arcstk::ARResponse;

	DefaultContentHandler c_handler;
	ARResponse result;

	c_handler.set_object(result);

	SECTION ( "Not copyable" )
	{
		CHECK ( not std::is_copy_constructible<DefaultContentHandler>::value );
		CHECK ( not std::is_copy_assignable<DefaultContentHandler>::value    );
	}

	SECTION ( "Move constructor" )
	{
		DefaultContentHandler c_handler_moved(std::move(c_handler));

		CHECK ( &c_handler_moved.object() == &result );
		CHECK ( c_handler_moved.object()  == result );
	}

	SECTION ( "Move assignment" )
	{
		DefaultContentHandler c_handler_moved = std::move(c_handler);

		CHECK ( &c_handler_moved.object() == &result );
		CHECK ( c_handler_moved.object()  == result );
	}

	SECTION ( "DefaultContentHandler returns correct ARResponse" )
	{
		// Implicitly testing the appending to ARResponse and ARBlock!

		// The functionality of DefaultContentHandler is implicitly tested
		// by the testcases for ARParser and ARFileParser.

		// start parsing
		c_handler.start_input();
		c_handler.start_block();

		// block 0
		c_handler.id(5, 123, 456, 789);
		c_handler.triplet(12345, 20, 45551);
		c_handler.triplet(23456, 20, 56677);
		c_handler.triplet(34567, 21, 65599);
		c_handler.triplet(45678, 21, 43322);
		c_handler.triplet(56789, 21, 45533);

		c_handler.end_block();
		c_handler.start_block();

		// block 1
		c_handler.id(5, 123, 456, 789); // same id as block 0
		c_handler.triplet(23456, 20, 45551);
		c_handler.triplet(34567, 20, 56677);
		c_handler.triplet(45678, 21, 65599);
		c_handler.triplet(56789, 21, 43322);
		c_handler.triplet(67890, 21, 45533);

		c_handler.end_block();
		c_handler.end_input();
		// end parsing


		CHECK ( result.size()    == 2 );

		// block 0
		CHECK ( result[0].size() == 5 );
		CHECK ( result[0].id()   == ARId { 5, 123, 456, 789 } );

		CHECK ( result[0][0].arcs()          == 12345 );
		CHECK ( result[0][0].confidence()    ==    20 );
		CHECK ( result[0][0].frame450_arcs() == 45551 );
		CHECK ( result[0][1].arcs()          == 23456 );
		CHECK ( result[0][1].confidence()    ==    20 );
		CHECK ( result[0][1].frame450_arcs() == 56677 );
		CHECK ( result[0][2].arcs()          == 34567 );
		CHECK ( result[0][2].confidence()    ==    21 );
		CHECK ( result[0][2].frame450_arcs() == 65599 );
		CHECK ( result[0][3].arcs()          == 45678 );
		CHECK ( result[0][3].confidence()    ==    21 );
		CHECK ( result[0][3].frame450_arcs() == 43322 );
		CHECK ( result[0][4].arcs()          == 56789 );
		CHECK ( result[0][4].confidence()    ==    21 );
		CHECK ( result[0][4].frame450_arcs() == 45533 );

		// block 1
		CHECK ( result[1].size() == 5 );
		CHECK ( result[1].id()   == ARId { 5, 123, 456, 789 } );

		CHECK ( result[1][0].arcs()          == 23456 );
		CHECK ( result[1][0].confidence()    ==    20 );
		CHECK ( result[1][0].frame450_arcs() == 45551 );
		CHECK ( result[1][1].arcs()          == 34567 );
		CHECK ( result[1][1].confidence()    ==    20 );
		CHECK ( result[1][1].frame450_arcs() == 56677 );
		CHECK ( result[1][2].arcs()          == 45678 );
		CHECK ( result[1][2].confidence()    ==    21 );
		CHECK ( result[1][2].frame450_arcs() == 65599 );
		CHECK ( result[1][3].arcs()          == 56789 );
		CHECK ( result[1][3].confidence()    ==    21 );
		CHECK ( result[1][3].frame450_arcs() == 43322 );
		CHECK ( result[1][4].arcs()          == 67890 );
		CHECK ( result[1][4].confidence()    ==    21 );
		CHECK ( result[1][4].frame450_arcs() == 45533 );
	}
}


TEST_CASE ( "DefaultErrorHandler", "[defaulterrorhandler]" )
{
	using DefaultErrorHandler = arcstk::DefaultErrorHandler;

	DefaultErrorHandler e_handler;

	SECTION ( "Not copyable" )
	{
		CHECK ( not std::is_copy_constructible<DefaultErrorHandler>::value );
		CHECK ( not std::is_copy_assignable<DefaultErrorHandler>::value    );
	}

	SECTION ( "Move constructor (NO CHECKS)" )
	{
		DefaultErrorHandler e_handler_moved(std::move(e_handler));
	}

	SECTION ( "Move assignment (NO CHECKS)" )
	{
		DefaultErrorHandler e_handler_moved = std::move(e_handler);
	}
}


TEST_CASE ( "ARStreamParser::parse_stream", "[parse]" )
{
	using ARParser = arcstk::ARParser;
	using ARResponse = arcstk::ARResponse;
	using DefaultContentHandler = arcstk::DefaultContentHandler;

	ARResponse result;

	auto c_handler = std::make_unique<DefaultContentHandler>();
	c_handler->set_object(result);

	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);


	// A parse error can only occurr if the input ends prematurely, e.g. if
	// an input block, for any reason, is not 13 + (n * 9) bytes long (with n
	// being the track number.
	//
	// 22 cases are tested:
	//
	// Header is 13 bytes long, hence there are 13 positions (after byte 1 - 13)
	// for errors that are covered by the input files *H_01-H_013.
	//
	// Triplet is 9 bytes long, hence there are 9 different positions (before
	// byte 1 and after byte 1 - 8) for errors that are covered by the input
	// files *T_00-T_08.

	// There is no easy or comfortable way in Catch2 to access the
	// exception object thrown via its genuine interface. (However, you can
	// access the "what" message via CHECK_THROWS_WITH).
	//
	// The following workaround addresses this by requiring an exception
	// manually and catching the exception object.
	//
	// Confer: https://github.com/catchorg/Catch2/issues/394


	SECTION ( "Parse files with incomplete header: no disc id1 (pos 1)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+01.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+01.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 1 );
			CHECK ( e.byte_position()       == 149 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id1 (pos 2)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+02.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+02.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 2 );
			CHECK ( e.byte_position()       == 150 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id1 (pos 3)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+03.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+03.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 3 );
			CHECK ( e.byte_position()       == 151 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id1 (pos 4)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+04.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+04.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 4 );
			CHECK ( e.byte_position()       == 152 );
		}
	}


	SECTION ( "Parse files with incomplete header: no disc id2 (pos 5)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+05.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+05.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 5 );
			CHECK ( e.byte_position()       == 153 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id2 (pos 6)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+06.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+06.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 6 );
			CHECK ( e.byte_position()       == 154 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id2 (pos 7)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+07.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+07.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 7 );
			CHECK ( e.byte_position()       == 155 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id2 (pos 8)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+08.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+08.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 8 );
			CHECK ( e.byte_position()       == 156 );
		}
	}


	SECTION ( "Parse files with incomplete header: no cddb id (pos 9)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+09.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+09.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 9 );
			CHECK ( e.byte_position()       == 157 );
		}
	}


	SECTION ( "Parse files with incomplete header: cddb id (pos 10)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+10.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+10.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 10 );
			CHECK ( e.byte_position()       == 158 );
		}
	}


	SECTION ( "Parse files with incomplete header: cddb id (pos 11)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+11.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+11.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 11 );
			CHECK ( e.byte_position()       == 159 );
		}
	}


	SECTION ( "Parse files with incomplete header: cddb id (pos 12)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+12.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+12.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 12 );
			CHECK ( e.byte_position()       == 160 );
		}
	}


	SECTION ( "Parse files with incomplete block: only header" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_H+13.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_H+13.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 13 );
			CHECK ( e.byte_position()       == 161 );
		}
	}


	SECTION ( "Parse files with triplet missing (triplet pos 0)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+0.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+0.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			// End of a triplet + 0 byte => one or more triplets missing
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));

			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 139 );
			CHECK ( e.byte_position()       == 287 );
		}
	}


	SECTION ( "Parse files with missing ARCS (triplet pos 1)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+1.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+1.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			// End of last triplet + 1 byte =>
			// triplet invalid, confidence ok, ARCS missing
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));

			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 140 );
			CHECK ( e.byte_position()       == 288 );
		}
	}


	SECTION ( "Parse files with broken ARCS (triplet pos 2)" )
	{
		// End of last triplet + 2,3 or 4 bytes =>
		// triplet invalid, confidence ok, ARCS incomplete
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+2.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+2.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 141 );
			CHECK ( e.byte_position()       == 289 );
		}
	}


	SECTION ( "Parse files with broken ARCS (triplet pos 3)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+3.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+3.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 142 );
			CHECK ( e.byte_position()       == 290 );
		}
	}


	SECTION ( "Parse files with broken ARCS (triplet pos 4)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+4.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+4.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 143 );
			CHECK ( e.byte_position()       == 291 );
		}
	}


	SECTION ( "Parse files with missing frame450_arcs (triplet pos 5)" )
	{
		// End of last triplet + 5 bytes =>
		// triplet invalid, confidence + ARCS ok, frame450_arcs missing

		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+5.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+5.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 144 );
			CHECK ( e.byte_position()       == 292 );
		}
	}


	SECTION ( "Parse files with broken frame450_arcs (triplet pos 6)" )
	{
		// End of last triplet + 6,7 or 8 bytes =>
		// triplet invalid, confidence + ARCS ok, frame450_arcs incomplete

		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+6.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+6.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 145 );
			CHECK ( e.byte_position()       == 293 );
		}
	}


	SECTION ( "Parse files with broken frame450_arcs (triplet pos 7)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+7.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+7.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 146 );
			CHECK ( e.byte_position()       == 294 );
		}
	}


	SECTION ( "Parse files with broken frame450_arcs (triplet pos 8)" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f_T+8.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f_T+8.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		try
		{
			ARParser parser(file);

			// content handler but no error handler
			parser.set_content_handler(std::move(c_handler));
			parser.parse();

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcstk::StreamReadException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 147 );
			CHECK ( e.byte_position()       == 295 );
		}
	}
}

