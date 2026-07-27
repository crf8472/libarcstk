#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for checksum.hpp.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // TO BE TESTED
#endif


TEST_CASE ( "checksum::name provides correct names",
		"[checksum::name] [calc]" )
{
	using arcstk::checksum::type;
	using arcstk::checksum::name;

	CHECK ( name(type::ARCS1) == "ARCSv1" );
	CHECK ( name(type::ARCS2) == "ARCSv2" );
	//CHECK ( name(type::CRC32)   == "CRC32" );
	//CHECK ( name(type::CRC32ns) == "CRC32ns" );
}


TEST_CASE ( "TOTAL_HEX_DIGITS", "[checksum] [calc]" )
{
	using arcstk::Checksum;

	SECTION ( "is 8" )
	{
		CHECK ( Checksum::TOTAL_HEX_DIGITS == 8 );
	}
}

