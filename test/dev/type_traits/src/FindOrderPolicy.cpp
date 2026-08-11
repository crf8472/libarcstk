#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for FindOrderPolicy.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"          // for FindOrderPolicy
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "FindOrderPolicy is non-abstract, final and non-empty",
		"[findorderpolicy] [verify] [verify]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::FindOrderPolicy> );
		CHECK ( ! std::is_abstract_v<arcstk::FindOrderPolicy> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::FindOrderPolicy> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::FindOrderPolicy> );
	}
}


TEST_CASE ( "FindOrderPolicy special members",
		"[findorderpolicy] [verify] [verify]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::FindOrderPolicy>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::FindOrderPolicy>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::FindOrderPolicy>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::FindOrderPolicy>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::FindOrderPolicy>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::FindOrderPolicy> );
	}
}


TEST_CASE ( "FindOrderPolicy is swappable, comparable, string convertible",
		"[findorderpolicy] [verify] [verify]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::FindOrderPolicy> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::FindOrderPolicy> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::FindOrderPolicy>::value );
	}
}

