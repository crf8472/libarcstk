#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for identifier_details.hpp.
 */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"
#endif
#ifndef LIBARCSTK_IDENTIFIER_DETAILS_HPP_
#include "identifier_details.hpp" // TO BE TESTED
#endif

#include <string>                 // for string
#include <vector>                 // for vector


TEST_CASE ( "disc_id_1, disc_id_2, cddb_id", "[id]" )
{
	const auto offsets1 = std::vector<int32_t>
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 };
	const auto leadout1 = int32_t { 253038 };

	const auto offsets2 = std::vector<int32_t>
		{ 32, 96985, 166422 };
	const auto leadout2 = int32_t { 264957 };

	const auto offsets3 = std::vector<int32_t>
		{ 33, 34283, 49908, 71508, 97983, 111183, 126708, 161883, 187158 };
	const auto leadout3 = int32_t { 210143 };

	const auto offsets4 = std::vector<int32_t>
		{ 0, 29042, 53880, 58227, 84420, 94192, 119165, 123030, 147500,
			148267, 174602, 208125, 212705, 239890, 268705, 272055, 291720,
			319992 };
	const auto leadout4 = int32_t { 332075 };

	const auto offsets5 = std::vector<int32_t> { 33 };
	const auto leadout5 = int32_t { 233484 };


	SECTION ( "disc_id_1() works" )
	{
		using arcstk::arid::disc_id_1;

		CHECK ( 0x001B9178 == disc_id_1(offsets1, leadout1) );
		CHECK ( 0x0008100C == disc_id_1(offsets2, leadout2) );
		CHECK ( 0x001008A6 == disc_id_1(offsets3, leadout3) );
		CHECK ( 0x00307C78 == disc_id_1(offsets4, leadout4) );
		CHECK ( 0x0003902D == disc_id_1(offsets5, leadout5) );
	}

	SECTION ( "disc_id_1() is 0 for empty or zero input" )
	{
		using arcstk::arid::disc_id_1;

		CHECK ( 0 == disc_id_1({ /*empty*/  },  0) );
		CHECK ( 0 == disc_id_1({ 0, 0, 0, 0 },  0) );
	}

	SECTION ( "disc_id_2() works" )
	{
		using arcstk::arid::disc_id_2;

		CHECK ( 0x014BE24E == disc_id_2(offsets1, leadout1) );
		CHECK ( 0x001AC008 == disc_id_2(offsets2, leadout2) );
		CHECK ( 0x007469B8 == disc_id_2(offsets3, leadout3) );
		CHECK ( 0x0281351D == disc_id_2(offsets4, leadout4) );
		CHECK ( 0x00072039 == disc_id_2(offsets5, leadout5) );
	}

	SECTION ( "disc_id_2() works for empty or zero input" )
	{
		using arcstk::arid::disc_id_2;

		CHECK ( 0 == disc_id_2({ /*empty*/  },  0) );
		CHECK ( 1 * 1 + 1 * 2 + 1 * 3 + 1 * 4 == disc_id_2({ 0, 0, 0, 0 },  0) );
	}

	SECTION ( "cddb_id() works" )
	{
		using arcstk::arid::cddb_id;

		CHECK ( 0xB40d2d0f == cddb_id(offsets1, leadout1) );
		CHECK ( 0x190DCC03 == cddb_id(offsets2, leadout2) );
		CHECK ( 0x870AF109 == cddb_id(offsets3, leadout3) );
		CHECK ( 0x27114B12 == cddb_id(offsets4, leadout4) );
		CHECK ( 0x020C2901 == cddb_id(offsets5, leadout5) );
	}

	SECTION ( "cddb_id() works for empty or zero input" )
	{
		using arcstk::arid::cddb_id;

		CHECK ( 0 == cddb_id({ /*empty*/  },  0) );
		CHECK ( 0x08000004 == cddb_id({ 0, 0, 0, 0 },  0) );
	}
}


TEST_CASE ( "sum_digits", "[id]" )
{
	using arcstk::arid::sum_digits;

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


TEST_CASE ( "construct_filename", "[id]" )
{
	using arcstk::arid::construct_filename;

	SECTION ( "Constructing regular dBAR filenames works" )
	{
		CHECK ( construct_filename(10, 0x02C34FD0, 0x01F880CC, 0xBC55023F)
				== "dBAR-010-02c34fd0-01f880cc-bc55023f.bin" );

		CHECK ( construct_filename(15, 0x001B9178, 0x014BE24E, 0xB40d2d0f)
				== "dBAR-015-001b9178-014be24e-b40d2d0f.bin" );

		CHECK ( construct_filename(3, 0x0008100C, 0x001AC008, 0x190DCC03)
				== "dBAR-003-0008100c-001ac008-190dcc03.bin" );

		CHECK ( construct_filename(9, 0x001008A6, 0x007469B8, 0x870AF109)
				== "dBAR-009-001008a6-007469b8-870af109.bin" );

		CHECK ( construct_filename(18, 0x00307C78, 0x0281351D, 0x27114B12)
				== "dBAR-018-00307c78-0281351d-27114b12.bin" );

		CHECK ( construct_filename(1, 0x0003902D, 0x00072039, 0x020C2901)
				== "dBAR-001-0003902d-00072039-020c2901.bin" );
	}
}


TEST_CASE ( "construct_url", "[id]" )
{
	using arcstk::arid::construct_url;

	const auto url = std::string { "http://www.accuraterip.com/accuraterip/" };

	REQUIRE ( url == arcstk::ACCURATERIP::request_url_prefix());


	SECTION ( "Constructing regular AccurateRip URLs works" )
	{
		CHECK ( construct_url(10, 0x02C34FD0, 0x01F880CC, 0xBC55023F)
				== url + "0/d/f/" + "dBAR-010-02c34fd0-01f880cc-bc55023f.bin");

		CHECK ( construct_url(15, 0x001B9178, 0x014BE24E, 0xB40d2d0f)
				== url + "8/7/1/" + "dBAR-015-001b9178-014be24e-b40d2d0f.bin");

		CHECK ( construct_url(3, 0x0008100C, 0x001AC008, 0x190DCC03)
				== url + "c/0/0/" + "dBAR-003-0008100c-001ac008-190dcc03.bin");

		CHECK ( construct_url(9, 0x001008A6, 0x007469B8, 0x870AF109)
				== url + "6/a/8/" + "dBAR-009-001008a6-007469b8-870af109.bin");

		CHECK ( construct_url(18, 0x00307C78, 0x0281351D, 0x27114B12)
				== url + "8/7/c/" + "dBAR-018-00307c78-0281351d-27114b12.bin");

		CHECK ( construct_url(1, 0x0003902D, 0x00072039, 0x020C2901)
				== url + "d/2/0/" + "dBAR-001-0003902d-00072039-020c2901.bin");
	}
}


TEST_CASE ( "construct_id", "[id]" )
{
	using arcstk::arid::construct_id;

	SECTION ( "Constructing regular AccurateRip ids works" )
	{
		CHECK ( construct_id(10, 0x02C34FD0, 0x01F880CC, 0xBC55023F)
				== "010-02c34fd0-01f880cc-bc55023f");

		CHECK ( construct_id(15, 0x001B9178, 0x014BE24E, 0xB40d2d0f)
				== "015-001b9178-014be24e-b40d2d0f");

		CHECK ( construct_id(3, 0x0008100C, 0x001AC008, 0x190DCC03)
				== "003-0008100c-001ac008-190dcc03");

		CHECK ( construct_id(9, 0x001008A6, 0x007469B8, 0x870AF109)
				== "009-001008a6-007469b8-870af109");

		CHECK ( construct_id(18, 0x00307C78, 0x0281351D, 0x27114B12)
				== "018-00307c78-0281351d-27114b12");

		CHECK ( construct_id(1, 0x0003902D, 0x00072039, 0x020C2901)
				== "001-0003902d-00072039-020c2901");
	}
}

