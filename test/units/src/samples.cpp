#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for samples.hpp.
 */

#ifndef LIBARCSTK_SAMPLESEQ_HPP_
#include "samples.hpp"          // TO BE TESTED
#endif

#include <cstdint>              // for int16_t, uint8_t, uint32_t, int32_t,...
#include <vector>               // for vector


TEST_CASE ( "channel", "[channel]" )
{
	using arcstk::details::channel;

	SECTION ( "left()")
	{
		CHECK ( channel::left(false) == 0 );
		CHECK ( channel::left(true)  == 1 );
	}

	SECTION ( "right()")
	{
		CHECK ( channel::right(false) == 1 );
		CHECK ( channel::right(true)  == 0 );
	}
}


TEST_CASE ( "SampleBufferWrapper planar with local vector",
		"[samplebufferwrapper]" )
{
	using arcstk::details::SampleBufferWrapper;
	using v_int32_t = std::vector<int32_t>;

	const auto planar0 = v_int32_t {   1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA };
	const auto planar1 = v_int32_t { 0xA, 9, 8, 7, 6, 5, 4, 3, 2,   1 };

	SECTION ( "<int32_t> operator[] for channel ordering 0/1" )
	{
		SampleBufferWrapper<int32_t, true> buf { planar0.data(), planar1.data(),
			10, false/* not swapped*/ };

		REQUIRE ( 10 == buf.size() );

		CHECK ( buf[0] == 0xA0001 ); // 655361
		CHECK ( buf[1] == 0x90002 ); // 589826
		CHECK ( buf[2] == 0x80003 ); // 524291
		CHECK ( buf[3] == 0x70004 ); // 458756
		CHECK ( buf[4] == 0x60005 ); // 393221
		CHECK ( buf[5] == 0x50006 ); // 327686
		CHECK ( buf[6] == 0x40007 ); // 262151
		CHECK ( buf[7] == 0x30008 ); // 196616
		CHECK ( buf[8] == 0x20009 ); // 131081
		CHECK ( buf[9] == 0x1000A ); //  65546
	}

	SECTION ( "<int32_t> operator[] for channel ordering 1/0 (swapped)" )
	{
		SampleBufferWrapper<int32_t, true> buf
			{ planar0.data(), planar1.data(), 10, true/* channels swapped*/ };

		REQUIRE ( 10 == buf.size() );

		CHECK ( buf[0] == 0x1000A ); //  65546
		CHECK ( buf[1] == 0x20009 ); // 131081
		CHECK ( buf[2] == 0x30008 ); // 196616
		CHECK ( buf[3] == 0x40007 ); // 262151
		CHECK ( buf[4] == 0x50006 ); // 327686
		CHECK ( buf[5] == 0x60005 ); // 393221
		CHECK ( buf[6] == 0x70004 ); // 458756
		CHECK ( buf[7] == 0x80003 ); // 524291
		CHECK ( buf[8] == 0x90002 ); // 589826
		CHECK ( buf[9] == 0xA0001 ); // 655361
	}
}


TEST_CASE ( "SampleBufferWrapper interleaved with local vector",
		"[samplebufferwrapper]" )
{
	using arcstk::details::SampleBufferWrapper;
	using v_int32_t = std::vector<int32_t>;

	const auto interleaved = v_int32_t {
		1, 0xA, 2, 9, 3, 8, 4, 7, 5, 6, 6, 5, 7, 4, 8, 3, 9, 2, 0xA, 1 };

	SECTION ( "<int32_t> operator[] for channel ordering 0/1" )
	{
		SampleBufferWrapper<int32_t, false> buf
			{ interleaved.data(), 20, false/* not swapped*/ };

		REQUIRE ( 10 == buf.size() );

		CHECK ( buf[0] == 0xA0001 ); // 655361
		CHECK ( buf[1] == 0x90002 ); // 589826
		CHECK ( buf[2] == 0x80003 ); // 524291
		CHECK ( buf[3] == 0x70004 ); // 458756
		CHECK ( buf[4] == 0x60005 ); // 393221
		CHECK ( buf[5] == 0x50006 ); // 327686
		CHECK ( buf[6] == 0x40007 ); // 262151
		CHECK ( buf[7] == 0x30008 ); // 196616
		CHECK ( buf[8] == 0x20009 ); // 131081
		CHECK ( buf[9] == 0x1000A ); //  65546
	}

	SECTION ( "<int32_t> operator[] for channel ordering 1/0 (swapped)" )
	{
		SampleBufferWrapper<int32_t, false> buf
			{ interleaved.data(), 20, true/* swapped*/ };

		REQUIRE ( 10 == buf.size() );

		CHECK ( buf[0] == 0x1000A ); //  65546
		CHECK ( buf[1] == 0x20009 ); // 131081
		CHECK ( buf[2] == 0x30008 ); // 196616
		CHECK ( buf[3] == 0x40007 ); // 262151
		CHECK ( buf[4] == 0x50006 ); // 327686
		CHECK ( buf[5] == 0x60005 ); // 393221
		CHECK ( buf[6] == 0x70004 ); // 458756
		CHECK ( buf[7] == 0x80003 ); // 524291
		CHECK ( buf[8] == 0x90002 ); // 589826
		CHECK ( buf[9] == 0xA0001 ); // 655361
	}
}

