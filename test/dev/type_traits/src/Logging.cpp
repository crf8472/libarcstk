#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Logging.
 */

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // for Logging
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Logging is non-abstract, final and non-empty",
		"[logging] [logging] [logging]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Logging> );
		CHECK ( ! std::is_abstract_v<arcstk::Logging> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Logging> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Logging> );
	}
}


TEST_CASE ( "Logging special members",
		"[logging] [logging] [logging]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::Logging>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::Logging>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::Logging>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::Logging>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::Logging>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Logging> );
	}
}


TEST_CASE ( "Logging is swappable, comparable, string convertible",
		"[logging] [logging] [logging]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::Logging> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::Logging> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::Logging>::value );
	}
}

