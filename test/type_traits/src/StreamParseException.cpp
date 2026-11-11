#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for StreamParseException.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // for StreamParseException
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "StreamParseException is non-abstract, final and non-empty",
		"[streamparseexception][dbar]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::StreamParseException> );
		CHECK ( ! std::is_abstract_v<arcstk::StreamParseException> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::StreamParseException> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::StreamParseException> );
	}
}


TEST_CASE ( "StreamParseException special members",
		"[streamparseexception][dbar]" )
{
	SECTION ( "is not default constructable")
	{
		CHECK ( ! std::is_default_constructible_v<arcstk::StreamParseException>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::StreamParseException>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::StreamParseException>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::StreamParseException>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::StreamParseException>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::StreamParseException> );
	}
}


TEST_CASE ( "StreamParseException is swappable",
		"[streamparseexception][dbar]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::StreamParseException> );
	}
}

