#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for TrackTraversal.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"          // for TrackTraversal
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "TrackTraversal is non-abstract, final and non-empty",
		"[tracktraversal] [verify] [verify]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::TrackTraversal> );
		CHECK ( ! std::is_abstract_v<arcstk::TrackTraversal> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::TrackTraversal> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::TrackTraversal> );
	}
}


TEST_CASE ( "TrackTraversal special members",
		"[tracktraversal] [verify] [verify]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::TrackTraversal>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::TrackTraversal>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::TrackTraversal>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::TrackTraversal>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::TrackTraversal>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::TrackTraversal> );
	}
}


TEST_CASE ( "TrackTraversal is swappable, comparable, string convertible",
		"[tracktraversal] [verify] [verify]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::TrackTraversal> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::TrackTraversal> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::TrackTraversal>::value );
	}
}

