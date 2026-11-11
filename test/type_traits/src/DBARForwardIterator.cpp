#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for DBARForwardIterator<>.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"                // for DBARForwardIterator
#endif

#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "DBARForwardIterator is non-abstract, final and non-empty",
		"[dbarforwarditerator][dbar][details]" )
{
	using arcstk::DBAR;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::DBARForwardIterator<DBAR, true>> );
		CHECK ( ! std::is_abstract_v<arcstk::details::DBARForwardIterator<DBAR, true>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::DBARForwardIterator<DBAR, true>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::details::DBARForwardIterator<DBAR, true>> );
	}
}


TEST_CASE ( "DBARForwardIterator special members",
		"[dbarforwarditerator][dbar][details]" )
{
	using arcstk::DBAR;

	using DBARForwardIterator_t =
		arcstk::details::DBARForwardIterator<DBAR, true>;

	SECTION ( "is not default constructable")
	{
		CHECK ( ! std::is_default_constructible_v<DBARForwardIterator_t>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<DBARForwardIterator_t>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<DBARForwardIterator_t>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<DBARForwardIterator_t>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<DBARForwardIterator_t>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<DBARForwardIterator_t> );
	}
}


TEST_CASE ( "DBARForwardIterator is swappable and comparable",
		"[dbarforwarditerator][dbar][details]" )
{
	using arcstk::DBAR;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::details::DBARForwardIterator<DBAR, true>> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::details::DBARForwardIterator<DBAR, true>> );
	}
}

