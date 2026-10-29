#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Appender.
 */

#include <string>               // for to_string

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"     // for Copy, Move
#endif
#ifndef LIBARCSTK_TEMPFILE_HPP_
#include "tempfile.hpp"         // for TemporaryFile
#endif


TEST_CASE ( "Appender", "[appender] [logging]" )
{
	//using arcstk::testing::Copy; // no copy possible
	using arcstk::testing::Move;
	using arcstk::testing::TemporaryFile;

	using arcstk::Appender;

	const auto tempfile_prefix = std::string { "LIBARCSTK-TEST-UNIT-Appender" };

    TemporaryFile tempfile { tempfile_prefix };

	auto instance = Appender { tempfile.string() };


	SECTION ("Parametized construction is correct")
	{
		CHECK ( ! instance.name().empty() );
	}

	SECTION ("Move construction is correct")
	{
		const auto path = instance.name();

		// Test: is the name_hint part of the actual name?

		REQUIRE ( path.find(tempfile_prefix) != std::string::npos );

		const Appender moved { Move<Appender>::construct(instance) };

        CHECK ( moved.name().find(tempfile_prefix) != std::string::npos );
	}

	SECTION ("Move assignment is correct")
	{
		const auto path = instance.name();

		// Test: is the name_hint part of the actual name?

		REQUIRE ( path.find(tempfile_prefix) != std::string::npos );

		const auto tempfile2_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender2" };

		TemporaryFile tempfile2 { tempfile2_prefix };

		auto moved = Appender { tempfile2.string() };
		Move<Appender>::assign(moved, instance);

        CHECK ( moved.name().find(tempfile_prefix) != std::string::npos );
	}
}


TEST_CASE ( "Appender property", "[appender] [logging]" )
{
	using arcstk::Appender;
	using arcstk::testing::TemporaryFile;

	SECTION ("swap() is correct")
	{
		const auto file1_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-52307562316"
		};

		const auto file2_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-41296451205"
		};

		TemporaryFile tempfile1 { file1_prefix };
		TemporaryFile tempfile2 { file2_prefix };

		auto ap1 = Appender { tempfile1.string() };
		auto ap2 = Appender { tempfile2.string() };

		REQUIRE ( ap1.name() == tempfile1.string() );
		REQUIRE ( ap2.name() == tempfile2.string() );

		using std::swap;
		swap(ap1, ap2);

		CHECK ( ap1.name() == tempfile2.string() );
		CHECK ( ap2.name() == tempfile1.string() );
	}
}


TEST_CASE ( "Appender functions", "[appender] [logging]" )
{
	using arcstk::Appender;
	using arcstk::testing::TemporaryFile;

	SECTION ("append() is correct")
	{
		const auto file_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-63418673427"
		};

		TemporaryFile tempfile { file_prefix };

		auto apdr = Appender { tempfile.string() };

		CHECK_NOTHROW ( apdr.append("This does not cause an exception") );
	}

	SECTION ("name() is correct")
	{
		const auto file_prefix = std::string {
			"LIBARCSTK-TEST-UNIT-Appender-74529784538"
		};

		TemporaryFile tempfile { file_prefix };

		auto apdr = Appender { tempfile.string() };


		// Test: is the name_hint part of the actual name?

		CHECK ( apdr.name().find(file_prefix) != std::string::npos );
	}
}

