#include "catch2/catch.hpp"

#include <cstdint>
#include <fstream>
#include <memory>

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_PARSE_HPP__
#include "parse.hpp"
#endif


/**
 * \file parse.cpp Fixtures for \ref ARTriplet, \ref ARBlock, \ref ARParser and handlers
 */


TEST_CASE ( "ARTriplet", "[parse] [artriplet]" )
{
	SECTION ( "Construct complete triplet" )
	{
		arcs::ARTriplet triplet(0xABCDEF00, 100, 0x0023BFCC);

		REQUIRE ( triplet.arcs()          == 0xABCDEF00 );
		REQUIRE ( triplet.confidence()    == 100 );
		REQUIRE ( triplet.frame450_arcs() == 0x0023BFCC );

		REQUIRE ( triplet.arcs_valid() );
		REQUIRE ( triplet.confidence_valid() );
		REQUIRE ( triplet.frame450_arcs_valid() );
	}


//	SECTION ( "Construct incomplete triplet" )
//	{
//		arcs::ARIncompleteTriplet triplet(0xABCDEF00, 100, 0x0023BFCC,
//				true, false, true);
//
//		REQUIRE ( triplet.arcs()          == 0xABCDEF00 );
//		REQUIRE ( triplet.confidence()    == 100 );
//		REQUIRE ( triplet.frame450_arcs() == 0x0023BFCC );
//
//		REQUIRE ( triplet.arcs_valid() );
//		REQUIRE ( not triplet.confidence_valid() );
//		REQUIRE ( triplet.frame450_arcs_valid() );
//	}

}


TEST_CASE ( "ARBlock", "[parse] [arblock]" )
{
	arcs::ARBlock block( { 15, 0x001b9178, 0x014be24e, 0xb40d2d0f } );

	REQUIRE ( block.id().track_count() == 15 );
	REQUIRE ( block.id().disc_id_1()   == 0x001b9178 );
	REQUIRE ( block.id().disc_id_2()   == 0x014be24e );
	REQUIRE ( block.id().cddb_id()     == 0xb40d2d0f );


	SECTION ( "Append triplets" )
	{
		block.append(arcs::ARTriplet(0xABCDEF00, 100, 0x0023BFCC));

		REQUIRE ( block.size() == 1 );

		REQUIRE ( block.begin()->arcs()          == 0xABCDEF00 );
		REQUIRE ( block.begin()->confidence()    == 100 );
		REQUIRE ( block.begin()->frame450_arcs() == 0x0023BFCC );

		REQUIRE ( block.begin()->arcs_valid() );
		REQUIRE ( block.begin()->confidence_valid() );
		REQUIRE ( block.begin()->frame450_arcs_valid() );
	}
}


TEST_CASE ( "DefaultContentHandler", "[parse] [defaulthandler]" )
{
	arcs::DefaultContentHandler c_handler;

	// The functionality of DefaultContentHandler is implicitly tested
	// by the testcases for ARParser and ARFileParser.

	c_handler.start_input();
	c_handler.start_block();

	c_handler.id(5, 123, 456, 789);
	c_handler.triplet(12345, 20, 45551);
	c_handler.triplet(23456, 20, 56677);
	c_handler.triplet(34567, 21, 65599);
	c_handler.triplet(45678, 21, 43322);
	c_handler.triplet(56789, 21, 45533);

	c_handler.end_block();
	c_handler.end_input();

	auto result = c_handler.result();

	REQUIRE ( result.size()    == 1 );
	REQUIRE ( result[0].size() == 5 );
	REQUIRE ( result[0].id()   == arcs::ARId(5, 123, 456, 789) );
	REQUIRE ( result[0][0].arcs()          == 12345 );
	REQUIRE ( result[0][0].confidence()    ==    20 );
	REQUIRE ( result[0][0].frame450_arcs() == 45551 );
	REQUIRE ( result[0][1].arcs()          == 23456 );
	REQUIRE ( result[0][1].confidence()    ==    20 );
	REQUIRE ( result[0][1].frame450_arcs() == 56677 );
	REQUIRE ( result[0][2].arcs()          == 34567 );
	REQUIRE ( result[0][2].confidence()    ==    21 );
	REQUIRE ( result[0][2].frame450_arcs() == 65599 );
	REQUIRE ( result[0][3].arcs()          == 45678 );
	REQUIRE ( result[0][3].confidence()    ==    21 );
	REQUIRE ( result[0][3].frame450_arcs() == 43322 );
	REQUIRE ( result[0][4].arcs()          == 56789 );
	REQUIRE ( result[0][4].confidence()    ==    21 );
	REQUIRE ( result[0][4].frame450_arcs() == 45533 );

	SECTION ( "Copy constructor" )
	{
		arcs::DefaultContentHandler c_handler_copy(c_handler);

		auto result_copy = c_handler_copy.result();

		REQUIRE ( result_copy.size()    == 1 );
		REQUIRE ( result_copy[0].size() == 5 );
		REQUIRE ( result_copy[0].id()   == arcs::ARId(5, 123, 456, 789) );
		REQUIRE ( result_copy[0][0].arcs()          == 12345 );
		REQUIRE ( result_copy[0][0].confidence()    ==    20 );
		REQUIRE ( result_copy[0][0].frame450_arcs() == 45551 );
		REQUIRE ( result_copy[0][1].arcs()          == 23456 );
		REQUIRE ( result_copy[0][1].confidence()    ==    20 );
		REQUIRE ( result_copy[0][1].frame450_arcs() == 56677 );
		REQUIRE ( result_copy[0][2].arcs()          == 34567 );
		REQUIRE ( result_copy[0][2].confidence()    ==    21 );
		REQUIRE ( result_copy[0][2].frame450_arcs() == 65599 );
		REQUIRE ( result_copy[0][3].arcs()          == 45678 );
		REQUIRE ( result_copy[0][3].confidence()    ==    21 );
		REQUIRE ( result_copy[0][3].frame450_arcs() == 43322 );
		REQUIRE ( result_copy[0][4].arcs()          == 56789 );
		REQUIRE ( result_copy[0][4].confidence()    ==    21 );
		REQUIRE ( result_copy[0][4].frame450_arcs() == 45533 );
	}
}


TEST_CASE ( "DefaultErrorHandler", "[defaulterrorhandler]" )
{
	arcs::DefaultErrorHandler e_handler;
}


TEST_CASE ( "ARFileParser", "[parse] [arfileparser]" )
{
	arcs::ARFileParser parser;

	// content handler but no error handler
	auto c_handler = std::make_unique<arcs::DefaultContentHandler>();
	parser.set_content_handler(std::move(c_handler));


	SECTION ( "Parse valid file" )
	{
		parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f.bin");

		// we positively _know_ the derived type, so downcast is ok
		auto result = dynamic_cast<const arcs::DefaultContentHandler &>
			(parser.content_handler()).result();


		REQUIRE ( result.size() == 3 );

		// Block 1

		REQUIRE ( result[0].id() ==
				arcs::ARId(15, 0x001b9178, 0x014be24e, 0xb40d2d0f) );

		REQUIRE ( result[0].size() == 15 );

		REQUIRE ( result[0][0].arcs()           == 0xB89992E5 );
		REQUIRE ( result[0][0].confidence()     == 24 );
		REQUIRE ( result[0][0].frame450_arcs()  == 0x126D875E );

		REQUIRE ( result[0][1].arcs()           == 0x4F77EB03  );
		REQUIRE ( result[0][1].confidence()     == 24 );
		REQUIRE ( result[0][1].frame450_arcs()  == 0xABF770DA );

		REQUIRE ( result[0][2].arcs()           == 0x56582282 );
		REQUIRE ( result[0][2].confidence()     == 24 );
		REQUIRE ( result[0][2].frame450_arcs()  == 0x80ACAF3C );

		REQUIRE ( result[0][3].arcs()           == 0x9E2187F9  );
		REQUIRE ( result[0][3].confidence()     == 24 );
		REQUIRE ( result[0][3].frame450_arcs()  == 0x8EB77C86 );

		REQUIRE ( result[0][4].arcs()           == 0x6BE71E50  );
		REQUIRE ( result[0][4].confidence()     == 24 );
		REQUIRE ( result[0][4].frame450_arcs()  == 0xF62D90FC );

		REQUIRE ( result[0][5].arcs()           == 0x01E7235F  );
		REQUIRE ( result[0][5].confidence()     == 24 );
		REQUIRE ( result[0][5].frame450_arcs()  == 0x56C6AF06 );

		REQUIRE ( result[0][6].arcs()           == 0xD8F7763C );
		REQUIRE ( result[0][6].confidence()     == 24 ) ;
		REQUIRE ( result[0][6].frame450_arcs()  == 0x76274140 );

		REQUIRE ( result[0][7].arcs()           == 0x8480223E );
		REQUIRE ( result[0][7].confidence()     == 24 ) ;
		REQUIRE ( result[0][7].frame450_arcs()  == 0x73A608D0 );

		REQUIRE ( result[0][8].arcs()           == 0x42C5061C );
		REQUIRE ( result[0][8].confidence()     == 24 ) ;
		REQUIRE ( result[0][8].frame450_arcs()  == 0x9D7A1F4B );

		REQUIRE ( result[0][9].arcs()           == 0x47A70F02 );
		REQUIRE ( result[0][9].confidence()     == 23 ) ;
		REQUIRE ( result[0][9].frame450_arcs()  == 0x37871A8C );

		REQUIRE ( result[0][10].arcs()          == 0xBABF08CC );
		REQUIRE ( result[0][10].confidence()    == 23 ) ;
		REQUIRE ( result[0][10].frame450_arcs() == 0xF6360C0B );

		REQUIRE ( result[0][11].arcs()          == 0x563EDCCB );
		REQUIRE ( result[0][11].confidence()    == 23 ) ;
		REQUIRE ( result[0][11].frame450_arcs() == 0xCB1FE45D );

		REQUIRE ( result[0][12].arcs()          == 0xAB123C7C );
		REQUIRE ( result[0][12].confidence()    == 23 ) ;
		REQUIRE ( result[0][12].frame450_arcs() == 0xBCC08EDA );

		REQUIRE ( result[0][13].arcs()          == 0xC65C20E4 );
		REQUIRE ( result[0][13].confidence()    == 22 ) ;
		REQUIRE ( result[0][13].frame450_arcs() == 0xE467DE8E );

		REQUIRE ( result[0][14].arcs()          == 0x58FC3C3E );
		REQUIRE ( result[0][14].confidence()    == 22 ) ;
		REQUIRE ( result[0][14].frame450_arcs() == 0x9537953F );

		// Block 2

		REQUIRE ( result[1].id() ==
				arcs::ARId(15, 0x001b9178, 0x014be24e, 0xb40d2d0f) );

		REQUIRE ( result[1].size() == 15 );

		REQUIRE ( result[1][0].arcs()           == 0x98B10E0F );
		REQUIRE ( result[1][0].confidence()     == 20 );
		REQUIRE ( result[1][0].frame450_arcs()  == 0x35DC25F3 );

		REQUIRE ( result[1][1].arcs()           == 0x475F57E9 );
		REQUIRE ( result[1][1].confidence()     == 20 );
		REQUIRE ( result[1][1].frame450_arcs()  == 0xDCCF2356 );

		REQUIRE ( result[1][2].arcs()           == 0x7304F1C4 );
		REQUIRE ( result[1][2].confidence()     == 20 );
		REQUIRE ( result[1][2].frame450_arcs()  == 0x5FA89D66 );

		REQUIRE ( result[1][3].arcs()           == 0xF2472287 );
		REQUIRE ( result[1][3].confidence()     == 20 );
		REQUIRE ( result[1][3].frame450_arcs()  == 0xB0330387 );

		REQUIRE ( result[1][4].arcs()           == 0x881BC504 );
		REQUIRE ( result[1][4].confidence()     == 20 );
		REQUIRE ( result[1][4].frame450_arcs()  == 0x8442806E );

		REQUIRE ( result[1][5].arcs()           == 0xBB94BFD4 );
		REQUIRE ( result[1][5].confidence()     == 20 );
		REQUIRE ( result[1][5].frame450_arcs()  == 0xF13BC09B );

		REQUIRE ( result[1][6].arcs()           == 0xF9CAEE76 );
		REQUIRE ( result[1][6].confidence()     == 20 );
		REQUIRE ( result[1][6].frame450_arcs()  == 0xC0AB9412 );

		REQUIRE ( result[1][7].arcs()           == 0xF9F60BC1 );
		REQUIRE ( result[1][7].confidence()     == 20 );
		REQUIRE ( result[1][7].frame450_arcs()  == 0xC7836441 );

		REQUIRE ( result[1][8].arcs()           == 0x2C736302 );
		REQUIRE ( result[1][8].confidence()     == 19 );
		REQUIRE ( result[1][8].frame450_arcs()  == 0xF1FD38D3 );

		REQUIRE ( result[1][9].arcs()           == 0x1C955978 );
		REQUIRE ( result[1][9].confidence()     == 20 );
		REQUIRE ( result[1][9].frame450_arcs()  == 0x0860E08B );

		REQUIRE ( result[1][10].arcs()          == 0xFDA6D833 );
		REQUIRE ( result[1][10].confidence()    == 19 );
		REQUIRE ( result[1][10].frame450_arcs() == 0x9C8202BE );

		REQUIRE ( result[1][11].arcs()          == 0x3A57E5D1 );
		REQUIRE ( result[1][11].confidence()    == 21 );
		REQUIRE ( result[1][11].frame450_arcs() == 0xC6FF01AE );

		REQUIRE ( result[1][12].arcs()          == 0x6ED5F3E7 );
		REQUIRE ( result[1][12].confidence()    == 19 );
		REQUIRE ( result[1][12].frame450_arcs() == 0x4C92FCBE );

		REQUIRE ( result[1][13].arcs()          == 0x4A5C3872 );
		REQUIRE ( result[1][13].confidence()    == 20 );
		REQUIRE ( result[1][13].frame450_arcs() == 0x8FB684A7 );

		REQUIRE ( result[1][14].arcs()          == 0x5FE8B032 );
		REQUIRE ( result[1][14].confidence()    == 19 );
		REQUIRE ( result[1][14].frame450_arcs() == 0x405711AA );

		// Block 3

		REQUIRE ( result[2][0].arcs()           == 0x54FE0533 );
		REQUIRE ( result[2][0].confidence()     == 2 );
		REQUIRE ( result[2][0].frame450_arcs()  == 0 );

		REQUIRE ( result[2][1].arcs()           == 0xAFEC147E );
		REQUIRE ( result[2][1].confidence()     == 2 );
		REQUIRE ( result[2][1].frame450_arcs()  == 0 );

		REQUIRE ( result[2][2].arcs()           == 0x2BFB5AEC );
		REQUIRE ( result[2][2].confidence()     == 2 );
		REQUIRE ( result[2][2].frame450_arcs()  == 0 );

		REQUIRE ( result[2][3].arcs()           == 0xED6E7215 );
		REQUIRE ( result[2][3].confidence()     == 2 );
		REQUIRE ( result[2][3].frame450_arcs()  == 0 );

		REQUIRE ( result[2][4].arcs()           == 0xA0847CEF );
		REQUIRE ( result[2][4].confidence()     == 2 );
		REQUIRE ( result[2][4].frame450_arcs()  == 0 );

		REQUIRE ( result[2][5].arcs()           == 0xAE066CD1 );
		REQUIRE ( result[2][5].confidence()     == 2 );
		REQUIRE ( result[2][5].frame450_arcs()  == 0 );

		REQUIRE ( result[2][6].arcs()           == 0xE1AB3B46 );
		REQUIRE ( result[2][6].confidence()     == 2 );
		REQUIRE ( result[2][6].frame450_arcs()  == 0 );

		REQUIRE ( result[2][7].arcs()           == 0xE75E70BA );
		REQUIRE ( result[2][7].confidence()     == 2 );
		REQUIRE ( result[2][7].frame450_arcs()  == 0 );

		REQUIRE ( result[2][8].arcs()           == 0x6D72D1AC );
		REQUIRE ( result[2][8].confidence()     == 2 );
		REQUIRE ( result[2][8].frame450_arcs()  == 0 );

		REQUIRE ( result[2][9].arcs()           == 0x89C19A02 );
		REQUIRE ( result[2][9].confidence()     == 2 );
		REQUIRE ( result[2][9].frame450_arcs()  == 0 );

		REQUIRE ( result[2][10].arcs()          == 0x4A5CE2AB );
		REQUIRE ( result[2][10].confidence()    == 2 );
		REQUIRE ( result[2][10].frame450_arcs() == 0 );

		REQUIRE ( result[2][11].arcs()          == 0x4D23C1D4 );
		REQUIRE ( result[2][11].confidence()    == 2 );
		REQUIRE ( result[2][11].frame450_arcs() == 0 );

		REQUIRE ( result[2][12].arcs()          == 0x80AA0FB6 );
		REQUIRE ( result[2][12].confidence()    == 2 );
		REQUIRE ( result[2][12].frame450_arcs() == 0 );

		REQUIRE ( result[2][13].arcs()          == 0x9378FD52 );
		REQUIRE ( result[2][13].confidence()    == 2 );
		REQUIRE ( result[2][13].frame450_arcs() == 0 );

		REQUIRE ( result[2][14].arcs()          == 0x6A8A614C );
		REQUIRE ( result[2][14].confidence()    == 2 );
		REQUIRE ( result[2][14].frame450_arcs() == 0 );
	}

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
	// access the "what" message via REQUIRE_THROWS_WITH).
	//
	// The following workaround addresses this by requiring an exception
	// manually and catching the exception object.
	//
	// Confer: https://github.com/catchorg/Catch2/issues/394


	SECTION ( "Parse files with incomplete header: no disc id1 (pos 1)" )
	{
		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+01.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 1 );
			REQUIRE ( e.byte_position()       == 149 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id1 (pos 2, 3 or 4)" )
	{
		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+02.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 2 );
			REQUIRE ( e.byte_position()       == 150 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+03.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 3 );
			REQUIRE ( e.byte_position()       == 151 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+04.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 4 );
			REQUIRE ( e.byte_position()       == 152 );
		}
	}


	SECTION ( "Parse files with incomplete header: no disc id2 (pos 5)" )
	{
		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+05.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 5 );
			REQUIRE ( e.byte_position()       == 153 );
		}
	}


	SECTION ( "Parse files with incomplete header: disc id2 (pos 6, 7 or 8)" )
	{
		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+06.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 6 );
			REQUIRE ( e.byte_position()       == 154 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+07.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 7 );
			REQUIRE ( e.byte_position()       == 155 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+08.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 8 );
			REQUIRE ( e.byte_position()       == 156 );
		}
	}


	SECTION ( "Parse files with incomplete header: no cddb id (pos 9)" )
	{
		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+09.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 9 );
			REQUIRE ( e.byte_position()       == 157 );
		}
	}


	SECTION ( "Parse files with incomplete header: cddb id (pos 10, 11 or 12)" )
	{
		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+10.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 10 );
			REQUIRE ( e.byte_position()       == 158 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+11.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 11 );
			REQUIRE ( e.byte_position()       == 159 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+12.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 12 );
			REQUIRE ( e.byte_position()       == 160 );
		}
	}


	SECTION ( "Parse files with incomplete block: only header" )
	{
		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_H+13.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 13 );
			REQUIRE ( e.byte_position()       == 161 );
		}
	}


	SECTION ( "Parse files with triplet missing (triplet pos 0)" )
	{
		try
		{
			// End of a triplet + 0 byte => one or more triplets missing

			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+0.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 139 );
			REQUIRE ( e.byte_position()       == 287 );
		}
	}


	SECTION ( "Parse files with missing ARCS (triplet pos 1)" )
	{
		try
		{
			// End of last triplet + 1 byte =>
			// triplet invalid, confidence ok, ARCS missing

			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+1.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 140 );
			REQUIRE ( e.byte_position()       == 288 );
		}
	}


	SECTION ( "Parse files with broken ARCS (triplet pos 2,3 or 4)" )
	{
		// End of last triplet + 2,3 or 4 bytes =>
		// triplet invalid, confidence ok, ARCS incomplete

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+2.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 141 );
			REQUIRE ( e.byte_position()       == 289 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+3.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 142 );
			REQUIRE ( e.byte_position()       == 290 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+4.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 143 );
			REQUIRE ( e.byte_position()       == 291 );
		}
	}


	SECTION ( "Parse files with missing frame450_arcs (triplet pos 5)" )
	{
		// End of last triplet + 5 bytes =>
		// triplet invalid, confidence + ARCS ok, frame450_arcs missing

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+5.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 144 );
			REQUIRE ( e.byte_position()       == 292 );
		}
	}


	SECTION ( "Parse files with broken frame450_arcs (triplet pos 6,7 or 8)" )
	{
		// End of last triplet + 6,7 or 8 bytes =>
		// triplet invalid, confidence + ARCS ok, frame450_arcs incomplete

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+6.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 145 );
			REQUIRE ( e.byte_position()       == 293 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+7.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 146 );
			REQUIRE ( e.byte_position()       == 294 );
		}

		try
		{
			parser.parse("dBAR-015-001b9178-014be24e-b40d2d0f_T+8.bin");

			FAIL ( "Expected StreamReadException was not thrown" );
		} catch (const arcs::StreamReadException &e)
		{
			REQUIRE ( e.block()               == 2 );
			REQUIRE ( e.block_byte_position() == 147 );
			REQUIRE ( e.byte_position()       == 295 );
		}
	}
}

