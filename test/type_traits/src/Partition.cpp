#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Partition.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for Partition
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "Partition is non-abstract, final and non-empty",
		"[partition] [calc] [calculate]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Partition> );
		CHECK ( ! std::is_abstract_v<arcstk::Partition> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Partition> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Partition> );
	}
}


TEST_CASE ( "Partition special members",
		"[partition] [calc] [calculate]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::Partition>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::Partition>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::Partition>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::Partition>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::Partition>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Partition> );
	}
}


TEST_CASE ( "Partition is swappable, comparable, string convertible",
		"[partition] [calc] [calculate]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::Partition> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::Partition> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::Partition>::value );
	}
}

