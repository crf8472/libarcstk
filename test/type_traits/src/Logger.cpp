#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Logger.
 */

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // for Logger
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Logger is non-abstract, final and non-empty",
		"[logger] [logging] [logging]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Logger> );
		CHECK ( ! std::is_abstract_v<arcstk::Logger> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Logger> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Logger> );
	}
}


TEST_CASE ( "Logger special members",
		"[logger] [logging] [logging]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::Logger>);
	}

	SECTION ( "is not copy-constructable")
	{
		CHECK ( !std::is_copy_constructible_v<arcstk::Logger>);
	}

	SECTION ( "is not copy-assignable")
	{
		CHECK ( !std::is_copy_assignable_v<arcstk::Logger>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::Logger>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::Logger>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Logger> );
	}
}

