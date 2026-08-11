#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for UpdateableSubtotals.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"          // for UpdateableSubtotals
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "UpdateableSubtotals is non-abstract, final and non-empty",
		"[updateablesubtotals] [calc] [accuraterip]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::UpdateableSubtotals> );
		CHECK ( ! std::is_abstract_v<arcstk::UpdateableSubtotals> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::UpdateableSubtotals> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::UpdateableSubtotals> );
	}
}


TEST_CASE ( "UpdateableSubtotals special members",
		"[updateablesubtotals] [calc] [accuraterip]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::UpdateableSubtotals>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::UpdateableSubtotals>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::UpdateableSubtotals>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::UpdateableSubtotals>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::UpdateableSubtotals>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::UpdateableSubtotals> );
	}
}


TEST_CASE ( "UpdateableSubtotals is swappable, comparable, string convertible",
		"[updateablesubtotals] [calc] [accuraterip]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::UpdateableSubtotals> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::UpdateableSubtotals> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::UpdateableSubtotals>::value );
	}
}

