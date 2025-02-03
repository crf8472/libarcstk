#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for samples.hpp
 */

#ifndef __LIBARCSTK_SAMPLES_HPP__
#include "samples.hpp"            // TO BE TESTED
#endif

#include <algorithm>              // for max
#include <cstdint>                // for int16_t, uint8_t, uint32_t, int32_t,...
#include <fstream>                // for ifstream,...
#include <iterator>               // for istreambuf_iterator, operator!=
#include <vector>                 // for vector


TEST_CASE ( "SampleSequence allows int16_t and int32_t",
		"[samplesequence] [construction]" )
{
	using arcstk::PlanarSamples;
	using arcstk::InterleavedSamples;

	PlanarSamples<int16_t>      seq_planar_16;
	InterleavedSamples<int16_t> seq_interl_16;
	PlanarSamples<int32_t>      seq_planar_32;
	InterleavedSamples<int32_t> seq_interl_32;
}


TEST_CASE ( "SampleSequence index access works correctly",
		"[samplesequence] [subscript]" )
{
	using arcstk::PlanarSamples;
	using arcstk::InterleavedSamples;

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

	SECTION ("Subscript in16_t interleaved sequence access works as expected")
	{
		InterleavedSamples<int16_t> sequence;

		CHECK ( sequence.typesize() == 2 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xC711C2A5 );
		CHECK ( sequence[1] == 0xF35B9ECC );
		CHECK ( sequence[2] == 0x2C504D95 );
		CHECK ( sequence[3] == 0x96BE65DC );
		CHECK ( sequence[4] == 0x4BB9979D );
		CHECK ( sequence[5] == 0xAE450C0F );
		CHECK ( sequence[6] == 0xDCDE9BD5 );
		CHECK ( sequence[7] == 0x1CFD8469 );
		CHECK ( sequence[8] == 0xCBE1F9E6 );
		CHECK ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x338DE185 );
		CHECK ( sequence[247] == 0x1B0FA419 );
		CHECK ( sequence[248] == 0xE4D030D6 );
		CHECK ( sequence[249] == 0xA156A7ED );
		CHECK ( sequence[250] == 0x7A99BB36 );
		CHECK ( sequence[251] == 0xCA1A363F );
		CHECK ( sequence[252] == 0xA856E817 );
		CHECK ( sequence[253] == 0xD686E0EB );
		CHECK ( sequence[254] == 0xF617ABA8 );
		CHECK ( sequence[255] == 0x1571DD6D );

		sequence.wrap_int_buffer(reinterpret_cast<int16_t*>(&bytes[0]), 512,
				true); // int16_t's

		CHECK ( sequence.size() == 256 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xC711C2A5 );
		CHECK ( sequence[1] == 0xF35B9ECC );
		CHECK ( sequence[2] == 0x2C504D95 );
		CHECK ( sequence[3] == 0x96BE65DC );
		CHECK ( sequence[4] == 0x4BB9979D );
		CHECK ( sequence[5] == 0xAE450C0F );
		CHECK ( sequence[6] == 0xDCDE9BD5 );
		CHECK ( sequence[7] == 0x1CFD8469 );
		CHECK ( sequence[8] == 0xCBE1F9E6 );
		CHECK ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x338DE185 );
		CHECK ( sequence[247] == 0x1B0FA419 );
		CHECK ( sequence[248] == 0xE4D030D6 );
		CHECK ( sequence[249] == 0xA156A7ED );
		CHECK ( sequence[250] == 0x7A99BB36 );
		CHECK ( sequence[251] == 0xCA1A363F );
		CHECK ( sequence[252] == 0xA856E817 );
		CHECK ( sequence[253] == 0xD686E0EB );
		CHECK ( sequence[254] == 0xF617ABA8 );
		CHECK ( sequence[255] == 0x1571DD6D );
	}

	SECTION ("Iterating over a in16_t interleaved sequence works as expected")
	{
		InterleavedSamples<int16_t> sequence;

		CHECK ( sequence.typesize() == 2 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );

		auto it_begin { sequence.begin() };

		// begin: first 10 samples
		CHECK ( *it_begin == 0xC711C2A5 );
		++it_begin;
		CHECK ( *it_begin == 0xF35B9ECC );
		++it_begin;
		CHECK ( *it_begin == 0x2C504D95 );
		++it_begin;
		CHECK ( *it_begin == 0x96BE65DC );
		++it_begin;
		CHECK ( *it_begin == 0x4BB9979D );
		++it_begin;
		CHECK ( *it_begin == 0xAE450C0F );
		++it_begin;
		CHECK ( *it_begin == 0xDCDE9BD5 );
		++it_begin;
		CHECK ( *it_begin == 0x1CFD8469 );
		++it_begin;
		CHECK ( *it_begin == 0xCBE1F9E6 );
		++it_begin;
		CHECK ( *it_begin == 0x0DF0F5F6 );

		auto it_end { sequence.end() };

		// end: last 10 samples
		--it_end;
		CHECK ( *it_end == 0x1571DD6D );
		--it_end;
		CHECK ( *it_end == 0xF617ABA8 );
		--it_end;
		CHECK ( *it_end == 0xD686E0EB );
		--it_end;
		CHECK ( *it_end == 0xA856E817 );
		--it_end;
		CHECK ( *it_end == 0xCA1A363F );
		--it_end;
		CHECK ( *it_end == 0x7A99BB36 );
		--it_end;
		CHECK ( *it_end == 0xA156A7ED );
		--it_end;
		CHECK ( *it_end == 0xE4D030D6 );
		--it_end;
		CHECK ( *it_end == 0x1B0FA419 );
		--it_end;
		CHECK ( *it_end == 0x338DE185 );

		sequence.wrap_int_buffer(reinterpret_cast<int16_t*>(&bytes[0]), 512,
				true); // int16_t's

		CHECK ( sequence.size() == 256 );

		auto it_begin2 { sequence.begin() };

		// begin: first 10 samples
		CHECK ( *it_begin2 == 0xC711C2A5 );
		++it_begin2;
		CHECK ( *it_begin2 == 0xF35B9ECC );
		++it_begin2;
		CHECK ( *it_begin2 == 0x2C504D95 );
		++it_begin2;
		CHECK ( *it_begin2 == 0x96BE65DC );
		++it_begin2;
		CHECK ( *it_begin2 == 0x4BB9979D );
		++it_begin2;
		CHECK ( *it_begin2 == 0xAE450C0F );
		++it_begin2;
		CHECK ( *it_begin2 == 0xDCDE9BD5 );
		++it_begin2;
		CHECK ( *it_begin2 == 0x1CFD8469 );
		++it_begin2;
		CHECK ( *it_begin2 == 0xCBE1F9E6 );
		++it_begin2;
		CHECK ( *it_begin2 == 0x0DF0F5F6 );

		auto it_end2 { sequence.end() };

		// end: last 10 samples
		--it_end2;
		CHECK ( *it_end2 == 0x1571DD6D );
		--it_end2;
		CHECK ( *it_end2 == 0xF617ABA8 );
		--it_end2;
		CHECK ( *it_end2 == 0xD686E0EB );
		--it_end2;
		CHECK ( *it_end2 == 0xA856E817 );
		--it_end2;
		CHECK ( *it_end2 == 0xCA1A363F );
		--it_end2;
		CHECK ( *it_end2 == 0x7A99BB36 );
		--it_end2;
		CHECK ( *it_end2 == 0xA156A7ED );
		--it_end2;
		CHECK ( *it_end2 == 0xE4D030D6 );
		--it_end2;
		CHECK ( *it_end2 == 0x1B0FA419 );
		--it_end2;
		CHECK ( *it_end2 == 0x338DE185 );


		// Random tests for some operators

		auto it { sequence.begin() };

		it += 7;

		CHECK ( *it == 0x1CFD8469 );

		it -= 3;

		CHECK ( *it == 0x4BB9979D );

		auto it2 { it + 5 };

		CHECK ( *it2 == 0x0DF0F5F6 );

		auto it3 { it2 - 4 };

		CHECK ( *it3 == 0xAE450C0F );

		CHECK ( it3 != sequence.begin() );

		it3 -= 4;

		CHECK ( it3 == sequence.begin() + 1 );
	}

	SECTION ("Iteration and Subscript are equivalent for in16_t interleaved sequence")
	{
		InterleavedSamples<int16_t> sequence;

		CHECK ( sequence.typesize() == 2 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );

		decltype( sequence )::size_type index = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index] );
			++index;
		}

		CHECK ( index == 256 );

		sequence.wrap_int_buffer(reinterpret_cast<int16_t*>(&bytes[0]), 512,
				true); // int16_t's

		CHECK ( sequence.size() == 256 );

		InterleavedSamples<int16_t>::size_type index2 = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index2] );
			++index2;
		}

		CHECK ( index2 == 256 );
	}

	SECTION ("Subscript in32_t interleaved sequence")
	{
		InterleavedSamples<int32_t> sequence;

		CHECK ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		CHECK ( 128 == sequence.size() );
		CHECK ( 128 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0x9ECCC2A5 );
		CHECK ( sequence[1] == 0x65DC4D95 );
		CHECK ( sequence[2] == 0x0C0F979D );
		CHECK ( sequence[3] == 0x84699BD5 );
		CHECK ( sequence[4] == 0xF5F6F9E6 );
		CHECK ( sequence[5] == 0xE6EAC2DA );
		CHECK ( sequence[6] == 0x8E86AA07 );
		CHECK ( sequence[7] == 0x60F6FA60 );
		CHECK ( sequence[8] == 0x4A1FF5A5 );
		CHECK ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0xE063C7F1 );
		CHECK ( sequence[55] == 0x8F1BECB6 );
		CHECK ( sequence[56] == 0x192C81F8 );
		CHECK ( sequence[57] == 0x44687814 );
		CHECK ( sequence[58] == 0xDDDA357B );
		CHECK ( sequence[59] == 0x50036E87 );
		CHECK ( sequence[60] == 0xB5BA4F16 );
		CHECK ( sequence[61] == 0xDB434C76 );
		CHECK ( sequence[62] == 0x179514D8 );
		CHECK ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE6791252 );
		CHECK ( sequence[119] == 0xE46ECE70 );
		CHECK ( sequence[120] == 0x352BB52A );
		CHECK ( sequence[121] == 0x59952BDA );
		CHECK ( sequence[122] == 0x31C575C7 );
		CHECK ( sequence[123] == 0xA419E185 );
		CHECK ( sequence[124] == 0xA7ED30D6 );
		CHECK ( sequence[125] == 0x363FBB36 );
		CHECK ( sequence[126] == 0xE0EBE817 );
		CHECK ( sequence[127] == 0xDD6DABA8 );

		sequence.wrap_int_buffer(reinterpret_cast<int32_t*>(&bytes[0]), 256,
				true); // int32_t's

		CHECK ( sequence.size() == 128 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0x9ECCC2A5 );
		CHECK ( sequence[1] == 0x65DC4D95 );
		CHECK ( sequence[2] == 0x0C0F979D );
		CHECK ( sequence[3] == 0x84699BD5 );
		CHECK ( sequence[4] == 0xF5F6F9E6 );
		CHECK ( sequence[5] == 0xE6EAC2DA );
		CHECK ( sequence[6] == 0x8E86AA07 );
		CHECK ( sequence[7] == 0x60F6FA60 );
		CHECK ( sequence[8] == 0x4A1FF5A5 );
		CHECK ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0xE063C7F1 );
		CHECK ( sequence[55] == 0x8F1BECB6 );
		CHECK ( sequence[56] == 0x192C81F8 );
		CHECK ( sequence[57] == 0x44687814 );
		CHECK ( sequence[58] == 0xDDDA357B );
		CHECK ( sequence[59] == 0x50036E87 );
		CHECK ( sequence[60] == 0xB5BA4F16 );
		CHECK ( sequence[61] == 0xDB434C76 );
		CHECK ( sequence[62] == 0x179514D8 );
		CHECK ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE6791252 );
		CHECK ( sequence[119] == 0xE46ECE70 );
		CHECK ( sequence[120] == 0x352BB52A );
		CHECK ( sequence[121] == 0x59952BDA );
		CHECK ( sequence[122] == 0x31C575C7 );
		CHECK ( sequence[123] == 0xA419E185 );
		CHECK ( sequence[124] == 0xA7ED30D6 );
		CHECK ( sequence[125] == 0x363FBB36 );
		CHECK ( sequence[126] == 0xE0EBE817 );
		CHECK ( sequence[127] == 0xDD6DABA8 );
	}

	SECTION ("Iteration and Subscript are equivalent for in32_t interleaved sequence")
	{
		InterleavedSamples<int32_t> sequence;

		CHECK ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		CHECK ( 128 == sequence.size() );
		CHECK ( 128 == sequence.end() - sequence.begin() );

		InterleavedSamples<int32_t>::size_type index = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index] );
			++index;
		}

		CHECK ( index == 128 );

		sequence.wrap_int_buffer(reinterpret_cast<int32_t*>(&bytes[0]), 256,
				true); // int32_t's

		CHECK ( sequence.size() == 128 );

		InterleavedSamples<int32_t>::size_type index2 = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index2] );
			++index2;
		}

		CHECK ( index2 == 128 );
	}

	SECTION ("Subscript in16_t planar sequence")
	{
		PlanarSamples<int16_t> sequence;

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x4293C711 );
		CHECK ( sequence[2] == 0x920a9ECC );
		CHECK ( sequence[3] == 0x6C51F35B );
		CHECK ( sequence[4] == 0x602D4D95 );
		CHECK ( sequence[5] == 0xAEE52C50 );
		CHECK ( sequence[6] == 0x870465DC );
		CHECK ( sequence[7] == 0x3C1296BE );
		CHECK ( sequence[8] == 0xB852979D );
		CHECK ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x363FDB43 );
		CHECK ( sequence[247] == 0xCA1A72F6 );
		CHECK ( sequence[248] == 0xE81714D8 );
		CHECK ( sequence[249] == 0xA856DB4A );
		CHECK ( sequence[250] == 0xE0EB1795 );
		CHECK ( sequence[251] == 0xD6862DFE );
		CHECK ( sequence[252] == 0xABA8E966 );
		CHECK ( sequence[253] == 0xF617700E );
		CHECK ( sequence[254] == 0xDD6D28EF );
		CHECK ( sequence[255] == 0x157171F9 );

		sequence.wrap_int_buffer(reinterpret_cast<int16_t*>(&bytes[0]),
				reinterpret_cast<int16_t*>(&bytes[512]),
				256, true); // int16_t's

		CHECK ( sequence.size() == 256 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x4293C711 );
		CHECK ( sequence[2] == 0x920a9ECC );
		CHECK ( sequence[3] == 0x6C51F35B );
		CHECK ( sequence[4] == 0x602D4D95 );
		CHECK ( sequence[5] == 0xAEE52C50 );
		CHECK ( sequence[6] == 0x870465DC );
		CHECK ( sequence[7] == 0x3C1296BE );
		CHECK ( sequence[8] == 0xB852979D );
		CHECK ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x363FDB43 );
		CHECK ( sequence[247] == 0xCA1A72F6 );
		CHECK ( sequence[248] == 0xE81714D8 );
		CHECK ( sequence[249] == 0xA856DB4A );
		CHECK ( sequence[250] == 0xE0EB1795 );
		CHECK ( sequence[251] == 0xD6862DFE );
		CHECK ( sequence[252] == 0xABA8E966 );
		CHECK ( sequence[253] == 0xF617700E );
		CHECK ( sequence[254] == 0xDD6D28EF );
		CHECK ( sequence[255] == 0x157171F9 );
	}

	SECTION ("Iteration and Subscript are equivalent for in16_t planar sequence")
	{
		PlanarSamples<int16_t> sequence;

		CHECK ( sequence.typesize() == 2 );

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );

		PlanarSamples<int16_t>::size_type index = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index] );
			++index;
		}

		CHECK ( index == 256 );

		sequence.wrap_int_buffer(reinterpret_cast<int16_t*>(&bytes[0]),
				reinterpret_cast<int16_t*>(&bytes[0]),
				256, true); // int16_t's

		CHECK ( sequence.size() == 256 );

		PlanarSamples<int16_t>::size_type index2 = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index2] );
			++index2;
		}

		CHECK ( index2 == 256 );
	}

	SECTION ("Subscript in32_t planar sequence")
	{
		PlanarSamples<int32_t> sequence;

		CHECK ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( 128 == sequence.size() );
		CHECK ( 128 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x920A9ECC );
		CHECK ( sequence[2] == 0x602D4D95 );
		CHECK ( sequence[3] == 0x870465DC );
		CHECK ( sequence[4] == 0xB852979D );
		CHECK ( sequence[5] == 0xA2D90C0F );
		CHECK ( sequence[6] == 0x001B9BD5 );
		CHECK ( sequence[7] == 0xFDD48469 );
		CHECK ( sequence[8] == 0x4A40F9E6 );
		CHECK ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0x0AA1B644 );
		CHECK ( sequence[55] == 0x444619CE );
		CHECK ( sequence[56] == 0xD3770215 );
		CHECK ( sequence[57] == 0xA54964E1 );
		CHECK ( sequence[58] == 0x3D925080 );
		CHECK ( sequence[59] == 0xABAA15C9 );
		CHECK ( sequence[60] == 0x386E5B27 );
		CHECK ( sequence[61] == 0xAAECFC5D );
		CHECK ( sequence[62] == 0x3628136F );
		CHECK ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE1856E87 );
		CHECK ( sequence[119] == 0xA4195003 );
		CHECK ( sequence[120] == 0x30D64F16 );
		CHECK ( sequence[121] == 0xA7EDB5BA );
		CHECK ( sequence[122] == 0xBB364C76 );
		CHECK ( sequence[123] == 0x363FDB43 );
		CHECK ( sequence[124] == 0xE81714D8 );
		CHECK ( sequence[125] == 0xE0EB1795 );
		CHECK ( sequence[126] == 0xABA8E966 );
		CHECK ( sequence[127] == 0xDD6D28EF );

		sequence.wrap_int_buffer(reinterpret_cast<int32_t*>(&bytes[0]),
				reinterpret_cast<int32_t*>(&bytes[512]),
				128, true); // int32_t's

		CHECK ( sequence.size() == 128 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x920A9ECC );
		CHECK ( sequence[2] == 0x602D4D95 );
		CHECK ( sequence[3] == 0x870465DC );
		CHECK ( sequence[4] == 0xB852979D );
		CHECK ( sequence[5] == 0xA2D90C0F );
		CHECK ( sequence[6] == 0x001B9BD5 );
		CHECK ( sequence[7] == 0xFDD48469 );
		CHECK ( sequence[8] == 0x4A40F9E6 );
		CHECK ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0x0AA1B644 );
		CHECK ( sequence[55] == 0x444619CE );
		CHECK ( sequence[56] == 0xD3770215 );
		CHECK ( sequence[57] == 0xA54964E1 );
		CHECK ( sequence[58] == 0x3D925080 );
		CHECK ( sequence[59] == 0xABAA15C9 );
		CHECK ( sequence[60] == 0x386E5B27 );
		CHECK ( sequence[61] == 0xAAECFC5D );
		CHECK ( sequence[62] == 0x3628136F );
		CHECK ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE1856E87 );
		CHECK ( sequence[119] == 0xA4195003 );
		CHECK ( sequence[120] == 0x30D64F16 );
		CHECK ( sequence[121] == 0xA7EDB5BA );
		CHECK ( sequence[122] == 0xBB364C76 );
		CHECK ( sequence[123] == 0x363FDB43 );
		CHECK ( sequence[124] == 0xE81714D8 );
		CHECK ( sequence[125] == 0xE0EB1795 );
		CHECK ( sequence[126] == 0xABA8E966 );
		CHECK ( sequence[127] == 0xDD6D28EF );
	}

	SECTION ("Iteration and Subscript are equivalent for in32_t planar sequence")
	{
		PlanarSamples<int32_t> sequence;

		CHECK ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( 128 == sequence.size() );
		CHECK ( 128 == sequence.end() - sequence.begin() );

		PlanarSamples<int32_t>::size_type index = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index] );
			++index;
		}

		CHECK ( index == 128 );

		sequence.wrap_int_buffer(reinterpret_cast<int32_t*>(&bytes[0]),
				reinterpret_cast<int32_t*>(&bytes[0]),
				128, true); // int32_t's

		CHECK ( sequence.size() == 128 );

		PlanarSamples<int32_t>::size_type index2 = 0;
		for (const auto& sample : sequence)
		{
			CHECK ( sample == sequence[index2] );
			++index2;
		}

		CHECK ( index2 == 128 );
	}

	SECTION ("Subscript uin16_t interleaved sequence")
	{
		InterleavedSamples<uint16_t> sequence;

		CHECK ( sequence.typesize() == 2 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xC711C2A5 );
		CHECK ( sequence[1] == 0xF35B9ECC );
		CHECK ( sequence[2] == 0x2C504D95 );
		CHECK ( sequence[3] == 0x96BE65DC );
		CHECK ( sequence[4] == 0x4BB9979D );
		CHECK ( sequence[5] == 0xAE450C0F );
		CHECK ( sequence[6] == 0xDCDE9BD5 );
		CHECK ( sequence[7] == 0x1CFD8469 );
		CHECK ( sequence[8] == 0xCBE1F9E6 );
		CHECK ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x338DE185 );
		CHECK ( sequence[247] == 0x1B0FA419 );
		CHECK ( sequence[248] == 0xE4D030D6 );
		CHECK ( sequence[249] == 0xA156A7ED );
		CHECK ( sequence[250] == 0x7A99BB36 );
		CHECK ( sequence[251] == 0xCA1A363F );
		CHECK ( sequence[252] == 0xA856E817 );
		CHECK ( sequence[253] == 0xD686E0EB );
		CHECK ( sequence[254] == 0xF617ABA8 );
		CHECK ( sequence[255] == 0x1571DD6D );

		sequence.wrap_int_buffer(reinterpret_cast<uint16_t*>(&bytes[0]), 512,
				true); // uint16_t

		CHECK ( sequence.size() == 256 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xC711C2A5 );
		CHECK ( sequence[1] == 0xF35B9ECC );
		CHECK ( sequence[2] == 0x2C504D95 );
		CHECK ( sequence[3] == 0x96BE65DC );
		CHECK ( sequence[4] == 0x4BB9979D );
		CHECK ( sequence[5] == 0xAE450C0F );
		CHECK ( sequence[6] == 0xDCDE9BD5 );
		CHECK ( sequence[7] == 0x1CFD8469 );
		CHECK ( sequence[8] == 0xCBE1F9E6 );
		CHECK ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x338DE185 );
		CHECK ( sequence[247] == 0x1B0FA419 );
		CHECK ( sequence[248] == 0xE4D030D6 );
		CHECK ( sequence[249] == 0xA156A7ED );
		CHECK ( sequence[250] == 0x7A99BB36 );
		CHECK ( sequence[251] == 0xCA1A363F );
		CHECK ( sequence[252] == 0xA856E817 );
		CHECK ( sequence[253] == 0xD686E0EB );
		CHECK ( sequence[254] == 0xF617ABA8 );
		CHECK ( sequence[255] == 0x1571DD6D );
	}

	SECTION ("Subscript uin16_t planar sequence")
	{
		PlanarSamples<uint16_t> sequence;

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x4293C711 );
		CHECK ( sequence[2] == 0x920a9ECC );
		CHECK ( sequence[3] == 0x6C51F35B );
		CHECK ( sequence[4] == 0x602D4D95 );
		CHECK ( sequence[5] == 0xAEE52C50 );
		CHECK ( sequence[6] == 0x870465DC );
		CHECK ( sequence[7] == 0x3C1296BE );
		CHECK ( sequence[8] == 0xB852979D );
		CHECK ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x363FDB43 );
		CHECK ( sequence[247] == 0xCA1A72F6 );
		CHECK ( sequence[248] == 0xE81714D8 );
		CHECK ( sequence[249] == 0xA856DB4A );
		CHECK ( sequence[250] == 0xE0EB1795 );
		CHECK ( sequence[251] == 0xD6862DFE );
		CHECK ( sequence[252] == 0xABA8E966 );
		CHECK ( sequence[253] == 0xF617700E );
		CHECK ( sequence[254] == 0xDD6D28EF );
		CHECK ( sequence[255] == 0x157171F9 );

		sequence.wrap_int_buffer(reinterpret_cast<uint16_t*>(&bytes[0]),
				reinterpret_cast<uint16_t*>(&bytes[512]),
				256, true); // uint16_t's

		CHECK ( sequence.size() == 256 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x4293C711 );
		CHECK ( sequence[2] == 0x920a9ECC );
		CHECK ( sequence[3] == 0x6C51F35B );
		CHECK ( sequence[4] == 0x602D4D95 );
		CHECK ( sequence[5] == 0xAEE52C50 );
		CHECK ( sequence[6] == 0x870465DC );
		CHECK ( sequence[7] == 0x3C1296BE );
		CHECK ( sequence[8] == 0xB852979D );
		CHECK ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		CHECK ( sequence[246] == 0x363FDB43 );
		CHECK ( sequence[247] == 0xCA1A72F6 );
		CHECK ( sequence[248] == 0xE81714D8 );
		CHECK ( sequence[249] == 0xA856DB4A );
		CHECK ( sequence[250] == 0xE0EB1795 );
		CHECK ( sequence[251] == 0xD6862DFE );
		CHECK ( sequence[252] == 0xABA8E966 );
		CHECK ( sequence[253] == 0xF617700E );
		CHECK ( sequence[254] == 0xDD6D28EF );
		CHECK ( sequence[255] == 0x157171F9 );
	}

	SECTION ("Subscript uin32_t interleaved sequence")
	{
		InterleavedSamples<uint32_t> sequence;

		CHECK ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		CHECK ( 128 == sequence.size() );
		CHECK ( 128 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0x9ECCC2A5 );
		CHECK ( sequence[1] == 0x65DC4D95 );
		CHECK ( sequence[2] == 0x0C0F979D );
		CHECK ( sequence[3] == 0x84699BD5 );
		CHECK ( sequence[4] == 0xF5F6F9E6 );
		CHECK ( sequence[5] == 0xE6EAC2DA );
		CHECK ( sequence[6] == 0x8E86AA07 );
		CHECK ( sequence[7] == 0x60F6FA60 );
		CHECK ( sequence[8] == 0x4A1FF5A5 );
		CHECK ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0xE063C7F1 );
		CHECK ( sequence[55] == 0x8F1BECB6 );
		CHECK ( sequence[56] == 0x192C81F8 );
		CHECK ( sequence[57] == 0x44687814 );
		CHECK ( sequence[58] == 0xDDDA357B );
		CHECK ( sequence[59] == 0x50036E87 );
		CHECK ( sequence[60] == 0xB5BA4F16 );
		CHECK ( sequence[61] == 0xDB434C76 );
		CHECK ( sequence[62] == 0x179514D8 );
		CHECK ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE6791252 );
		CHECK ( sequence[119] == 0xE46ECE70 );
		CHECK ( sequence[120] == 0x352BB52A );
		CHECK ( sequence[121] == 0x59952BDA );
		CHECK ( sequence[122] == 0x31C575C7 );
		CHECK ( sequence[123] == 0xA419E185 );
		CHECK ( sequence[124] == 0xA7ED30D6 );
		CHECK ( sequence[125] == 0x363FBB36 );
		CHECK ( sequence[126] == 0xE0EBE817 );
		CHECK ( sequence[127] == 0xDD6DABA8 );

		sequence.wrap_int_buffer(reinterpret_cast<uint32_t*>(&bytes[0]), 256,
				true); // uint32_t

		CHECK ( sequence.size() == 128 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0x9ECCC2A5 );
		CHECK ( sequence[1] == 0x65DC4D95 );
		CHECK ( sequence[2] == 0x0C0F979D );
		CHECK ( sequence[3] == 0x84699BD5 );
		CHECK ( sequence[4] == 0xF5F6F9E6 );
		CHECK ( sequence[5] == 0xE6EAC2DA );
		CHECK ( sequence[6] == 0x8E86AA07 );
		CHECK ( sequence[7] == 0x60F6FA60 );
		CHECK ( sequence[8] == 0x4A1FF5A5 );
		CHECK ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0xE063C7F1 );
		CHECK ( sequence[55] == 0x8F1BECB6 );
		CHECK ( sequence[56] == 0x192C81F8 );
		CHECK ( sequence[57] == 0x44687814 );
		CHECK ( sequence[58] == 0xDDDA357B );
		CHECK ( sequence[59] == 0x50036E87 );
		CHECK ( sequence[60] == 0xB5BA4F16 );
		CHECK ( sequence[61] == 0xDB434C76 );
		CHECK ( sequence[62] == 0x179514D8 );
		CHECK ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE6791252 );
		CHECK ( sequence[119] == 0xE46ECE70 );
		CHECK ( sequence[120] == 0x352BB52A );
		CHECK ( sequence[121] == 0x59952BDA );
		CHECK ( sequence[122] == 0x31C575C7 );
		CHECK ( sequence[123] == 0xA419E185 );
		CHECK ( sequence[124] == 0xA7ED30D6 );
		CHECK ( sequence[125] == 0x363FBB36 );
		CHECK ( sequence[126] == 0xE0EBE817 );
		CHECK ( sequence[127] == 0xDD6DABA8 );
	}

	SECTION ("Subscript uin32_t planar sequence")
	{
		PlanarSamples<uint32_t> sequence;

		CHECK ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( 128 == sequence.size() );
		CHECK ( 128 == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x920A9ECC );
		CHECK ( sequence[2] == 0x602D4D95 );
		CHECK ( sequence[3] == 0x870465DC );
		CHECK ( sequence[4] == 0xB852979D );
		CHECK ( sequence[5] == 0xA2D90C0F );
		CHECK ( sequence[6] == 0x001B9BD5 );
		CHECK ( sequence[7] == 0xFDD48469 );
		CHECK ( sequence[8] == 0x4A40F9E6 );
		CHECK ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0x0AA1B644 );
		CHECK ( sequence[55] == 0x444619CE );
		CHECK ( sequence[56] == 0xD3770215 );
		CHECK ( sequence[57] == 0xA54964E1 );
		CHECK ( sequence[58] == 0x3D925080 );
		CHECK ( sequence[59] == 0xABAA15C9 );
		CHECK ( sequence[60] == 0x386E5B27 );
		CHECK ( sequence[61] == 0xAAECFC5D );
		CHECK ( sequence[62] == 0x3628136F );
		CHECK ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE1856E87 );
		CHECK ( sequence[119] == 0xA4195003 );
		CHECK ( sequence[120] == 0x30D64F16 );
		CHECK ( sequence[121] == 0xA7EDB5BA );
		CHECK ( sequence[122] == 0xBB364C76 );
		CHECK ( sequence[123] == 0x363FDB43 );
		CHECK ( sequence[124] == 0xE81714D8 );
		CHECK ( sequence[125] == 0xE0EB1795 );
		CHECK ( sequence[126] == 0xABA8E966 );
		CHECK ( sequence[127] == 0xDD6D28EF );

		sequence.wrap_int_buffer(reinterpret_cast<uint32_t*>(&bytes[0]),
				reinterpret_cast<uint32_t*>(&bytes[512]),
				128, true); // uint32_t's

		CHECK ( sequence.size() == 128 );

		// begin: first 10 samples
		CHECK ( sequence[0] == 0xD9DBC2A5 );
		CHECK ( sequence[1] == 0x920A9ECC );
		CHECK ( sequence[2] == 0x602D4D95 );
		CHECK ( sequence[3] == 0x870465DC );
		CHECK ( sequence[4] == 0xB852979D );
		CHECK ( sequence[5] == 0xA2D90C0F );
		CHECK ( sequence[6] == 0x001B9BD5 );
		CHECK ( sequence[7] == 0xFDD48469 );
		CHECK ( sequence[8] == 0x4A40F9E6 );
		CHECK ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		CHECK ( sequence[54] == 0x0AA1B644 );
		CHECK ( sequence[55] == 0x444619CE );
		CHECK ( sequence[56] == 0xD3770215 );
		CHECK ( sequence[57] == 0xA54964E1 );
		CHECK ( sequence[58] == 0x3D925080 );
		CHECK ( sequence[59] == 0xABAA15C9 );
		CHECK ( sequence[60] == 0x386E5B27 );
		CHECK ( sequence[61] == 0xAAECFC5D );
		CHECK ( sequence[62] == 0x3628136F );
		CHECK ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		CHECK ( sequence[118] == 0xE1856E87 );
		CHECK ( sequence[119] == 0xA4195003 );
		CHECK ( sequence[120] == 0x30D64F16 );
		CHECK ( sequence[121] == 0xA7EDB5BA );
		CHECK ( sequence[122] == 0xBB364C76 );
		CHECK ( sequence[123] == 0x363FDB43 );
		CHECK ( sequence[124] == 0xE81714D8 );
		CHECK ( sequence[125] == 0xE0EB1795 );
		CHECK ( sequence[126] == 0xABA8E966 );
		CHECK ( sequence[127] == 0xDD6D28EF );
	}
}


TEST_CASE ( "SampleIterator increment and decrement",
		"[samplesequence] [iterator]" )
{
	using arcstk::PlanarSamples;
	using arcstk::InterleavedSamples;

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


	SECTION ("Iterator Equality")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( 128 == sequence.size() );
		REQUIRE ( 128 == sequence.end() - sequence.begin() );

		const auto begin1 = sequence.begin();
		auto begin2 = sequence.begin();
		auto begin3 = sequence.begin();

		// const == non-const
		CHECK ( begin1 == begin2 );
		CHECK ( not(begin1 != begin2) );

		// non-const == non-const
		CHECK ( begin3 == begin2 );
		CHECK ( not(begin3 != begin2) );

		// move one of them
		begin2++;
		CHECK ( not(begin3 == begin2) );
		CHECK ( begin3 != begin2 );

		// take begin of other sequence

		InterleavedSamples<uint32_t> sequence_other;
		sequence_other.wrap_byte_buffer(&bytes[0], 1024, true); // bytes
		auto begin_other = sequence_other.begin();

		CHECK ( not(begin_other == begin2) );
		CHECK ( begin_other != begin2 );
	}

	SECTION ("Iterator is default constructible")
	{
		using arcstk::SampleIterator;

		SampleIterator<int16_t,  true,  false> iterator1;
		SampleIterator<int16_t,  false, false> iterator2;
		SampleIterator<int16_t,  true,  true>  iterator3;
		SampleIterator<int16_t,  false, true>  iterator4;

		SampleIterator<uint16_t, true,  false> iterator5;
		SampleIterator<uint16_t, false, false> iterator6;
		SampleIterator<uint16_t, true,  true>  iterator7;
		SampleIterator<uint16_t, false, true>  iterator8;

		SampleIterator<int32_t,  true,  false> iterator9;
		SampleIterator<int32_t,  false, false> iterator10;
		SampleIterator<int32_t,  true,  true>  iterator11;
		SampleIterator<int32_t,  false, true>  iterator12;

		SampleIterator<uint32_t, true,  false> iterator13;
		SampleIterator<uint32_t, false, false> iterator14;
		SampleIterator<uint32_t, true,  true>  iterator15;
		SampleIterator<uint32_t, false, true>  iterator16;
	}

	SECTION ("Iterator is destructible")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		auto pointer = new SampleIterator<uint32_t, false, false>(
				sequence.begin());

		delete pointer;
	}

	SECTION ("Iterator Copy Constructor")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( 128 == sequence.size() );
		REQUIRE ( 128 == sequence.end() - sequence.begin() );

		auto const_begin { sequence.cbegin() };
		auto const_end   { sequence.cend() };

		REQUIRE ( const_begin != const_end );

		// Copy from const to const iterator

		SampleIterator<uint32_t, false, true> const_copy { const_begin };

		CHECK ( const_copy == const_begin );
		CHECK ( not(const_copy != const_begin) );

		// Copy from non-const to non-const iterator

		SampleIterator<uint32_t, false, false> nonconst_copy(sequence.begin());

		CHECK ( nonconst_copy == sequence.begin() );
		CHECK ( not(nonconst_copy != sequence.begin()) );

		// Copy from non-const to const iterator

		SampleIterator<uint32_t, false, true> const_copy2(sequence.begin());

		CHECK ( const_copy2 == sequence.begin() );
		CHECK ( not(const_copy2 != sequence.begin()) );

		// Copy from const to non-const iterator is forbidden!

		//SampleIterator<uint32_t, false, false> const_copy3(sequence.cbegin());

		//CHECK ( const_copy3 == sequence.begin() );
		//CHECK ( not(const_copy3 != sequence.begin()) );
	}

	SECTION ("Iterator Copy Assignment")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( 128 == sequence.size() );
		REQUIRE ( 128 == sequence.end() - sequence.begin() );

		auto const_begin { sequence.cbegin() };
		auto const_end   { sequence.cend() };

		REQUIRE ( const_begin != const_end );


		SampleIterator<uint32_t, false, true> const_it;

		const_it = const_begin;

		CHECK ( const_it == const_begin );
		CHECK ( const_it != const_end   );

		const_it = const_end;

		CHECK ( const_it != const_begin );
		CHECK ( const_it == const_end   );
	}

	SECTION ("Iterator Move constructor")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( 128 == sequence.size() );
		REQUIRE ( 128 == sequence.end() - sequence.begin() );

		auto const_begin { sequence.cbegin() };
		auto const_end   { sequence.cend() };

		REQUIRE ( const_begin != const_end );


		auto const_it1(std::move(const_begin)); // TODO Tests copy-or-move

		CHECK ( const_it1 == sequence.cbegin() );

		auto const_it2(std::move(const_end));

		CHECK ( const_it2 == sequence.end() );
	}

	SECTION ("Iterator Move Assignment")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( 128 == sequence.size() );
		REQUIRE ( 128 == sequence.end() - sequence.begin() );

		auto const_begin { sequence.cbegin() };
		auto const_end   { sequence.cend() };

		REQUIRE ( const_begin != const_end );

		auto const_it = sequence.cbegin();


		const_it = std::move(const_end); // TODO Tests copy-or-move

		CHECK ( const_it == sequence.cend() );

		const_it = std::move(const_begin);

		CHECK ( const_it == sequence.cbegin() );
	}

	SECTION ("Iterator Swap")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( 128 == sequence.size() );
		REQUIRE ( 128 == sequence.end() - sequence.begin() );

		auto begin { sequence.begin() };
		auto end   { sequence.end() };

		CHECK ( begin == sequence.begin() );
		CHECK ( end   == sequence.end() );

		swap(begin, end);

		CHECK ( end   == sequence.begin() );
		CHECK ( begin == sequence.end() );
	}

	SECTION ("Iterator 16 bit begin and end")
	{
		PlanarSamples<int16_t> sequence;

		CHECK ( sequence.begin() == sequence.end() );
		CHECK ( 0 == sequence.size() );
		CHECK ( 0 == sequence.end() - sequence.begin() );
		CHECK ( 0 == std::end(sequence) - std::begin(sequence) );
		//CHECK ( sequence.empty() );

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( sequence.begin() != sequence.end() );
		CHECK ( 256 == sequence.size() );
		CHECK ( 256 == sequence.end() - sequence.begin() );
		CHECK ( 256 == std::end(sequence) - std::begin(sequence) );
		//CHECK ( not sequence.empty() );

		CHECK ( *sequence.begin()     == 0xD9DBC2A5 );
		CHECK ( *std::begin(sequence) == 0xD9DBC2A5 );
	}

	SECTION ("Iterator 32 bit begin and end")
	{
		PlanarSamples<int32_t> sequence;

		CHECK ( sequence.begin() == sequence.end() );
		CHECK ( 0 == sequence.size() );
		CHECK ( 0 == sequence.end() - sequence.begin() );
		CHECK ( 0 == std::end(sequence) - std::begin(sequence) );
		//CHECK ( sequence.empty() );

		sequence.wrap_byte_buffer(&bytes[0], &bytes[512], 512, true); // bytes

		CHECK ( sequence.begin() != sequence.end() );
		CHECK ( 128 == sequence.size() );
		CHECK ( 128 == sequence.end() - sequence.begin() );
		CHECK ( 128 == std::end(sequence) - std::begin(sequence) );
		//CHECK ( not sequence.empty() );

		CHECK ( *sequence.begin()     == 0xD9DBC2A5 );
		CHECK ( *std::begin(sequence) == 0xD9DBC2A5 );
	}

	SECTION ("Iterator prefix increment begins on beginning and ends on end")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;
		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( sequence.size() == 128 );

		auto ptr = sequence.begin();

		// begin: first 10 samples
		CHECK (    *ptr  == 0x9ECCC2A5 );
		CHECK ( *(++ptr) == 0x65DC4D95 );
		CHECK ( *(++ptr) == 0x0C0F979D );
		CHECK ( *(++ptr) == 0x84699BD5 );
		CHECK ( *(++ptr) == 0xF5F6F9E6 );
		CHECK ( *(++ptr) == 0xE6EAC2DA );
		CHECK ( *(++ptr) == 0x8E86AA07 );
		CHECK ( *(++ptr) == 0x60F6FA60 );
		CHECK ( *(++ptr) == 0x4A1FF5A5 );
		CHECK ( *(++ptr) == 0x5BCA0129 );

		ptr += 109;

		// end: last 10 samples
		CHECK (    *ptr  == 0xE6791252 ); // 118
		CHECK ( *(++ptr) == 0xE46ECE70 );
		CHECK ( *(++ptr) == 0x352BB52A );
		CHECK ( *(++ptr) == 0x59952BDA );
		CHECK ( *(++ptr) == 0x31C575C7 );
		CHECK ( *(++ptr) == 0xA419E185 );
		CHECK ( *(++ptr) == 0xA7ED30D6 );
		CHECK ( *(++ptr) == 0x363FBB36 );
		CHECK ( *(++ptr) == 0xE0EBE817 );
		CHECK ( *(++ptr) == 0xDD6DABA8 );

		CHECK ( ++ptr == sequence.end() );
	}

	SECTION ("Iterator std::next begins on beginning and ends on end")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;
		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( sequence.size() == 128 );

		auto ptr = std::begin(sequence);

		// begin: first 10 samples
		CHECK ( *ptr  == 0x9ECCC2A5 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x65DC4D95 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x0C0F979D );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x84699BD5 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0xF5F6F9E6 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0xE6EAC2DA );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x8E86AA07 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x60F6FA60 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x4A1FF5A5 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x5BCA0129 );

		ptr += 109;

		// end: last 10 samples
		CHECK ( *ptr  == 0xE6791252 ); // 118

		ptr = std::next(ptr);
		CHECK ( *ptr == 0xE46ECE70 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x352BB52A );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x59952BDA );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x31C575C7 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0xA419E185 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0xA7ED30D6 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0x363FBB36 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0xE0EBE817 );

		ptr = std::next(ptr);
		CHECK ( *ptr == 0xDD6DABA8 );

		ptr = std::next(ptr);
		CHECK ( ptr == sequence.end() );
	}

	SECTION ("Iterator prefix decrement begins on end and ends on beginning")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;
		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( sequence.size() == 128 );

		auto ptr = std::end(sequence);

		// end: last 10 samples
		CHECK ( *(--ptr) == 0xDD6DABA8 );
		CHECK ( *(--ptr) == 0xE0EBE817 );
		CHECK ( *(--ptr) == 0x363FBB36 );
		CHECK ( *(--ptr) == 0xA7ED30D6 );
		CHECK ( *(--ptr) == 0xA419E185 );
		CHECK ( *(--ptr) == 0x31C575C7 );
		CHECK ( *(--ptr) == 0x59952BDA );
		CHECK ( *(--ptr) == 0x352BB52A );
		CHECK ( *(--ptr) == 0xE46ECE70 );
		CHECK ( *(--ptr) == 0xE6791252 ); // 118

		ptr -= 108;

		// begin: first 10 samples
		CHECK ( *(--ptr) == 0x5BCA0129 );
		CHECK ( *(--ptr) == 0x4A1FF5A5 );
		CHECK ( *(--ptr) == 0x60F6FA60 );
		CHECK ( *(--ptr) == 0x8E86AA07 );
		CHECK ( *(--ptr) == 0xE6EAC2DA );
		CHECK ( *(--ptr) == 0xF5F6F9E6 );
		CHECK ( *(--ptr) == 0x84699BD5 );
		CHECK ( *(--ptr) == 0x0C0F979D );
		CHECK ( *(--ptr) == 0x65DC4D95 );
		CHECK ( *(--ptr) == 0x9ECCC2A5 );

		CHECK ( ptr == sequence.begin() );
	}

	SECTION ("Iterator std::prev begins on end and ends on beginning")
	{
		using arcstk::SampleIterator;

		InterleavedSamples<uint32_t> sequence;
		sequence.wrap_byte_buffer(&bytes[0], 1024, true); // bytes

		REQUIRE ( sequence.size() == 128 );

		// Position on last element
		auto ptr = std::prev(std::end(sequence));

		// end: last 10 samples
		CHECK ( *ptr == 0xDD6DABA8 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0xE0EBE817 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x363FBB36 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0xA7ED30D6 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0xA419E185 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x31C575C7 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x59952BDA );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x352BB52A );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0xE46ECE70 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0xE6791252 ); // 118

		ptr -= 108;

		// begin: first 10 samples

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x5BCA0129 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x4A1FF5A5 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x60F6FA60 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x8E86AA07 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0xE6EAC2DA );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0xF5F6F9E6 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x84699BD5 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x0C0F979D );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x65DC4D95 );

		ptr = std::prev(ptr);
		CHECK ( *ptr == 0x9ECCC2A5 );

		CHECK ( ptr == sequence.begin() );
	}
}

