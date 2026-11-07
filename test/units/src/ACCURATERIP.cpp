#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ACCURATERIP.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip_func.hpp"     // for
#endif


TEST_CASE ( "ACCURATERIP default constructed instance",
		"[accuraterip][calc][accuraterip]" )
{
	const auto defaulted = arcstk::ACCURATERIP {};

	SECTION ("url prefix has default value")
	{
		CHECK( defaulted.request_url_prefix() ==
				defaulted.default_request_url_prefix() );
	}
}


TEST_CASE ( "ACCURATERIP functions", "[accuraterip][calc][accuraterip]" )
{
	using arcstk::ACCURATERIP;


	SECTION ("request_url_prefix() has default value")
	{
		CHECK( ACCURATERIP::request_url_prefix() ==
				ACCURATERIP::default_request_url_prefix() );
	}

	SECTION ("default_request_url_prefix() is correct")
	{
		CHECK( "http://www.accuraterip.com/accuraterip/" ==
				ACCURATERIP::default_request_url_prefix() );
	}

	// SECTION ("set_request_url_prefix() is correct")
	// {
	// }
	//
	// SECTION ("reset_request_url_prefix() is correct")
	// {
	// }

	SECTION ("default_arcs_format() is correct")
	{
		CHECK( ACCURATERIP::default_arcs_format( 0) == "00000000" );
		CHECK( ACCURATERIP::default_arcs_format(16) == "00000010" );
		CHECK( ACCURATERIP::default_arcs_format(749321) == "000B6F09" );
		CHECK( ACCURATERIP::default_arcs_format(2059706353) == "7AC49FF1" );
	}
}

