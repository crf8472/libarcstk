#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for dbar.hpp
 */

#ifndef __LIBARCSTK_DBAR_HPP__
#include "dbar.hpp"               // TO BE TESTED
#endif
#ifndef __LIBARCSTK_DBAR_DETAILS_HPP__
#include "dbar_details.hpp"       // for parse_dbar_stream
#endif

#include <fstream>                // for ifstream
#include <memory>                 // for make_unique, unique_ptr


TEST_CASE ( "DBARBlock", "[dbarblock]" )
{
	using arcstk::DBAR;
	using arcstk::DBARBlock;
	using arcstk::get_element;

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
			{ 0x58FC3C3E, 28, 0x6F309B40 }
		} },
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
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

	auto block = DBARBlock { dBAR, 1 };

	SECTION ( "Can be forward-iterated correctly" )
	{
		auto it { block.begin() };

		CHECK ( it->arcs() == 0x98B10E0F );
		CHECK ( it->confidence() == 3 );
		CHECK ( it->frame450_arcs() == 0xC19172F9 );

		++it;

		CHECK ( it->arcs() == 0x475F57E9 );
		CHECK ( it->confidence() == 4 );
		CHECK ( it->frame450_arcs() == 0x6F71EA01 );
	}

	SECTION ( "Can be accessed correctly by get_element()" )
	{
		auto mytriplet = get_element(block, 0);

		CHECK ( mytriplet.arcs()          == 0x98B10E0F );
		CHECK ( mytriplet.confidence()    == 3 );
		CHECK ( mytriplet.frame450_arcs() == 0xC19172F9 );
	}
}

TEST_CASE ( "DBAR Construction by DBARBuilder", "[dbar]")
{
	using arcstk::DBAR;
	using arcstk::DBARBuilder;
	using arcstk::details::parse_dbar_stream;

	DBARBuilder builder;

	{
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string { "Failed to open file "
					"'dBAR-015-001b9178-014be24e-b40d2d0f.bin', got: " }
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		const auto b = parse_dbar_stream(file, &builder, nullptr);

		REQUIRE ( b == 444 );
	}

	const auto dBAR_file = builder.result();


	SECTION ( "DBARBuilder constructs DBAR with correct values" )
	{
		CHECK ( dBAR_file.size() == 3 );

		CHECK ( dBAR_file.block(0).size() == 15 );
		CHECK ( dBAR_file.block(1).size() == 15 );
		CHECK ( dBAR_file.block(2).size() == 15 );

		const auto block0 = dBAR_file.block(0);

		CHECK ( block0.size() == 15 );
		CHECK ( block0.index() == 0 );

		const auto header0 = dBAR_file.block(0).header();

		CHECK ( header0.total_tracks() == 15 );
		CHECK ( header0.id1() == 0x001b9178 );
		CHECK ( header0.id2() == 0x014be24e );
		CHECK ( header0.cddb_id() == 0xb40d2d0f );

		const auto block1 = dBAR_file.block(1);
		CHECK ( block1.index() == 1 );

		const auto header1 = dBAR_file.block(1).header();

		CHECK ( header1.total_tracks() == 15 );
		CHECK ( header1.id1() == 0x001b9178 );
		CHECK ( header1.id2() == 0x014be24e );
		CHECK ( header1.cddb_id() == 0xb40d2d0f );

		const auto block2 = dBAR_file.block(2);
		CHECK ( block2.index() == 2 );

		const auto header2 = dBAR_file.block(2).header();

		CHECK ( header2.total_tracks() == 15 );
		CHECK ( header2.id1() == 0x001b9178 );
		CHECK ( header2.id2() == 0x014be24e );
		CHECK ( header2.cddb_id() == 0xb40d2d0f );


		CHECK ( block0.triplet(0).arcs() == 0xB89992E5 );
		CHECK ( block0.triplet(0).frame450_arcs() == 0x126D875E );
		CHECK ( block0.triplet(0).confidence() == 24 );

		// TODO block 0, track 1 - 14

		CHECK ( block1.triplet(0).arcs() == 0x98B10E0F );
		CHECK ( block1.triplet(0).frame450_arcs() == 0x35DC25F3 );
		CHECK ( block1.triplet(0).confidence() == 20 );

		// TODO block 1, track 1 - 14
	}

	SECTION ( "Range-based for loop works correctly" )
	{
		for (const auto& block : dBAR_file)
		{
			CHECK ( block.header().total_tracks() > 0 );

			for (const auto& triplet : block)
			{
				CHECK ( triplet.arcs() > 0 );
			}
		}
	}
}

TEST_CASE ( "DBAR", "[dbar]" )
{
	using arcstk::DBAR;

	const auto dBAR = DBAR {
		{ { 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
		{ /* triplets */
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
		} },
		{ { 15, 0x101B9178, 0xB14BE24E, 0x540C2D0A },
		{ /* triplets */
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
		} }
	};

	SECTION ( "DBAR initializer_list constructor works correctly")
	{
		CHECK ( dBAR.size() == 2 );

		const auto block0 = dBAR.block(0);

		CHECK ( block0.header().total_tracks() == 15 );
		CHECK ( block0.header().id1() == 0x001B9178 );
		CHECK ( block0.header().id2() == 0x014BE24E );
		CHECK ( block0.header().cddb_id() == 0xB40D2D0F );
		CHECK ( block0.size() == 15 );

		const auto block1 = dBAR.block(1);

		CHECK ( block1.header().total_tracks() == 15 );
		CHECK ( block1.header().id1() == 0x101B9178 );
		CHECK ( block1.header().id2() == 0xB14BE24E );
		CHECK ( block1.header().cddb_id() == 0x540C2D0A );
		CHECK ( block1.size() == 15 );
	}

	SECTION ( "Can be forward-iterated correctly" )
	{
		auto it { dBAR.begin() };

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

	SECTION ( "Range-based for loop on initalizer_list constructed DBAR works correctly" )
	{
		for (const auto& block : dBAR)
		{
			CHECK ( block.header().total_tracks() > 0 );

			for (const auto& triplet : block)
			{
				CHECK ( triplet.arcs() > 0 );
			}
		}
	}

	SECTION ( "Copy constructor works correctly" )
	{
		const auto dBAR_copy { dBAR };

		CHECK ( dBAR_copy.size() == 2 );

		const auto block0 = dBAR_copy.block(0);

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

		const auto block1 = dBAR_copy.block(1);

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

	SECTION ( "Range-based for loop on copy-constructed DBAR works correctly" )
	{
		for (const auto& block : dBAR)
		{
			CHECK ( block.header().total_tracks() > 0 );

			for (const auto& triplet : block)
			{
				CHECK ( triplet.arcs() > 0 );
			}
		}
	}

	SECTION ( "Move constructor works correctly" )
	{
		const auto dBAR_move { std::move(dBAR) };

		//REQUIRE ( dBAR.size() == 0 );
		//REQUIRE ( dBAR.block(0).size() == 0 );
		//REQUIRE ( dBAR.block(1).size() == 0 );

		CHECK ( dBAR_move.size() == 2 );

		const auto block0 = dBAR_move.block(0);

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

		const auto block1 = dBAR_move.block(1);

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

	SECTION ( "DBAR default constructor works correctly" )
	{
		DBAR dBAR1;

		CHECK ( dBAR1.size() == 0 );
		CHECK ( dBAR1.empty() );

		DBAR dBAR2 {/*empty*/};

		CHECK ( dBAR2.size() == 0 );
		CHECK ( dBAR2.empty() );
	}

}

TEST_CASE ( "parse_dbar_stream", "[parse_dbar_stream]" )
{
	using arcstk::details::parse_dbar_stream;
	using arcstk::DBARBuilder;

	DBARBuilder builder;

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

	SECTION ( "Parse intact file" )
	{
		try
		{
			file.open("dBAR-015-001b9178-014be24e-b40d2d0f.bin",
					std::ifstream::in | std::ifstream::binary);
		}
		catch (const std::ifstream::failure& f)
		{
			throw std::runtime_error(
				std::string{"Failed to open file "
				"'dBAR-015-001b9178-014be24e-b40d2d0f.bin', got: "}
				+ typeid(f).name()
				+ ", message: " + f.what());
		}

		auto b = parse_dbar_stream(file, &builder, nullptr);

		CHECK ( b == 444 );
	}

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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
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
			auto b = parse_dbar_stream(file, &builder, nullptr);
			static_cast<void>(b); // to avoid -Wunused-variable

			FAIL ( "Expected StreamParseException was not thrown" );
		} catch (const arcstk::StreamParseException &e)
		{
			CHECK ( e.block()               == 2 );
			CHECK ( e.block_byte_position() == 147 );
			CHECK ( e.byte_position()       == 295 );
		}
	}
}

