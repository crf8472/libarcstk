#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for AlgorithmTypes<>.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for AlgorithmTypes
#endif
#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"         // for Version1
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "AlgorithmTypes is non-abstract, final and non-empty",
		"[algorithmtypes][calc][calculate]" )
{
	using arcstk::accuraterip::algorithm::Version1;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::AlgorithmTypes<Version1>> );
		CHECK ( ! std::is_abstract_v<arcstk::AlgorithmTypes<Version1>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::AlgorithmTypes<Version1>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::AlgorithmTypes<Version1>> );
	}
}


TEST_CASE ( "AlgorithmTypes<Version1> special members",
		"[algorithmtypes][calc][calculate]" )
{
	using arcstk::accuraterip::algorithm::Version1;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::AlgorithmTypes<Version1>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::AlgorithmTypes<Version1>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::AlgorithmTypes<Version1>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::AlgorithmTypes<Version1>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::AlgorithmTypes<Version1>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::AlgorithmTypes<Version1>> );
	}
}


TEST_CASE ( "AlgorithmTypes<Version1> is swappable, comparable, string convertible",
		"[algorithmtypes][calc][calculate]" )
{
	using arcstk::accuraterip::algorithm::Version1;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::AlgorithmTypes<Version1>> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::AlgorithmTypes<Version1>> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::AlgorithmTypes<Version1>>::value );
	}
}

