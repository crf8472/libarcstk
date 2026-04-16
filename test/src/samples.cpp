#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for samples.hpp.
 */

#ifndef LIBARCSTK_SAMPLESEQ_HPP_
#include "samples.hpp"          // TO BE TESTED
#endif

#include <cstdint>                // for int16_t, uint8_t, uint32_t, int32_t,...
#include <fstream>                // for ifstream, istreambuf_iterator
#include <iterator>               // for begin, end, next, prev
#include <vector>                 // for vector


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


TEST_CASE ( "SampleBufferWrapper planar file", "[samplebufferwrapper]" )
{
	using arcstk::details::SampleBufferWrapper;

	// Load example samples

	std::ifstream in;
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		in.open("samplesequence-test-01.bin",
				std::ifstream::in | std::ifstream::binary);
	} catch (const std::ifstream::failure& f)
	{
		FAIL ("Could not open test data file samplesequence-test-01.bin");
	}

	std::vector<uint8_t> bytes(
			(std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>()
	);

	in.close();

	REQUIRE ( bytes.size() == 1024 );

	REQUIRE ( bytes[   0] == 0xA5 );
	REQUIRE ( bytes[   1] == 0xC2 );
	REQUIRE ( bytes[   2] == 0x11 );
	REQUIRE ( bytes[   3] == 0xC7 );
	// ... all bytes between
	REQUIRE ( bytes[ 512] == 0xDB );
	REQUIRE ( bytes[ 513] == 0xD9 );
	REQUIRE ( bytes[ 514] == 0x93 );
	REQUIRE ( bytes[ 515] == 0x42 );
	// ... all bytes between
	REQUIRE ( bytes[1020] == 0x6D );
	REQUIRE ( bytes[1021] == 0xDD );
	REQUIRE ( bytes[1022] == 0x71 );
	REQUIRE ( bytes[1023] == 0x15 );

	SECTION ("uint16_t planar buffer access works as expected")
	{
		// 1024 bytes == 1024 uint8_t
		// 1024 uint8_t == 512 int16_t values
		// 512 values / 2 channels == 256 pairs/samples

		auto buf = SampleBufferWrapper<uint16_t, true> {
			bytes.data(), bytes.data() + 512, 512, false };

		CHECK ( 256 == buf.size() );

		CHECK ( buf.left_channel (0) == 0xC2A5 );
		CHECK ( buf.right_channel(0) == 0xD9DB );
		CHECK ( buf.left_channel (1) == 0xC711 );
		CHECK ( buf.right_channel(1) == 0x4293 );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0xD9DBC2A5 );
		CHECK ( buf[  1] == 0x4293C711 );
		CHECK ( buf[  2] == 0x920a9ECC );
		CHECK ( buf[  3] == 0x6C51F35B );
		CHECK ( buf[  4] == 0x602D4D95 );
		CHECK ( buf[  5] == 0xAEE52C50 );
		CHECK ( buf[  6] == 0x870465DC );
		CHECK ( buf[  7] == 0x3C1296BE );
		CHECK ( buf[  8] == 0xB852979D );
		CHECK ( buf[  9] == 0xFF364BB9 );

		// end: last 10 samples
		CHECK ( buf[246] == 0x363FDB43 );
		CHECK ( buf[247] == 0xCA1A72F6 );
		CHECK ( buf[248] == 0xE81714D8 );
		CHECK ( buf[249] == 0xA856DB4A );
		CHECK ( buf[250] == 0xE0EB1795 );
		CHECK ( buf[251] == 0xD6862DFE );
		CHECK ( buf[252] == 0xABA8E966 );
		CHECK ( buf[253] == 0xF617700E );
		CHECK ( buf[254] == 0xDD6D28EF );
		CHECK ( buf[255] == 0x157171F9 );
	}

	SECTION ("int16_t planar buffer access works as expected")
	{
		// 1024 bytes each interpreted as int16_t == 512 int16_t values
		// 512 values / 2 channels == 256 pairs/samples

		auto buf = SampleBufferWrapper<int16_t, true> {
			bytes.data(), bytes.data() + 512, 512, false };

		CHECK ( 256 == buf.size() );

		CHECK ( buf.left_channel (0) == -15707 ); // is now signed!
		CHECK ( buf.right_channel(0) ==  -9765 );
		CHECK ( buf.left_channel (1) == -14575 );
		CHECK ( buf.right_channel(1) == 0x4293 );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0xD9DBC2A5 );
		CHECK ( buf[  1] == 0x4293C711 );
		CHECK ( buf[  2] == 0x920a9ECC );
		CHECK ( buf[  3] == 0x6C51F35B );
		CHECK ( buf[  4] == 0x602D4D95 );
		CHECK ( buf[  5] == 0xAEE52C50 );
		CHECK ( buf[  6] == 0x870465DC );
		CHECK ( buf[  7] == 0x3C1296BE );
		CHECK ( buf[  8] == 0xB852979D );
		CHECK ( buf[  9] == 0xFF364BB9 );

		// end: last 10 samples
		CHECK ( buf[246] == 0x363FDB43 );
		CHECK ( buf[247] == 0xCA1A72F6 );
		CHECK ( buf[248] == 0xE81714D8 );
		CHECK ( buf[249] == 0xA856DB4A );
		CHECK ( buf[250] == 0xE0EB1795 );
		CHECK ( buf[251] == 0xD6862DFE );
		CHECK ( buf[252] == 0xABA8E966 );
		CHECK ( buf[253] == 0xF617700E );
		CHECK ( buf[254] == 0xDD6D28EF );
		CHECK ( buf[255] == 0x157171F9 );
	}

	SECTION ("uint32_t planar buffer access works as expected")
	{
		// 1024 bytes each interpreted as uint32_t == 256 uint32_t values
		// 256 values / 2 channels == 128 pairs/samples

		auto buf = SampleBufferWrapper<uint32_t, true> {
			bytes.data(), bytes.data() + 512, 512, false };

		CHECK ( 128 == buf.size() );

		// This are now "other" samples than the 16 bit interpretations!

		CHECK ( buf.left_channel (0) == 0xC711C2A5 );
		CHECK ( buf.right_channel(0) == 0x4293D9DB );
		CHECK ( buf.left_channel (1) == 0xF35B9ECC );
		CHECK ( buf.right_channel(1) == 0x6C51920A );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0xD9DBC2A5 );
		CHECK ( buf[  1] == 0x920A9ECC );
		CHECK ( buf[  2] == 0x602D4D95 );
		CHECK ( buf[  3] == 0x870465DC );
		CHECK ( buf[  4] == 0xB852979D );
		CHECK ( buf[  5] == 0xA2D90C0F );
		CHECK ( buf[  6] == 0x001B9BD5 );
		CHECK ( buf[  7] == 0xFDD48469 );
		CHECK ( buf[  8] == 0x4A40F9E6 );
		CHECK ( buf[  9] == 0xA4D8F5F6 );

		// near mid 10 samples
		CHECK ( buf[ 54] == 0x0AA1B644 );
		CHECK ( buf[ 55] == 0x444619CE );
		CHECK ( buf[ 56] == 0xD3770215 );
		CHECK ( buf[ 57] == 0xA54964E1 );
		CHECK ( buf[ 58] == 0x3D925080 );
		CHECK ( buf[ 59] == 0xABAA15C9 );
		CHECK ( buf[ 60] == 0x386E5B27 );
		CHECK ( buf[ 61] == 0xAAECFC5D );
		CHECK ( buf[ 62] == 0x3628136F );
		CHECK ( buf[ 63] == 0x0D4C7D86 );

		// end: last 10 samples
		CHECK ( buf[118] == 0xE1856E87 );
		CHECK ( buf[119] == 0xA4195003 );
		CHECK ( buf[120] == 0x30D64F16 );
		CHECK ( buf[121] == 0xA7EDB5BA );
		CHECK ( buf[122] == 0xBB364C76 );
		CHECK ( buf[123] == 0x363FDB43 );
		CHECK ( buf[124] == 0xE81714D8 );
		CHECK ( buf[125] == 0xE0EB1795 );
		CHECK ( buf[126] == 0xABA8E966 );
		CHECK ( buf[127] == 0xDD6D28EF );
	}

	SECTION ("int32_t planar buffer access works as expected")
	{
		// 1024 bytes each interpreted as int32_t == 256 int32_t values
		// 256 values / 2 channels == 128 pairs/samples

		auto buf = SampleBufferWrapper<int32_t, true> {
			bytes.data(), bytes.data() + 512, 512, false };

		CHECK ( 128 == buf.size() );

		CHECK ( buf.left_channel (0) == -955137371 ); // is now signed!
		CHECK ( buf.right_channel(0) == 0x4293D9DB );
		CHECK ( buf.left_channel (1) == -212099380 );
		CHECK ( buf.right_channel(1) == 0x6C51920A );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0xD9DBC2A5 );
		CHECK ( buf[  1] == 0x920A9ECC );
		CHECK ( buf[  2] == 0x602D4D95 );
		CHECK ( buf[  3] == 0x870465DC );
		CHECK ( buf[  4] == 0xB852979D );
		CHECK ( buf[  5] == 0xA2D90C0F );
		CHECK ( buf[  6] == 0x001B9BD5 );
		CHECK ( buf[  7] == 0xFDD48469 );
		CHECK ( buf[  8] == 0x4A40F9E6 );
		CHECK ( buf[  9] == 0xA4D8F5F6 );

		// near mid 10 samples
		CHECK ( buf[ 54] == 0x0AA1B644 );
		CHECK ( buf[ 55] == 0x444619CE );
		CHECK ( buf[ 56] == 0xD3770215 );
		CHECK ( buf[ 57] == 0xA54964E1 );
		CHECK ( buf[ 58] == 0x3D925080 );
		CHECK ( buf[ 59] == 0xABAA15C9 );
		CHECK ( buf[ 60] == 0x386E5B27 );
		CHECK ( buf[ 61] == 0xAAECFC5D );
		CHECK ( buf[ 62] == 0x3628136F );
		CHECK ( buf[ 63] == 0x0D4C7D86 );

		// end: last 10 samples
		CHECK ( buf[118] == 0xE1856E87 );
		CHECK ( buf[119] == 0xA4195003 );
		CHECK ( buf[120] == 0x30D64F16 );
		CHECK ( buf[121] == 0xA7EDB5BA );
		CHECK ( buf[122] == 0xBB364C76 );
		CHECK ( buf[123] == 0x363FDB43 );
		CHECK ( buf[124] == 0xE81714D8 );
		CHECK ( buf[125] == 0xE0EB1795 );
		CHECK ( buf[126] == 0xABA8E966 );
		CHECK ( buf[127] == 0xDD6D28EF );
	}
}


TEST_CASE ( "SampleBufferWrapper interleaved file", "[samplebufferwrapper]" )
{
	using arcstk::details::SampleBufferWrapper;

	// Load example samples

	std::ifstream in;
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		in.open("samplesequence-test-01.bin",
				std::ifstream::in | std::ifstream::binary);
	} catch (const std::ifstream::failure& f)
	{
		FAIL ("Could not open test data file samplesequence-test-01.bin");
	}

	std::vector<uint8_t> bytes(
			(std::istreambuf_iterator<char>(in)),
			std::istreambuf_iterator<char>()
	);

	in.close();

	REQUIRE ( bytes.size() == 1024 );

	REQUIRE ( bytes[   0] == 0xA5 );
	REQUIRE ( bytes[   1] == 0xC2 );
	REQUIRE ( bytes[   2] == 0x11 );
	REQUIRE ( bytes[   3] == 0xC7 );
	// ... all bytes between
	REQUIRE ( bytes[1020] == 0x6D );
	REQUIRE ( bytes[1021] == 0xDD );
	REQUIRE ( bytes[1022] == 0x71 );
	REQUIRE ( bytes[1023] == 0x15 );


	SECTION ("uint16_t interleaved buffer access works as expected")
	{
		// 1024 bytes == 1024 uint8_t
		// 1024 uint8_t == 512 uint16_t values
		// 512 values / 2 channels == 256 pairs/samples

		auto buf = SampleBufferWrapper<uint16_t, false> { bytes.data(), 1024,
			false };

		CHECK ( 256 == buf.size() );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0xC711C2A5 );
		CHECK ( buf[  1] == 0xF35B9ECC );
		CHECK ( buf[  2] == 0x2C504D95 );
		CHECK ( buf[  3] == 0x96BE65DC );
		CHECK ( buf[  4] == 0x4BB9979D );
		CHECK ( buf[  5] == 0xAE450C0F );
		CHECK ( buf[  6] == 0xDCDE9BD5 );
		CHECK ( buf[  7] == 0x1CFD8469 );
		CHECK ( buf[  8] == 0xCBE1F9E6 );
		CHECK ( buf[  9] == 0x0DF0F5F6 );

		// end: last 10 samples
		CHECK ( buf[246] == 0x338DE185 );
		CHECK ( buf[247] == 0x1B0FA419 );
		CHECK ( buf[248] == 0xE4D030D6 );
		CHECK ( buf[249] == 0xA156A7ED );
		CHECK ( buf[250] == 0x7A99BB36 );
		CHECK ( buf[251] == 0xCA1A363F );
		CHECK ( buf[252] == 0xA856E817 );
		CHECK ( buf[253] == 0xD686E0EB );
		CHECK ( buf[254] == 0xF617ABA8 );
		CHECK ( buf[255] == 0x1571DD6D );
	}

	SECTION ("int16_t interleaved buffer access works as expected")
	{
		// 1024 bytes each interpreted as int16_t == 512 int16_t values
		// 512 values / 2 channels == 256 pairs/samples

		auto buf = SampleBufferWrapper<int16_t, false> { bytes.data(), 1024,
			false };

		CHECK ( 256 == buf.size() );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0xC711C2A5 );
		CHECK ( buf[  1] == 0xF35B9ECC );
		CHECK ( buf[  2] == 0x2C504D95 );
		CHECK ( buf[  3] == 0x96BE65DC );
		CHECK ( buf[  4] == 0x4BB9979D );
		CHECK ( buf[  5] == 0xAE450C0F );
		CHECK ( buf[  6] == 0xDCDE9BD5 );
		CHECK ( buf[  7] == 0x1CFD8469 );
		CHECK ( buf[  8] == 0xCBE1F9E6 );
		CHECK ( buf[  9] == 0x0DF0F5F6 );

		// end: last 10 samples
		CHECK ( buf[246] == 0x338DE185 );
		CHECK ( buf[247] == 0x1B0FA419 );
		CHECK ( buf[248] == 0xE4D030D6 );
		CHECK ( buf[249] == 0xA156A7ED );
		CHECK ( buf[250] == 0x7A99BB36 );
		CHECK ( buf[251] == 0xCA1A363F );
		CHECK ( buf[252] == 0xA856E817 );
		CHECK ( buf[253] == 0xD686E0EB );
		CHECK ( buf[254] == 0xF617ABA8 );
		CHECK ( buf[255] == 0x1571DD6D );
	}

	SECTION ("uint32_t interleaved buffer access works as expected")
	{
		// 1024 bytes each interpreted as uint32_t == 256 uint32_t values
		// 256 values / 2 channels == 128 pairs/samples

		auto buf = SampleBufferWrapper<uint32_t, false> { bytes.data(), 1024,
			false };

		CHECK ( 128 == buf.size() );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0x9ECCC2A5 );
		CHECK ( buf[  1] == 0x65DC4D95 );
		CHECK ( buf[  2] == 0x0C0F979D );
		CHECK ( buf[  3] == 0x84699BD5 );
		CHECK ( buf[  4] == 0xF5F6F9E6 );
		CHECK ( buf[  5] == 0xE6EAC2DA );
		CHECK ( buf[  6] == 0x8E86AA07 );
		CHECK ( buf[  7] == 0x60F6FA60 );
		CHECK ( buf[  8] == 0x4A1FF5A5 );
		CHECK ( buf[  9] == 0x5BCA0129 );

		// near mid 10 samples
		CHECK ( buf[ 54] == 0xE063C7F1 );
		CHECK ( buf[ 55] == 0x8F1BECB6 );
		CHECK ( buf[ 56] == 0x192C81F8 );
		CHECK ( buf[ 57] == 0x44687814 );
		CHECK ( buf[ 58] == 0xDDDA357B );
		CHECK ( buf[ 59] == 0x50036E87 );
		CHECK ( buf[ 60] == 0xB5BA4F16 );
		CHECK ( buf[ 61] == 0xDB434C76 );
		CHECK ( buf[ 62] == 0x179514D8 );
		CHECK ( buf[ 63] == 0x28EFE966 );

		// end: last 10 samples
		CHECK ( buf[118] == 0xE6791252 );
		CHECK ( buf[119] == 0xE46ECE70 );
		CHECK ( buf[120] == 0x352BB52A );
		CHECK ( buf[121] == 0x59952BDA );
		CHECK ( buf[122] == 0x31C575C7 );
		CHECK ( buf[123] == 0xA419E185 );
		CHECK ( buf[124] == 0xA7ED30D6 );
		CHECK ( buf[125] == 0x363FBB36 );
		CHECK ( buf[126] == 0xE0EBE817 );
		CHECK ( buf[127] == 0xDD6DABA8 );
	}

	SECTION ("int32_t interleaved buffer access works as expected")
	{
		// 1024 bytes each interpreted as int32_t == 256 int32_t values
		// 256 values / 2 channels == 128 pairs/samples

		auto buf = SampleBufferWrapper<int32_t, false> { bytes.data(), 1024,
			false };

		CHECK ( 128 == buf.size() );

		// begin: first 10 samples
		CHECK ( buf[  0] == 0x9ECCC2A5 );
		CHECK ( buf[  1] == 0x65DC4D95 );
		CHECK ( buf[  2] == 0x0C0F979D );
		CHECK ( buf[  3] == 0x84699BD5 );
		CHECK ( buf[  4] == 0xF5F6F9E6 );
		CHECK ( buf[  5] == 0xE6EAC2DA );
		CHECK ( buf[  6] == 0x8E86AA07 );
		CHECK ( buf[  7] == 0x60F6FA60 );
		CHECK ( buf[  8] == 0x4A1FF5A5 );
		CHECK ( buf[  9] == 0x5BCA0129 );

		// near mid 10 samples
		CHECK ( buf[ 54] == 0xE063C7F1 );
		CHECK ( buf[ 55] == 0x8F1BECB6 );
		CHECK ( buf[ 56] == 0x192C81F8 );
		CHECK ( buf[ 57] == 0x44687814 );
		CHECK ( buf[ 58] == 0xDDDA357B );
		CHECK ( buf[ 59] == 0x50036E87 );
		CHECK ( buf[ 60] == 0xB5BA4F16 );
		CHECK ( buf[ 61] == 0xDB434C76 );
		CHECK ( buf[ 62] == 0x179514D8 );
		CHECK ( buf[ 63] == 0x28EFE966 );

		// end: last 10 samples
		CHECK ( buf[118] == 0xE6791252 );
		CHECK ( buf[119] == 0xE46ECE70 );
		CHECK ( buf[120] == 0x352BB52A );
		CHECK ( buf[121] == 0x59952BDA );
		CHECK ( buf[122] == 0x31C575C7 );
		CHECK ( buf[123] == 0xA419E185 );
		CHECK ( buf[124] == 0xA7ED30D6 );
		CHECK ( buf[125] == 0x363FBB36 );
		CHECK ( buf[126] == 0xE0EBE817 );
		CHECK ( buf[127] == 0xDD6DABA8 );
	}
}

