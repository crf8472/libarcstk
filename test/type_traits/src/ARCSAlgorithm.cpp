#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for ARCSAlgorithm.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"        // for ARCSAlgorithm
#endif

#include <type_traits>            // for is_*_{constructible,assignable}

#include "type_traits.hpp"        // for is_comparable,
								  // has_tostring_functionality


TEST_CASE ( "ARCSAlgorithm<> is non-abstract, final and non-empty",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	using ARv1  = arcstk::accuraterip::algorithm::Version1;
	using ARv2  = arcstk::accuraterip::algorithm::Version2;
	using ARv12 = arcstk::accuraterip::algorithm::Versions1and2;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<ARv1> );
		CHECK ( ! std::is_abstract_v<ARv1> );

		CHECK ( std::is_class_v<ARv2> );
		CHECK ( ! std::is_abstract_v<ARv2> );

		CHECK ( std::is_class_v<ARv12> );
		CHECK ( ! std::is_abstract_v<ARv12> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<ARv1> );
		CHECK ( std::is_final_v<ARv2> );
		CHECK ( std::is_final_v<ARv12> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<ARv1> );
		CHECK ( ! std::is_empty_v<ARv2> );
		CHECK ( ! std::is_empty_v<ARv12> );
	}
}


TEST_CASE ( "ARCSAlgorithm<> special members",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	using ARv1  = arcstk::accuraterip::algorithm::Version1;
	using ARv2  = arcstk::accuraterip::algorithm::Version2;
	using ARv12 = arcstk::accuraterip::algorithm::Versions1and2;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<ARv1>);
		CHECK ( std::is_default_constructible_v<ARv2>);
		CHECK ( std::is_default_constructible_v<ARv12>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<ARv1>);
		CHECK ( std::is_copy_constructible_v<ARv2>);
		CHECK ( std::is_copy_constructible_v<ARv12>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<ARv1>);
		CHECK ( std::is_copy_assignable_v<ARv2>);
		CHECK ( std::is_copy_assignable_v<ARv12>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_move_constructible_v<ARv1>);
		CHECK ( std::is_move_constructible_v<ARv2>);
		CHECK ( std::is_move_constructible_v<ARv12>);

		// FIXME Should be nothrow!
		// CHECK ( std::is_nothrow_move_constructible_v<ARv1>);
		// CHECK ( std::is_nothrow_move_constructible_v<ARv2>);
		// CHECK ( std::is_nothrow_move_constructible_v<ARv12>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_move_assignable_v<ARv1>);
		CHECK ( std::is_move_assignable_v<ARv2>);
		CHECK ( std::is_move_assignable_v<ARv12>);

		// FIXME Should be nothrow!
		// CHECK ( std::is_nothrow_move_assignable_v<ARv1>);
		// CHECK ( std::is_nothrow_move_assignable_v<ARv2>);
		// CHECK ( std::is_nothrow_move_assignable_v<ARv12>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<ARv1> );
		CHECK ( std::is_destructible_v<ARv2> );
		CHECK ( std::is_destructible_v<ARv12> );
	}
}


TEST_CASE ( "ARCSAlgorithm<> is swappable, comparable, string convertible",
		"[arcsalgorithm] [calc] [accuraterip]" )
{
	using ARv1  = arcstk::accuraterip::algorithm::Version1;
	using ARv2  = arcstk::accuraterip::algorithm::Version2;
	using ARv12 = arcstk::accuraterip::algorithm::Versions1and2;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<ARv1> );
		CHECK ( std::is_nothrow_swappable_v<ARv2> );
		CHECK ( std::is_nothrow_swappable_v<ARv12> );
	}

	// SECTION ("has comparability")
	// {
	// 	using arcstk::meta::is_comparable_v;
	//
	// 	CHECK ( is_comparable_v<ARv1> );
	// 	CHECK ( is_comparable_v<ARv2> );
	// 	CHECK ( is_comparable_v<ARv12> );
	// }

	// SECTION ("has string convertibility")
	// {
	// 	using arcstk::meta::has_tostring_functionality;
	//
	// 	CHECK ( has_tostring_functionality<ARv1>::value );
	// 	CHECK ( has_tostring_functionality<ARv2>::value );
	// 	CHECK ( has_tostring_functionality<ARv12>::value );
	// }
}

