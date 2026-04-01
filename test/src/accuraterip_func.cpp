#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for accuraterip_func.hpp.
 */

#ifndef LIBARCSTK_ACCURATERIP_FUNC_HPP_
#include "accuraterip_func.hpp"   // TO BE TESTED
#endif


TEST_CASE ( "sum_digits", "[id]" )
{
	using arcstk::accuraterip::details::sum_digits;

	CHECK ( sum_digits(0xFFFFFFFF)  == 57 ); // 4294967295
	CHECK ( sum_digits(0x00000000)  ==  0 );

	CHECK ( sum_digits(1234567890u) == 45 );
	CHECK ( sum_digits( 123456789u) == 45 );
	CHECK ( sum_digits(  12345678u) == 36 );
	CHECK ( sum_digits(   1234567u) == 28 );
	CHECK ( sum_digits(    123456u) == 21 );
	CHECK ( sum_digits(     12345u) == 15 );
	CHECK ( sum_digits(      1234u) == 10 );
	CHECK ( sum_digits(       123u) ==  6 );
	CHECK ( sum_digits(        12u) ==  3 );
	CHECK ( sum_digits(         1u) ==  1 );
	CHECK ( sum_digits(          0) ==  0 );
}

// TODO print_impl()

