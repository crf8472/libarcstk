#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for ARCSAlgorithm.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"          // for ARCSAlgorithm
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ARCSAlgorithm is non-abstract, final and non-empty",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::ARCSAlgorithm> );
		CHECK ( ! std::is_abstract_v<arcstk::ARCSAlgorithm> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::ARCSAlgorithm> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::ARCSAlgorithm> );
	}
}


TEST_CASE ( "ARCSAlgorithm special members",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::ARCSAlgorithm>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::ARCSAlgorithm>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::ARCSAlgorithm>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::ARCSAlgorithm>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::ARCSAlgorithm>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::ARCSAlgorithm> );
	}
}


TEST_CASE ( "ARCSAlgorithm is swappable, comparable, string convertible",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::ARCSAlgorithm> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::ARCSAlgorithm> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::ARCSAlgorithm>::value );
	}
}

