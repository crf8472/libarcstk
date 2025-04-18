#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for version.hpp.
 */

#ifndef __LIBARCSTK_VERSION_HPP__
#include "version.hpp"            // TO BE TESTED
#endif


TEST_CASE ( "constants", "[version]" )
{
	CHECK ( arcstk::v_1_0_0::LIBARCSTK_NAME == "libarcstk" );

	CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_MAJOR  == 0 );
	CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_MINOR  == 3 );
	CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_PATCH  == 0 );
	CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION_SUFFIX == "alpha.1" );
	CHECK ( arcstk::v_1_0_0::LIBARCSTK_VERSION        == "0.3.0-alpha.1" );
}


TEST_CASE ( "functions", "[version]" )
{
	using arcstk::v_1_0_0::api_version_is_at_least;

	SECTION ("api_version_is_at_least")
	{
		CHECK (  api_version_is_at_least(0, 0, 0) );
		CHECK (  api_version_is_at_least(0, 0, 1) );
		CHECK (  api_version_is_at_least(0, 0, 2) );
		CHECK (  api_version_is_at_least(0, 1, 0) );
		CHECK (  api_version_is_at_least(0, 1, 1) );
		CHECK (  api_version_is_at_least(0, 2, 0) );
		CHECK (  api_version_is_at_least(0, 2, 1) );
		CHECK (  api_version_is_at_least(0, 3, 0) ); // <= this version
		CHECK ( !api_version_is_at_least(0, 3, 1) );
		CHECK ( !api_version_is_at_least(1, 0, 0) );
		CHECK ( !api_version_is_at_least(1, 1, 0) );
		CHECK ( !api_version_is_at_least(1, 1, 1) );
		CHECK ( !api_version_is_at_least(2, 1, 1) );
	}
}

