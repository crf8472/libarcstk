#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for SampleSequence<>.
 */

#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"             // for SampleSequence
#endif

#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "SampleSequence is non-abstract, final and non-empty",
		"[samplesequence][calc][samples]" )
{
	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::SampleSequence<int32_t, true>> );
		CHECK ( ! std::is_abstract_v<arcstk::SampleSequence<int32_t, true>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::SampleSequence<int32_t, true>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::SampleSequence<int32_t, true>> );
	}
}


TEST_CASE ( "SampleSequence special members",
		"[samplesequence][calc][samples]" )
{
	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::SampleSequence<int32_t, true>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::SampleSequence<int32_t, true>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::SampleSequence<int32_t, true>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::SampleSequence<int32_t, true>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::SampleSequence<int32_t, true>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::SampleSequence<int32_t, true>> );
	}
}


TEST_CASE ( "SampleSequence is swappable",
		"[samplesequence][calc][samples]" )
{
	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::SampleSequence<int32_t, true>> );
	}

	// TODO Test for comparability once it is implemented

	// SECTION ("has comparability")
	// {
	// 	using arcstk::meta::is_comparable_v;
	//
	// 	CHECK ( is_comparable_v<arcstk::SampleSequence<int32_t, true>> );
	// }
}

