#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for AudioSize.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"          // for AudioSize
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "AudioSize is non-abstract, final and non-empty",
		"[audiosize] [metadata] [metadata]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::AudioSize> );
		CHECK ( ! std::is_abstract_v<arcstk::AudioSize> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::AudioSize> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::AudioSize> );
	}
}


TEST_CASE ( "AudioSize special members",
		"[audiosize] [metadata] [metadata]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::AudioSize>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::AudioSize>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::AudioSize>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::AudioSize>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::AudioSize>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::AudioSize> );
	}
}


TEST_CASE ( "AudioSize is swappable, comparable, string convertible",
		"[audiosize] [metadata] [metadata]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::AudioSize> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::AudioSize> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::AudioSize>::value );
	}
}

