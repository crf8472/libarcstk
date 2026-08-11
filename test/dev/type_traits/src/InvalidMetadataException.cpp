#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for InvalidMetadataException.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"          // for InvalidMetadataException
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "InvalidMetadataException is non-abstract, final and non-empty",
		"[invalidmetadataexception] [metadata] [metadata]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::InvalidMetadataException> );
		CHECK ( ! std::is_abstract_v<arcstk::InvalidMetadataException> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::InvalidMetadataException> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::InvalidMetadataException> );
	}
}


TEST_CASE ( "InvalidMetadataException special members",
		"[invalidmetadataexception] [metadata] [metadata]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::InvalidMetadataException>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::InvalidMetadataException>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::InvalidMetadataException>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::InvalidMetadataException>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::InvalidMetadataException>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::InvalidMetadataException> );
	}
}


TEST_CASE ( "InvalidMetadataException is swappable, comparable, string convertible",
		"[invalidmetadataexception] [metadata] [metadata]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::InvalidMetadataException> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::InvalidMetadataException> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::InvalidMetadataException>::value );
	}
}

