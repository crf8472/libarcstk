#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Test constants and functions of version.hpp.
 */

#ifndef LIBARCSTK_VERSION_HPP_
#include "version.hpp"             // TO BE TESTED
#endif


TEST_CASE ( "version", "[version]" )
{
	SECTION ( "name is 'libarcstk'" )
	{
		CHECK ( arcstk::v_1_0_0::LIBARCSTK_NAME           == "libarcstk" );
	}

	SECTION ( "major,minor,patch is 0,9,0" )
	{
		CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_MAJOR  == 0 );
		CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_MINOR  == 9 );
		CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_PATCH  == 0 );
	}

	SECTION ( "suffix is 'alpha.1'" )
	{
		CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_SUFFIX == "alpha.1" );
		CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION        == "0.9.0-alpha.1" );
	}
}


TEST_CASE ( "api_version_is_at_least()", "[api_version_is_at_least][version]" )
{
	using arcstk::v_1_0_0::api_version_is_at_least;

	SECTION ("is correct")
	{
		CHECK (  api_version_is_at_least(0, 0, 0) );
		CHECK (  api_version_is_at_least(0, 0, 1) );
		CHECK (  api_version_is_at_least(0, 0, 2) );
		CHECK (  api_version_is_at_least(0, 1, 0) );
		CHECK (  api_version_is_at_least(0, 1, 1) );
		CHECK (  api_version_is_at_least(0, 2, 0) );
		CHECK (  api_version_is_at_least(0, 2, 1) );
		CHECK (  api_version_is_at_least(0, 3, 0) );
		CHECK (  api_version_is_at_least(0, 3, 1) );
		CHECK (  api_version_is_at_least(0, 9, 0) ); // <= this version

		CHECK_FALSE ( api_version_is_at_least(0, 9, 1) );
		CHECK_FALSE ( api_version_is_at_least(1, 0, 0) );
		CHECK_FALSE ( api_version_is_at_least(1, 1, 0) );
		CHECK_FALSE ( api_version_is_at_least(1, 1, 1) );
		CHECK_FALSE ( api_version_is_at_least(2, 1, 1) );
	}
}

// vim:fdm=syntax

