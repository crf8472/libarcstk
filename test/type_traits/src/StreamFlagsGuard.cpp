#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for StreamFlagsGuard.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"            // for StreamFlagsGuard
#endif

#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "StreamFlagsGuard is non-abstract, final and non-empty",
		"[streamflagsguard][calc][checksum]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::StreamFlagsGuard> );
		CHECK ( ! std::is_abstract_v<arcstk::details::StreamFlagsGuard> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::StreamFlagsGuard> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::details::StreamFlagsGuard> );
	}
}


TEST_CASE ( "StreamFlagsGuard special members",
		"[streamflagsguard][calc][checksum]" )
{
	SECTION ( "is not default constructable")
	{
		CHECK ( ! std::is_default_constructible_v<arcstk::details::StreamFlagsGuard>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::details::StreamFlagsGuard> );
	}

	SECTION ( "is not copy-constructable")
	{
		CHECK ( ! std::is_copy_constructible_v<arcstk::details::StreamFlagsGuard>);
	}

	SECTION ( "is not copy-assignable")
	{
		CHECK ( ! std::is_copy_assignable_v<arcstk::details::StreamFlagsGuard>);
	}

	SECTION ( "is not nothrow move-constructable")
	{
		CHECK ( ! std::is_nothrow_move_constructible_v<arcstk::details::StreamFlagsGuard>);
	}

	SECTION ( "is not nothrow move-assignable")
	{
		CHECK ( ! std::is_nothrow_move_assignable_v<arcstk::details::StreamFlagsGuard>);
	}
}

