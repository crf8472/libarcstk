#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for UpdateableSubtotals<>.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"         // for UpdateableSubtotals
#endif

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"            // for checksum::type
#endif

#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "UpdateableSubtotals is non-abstract, final and non-empty",
		"[updateablesubtotals][calc][accuraterip]" )
{
	using arcstk::checksum::type;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>> );
		CHECK ( ! std::is_abstract_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>> );
	}
}


TEST_CASE ( "UpdateableSubtotals special members",
		"[updateablesubtotals][calc][accuraterip]" )
{
	using arcstk::checksum::type;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>> );
	}
}


TEST_CASE ( "UpdateableSubtotals is swappable",
		"[updateablesubtotals][calc][accuraterip]" )
{
	using arcstk::checksum::type;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::accuraterip::details::UpdateableSubtotals<type::ARCS1, type::ARCS2>> );
	}
}

