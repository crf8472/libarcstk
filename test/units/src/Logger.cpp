#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Logger.
 */

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"     // for Copy, Move
#endif
#ifndef LIBARCSTK_TEMPFILE_HPP_
#include "tempfile.hpp"         // for TemporaryFile
#endif


// TEST_CASE ( "Logger", "[logger] [logging]" )
// {
	// using arcstk::testing::Copy;
	// using arcstk::testing::Move;
	// using arcstk::testing::TemporaryFile;
	//
	// using arcstk::Appender;
	// using arcstk::Logger;
	//
	// const auto tempfile_prefix = std::string { "LIBARCSTK-TEST-UNIT-Logger" };
	//
	//    TemporaryFile tempfile { tempfile_prefix };
	//
	// auto appender = Appender { tempfile_prefix };

// 	auto instance = Logger {};
//
//
// 	SECTION ("Parametized construction is correct")
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ("Copy construction is correct")
// 	{
// 		const Logger copied { Copy<Logger>::construct(instance) };
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy construction test is missing" );
// 	}
//
// 	SECTION ("Move construction is correct")
// 	{
// 		const Logger moved { Move<Logger>::construct(instance) };
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move construction test is missing" );
// 	}
//
// 	SECTION ("Copy assignment is correct")
// 	{
// 		auto copied = Logger {};
// 		Copy<Logger>::assign(copied, instance);
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy assignment test is missing" );
// 	}
//
// 	SECTION ("Move assignment is correct")
// 	{
// 		auto moved = Logger {};
// 		Move<Logger>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move assignment test is missing" );
// 	}
// }


// TEST_CASE ( "Logger default constructed instance", "[logger] [logging]" )
// {
// 	using arcstk::Logger;
//
// 	auto defaulted = Logger {};
//
// 	SECTION ( "has_timestamps() provides default value")
// 	{
// 		CHECK ( !defaulted.has_timestamps() );
// 	}
//
// 	SECTION ( "step() provides default value")
// 	{
// 		CHECK ( defaulted.step() == 2 );
// 	}
//
// 	SECTION ( "indent_char() provides default value")
// 	{
// 		CHECK ( defaulted.indent_char() == ' ' );
// 	}
// }


// TEST_CASE ( "Logger property", "[logger] [logging] [logging]" )
// {
// 	using arcstk::Logger;
//
// 	auto instance = Logger {};
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


TEST_CASE ( "Logger functions", "[logger] [logging]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;
	using arcstk::testing::TemporaryFile;

	using arcstk::Logger;

	auto instance = Logger {};

	REQUIRE ( instance.step() == 2 );
	REQUIRE ( instance.indent_char() == ' ' );


	SECTION ("set_timestamps() is correct")
	{
		instance.set_timestamps(true);

		CHECK ( instance.has_timestamps() );
	}

	SECTION ("has_timestamps() is correct")
	{
		CHECK ( !instance.has_timestamps() );
	}

	SECTION ("set_step() is correct")
	{
		instance.set_step(5);

		CHECK ( 5 == instance.step() );
	}

	SECTION ("step() is correct")
	{
		CHECK ( 2 == instance.step() );
	}

	SECTION ("set_indent_char() is correct")
	{
		instance.set_indent_char('?');

		CHECK ( '?' == instance.indent_char() );
	}

	SECTION ("indent_char() is correct")
	{
		CHECK ( ' ' == instance.indent_char() );
	}

	SECTION ("add_appender() is correct")
	{
		using arcstk::Appender;
		const auto tempfile_prefix = std::string { "LIBARCSTK-TEST-UNIT-Logger" };
		TemporaryFile tempfile { tempfile_prefix };
		auto appender = std::make_unique<Appender>(tempfile_prefix);

		CHECK_NOTHROW ( instance.add_appender(std::move(appender)) );
	}

	SECTION ("remove_appender() is correct")
	{
		using arcstk::Appender;
		const auto tempfile_prefix = std::string { "LIBARCSTK-TEST-UNIT-Logger" };
		TemporaryFile tempfile { tempfile_prefix };
		auto appender = std::make_unique<Appender>(tempfile_prefix);

		const auto ptr = appender.get();
		instance.add_appender(std::move(appender));

		CHECK_NOTHROW ( instance.remove_appender(ptr) );
	}

	// SECTION ("log() is correct")
	// {
	// 	FAIL ("log() test is missing");
	// }
}

