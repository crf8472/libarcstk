#include "catch2/catch.hpp"

#include <array>

#ifndef __LIBARCS_CHECKSUM_HPP__
#include "checksum.hpp"
#endif


/**
 * \file checksum.cpp Fixtures for things in namespace arcs::checksum
 */


TEST_CASE ( "checksum::type_name provides correct names",
		"[checksum] [checksum::type_name]" )
{
	using type      = arcs::checksum::type;

	REQUIRE ( arcs::checksum::type_name(type::ARCS1)   == "ARCSv1" );
	REQUIRE ( arcs::checksum::type_name(type::ARCS2)   == "ARCSv2" );
	//REQUIRE ( arcs::checksum::type_name(type::CRC32)   == "CRC32" );
	//REQUIRE ( arcs::checksum::type_name(type::CRC32ns) == "CRC32ns" );
}

