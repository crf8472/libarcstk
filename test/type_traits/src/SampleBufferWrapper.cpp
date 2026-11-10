#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for SampleBufferWrapper<>.
 */

#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"             // for SampleBufferWrapper
#endif

#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "SampleBufferWrapper is non-abstract, final and non-empty",
		"[samplebufferwrapper][calc][samples]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::details::SampleBufferWrapper<int32_t, true>> );
		CHECK ( ! std::is_abstract_v<arcstk::details::SampleBufferWrapper<int32_t, true>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::details::SampleBufferWrapper<int32_t, true>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::details::SampleBufferWrapper<int32_t, true>> );
	}
}


TEST_CASE ( "SampleBufferWrapper special members",
		"[samplebufferwrapper][calc][samples]" )
{
	SECTION ( "is not default constructable")
	{
		CHECK ( ! std::is_default_constructible_v<arcstk::details::SampleBufferWrapper<int32_t, true>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::details::SampleBufferWrapper<int32_t, true>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::details::SampleBufferWrapper<int32_t, true>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::details::SampleBufferWrapper<int32_t, true>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::details::SampleBufferWrapper<int32_t, true>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::details::SampleBufferWrapper<int32_t, true>> );
	}
}


TEST_CASE ( "SampleBufferWrapper is swappable, comparable, string convertible",
		"[samplebufferwrapper][calc][samples]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::details::SampleBufferWrapper<int32_t, true>> );
	}

	SECTION ("has comparability")
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::details::SampleBufferWrapper<int32_t, true>> );
	}

	SECTION ("has string convertibility")
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::details::SampleBufferWrapper<int32_t, true>>::value );
	}
}

