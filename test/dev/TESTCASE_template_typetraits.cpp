#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Type traits testcases for ${CLASS}.
 */

#ifndef LIBARCSTK_${HEADER:u}_HPP_
#include "${HEADER}.hpp"           // for ${CLASS}
#endif

// testing

#ifndef LIBARCSTK_TYPE_TRAITS_HPP_
#include "type_traits.hpp"         // for is_comparable,
#endif                             // has_tostring_functionality

#include <type_traits>             // for is_*_{constructible,assignable}


TEST_CASE ( "${CLASS} traits", "[${CLASS:l}] [${MODULE}] [${HEADER}]" )
{
	SECTION ( "is a class" )
	{
		CHECK ( std::is_class_v<arcstk::${CLASS}> );
	}

	SECTION ( "is non-abstract" )
	{
		CHECK ( ! std::is_abstract_v<arcstk::${CLASS}> );
	}

	SECTION ( "is final" )
	{
		CHECK ( std::is_final_v<arcstk::${CLASS}> );
	}

	SECTION ( "has non-static members" )
	{
		CHECK ( ! std::is_empty_v<arcstk::${CLASS}> );
	}
}


TEST_CASE ( "${CLASS} construction traits",
		"[${CLASS:l}] [${MODULE}] [${HEADER:l}]" )
{
	SECTION ( "is default constructable" )
	{
		CHECK ( std::is_default_constructible_v<arcstk::${CLASS}>);
	}

	SECTION ( "is copy-constructable" )
	{
		CHECK ( std::is_copy_constructible_v<arcstk::${CLASS}>);
	}

	SECTION ( "is copy-assignable" )
	{
		CHECK ( std::is_copy_assignable_v<arcstk::${CLASS}>);
	}

	SECTION ( "is nothrow move-constructable" )
	{
		CHECK ( std::is_nothrow_move_constructible_v<arcstk::${CLASS}>);
	}

	SECTION ( "is nothrow move-assignable" )
	{
		CHECK ( std::is_nothrow_move_assignable_v<arcstk::${CLASS}>);
	}

	SECTION ( "is destructible" )
	{
		CHECK ( std::is_destructible_v<arcstk::${CLASS}> );
	}
}


TEST_CASE ( "${CLASS} value traits", "[${CLASS:l}] [${MODULE}] [${HEADER:l}]" )
{
	SECTION ( "is swappable without throwing" )
	{
		CHECK ( std::is_nothrow_swappable_v<arcstk::${CLASS}> );
	}

	SECTION ( "is convertible to bool" )
	{
		CHECK ( );
	}

	SECTION ( "is comparable" )
	{
		using arcstk::meta::is_comparable_v;

		CHECK ( is_comparable_v<arcstk::${CLASS}> );
	}

	// equals() noexcept?
	// size() noexcept
	// empty() noexcept

	SECTION ( "is convertible to string" )
	{
		using arcstk::meta::has_tostring_functionality;

		CHECK ( has_tostring_functionality<arcstk::${CLASS}>::value );
	}

	// <<
	// clone
}

