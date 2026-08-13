#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Settings.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Settings", "[settings] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Context;
	using arcstk::Settings;

	auto instance = Settings { Context::LAST_TRACK };


	SECTION ("Parametized construction is correct")
	{
		auto settings = Settings { Context::TRACK };

		CHECK ( settings.context() == Context::TRACK );
	}

	SECTION ("Copy construction is correct")
	{
		const Settings copied { Copy<Settings>::construct(instance) };

		// --

        CHECK ( copied.context() == Context::LAST_TRACK );
	}

	SECTION ("Move construction is correct")
	{
		const Settings moved { Move<Settings>::construct(instance) };

		// --

        CHECK ( moved.context() == Context::LAST_TRACK );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Settings {};
		Copy<Settings>::assign(copied, instance);

		// --

        CHECK ( copied.context() == Context::LAST_TRACK );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Settings {};
		Move<Settings>::assign(moved, instance);

		// --

        CHECK ( moved.context() == Context::LAST_TRACK );
	}
}


TEST_CASE ( "Settings default constructed instance",
		"[settings] [calc] [calculate]" )
{
	using arcstk::Context;

	const auto defaulted = arcstk::Settings{};


	SECTION ( "has default values")
	{
		CHECK ( defaulted.context() == Context::ALBUM );
	}
}


TEST_CASE ( "Settings property", "[settings] [calc] [calculate]" )
{
	using arcstk::Context;
	using arcstk::Settings;

	auto instance = Settings {};


	SECTION ("swap() is correct")
	{
		auto instance2 = Settings { Context::FIRST_TRACK };

		using std::swap;
		swap(instance, instance2);

		// --

		CHECK ( instance.context()  == Context::FIRST_TRACK );
		CHECK ( instance2.context() == Context::ALBUM );
	}

	SECTION ("equals() is correct")
	{
		const auto instance_not_equal = Settings { Context::FIRST_TRACK };
		const auto instance_equal     = Settings { instance };

		// --

		CHECK ( instance.equals(instance_equal) );
		CHECK ( ! instance.equals(instance_not_equal) );
	}

	SECTION ("to_string() is correct")
	{
		CHECK ( instance.to_string() == "Context: ALBUM" );
	}
}

