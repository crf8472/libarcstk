#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for FindOrderPolicy.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"
#endif
#ifndef LIBARCSTK_VERIFY_DETAILS_HPP_
#include "verify_details.hpp"      // TO BE TESTED
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "FindOrderPolicy is non-abstract, final and non-empty",
		"[findorderpolicy][verify][details]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::FindOrderPolicy> );
		CHECK ( ! std::is_abstract_v<arcstk::details::FindOrderPolicy> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::FindOrderPolicy> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::details::FindOrderPolicy> );
	}
}


TEST_CASE ( "FindOrderPolicy special members",
		"[findorderpolicy][verify][details]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::details::FindOrderPolicy>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::details::FindOrderPolicy>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::details::FindOrderPolicy>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::details::FindOrderPolicy>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::details::FindOrderPolicy>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::details::FindOrderPolicy> );
	}
}


TEST_CASE ( "FindOrderPolicy is swappable",
		"[findorderpolicy][verify][details]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::details::FindOrderPolicy> );
	}
}

