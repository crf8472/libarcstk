#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for accuraterip.hpp.
 */

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#define LIBARCSTK_ALGORITHMS_HPP_ // allow accuraterip.hpp
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"        // TO BE TESTED
#endif

#include <fstream>                // for ifstream
#include <unordered_set>          // for unordered_set
#include <vector>                 // for vector

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type
#endif
#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"         // for ARId, make_arid
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // for AudioSize
#endif
#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"            // for sample_t
#endif


TEST_CASE ( "AccurateRipCS", "[updatable]" )
{
	using arcstk::accuraterip::details::AccurateRipCS;
	using arcstk::checksum::type;

	AccurateRipCS<type::ARCS1,type::ARCS2> u1;
	AccurateRipCS<type::ARCS1,type::ARCS2> u2;

	std::vector<uint32_t> data { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

	using std::cbegin;
	using std::cend;

	u1.set_multiplier(1);
	u1.update(cbegin(data), cend(data));

	SECTION( "Swapping instances of AccurateRipCS<> works" )
	{
		CHECK ( u1.value().get(type::ARCS2).first.value() == 650 );
		CHECK ( u2.value().get(type::ARCS2).first.value() ==   0 );

		using std::swap;
		swap(u1, u2);

		CHECK ( u1.value().get(type::ARCS2).first.value() ==   0 );
		CHECK ( u2.value().get(type::ARCS2).first.value() == 650 );
	}
}


TEST_CASE ( "Updating ARCS v1+v2", "[arcsalgorithm] [calc]" )
{
	using arcstk::AudioSize;
	using arcstk::checksum::type;
	using arcstk::sample_t;
	namespace Details = arcstk::accuraterip::details;

	// fits calculation-test-01.bin
	//auto audiosize = AudioSize { 196608, UNIT::SAMPLES };

	SECTION ( "Updating ARCS 1 singletrack & aligned blocks is correct" )
	{
		auto algo = Details::Version1{};
		REQUIRE ( algo.types() == std::unordered_set<type>{ type::ARCS1 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				algo.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		algo.update(buffer.begin(), buffer.end());
		algo.track_finished(1, AudioSize{});

		//CHECK ( calculation.complete() );

		auto checksums { algo.result() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 1 /* types */ );
		CHECK ( 0x8FE8D29B == (checksums.get(type::ARCS1).first) );
	}


	SECTION ( "Updating ARCS 2 singletrack & aligned blocks is correct" )
	{
		auto state = Details::AccurateRipCS<type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{ type::ARCS2 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		state.update(buffer.begin(), buffer.end());

		//CHECK ( calculation.complete() );

		auto checksums { state.value() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 1 /* types */ );
		CHECK ( 0xD15BB487 == (checksums.get(type::ARCS2).first) );
	}


	SECTION ( "Updating ARCS v1+2 singletrack & aligned blocks is correct" )
	{
		auto state = Details::AccurateRipCS<type::ARCS1,type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{
				type::ARCS1, type::ARCS2 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		state.update(buffer.begin(), buffer.end());

		//CHECK ( calculation.complete() );

		auto checksums { state.value() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 2 /* types */ );
		CHECK ( 0xD15BB487 == (checksums.get(type::ARCS2).first) );
		CHECK ( 0x8FE8D29B == (checksums.get(type::ARCS1).first) );
	}


	SECTION ( "Updating ARCS v1+2 singletrack & non-aligned blocks is correct" )
	{
		auto state = Details::AccurateRipCS<type::ARCS1,type::ARCS2>{};

		REQUIRE ( state.types() == std::unordered_set<type>{
				type::ARCS1, type::ARCS2 } );

		// Initialize Buffer

		std::vector<sample_t> buffer(80000); // samples

		// => forms 3 blocks: 2 x 80000 samples and 1 x 36608 samples

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-01.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-01.bin");
		}

		for (int i = 0; i < 2; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 320000);
				// 320000 bytes == 80000 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-01.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			//CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(36608);
			in.read(reinterpret_cast<char*>(&buffer[0]), 146432);

		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-01.bin");
		}

		in.close();

		state.update(buffer.begin(), buffer.end());

		//CHECK ( calculation.complete() );

		auto checksums { state.value() };

		// Only track with correct ARCSs

		CHECK ( checksums.size() == 2 );
		CHECK ( 0xD15BB487 == (checksums.get(type::ARCS2)).first.value() );
		CHECK ( 0x8FE8D29B == (checksums.get(type::ARCS1)).first.value() );
	}
}


TEST_CASE ( "Current AccurateRip Request URL", "[make_empty_arid] [id]" )
{
	using arcstk::ACCURATERIP;

	REQUIRE ( ACCURATERIP::default_request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

	REQUIRE ( ACCURATERIP::request_url_prefix() ==
				ACCURATERIP::default_request_url_prefix() );

	// Bent: "Programmed to Love"

	const auto id4 = arcstk::make_arid(
			arcstk::ToC{ arcstk::toc::construct(
				// leadout
				332075,
				// offsets
				{
					0, 29042, 53880, 58227, 84420, 94192, 119165, 123030,
					147500, 148267, 174602, 208125, 212705, 239890, 268705,
					272055, 291720, 319992
				}
			)}
	);


	SECTION ( "request_url_prefix() returns updated URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://foobar/test/success");

		CHECK ( ACCURATERIP::request_url_prefix() ==
				"https://foobar/test/success" );
	}

	SECTION ( "reset_request_url_prefix() resets request URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://some/test/success/");

		CHECK ( ACCURATERIP::request_url_prefix() ==
				"https://some/test/success/" );

		ACCURATERIP::reset_request_url_prefix();

		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );
	}

	SECTION ( "ARId::url() returns updated URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		CHECK ( id4.url().substr(0, 39) ==
				"http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://some/test/at/least/");

		CHECK ( id4.url().substr(0, 27) ==
				"https://some/test/at/least/" );
	}

	SECTION ( "ARId::prefix() returns updated URL" )
	{
		CHECK ( ACCURATERIP::request_url_prefix() ==
				"http://www.accuraterip.com/accuraterip/" );

		CHECK ( id4.prefix() == "http://www.accuraterip.com/accuraterip/" );

		ACCURATERIP::set_request_url_prefix(
				"https://some/test/at/least/");

		CHECK ( id4.prefix() == "https://some/test/at/least/" );
	}

	ACCURATERIP::reset_request_url_prefix();
}


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
		using arcstk::accuraterip::id::disc_id_1;

		CHECK ( 0x001B9178 == disc_id_1(offsets1, leadout1) );
		CHECK ( 0x0008100C == disc_id_1(offsets2, leadout2) );
		CHECK ( 0x001008A6 == disc_id_1(offsets3, leadout3) );
		CHECK ( 0x00307C78 == disc_id_1(offsets4, leadout4) );
		CHECK ( 0x0003902D == disc_id_1(offsets5, leadout5) );
	}

	SECTION ( "disc_id_1() is 0 for empty or zero input" )
	{
		using arcstk::accuraterip::id::disc_id_1;

		CHECK ( 0 == disc_id_1({ /*empty*/  },  0) );
		CHECK ( 0 == disc_id_1({ 0, 0, 0, 0 },  0) );
	}

	SECTION ( "disc_id_2() works" )
	{
		using arcstk::accuraterip::id::disc_id_2;

		CHECK ( 0x014BE24E == disc_id_2(offsets1, leadout1) );
		CHECK ( 0x001AC008 == disc_id_2(offsets2, leadout2) );
		CHECK ( 0x007469B8 == disc_id_2(offsets3, leadout3) );
		CHECK ( 0x0281351D == disc_id_2(offsets4, leadout4) );
		CHECK ( 0x00072039 == disc_id_2(offsets5, leadout5) );
	}

	SECTION ( "disc_id_2() works for empty or zero input" )
	{
		using arcstk::accuraterip::id::disc_id_2;

		CHECK ( 0 == disc_id_2({ /*empty*/  },  0) );
		CHECK ( 1 * 1 + 1 * 2 + 1 * 3 + 1 * 4 == disc_id_2({ 0, 0, 0, 0 },  0) );
	}

	SECTION ( "cddb_id() works" )
	{
		using arcstk::accuraterip::id::cddb_id;

		CHECK ( 0xB40d2d0f == cddb_id(offsets1, leadout1) );
		CHECK ( 0x190DCC03 == cddb_id(offsets2, leadout2) );
		CHECK ( 0x870AF109 == cddb_id(offsets3, leadout3) );
		CHECK ( 0x27114B12 == cddb_id(offsets4, leadout4) );
		CHECK ( 0x020C2901 == cddb_id(offsets5, leadout5) );
	}

	SECTION ( "cddb_id() works for empty or zero input" )
	{
		using arcstk::accuraterip::id::cddb_id;

		CHECK ( 0 == cddb_id({ /*empty*/  },  0) );
		CHECK ( 0x08000004 == cddb_id({ 0, 0, 0, 0 },  0) );
	}
}


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


TEST_CASE ( "construct_filename", "[id]" )
{
	using arcstk::accuraterip::id::construct_filename;

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
	using arcstk::accuraterip::id::construct_url;

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
	using arcstk::accuraterip::id::construct_id;

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


//TEST_CASE ( "Updating ARCS v1+v2 with MultiTrackContext", "[update]" )
//{
/*
	using arcstk::details::TOCBuilder;
	using arcstk::make_context;

	auto toc { TOCBuilder::build(
		3, // track count
		{ 12, 433, 924 }, // offsets
		1233 // leadout
	)};

	auto mtcx { make_context(toc) };

	CHECK ( mtcx->total_tracks() == 3 );
	CHECK ( mtcx->offset(0) ==  12 );
	CHECK ( mtcx->offset(1) == 433 );
	CHECK ( mtcx->offset(2) == 924 );
	CHECK ( mtcx->audio_size().leadout_frame() == 1233 );
	CHECK ( mtcx->is_multi_track() );
	CHECK ( mtcx->skips_front() );
	CHECK ( mtcx->skips_back() );
	CHECK ( mtcx->num_skip_front() == 2939 );
	CHECK ( mtcx->num_skip_back()  == 2940 );
*/

/*
	SECTION ( "Correct ARCS1+2 with aligned blocks" )
	{
		arcstk::accuraterip::AccurateRipCS<type::ARCS1,type::ARCS2> state {};

		// Initialize Buffer

		std::vector<sample_t> buffer(181251); // samples

		// => forms 4 blocks with 181251 samples each
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-02.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-02.bin");
		}
		for (int i = 0; i < 4; ++i)
		{
			//CHECK ( not calculation.complete() );

			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 725004);
				// 725004 bytes == 181251 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-02.bin");
			}

			try
			{
				state.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}
		}

		in.close();

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 3 );


		// Checks

		auto track1 { checksums[0] };

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (track1.get(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (track1.get(type::ARCS1)).value());

		auto track2 { checksums[1] };

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (track2.get(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (track2.get(type::ARCS1)).value());

		auto track3 { checksums[2] };

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (track3.get(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (track3.get(type::ARCS1)).value());
	}


	SECTION ( "Correct ARCS1+2 with non-aligned blocks" )
	{
		using arcstk::sample_t;

		// Initialize Buffer

		std::vector<sample_t> buffer(241584); // samples

		// => forms 3 blocks: 2 x 241584 samples and 1 x 252 samples
		// (total: 725004 samples, 2900016 bytes)

		// Read Entire File Block-Wise and Update Calculation With the Blocks

		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			in.open("calculation-test-02.bin",
					std::ifstream::in | std::ifstream::binary);
		} catch (const std::ifstream::failure& f)
		{
			FAIL ("Could not open test data file calculation-test-02.bin");
		}
		for (int i = 0; i < 3; ++i)
		{
			try
			{
				in.read(reinterpret_cast<char*>(&buffer[0]), 966336);
				// 966336 bytes == 241584 samples

			} catch (const std::ifstream::failure& f)
			{
				in.close();
				FAIL ("Error while reading from file calculation-test-02.bin");
			}

			try
			{
				calculation.update(buffer.begin(), buffer.end());
			} catch (...)
			{
				in.close();
				FAIL ("Error while updating buffer");
			}

			CHECK ( not calculation.complete() );
		}
		try // last block is smaller
		{
			buffer.resize(252);
			in.read(reinterpret_cast<char*>(&buffer[0]), 1008);
		} catch (const std::ifstream::failure& f)
		{
			in.close();
			FAIL ("Error on last block from file calculation-test-02.bin");
		}

		in.close();

		calculation.update(buffer.begin(), buffer.end());

		CHECK ( calculation.complete() );

		auto checksums { calculation.result() };

		CHECK ( checksums.size() == 3 );


		// Checks

		auto track1 = checksums[0];

		CHECK ( track1.size() == 2 );
		CHECK ( 0x0DF230F0 == (track1.get(type::ARCS2)).value());
		CHECK ( 0x7C7BFAF4 == (track1.get(type::ARCS1)).value());

		auto track2 = checksums[1];

		CHECK ( track2.size() == 2 );
		CHECK ( 0x34C681C3 == (track2.get(type::ARCS2)).value());
		CHECK ( 0x5989C533 == (track2.get(type::ARCS1)).value());

		auto track3 = checksums[2];

		CHECK ( track3.size() == 2 );
		CHECK ( 0xB845A497 == (track3.get(type::ARCS2)).value());
		CHECK ( 0xDD95CE6C == (track3.get(type::ARCS1)).value());
	}
*/
//}

