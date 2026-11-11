#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for Updateable<>.
 */

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"           // for Updateable<Versions1and2>
#endif
#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"          // for Version1, Version2
#endif

#include <type_traits>             // for is_*_{constructible,assignable}

#include "type_traits.hpp"         // for is_comparable,
								   // has_tostring_functionality


TEST_CASE ( "Updateable<Versions1and2> is non-abstract, final and non-empty",
		"[updateable][calc][algorithm]" )
{
	using arcstk::accuraterip::algorithm::Versions1and2;

	SECTION ("is a non-abstract class")
	{
		CHECK ( std::is_class_v<arcstk::Updateable<Versions1and2>> );
		CHECK ( ! std::is_abstract_v<arcstk::Updateable<Versions1and2>> );
	}

	SECTION ("is final")
	{
		CHECK ( std::is_final_v<arcstk::Updateable<Versions1and2>> );
	}

	SECTION ("has non-static members")
	{
		CHECK ( ! std::is_empty_v<arcstk::Updateable<Versions1and2>> );
	}
}


TEST_CASE ( "Updateable<Versions1and2> special members",
		"[updateable][calc][algorithm]" )
{
	using arcstk::accuraterip::algorithm::Versions1and2;

	SECTION ( "is default constructable")
	{
		CHECK ( std::is_default_constructible_v<arcstk::Updateable<Versions1and2>>);
	}

	SECTION ( "is copy-constructable")
	{
		CHECK ( std::is_copy_constructible_v<arcstk::Updateable<Versions1and2>>);
	}

	SECTION ( "is copy-assignable")
	{
		CHECK ( std::is_copy_assignable_v<arcstk::Updateable<Versions1and2>>);
	}

	SECTION ( "is nothrow move-constructable")
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::Updateable<Versions1and2>>);
	}

	SECTION ( "is nothrow move-assignable")
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::Updateable<Versions1and2>>);
	}

	SECTION ("is destructible")
	{
		CHECK ( std::is_destructible_v<arcstk::Updateable<Versions1and2>> );
	}
}


TEST_CASE ( "Updateable<Versions1and2> is swappable",
		"[updateable][calc][algorithm]" )
{
	using arcstk::accuraterip::algorithm::Versions1and2;

	SECTION ("is nothrow swappable")
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::Updateable<Versions1and2>> );
	}
}

