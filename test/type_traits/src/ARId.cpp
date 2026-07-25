#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for ARId.
 */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"          // for ARId
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ARId is non-abstract, final and non-empty",
		"[arid] [id] [identifier]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::ARId> );
		CHECK ( ! std::is_abstract_v<arcstk::ARId> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::ARId> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::ARId> );
	}
}


TEST_CASE ( "ARId special members",
		"[arid] [id] [identifier]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::ARId>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::ARId>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::ARId>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::ARId>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::ARId>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::ARId> );
	}
}


TEST_CASE ( "ARId is swappable, comparable, string convertible",
		"[arid] [id] [identifier]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::ARId> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::ARId> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::ARId>::value );
	}
}

