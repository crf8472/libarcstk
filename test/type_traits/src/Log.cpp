#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Log.
 */

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // for Log
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Log is non-abstract, final and non-empty", "[log] [logging]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Log> );
		CHECK ( ! std::is_abstract_v<arcstk::Log> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Log> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Log> );
	}
}


TEST_CASE ( "Log special members", "[log] [logging]" )
{
	SECTION ( "is not default constructable")
	{
		CHECK ( !std::is_default_constructible_v<arcstk::Log>);
	}

	SECTION ( "is not copy-constructable")
	{
		CHECK ( !std::is_copy_constructible_v<arcstk::Log>);
	}

	SECTION ( "is not copy-assignable")
	{
		CHECK ( !std::is_copy_assignable_v<arcstk::Log>);
	}

	SECTION ( "is not nothrow move-constructable")
	{
		CHECK ( !std::is_nothrow_move_constructible_v<arcstk::Log>);
	}

	SECTION ( "is not nothrow move-assignable")
	{
		CHECK ( !std::is_nothrow_move_assignable_v<arcstk::Log>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Log> );
	}
}

