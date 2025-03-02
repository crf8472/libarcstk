#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for dbar_details.hpp.
 */

#ifndef __LIBARCSTK_DBAR_HPP__
#include "dbar.hpp"
#endif
#ifndef __LIBARCSTK_DBAR_DETAILS_HPP__
#include "dbar_details.hpp"       // TO BE TESTED
#endif

#include <fstream>                // for ifstream
#include <stdexcept>              // for runtime_error
#include <string>                 // for string

/**
 * \brief Complete test of possible input
 *
 * \details
 *
 * A parse error can only occurr if the input ends prematurely, e.g. if
 * an input block, for any reason, is not 13 + (n * 9) bytes long (with n
 * being the track number.
 *
 * 22 cases are tested:
 *
 * Header is 13 bytes long, hence there are 13 positions (after byte 1 - 13)
 * for errors that are covered by the input files *H_01-H_013.
 *
 * Triplet is 9 bytes long, hence there are 9 different positions (before
 * byte 1 and after byte 1 - 8) for errors that are covered by the input
 * files *T_00-T_08.

 * There is no easy or comfortable way in Catch2 to access the
 * exception object thrown via its genuine interface. (However, you can
 * access the "what" message via CHECK_THROWS_WITH).
 *
 * The following workaround addresses this by requiring an exception
 * manually and catching the exception object.
 *
 * Confer: https://github.com/catchorg/Catch2/issues/394
*/
TEST_CASE ( "parse_dbar_stream", "[parse_dbar_stream] [dbar]" )
{
	using arcstk::details::parse_dbar_stream;
	using arcstk::DBARBuilder;

	DBARBuilder builder;

	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);



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

