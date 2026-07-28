#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for SampleBufferWrapper.
 */

#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


// TODO Implement testcases
//TEST_CASE ( "SampleBufferWrapper", "[samplebufferwrapper] [calc] [samples]" )
//{
//	using arcstk::testing::Copy;
//	using arcstk::testing::Move;
//
//	using arcstk::SampleBufferWrapper;
//
//	auto instance = SampleBufferWrapper {};
//
//
//	SECTION ("Parametized construction is correct")
//	{
//		FAIL ( "Parametized construction test is missing" );
//	}
//
//	SECTION ("Copy construction is correct")
//	{
//		const SampleBufferWrapper copied { Copy<SampleBufferWrapper>::construct(instance) };
//
//		// --
//
//        //CHECK(copied.value() == instance.value());
//        //CHECK(copied.name()  == instance.name());
//		FAIL ( "Copy construction test is missing" );
//	}
//
//	SECTION ("Move construction is correct")
//	{
//		const SampleBufferWrapper moved { Move<SampleBufferWrapper>::construct(instance) };
//
//		// --
//
//        //CHECK(moved.value() == instance.value());
//        //CHECK(moved.name()  == instance.name());
//		FAIL ( "Move construction test is missing" );
//	}
//
//	SECTION ("Copy assignment is correct")
//	{
//		auto copied = SampleBufferWrapper {};
//		Copy<SampleBufferWrapper>::assign(copied, instance);
//
//		// --
//
//        //CHECK(copied.value() == instance.value());
//        //CHECK(copied.name()  == instance.name());
//		FAIL ( "Copy assignment test is missing" );
//	}
//
//	SECTION ("Move assignment is correct")
//	{
//		auto moved = SampleBufferWrapper {};
//		Move<SampleBufferWrapper>::assign(moved, instance);
//
//		// --
//
//        //CHECK(moved.value() == instance.value());
//        //CHECK(moved.name()  == instance.name());
//		FAIL ( "Move assignment test is missing" );
//	}
//}
//
//
//TEST_CASE ( "SampleBufferWrapper default constructed instance",
//		"[samplebufferwrapper] [calc] [samples]" )
//{
//	const auto instance = arcstk::SampleBufferWrapper{};
//
//	SECTION ( "is empty()")
//	{
//		CHECK ( instance.empty() );
//	}
//
//	SECTION ( "converts to FALSE")
//	{
//		CHECK ( !instance );
//	}
//}
//
//
//TEST_CASE ( "SampleBufferWrapper property", "[samplebufferwrapper] [calc] [samples]" )
//{
//	using arcstk::SampleBufferWrapper;
//
//	auto defaulted = SampleBufferWrapper {};
//
//
//	SECTION ("Equality operator == is correct")
//	{
//		FAIL ( "Equality operator test is missing" );
//	}
//
//	SECTION ("Stream-in operator << is correct")
//	{
//		FAIL ( "Stream-in operator << test is missing" );
//	}
//
//	SECTION ("operator bool() is correct")
//	{
//		FAIL ( "operator bool() test is missing" );
//	}
//
//	SECTION ("swap() is correct")
//	{
//		FAIL ( "swap() test is missing" );
//	}
//
//	SECTION ("to_string() is correct")
//	{
//		FAIL ( "to_string() test is missing" );
//	}
//
//	SECTION ("clone() is correct")
//	{
//		FAIL ( "clone() test is missing" );
//	}
//
//	SECTION ("size() is correct")
//	{
//		FAIL ( "size() test is missing" );
//	}
//
//	SECTION ("empty() is correct")
//	{
//		FAIL ( "empty() test is missing" );
//	}
//}


TEST_CASE ( "SampleBufferWrapper functions",
		"[samplebufferwrapper] [calc] [samples]" )
{
	using arcstk::details::SampleBufferWrapper;
	using v_int32_t = std::vector<int32_t>;

	const auto planar0 = v_int32_t {   1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA };
	const auto planar1 = v_int32_t { 0xA, 9, 8, 7, 6, 5, 4, 3, 2,   1 };

	const auto interleaved = v_int32_t {
		1, 0xA, 2, 9, 3, 8, 4, 7, 5, 6, 6, 5, 7, 4, 8, 3, 9, 2, 0xA, 1 };

	SECTION ( "<int32_t> operator[] for planar data w/o channel swap" )
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

	SECTION ( "<int32_t> operator[] for planar data with channels swapped" )
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

	SECTION ( "<int32_t> operator[] for interleaved data w/o channel swap" )
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

	SECTION ( "<int32_t> operator[] for interleaved data with channels swapped" )
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

