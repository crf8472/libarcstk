#include "catch2/catch.hpp"

#include <cstdint>
#include <fstream>
#include <memory>

#ifndef __LIBARCSTK_SAMPLES_HPP__
#include "samples.hpp"
#endif


/**
 * \file samplesequence.cpp Fixtures for SampleSequence
 */


TEST_CASE ( "SampleSequence allows int16_t and int32_t",
		"[samplesequence] [construction]" )
{
	arcstk::SampleSequence<int16_t, true>  seq_planar_16;
	arcstk::SampleSequence<int16_t, false> seq_interl_16;
	arcstk::SampleSequence<int32_t, true>  seq_planar_32;
	arcstk::SampleSequence<int32_t, false> seq_interl_32;
}


TEST_CASE ( "SampleSequence index access works correctly",
		"[samplesequence] [subscript] [iterator]" )
{
	// Load example samples

	std::ifstream in;
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		in.open("samplesequence-test-01.bin",
				std::ifstream::in | std::ifstream::binary);
	} catch (const std::ifstream::failure& f)
	{
		FAIL ("Could not open test data file calculation-test-01.bin");
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

	SECTION ("Subscript in16_t interleaved sequence")
	{
		arcstk::SampleSequence<int16_t, false> sequence;

		REQUIRE ( sequence.typesize() == 2 );

		sequence.wrap(&bytes[0], 1024); // bytes

		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xC711C2A5 );
		REQUIRE ( sequence[1] == 0xF35B9ECC );
		REQUIRE ( sequence[2] == 0x2C504D95 );
		REQUIRE ( sequence[3] == 0x96BE65DC );
		REQUIRE ( sequence[4] == 0x4BB9979D );
		REQUIRE ( sequence[5] == 0xAE450C0F );
		REQUIRE ( sequence[6] == 0xDCDE9BD5 );
		REQUIRE ( sequence[7] == 0x1CFD8469 );
		REQUIRE ( sequence[8] == 0xCBE1F9E6 );
		REQUIRE ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x338DE185 );
		REQUIRE ( sequence[247] == 0x1B0FA419 );
		REQUIRE ( sequence[248] == 0xE4D030D6 );
		REQUIRE ( sequence[249] == 0xA156A7ED );
		REQUIRE ( sequence[250] == 0x7A99BB36 );
		REQUIRE ( sequence[251] == 0xCA1A363F );
		REQUIRE ( sequence[252] == 0xA856E817 );
		REQUIRE ( sequence[253] == 0xD686E0EB );
		REQUIRE ( sequence[254] == 0xF617ABA8 );
		REQUIRE ( sequence[255] == 0x1571DD6D );

		sequence.reset(reinterpret_cast<int16_t*>(&bytes[0]), 512); // int16_t's

		REQUIRE ( sequence.size() == 256 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xC711C2A5 );
		REQUIRE ( sequence[1] == 0xF35B9ECC );
		REQUIRE ( sequence[2] == 0x2C504D95 );
		REQUIRE ( sequence[3] == 0x96BE65DC );
		REQUIRE ( sequence[4] == 0x4BB9979D );
		REQUIRE ( sequence[5] == 0xAE450C0F );
		REQUIRE ( sequence[6] == 0xDCDE9BD5 );
		REQUIRE ( sequence[7] == 0x1CFD8469 );
		REQUIRE ( sequence[8] == 0xCBE1F9E6 );
		REQUIRE ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x338DE185 );
		REQUIRE ( sequence[247] == 0x1B0FA419 );
		REQUIRE ( sequence[248] == 0xE4D030D6 );
		REQUIRE ( sequence[249] == 0xA156A7ED );
		REQUIRE ( sequence[250] == 0x7A99BB36 );
		REQUIRE ( sequence[251] == 0xCA1A363F );
		REQUIRE ( sequence[252] == 0xA856E817 );
		REQUIRE ( sequence[253] == 0xD686E0EB );
		REQUIRE ( sequence[254] == 0xF617ABA8 );
		REQUIRE ( sequence[255] == 0x1571DD6D );
	}

	SECTION ("Iterate in16_t interleaved sequence")
	{
		arcstk::SampleSequence<int16_t, false> sequence;

		REQUIRE ( sequence.typesize() == 2 );

		sequence.wrap(&bytes[0], 1024); // bytes

		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		auto it_begin { sequence.begin() };

		// begin: first 10 samples
		REQUIRE ( *it_begin == 0xC711C2A5 );
		++it_begin;
		REQUIRE ( *it_begin == 0xF35B9ECC );
		++it_begin;
		REQUIRE ( *it_begin == 0x2C504D95 );
		++it_begin;
		REQUIRE ( *it_begin == 0x96BE65DC );
		++it_begin;
		REQUIRE ( *it_begin == 0x4BB9979D );
		++it_begin;
		REQUIRE ( *it_begin == 0xAE450C0F );
		++it_begin;
		REQUIRE ( *it_begin == 0xDCDE9BD5 );
		++it_begin;
		REQUIRE ( *it_begin == 0x1CFD8469 );
		++it_begin;
		REQUIRE ( *it_begin == 0xCBE1F9E6 );
		++it_begin;
		REQUIRE ( *it_begin == 0x0DF0F5F6 );

		auto it_end { sequence.end() };

		// end: last 10 samples
		--it_end;
		REQUIRE ( *it_end == 0x1571DD6D );
		--it_end;
		REQUIRE ( *it_end == 0xF617ABA8 );
		--it_end;
		REQUIRE ( *it_end == 0xD686E0EB );
		--it_end;
		REQUIRE ( *it_end == 0xA856E817 );
		--it_end;
		REQUIRE ( *it_end == 0xCA1A363F );
		--it_end;
		REQUIRE ( *it_end == 0x7A99BB36 );
		--it_end;
		REQUIRE ( *it_end == 0xA156A7ED );
		--it_end;
		REQUIRE ( *it_end == 0xE4D030D6 );
		--it_end;
		REQUIRE ( *it_end == 0x1B0FA419 );
		--it_end;
		REQUIRE ( *it_end == 0x338DE185 );

		sequence.reset(reinterpret_cast<int16_t*>(&bytes[0]), 512); // int16_t's

		REQUIRE ( sequence.size() == 256 );

		auto it_begin2 { sequence.begin() };

		// begin: first 10 samples
		REQUIRE ( *it_begin2 == 0xC711C2A5 );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0xF35B9ECC );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0x2C504D95 );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0x96BE65DC );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0x4BB9979D );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0xAE450C0F );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0xDCDE9BD5 );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0x1CFD8469 );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0xCBE1F9E6 );
		++it_begin2;
		REQUIRE ( *it_begin2 == 0x0DF0F5F6 );

		auto it_end2 { sequence.end() };

		// end: last 10 samples
		--it_end2;
		REQUIRE ( *it_end2 == 0x1571DD6D );
		--it_end2;
		REQUIRE ( *it_end2 == 0xF617ABA8 );
		--it_end2;
		REQUIRE ( *it_end2 == 0xD686E0EB );
		--it_end2;
		REQUIRE ( *it_end2 == 0xA856E817 );
		--it_end2;
		REQUIRE ( *it_end2 == 0xCA1A363F );
		--it_end2;
		REQUIRE ( *it_end2 == 0x7A99BB36 );
		--it_end2;
		REQUIRE ( *it_end2 == 0xA156A7ED );
		--it_end2;
		REQUIRE ( *it_end2 == 0xE4D030D6 );
		--it_end2;
		REQUIRE ( *it_end2 == 0x1B0FA419 );
		--it_end2;
		REQUIRE ( *it_end2 == 0x338DE185 );


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
		arcstk::SampleSequence<int16_t, false> sequence;

		REQUIRE ( sequence.typesize() == 2 );

		sequence.wrap(&bytes[0], 1024); // bytes

		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		int index = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index] );
			++index;
		}

		REQUIRE ( index == 256 );

		sequence.reset(reinterpret_cast<int16_t*>(&bytes[0]), 512); // int16_t's

		REQUIRE ( sequence.size() == 256 );

		int index2 = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index2] );
			++index2;
		}

		REQUIRE ( index2 == 256 );
	}

	SECTION ("Subscript in32_t interleaved sequence")
	{
		arcstk::SampleSequence<int32_t, false> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap(&bytes[0], 1024); // bytes

		REQUIRE ( sequence.size() == 128 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0x9ECCC2A5 );
		REQUIRE ( sequence[1] == 0x65DC4D95 );
		REQUIRE ( sequence[2] == 0x0C0F979D );
		REQUIRE ( sequence[3] == 0x84699BD5 );
		REQUIRE ( sequence[4] == 0xF5F6F9E6 );
		REQUIRE ( sequence[5] == 0xE6EAC2DA );
		REQUIRE ( sequence[6] == 0x8E86AA07 );
		REQUIRE ( sequence[7] == 0x60F6FA60 );
		REQUIRE ( sequence[8] == 0x4A1FF5A5 );
		REQUIRE ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0xE063C7F1 );
		REQUIRE ( sequence[55] == 0x8F1BECB6 );
		REQUIRE ( sequence[56] == 0x192C81F8 );
		REQUIRE ( sequence[57] == 0x44687814 );
		REQUIRE ( sequence[58] == 0xDDDA357B );
		REQUIRE ( sequence[59] == 0x50036E87 );
		REQUIRE ( sequence[60] == 0xB5BA4F16 );
		REQUIRE ( sequence[61] == 0xDB434C76 );
		REQUIRE ( sequence[62] == 0x179514D8 );
		REQUIRE ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE6791252 );
		REQUIRE ( sequence[119] == 0xE46ECE70 );
		REQUIRE ( sequence[120] == 0x352BB52A );
		REQUIRE ( sequence[121] == 0x59952BDA );
		REQUIRE ( sequence[122] == 0x31C575C7 );
		REQUIRE ( sequence[123] == 0xA419E185 );
		REQUIRE ( sequence[124] == 0xA7ED30D6 );
		REQUIRE ( sequence[125] == 0x363FBB36 );
		REQUIRE ( sequence[126] == 0xE0EBE817 );
		REQUIRE ( sequence[127] == 0xDD6DABA8 );

		sequence.reset(reinterpret_cast<int32_t*>(&bytes[0]), 256); // int32_t's

		REQUIRE ( sequence.size() == 128 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0x9ECCC2A5 );
		REQUIRE ( sequence[1] == 0x65DC4D95 );
		REQUIRE ( sequence[2] == 0x0C0F979D );
		REQUIRE ( sequence[3] == 0x84699BD5 );
		REQUIRE ( sequence[4] == 0xF5F6F9E6 );
		REQUIRE ( sequence[5] == 0xE6EAC2DA );
		REQUIRE ( sequence[6] == 0x8E86AA07 );
		REQUIRE ( sequence[7] == 0x60F6FA60 );
		REQUIRE ( sequence[8] == 0x4A1FF5A5 );
		REQUIRE ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0xE063C7F1 );
		REQUIRE ( sequence[55] == 0x8F1BECB6 );
		REQUIRE ( sequence[56] == 0x192C81F8 );
		REQUIRE ( sequence[57] == 0x44687814 );
		REQUIRE ( sequence[58] == 0xDDDA357B );
		REQUIRE ( sequence[59] == 0x50036E87 );
		REQUIRE ( sequence[60] == 0xB5BA4F16 );
		REQUIRE ( sequence[61] == 0xDB434C76 );
		REQUIRE ( sequence[62] == 0x179514D8 );
		REQUIRE ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE6791252 );
		REQUIRE ( sequence[119] == 0xE46ECE70 );
		REQUIRE ( sequence[120] == 0x352BB52A );
		REQUIRE ( sequence[121] == 0x59952BDA );
		REQUIRE ( sequence[122] == 0x31C575C7 );
		REQUIRE ( sequence[123] == 0xA419E185 );
		REQUIRE ( sequence[124] == 0xA7ED30D6 );
		REQUIRE ( sequence[125] == 0x363FBB36 );
		REQUIRE ( sequence[126] == 0xE0EBE817 );
		REQUIRE ( sequence[127] == 0xDD6DABA8 );
	}

	SECTION ("Iteration and Subscript are equivalent for in32_t interleaved sequence")
	{
		arcstk::SampleSequence<int32_t, false> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap(&bytes[0], 1024); // bytes

		REQUIRE ( sequence.size() == 128 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		int index = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index] );
			++index;
		}

		REQUIRE ( index == 128 );

		sequence.reset(reinterpret_cast<int32_t*>(&bytes[0]), 256); // int32_t's

		REQUIRE ( sequence.size() == 128 );

		int index2 = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index2] );
			++index2;
		}

		REQUIRE ( index2 == 128 );
	}

	SECTION ("Subscript in16_t planar sequence")
	{
		arcstk::SampleSequence<int16_t, true> sequence;

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x4293C711 );
		REQUIRE ( sequence[2] == 0x920a9ECC );
		REQUIRE ( sequence[3] == 0x6C51F35B );
		REQUIRE ( sequence[4] == 0x602D4D95 );
		REQUIRE ( sequence[5] == 0xAEE52C50 );
		REQUIRE ( sequence[6] == 0x870465DC );
		REQUIRE ( sequence[7] == 0x3C1296BE );
		REQUIRE ( sequence[8] == 0xB852979D );
		REQUIRE ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x363FDB43 );
		REQUIRE ( sequence[247] == 0xCA1A72F6 );
		REQUIRE ( sequence[248] == 0xE81714D8 );
		REQUIRE ( sequence[249] == 0xA856DB4A );
		REQUIRE ( sequence[250] == 0xE0EB1795 );
		REQUIRE ( sequence[251] == 0xD6862DFE );
		REQUIRE ( sequence[252] == 0xABA8E966 );
		REQUIRE ( sequence[253] == 0xF617700E );
		REQUIRE ( sequence[254] == 0xDD6D28EF );
		REQUIRE ( sequence[255] == 0x157171F9 );

		sequence.reset(reinterpret_cast<int16_t*>(&bytes[0]),
				reinterpret_cast<int16_t*>(&bytes[512]),
				256); // int16_t's

		REQUIRE ( sequence.size() == 256 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x4293C711 );
		REQUIRE ( sequence[2] == 0x920a9ECC );
		REQUIRE ( sequence[3] == 0x6C51F35B );
		REQUIRE ( sequence[4] == 0x602D4D95 );
		REQUIRE ( sequence[5] == 0xAEE52C50 );
		REQUIRE ( sequence[6] == 0x870465DC );
		REQUIRE ( sequence[7] == 0x3C1296BE );
		REQUIRE ( sequence[8] == 0xB852979D );
		REQUIRE ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x363FDB43 );
		REQUIRE ( sequence[247] == 0xCA1A72F6 );
		REQUIRE ( sequence[248] == 0xE81714D8 );
		REQUIRE ( sequence[249] == 0xA856DB4A );
		REQUIRE ( sequence[250] == 0xE0EB1795 );
		REQUIRE ( sequence[251] == 0xD6862DFE );
		REQUIRE ( sequence[252] == 0xABA8E966 );
		REQUIRE ( sequence[253] == 0xF617700E );
		REQUIRE ( sequence[254] == 0xDD6D28EF );
		REQUIRE ( sequence[255] == 0x157171F9 );
	}

	SECTION ("Iteration and Subscript are equivalent for in16_t planar sequence")
	{
		arcstk::SampleSequence<int16_t, true> sequence;

		REQUIRE ( sequence.typesize() == 2 );

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		int index = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index] );
			++index;
		}

		REQUIRE ( index == 256 );

		sequence.reset(reinterpret_cast<int16_t*>(&bytes[0]),
				reinterpret_cast<int16_t*>(&bytes[0]),
				256); // int16_t's

		REQUIRE ( sequence.size() == 256 );

		int index2 = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index2] );
			++index2;
		}

		REQUIRE ( index2 == 256 );
	}

	SECTION ("Subscript in32_t planar sequence")
	{
		arcstk::SampleSequence<int32_t, true> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.size() == 128 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x920A9ECC );
		REQUIRE ( sequence[2] == 0x602D4D95 );
		REQUIRE ( sequence[3] == 0x870465DC );
		REQUIRE ( sequence[4] == 0xB852979D );
		REQUIRE ( sequence[5] == 0xA2D90C0F );
		REQUIRE ( sequence[6] == 0x001B9BD5 );
		REQUIRE ( sequence[7] == 0xFDD48469 );
		REQUIRE ( sequence[8] == 0x4A40F9E6 );
		REQUIRE ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0x0AA1B644 );
		REQUIRE ( sequence[55] == 0x444619CE );
		REQUIRE ( sequence[56] == 0xD3770215 );
		REQUIRE ( sequence[57] == 0xA54964E1 );
		REQUIRE ( sequence[58] == 0x3D925080 );
		REQUIRE ( sequence[59] == 0xABAA15C9 );
		REQUIRE ( sequence[60] == 0x386E5B27 );
		REQUIRE ( sequence[61] == 0xAAECFC5D );
		REQUIRE ( sequence[62] == 0x3628136F );
		REQUIRE ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE1856E87 );
		REQUIRE ( sequence[119] == 0xA4195003 );
		REQUIRE ( sequence[120] == 0x30D64F16 );
		REQUIRE ( sequence[121] == 0xA7EDB5BA );
		REQUIRE ( sequence[122] == 0xBB364C76 );
		REQUIRE ( sequence[123] == 0x363FDB43 );
		REQUIRE ( sequence[124] == 0xE81714D8 );
		REQUIRE ( sequence[125] == 0xE0EB1795 );
		REQUIRE ( sequence[126] == 0xABA8E966 );
		REQUIRE ( sequence[127] == 0xDD6D28EF );

		sequence.reset(reinterpret_cast<int32_t*>(&bytes[0]),
				reinterpret_cast<int32_t*>(&bytes[512]),
				128); // int32_t's

		REQUIRE ( sequence.size() == 128 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x920A9ECC );
		REQUIRE ( sequence[2] == 0x602D4D95 );
		REQUIRE ( sequence[3] == 0x870465DC );
		REQUIRE ( sequence[4] == 0xB852979D );
		REQUIRE ( sequence[5] == 0xA2D90C0F );
		REQUIRE ( sequence[6] == 0x001B9BD5 );
		REQUIRE ( sequence[7] == 0xFDD48469 );
		REQUIRE ( sequence[8] == 0x4A40F9E6 );
		REQUIRE ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0x0AA1B644 );
		REQUIRE ( sequence[55] == 0x444619CE );
		REQUIRE ( sequence[56] == 0xD3770215 );
		REQUIRE ( sequence[57] == 0xA54964E1 );
		REQUIRE ( sequence[58] == 0x3D925080 );
		REQUIRE ( sequence[59] == 0xABAA15C9 );
		REQUIRE ( sequence[60] == 0x386E5B27 );
		REQUIRE ( sequence[61] == 0xAAECFC5D );
		REQUIRE ( sequence[62] == 0x3628136F );
		REQUIRE ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE1856E87 );
		REQUIRE ( sequence[119] == 0xA4195003 );
		REQUIRE ( sequence[120] == 0x30D64F16 );
		REQUIRE ( sequence[121] == 0xA7EDB5BA );
		REQUIRE ( sequence[122] == 0xBB364C76 );
		REQUIRE ( sequence[123] == 0x363FDB43 );
		REQUIRE ( sequence[124] == 0xE81714D8 );
		REQUIRE ( sequence[125] == 0xE0EB1795 );
		REQUIRE ( sequence[126] == 0xABA8E966 );
		REQUIRE ( sequence[127] == 0xDD6D28EF );
	}

	SECTION ("Iteration and Subscript are equivalent for in32_t planar sequence")
	{
		arcstk::SampleSequence<int32_t, true> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.size() == 128 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		int index = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index] );
			++index;
		}

		REQUIRE ( index == 128 );

		sequence.reset(reinterpret_cast<int32_t*>(&bytes[0]),
				reinterpret_cast<int32_t*>(&bytes[0]),
				128); // int32_t's

		REQUIRE ( sequence.size() == 128 );

		int index2 = 0;
		for (const auto& sample : sequence)
		{
			REQUIRE ( sample == sequence[index2] );
			++index2;
		}

		REQUIRE ( index2 == 128 );
	}

	SECTION ("Subscript uin16_t interleaved sequence")
	{
		arcstk::SampleSequence<uint16_t, false> sequence;

		REQUIRE ( sequence.typesize() == 2 );

		sequence.wrap(&bytes[0], 1024); // bytes

		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xC711C2A5 );
		REQUIRE ( sequence[1] == 0xF35B9ECC );
		REQUIRE ( sequence[2] == 0x2C504D95 );
		REQUIRE ( sequence[3] == 0x96BE65DC );
		REQUIRE ( sequence[4] == 0x4BB9979D );
		REQUIRE ( sequence[5] == 0xAE450C0F );
		REQUIRE ( sequence[6] == 0xDCDE9BD5 );
		REQUIRE ( sequence[7] == 0x1CFD8469 );
		REQUIRE ( sequence[8] == 0xCBE1F9E6 );
		REQUIRE ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x338DE185 );
		REQUIRE ( sequence[247] == 0x1B0FA419 );
		REQUIRE ( sequence[248] == 0xE4D030D6 );
		REQUIRE ( sequence[249] == 0xA156A7ED );
		REQUIRE ( sequence[250] == 0x7A99BB36 );
		REQUIRE ( sequence[251] == 0xCA1A363F );
		REQUIRE ( sequence[252] == 0xA856E817 );
		REQUIRE ( sequence[253] == 0xD686E0EB );
		REQUIRE ( sequence[254] == 0xF617ABA8 );
		REQUIRE ( sequence[255] == 0x1571DD6D );

		sequence.reset(reinterpret_cast<uint16_t*>(&bytes[0]), 512); // uint16_t

		REQUIRE ( sequence.size() == 256 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xC711C2A5 );
		REQUIRE ( sequence[1] == 0xF35B9ECC );
		REQUIRE ( sequence[2] == 0x2C504D95 );
		REQUIRE ( sequence[3] == 0x96BE65DC );
		REQUIRE ( sequence[4] == 0x4BB9979D );
		REQUIRE ( sequence[5] == 0xAE450C0F );
		REQUIRE ( sequence[6] == 0xDCDE9BD5 );
		REQUIRE ( sequence[7] == 0x1CFD8469 );
		REQUIRE ( sequence[8] == 0xCBE1F9E6 );
		REQUIRE ( sequence[9] == 0x0DF0F5F6 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x338DE185 );
		REQUIRE ( sequence[247] == 0x1B0FA419 );
		REQUIRE ( sequence[248] == 0xE4D030D6 );
		REQUIRE ( sequence[249] == 0xA156A7ED );
		REQUIRE ( sequence[250] == 0x7A99BB36 );
		REQUIRE ( sequence[251] == 0xCA1A363F );
		REQUIRE ( sequence[252] == 0xA856E817 );
		REQUIRE ( sequence[253] == 0xD686E0EB );
		REQUIRE ( sequence[254] == 0xF617ABA8 );
		REQUIRE ( sequence[255] == 0x1571DD6D );
	}

	SECTION ("Subscript uin16_t planar sequence")
	{
		arcstk::SampleSequence<uint16_t, true> sequence;

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x4293C711 );
		REQUIRE ( sequence[2] == 0x920a9ECC );
		REQUIRE ( sequence[3] == 0x6C51F35B );
		REQUIRE ( sequence[4] == 0x602D4D95 );
		REQUIRE ( sequence[5] == 0xAEE52C50 );
		REQUIRE ( sequence[6] == 0x870465DC );
		REQUIRE ( sequence[7] == 0x3C1296BE );
		REQUIRE ( sequence[8] == 0xB852979D );
		REQUIRE ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x363FDB43 );
		REQUIRE ( sequence[247] == 0xCA1A72F6 );
		REQUIRE ( sequence[248] == 0xE81714D8 );
		REQUIRE ( sequence[249] == 0xA856DB4A );
		REQUIRE ( sequence[250] == 0xE0EB1795 );
		REQUIRE ( sequence[251] == 0xD6862DFE );
		REQUIRE ( sequence[252] == 0xABA8E966 );
		REQUIRE ( sequence[253] == 0xF617700E );
		REQUIRE ( sequence[254] == 0xDD6D28EF );
		REQUIRE ( sequence[255] == 0x157171F9 );

		sequence.reset(reinterpret_cast<uint16_t*>(&bytes[0]),
				reinterpret_cast<uint16_t*>(&bytes[512]),
				256); // uint16_t's

		REQUIRE ( sequence.size() == 256 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x4293C711 );
		REQUIRE ( sequence[2] == 0x920a9ECC );
		REQUIRE ( sequence[3] == 0x6C51F35B );
		REQUIRE ( sequence[4] == 0x602D4D95 );
		REQUIRE ( sequence[5] == 0xAEE52C50 );
		REQUIRE ( sequence[6] == 0x870465DC );
		REQUIRE ( sequence[7] == 0x3C1296BE );
		REQUIRE ( sequence[8] == 0xB852979D );
		REQUIRE ( sequence[9] == 0xFF364BB9 );

		// end: last 10 samples
		REQUIRE ( sequence[246] == 0x363FDB43 );
		REQUIRE ( sequence[247] == 0xCA1A72F6 );
		REQUIRE ( sequence[248] == 0xE81714D8 );
		REQUIRE ( sequence[249] == 0xA856DB4A );
		REQUIRE ( sequence[250] == 0xE0EB1795 );
		REQUIRE ( sequence[251] == 0xD6862DFE );
		REQUIRE ( sequence[252] == 0xABA8E966 );
		REQUIRE ( sequence[253] == 0xF617700E );
		REQUIRE ( sequence[254] == 0xDD6D28EF );
		REQUIRE ( sequence[255] == 0x157171F9 );
	}

	SECTION ("Subscript uin32_t interleaved sequence")
	{
		arcstk::SampleSequence<uint32_t, false> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap(&bytes[0], 1024); // bytes

		REQUIRE ( sequence.size() == 128 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0x9ECCC2A5 );
		REQUIRE ( sequence[1] == 0x65DC4D95 );
		REQUIRE ( sequence[2] == 0x0C0F979D );
		REQUIRE ( sequence[3] == 0x84699BD5 );
		REQUIRE ( sequence[4] == 0xF5F6F9E6 );
		REQUIRE ( sequence[5] == 0xE6EAC2DA );
		REQUIRE ( sequence[6] == 0x8E86AA07 );
		REQUIRE ( sequence[7] == 0x60F6FA60 );
		REQUIRE ( sequence[8] == 0x4A1FF5A5 );
		REQUIRE ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0xE063C7F1 );
		REQUIRE ( sequence[55] == 0x8F1BECB6 );
		REQUIRE ( sequence[56] == 0x192C81F8 );
		REQUIRE ( sequence[57] == 0x44687814 );
		REQUIRE ( sequence[58] == 0xDDDA357B );
		REQUIRE ( sequence[59] == 0x50036E87 );
		REQUIRE ( sequence[60] == 0xB5BA4F16 );
		REQUIRE ( sequence[61] == 0xDB434C76 );
		REQUIRE ( sequence[62] == 0x179514D8 );
		REQUIRE ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE6791252 );
		REQUIRE ( sequence[119] == 0xE46ECE70 );
		REQUIRE ( sequence[120] == 0x352BB52A );
		REQUIRE ( sequence[121] == 0x59952BDA );
		REQUIRE ( sequence[122] == 0x31C575C7 );
		REQUIRE ( sequence[123] == 0xA419E185 );
		REQUIRE ( sequence[124] == 0xA7ED30D6 );
		REQUIRE ( sequence[125] == 0x363FBB36 );
		REQUIRE ( sequence[126] == 0xE0EBE817 );
		REQUIRE ( sequence[127] == 0xDD6DABA8 );

		sequence.reset(reinterpret_cast<uint32_t*>(&bytes[0]), 256); // uint32_t

		REQUIRE ( sequence.size() == 128 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0x9ECCC2A5 );
		REQUIRE ( sequence[1] == 0x65DC4D95 );
		REQUIRE ( sequence[2] == 0x0C0F979D );
		REQUIRE ( sequence[3] == 0x84699BD5 );
		REQUIRE ( sequence[4] == 0xF5F6F9E6 );
		REQUIRE ( sequence[5] == 0xE6EAC2DA );
		REQUIRE ( sequence[6] == 0x8E86AA07 );
		REQUIRE ( sequence[7] == 0x60F6FA60 );
		REQUIRE ( sequence[8] == 0x4A1FF5A5 );
		REQUIRE ( sequence[9] == 0x5BCA0129 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0xE063C7F1 );
		REQUIRE ( sequence[55] == 0x8F1BECB6 );
		REQUIRE ( sequence[56] == 0x192C81F8 );
		REQUIRE ( sequence[57] == 0x44687814 );
		REQUIRE ( sequence[58] == 0xDDDA357B );
		REQUIRE ( sequence[59] == 0x50036E87 );
		REQUIRE ( sequence[60] == 0xB5BA4F16 );
		REQUIRE ( sequence[61] == 0xDB434C76 );
		REQUIRE ( sequence[62] == 0x179514D8 );
		REQUIRE ( sequence[63] == 0x28EFE966 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE6791252 );
		REQUIRE ( sequence[119] == 0xE46ECE70 );
		REQUIRE ( sequence[120] == 0x352BB52A );
		REQUIRE ( sequence[121] == 0x59952BDA );
		REQUIRE ( sequence[122] == 0x31C575C7 );
		REQUIRE ( sequence[123] == 0xA419E185 );
		REQUIRE ( sequence[124] == 0xA7ED30D6 );
		REQUIRE ( sequence[125] == 0x363FBB36 );
		REQUIRE ( sequence[126] == 0xE0EBE817 );
		REQUIRE ( sequence[127] == 0xDD6DABA8 );
	}

	SECTION ("Subscript uin32_t planar sequence")
	{
		arcstk::SampleSequence<uint32_t, true> sequence;

		REQUIRE ( sequence.typesize() == 4 );

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.size() == 128 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x920A9ECC );
		REQUIRE ( sequence[2] == 0x602D4D95 );
		REQUIRE ( sequence[3] == 0x870465DC );
		REQUIRE ( sequence[4] == 0xB852979D );
		REQUIRE ( sequence[5] == 0xA2D90C0F );
		REQUIRE ( sequence[6] == 0x001B9BD5 );
		REQUIRE ( sequence[7] == 0xFDD48469 );
		REQUIRE ( sequence[8] == 0x4A40F9E6 );
		REQUIRE ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0x0AA1B644 );
		REQUIRE ( sequence[55] == 0x444619CE );
		REQUIRE ( sequence[56] == 0xD3770215 );
		REQUIRE ( sequence[57] == 0xA54964E1 );
		REQUIRE ( sequence[58] == 0x3D925080 );
		REQUIRE ( sequence[59] == 0xABAA15C9 );
		REQUIRE ( sequence[60] == 0x386E5B27 );
		REQUIRE ( sequence[61] == 0xAAECFC5D );
		REQUIRE ( sequence[62] == 0x3628136F );
		REQUIRE ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE1856E87 );
		REQUIRE ( sequence[119] == 0xA4195003 );
		REQUIRE ( sequence[120] == 0x30D64F16 );
		REQUIRE ( sequence[121] == 0xA7EDB5BA );
		REQUIRE ( sequence[122] == 0xBB364C76 );
		REQUIRE ( sequence[123] == 0x363FDB43 );
		REQUIRE ( sequence[124] == 0xE81714D8 );
		REQUIRE ( sequence[125] == 0xE0EB1795 );
		REQUIRE ( sequence[126] == 0xABA8E966 );
		REQUIRE ( sequence[127] == 0xDD6D28EF );

		sequence.reset(reinterpret_cast<uint32_t*>(&bytes[0]),
				reinterpret_cast<uint32_t*>(&bytes[512]),
				128); // uint32_t's

		REQUIRE ( sequence.size() == 128 );

		// begin: first 10 samples
		REQUIRE ( sequence[0] == 0xD9DBC2A5 );
		REQUIRE ( sequence[1] == 0x920A9ECC );
		REQUIRE ( sequence[2] == 0x602D4D95 );
		REQUIRE ( sequence[3] == 0x870465DC );
		REQUIRE ( sequence[4] == 0xB852979D );
		REQUIRE ( sequence[5] == 0xA2D90C0F );
		REQUIRE ( sequence[6] == 0x001B9BD5 );
		REQUIRE ( sequence[7] == 0xFDD48469 );
		REQUIRE ( sequence[8] == 0x4A40F9E6 );
		REQUIRE ( sequence[9] == 0xA4D8F5F6 );

		// near mid 10 samples
		REQUIRE ( sequence[54] == 0x0AA1B644 );
		REQUIRE ( sequence[55] == 0x444619CE );
		REQUIRE ( sequence[56] == 0xD3770215 );
		REQUIRE ( sequence[57] == 0xA54964E1 );
		REQUIRE ( sequence[58] == 0x3D925080 );
		REQUIRE ( sequence[59] == 0xABAA15C9 );
		REQUIRE ( sequence[60] == 0x386E5B27 );
		REQUIRE ( sequence[61] == 0xAAECFC5D );
		REQUIRE ( sequence[62] == 0x3628136F );
		REQUIRE ( sequence[63] == 0x0D4C7D86 );

		// end: last 10 samples
		REQUIRE ( sequence[118] == 0xE1856E87 );
		REQUIRE ( sequence[119] == 0xA4195003 );
		REQUIRE ( sequence[120] == 0x30D64F16 );
		REQUIRE ( sequence[121] == 0xA7EDB5BA );
		REQUIRE ( sequence[122] == 0xBB364C76 );
		REQUIRE ( sequence[123] == 0x363FDB43 );
		REQUIRE ( sequence[124] == 0xE81714D8 );
		REQUIRE ( sequence[125] == 0xE0EB1795 );
		REQUIRE ( sequence[126] == 0xABA8E966 );
		REQUIRE ( sequence[127] == 0xDD6D28EF );
	}

	SECTION ("SampleIterator 16 bit begin and end", "[sampleiterator]")
	{
		arcstk::SampleSequence<int16_t, true> sequence;

		REQUIRE ( sequence.begin() == sequence.end() );
		REQUIRE ( sequence.size() == 0 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );
		//REQUIRE ( sequence.empty() );

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.begin() != sequence.end() );
		REQUIRE ( sequence.size() == 256 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );
		//REQUIRE ( not sequence.empty() );
	}

	SECTION ("SampleIterator 32 bit begin and end", "[sampleiterator]")
	{
		arcstk::SampleSequence<int32_t, true> sequence;

		REQUIRE ( sequence.begin() == sequence.end() );
		REQUIRE ( sequence.size() == 0 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );
		//REQUIRE ( sequence.empty() );

		sequence.wrap(&bytes[0], &bytes[512], 512); // bytes

		REQUIRE ( sequence.begin() != sequence.end() );
		REQUIRE ( sequence.size() == 128 );
		REQUIRE ( sequence.size() == sequence.end() - sequence.begin() );
		//REQUIRE ( not sequence.empty() );
	}
}

