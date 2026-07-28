#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for AudioSize.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "AudioSize", "[audiosize] [metadata] [metadata]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::AudioSize;
	using arcstk::UNIT;

	auto instance = AudioSize {};

	auto size1 = AudioSize { 253038,    UNIT::FRAMES };  // equal to size1 and size2
	auto size2 = AudioSize { 253038,    UNIT::FRAMES };  // equal to size1 and size2
	auto size3 = AudioSize { 253038,    UNIT::FRAMES };  // equal to size1 and size2
	auto size4 = AudioSize { 148786344, UNIT::SAMPLES }; // equal to size1 and size2
	auto size5 = AudioSize { 595145376, UNIT::BYTES };   // equal to size1 and size2

	auto different_size = AudioSize { 14827, UNIT::FRAMES };


	SECTION ("Parametized construction is correct")
	{
		CHECK ( instance.zero() );
		CHECK ( 0 == instance.bytes() );
		CHECK ( 0 == instance.frames() );

		// constructed with frames
		CHECK ( size1.frames()   ==    253038 );
		CHECK ( size1.samples()   == 148786344 );
		CHECK ( size1.bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size2.frames()   ==    253038 );
		CHECK ( size2.samples()   == 148786344 );
		CHECK ( size2.bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size3.frames()   ==    253038 );
		CHECK ( size3.samples()   == 148786344 );
		CHECK ( size3.bytes() == 595145376 );

		// constructed with samples
		CHECK ( size4.frames()   ==    253038 );
		CHECK ( size4.samples()   == 148786344 );
		CHECK ( size4.bytes() == 595145376 );

		// constructed with bytes
		CHECK ( size5.frames()   ==    253038 );
		CHECK ( size5.samples()   == 148786344 );
		CHECK ( size5.bytes() == 595145376 );

		// different size, constructed with frames
		CHECK ( different_size.frames()   ==    14827 );
		CHECK ( different_size.samples()   ==  8718276 );
		CHECK ( different_size.bytes() == 34873104 );
	}

	SECTION ("Copy construction is correct")
	{
		instance.set_frames(253038);

		REQUIRE ( instance.frames() == 253038 );

		const AudioSize copied { Copy<AudioSize>::construct(instance) };

		// --

		CHECK ( copied.frames() == 253038 );
	}

	SECTION ("Move construction is correct")
	{
		instance.set_frames(253038);

		REQUIRE ( instance.frames() == 253038 );

		const AudioSize moved { Move<AudioSize>::construct(instance) };

		// --

		CHECK ( moved.frames() == 253038 );
	}

	SECTION ("Copy assignment is correct")
	{
		instance.set_frames(253038);

		REQUIRE ( instance.frames() == 253038 );

		auto copied = AudioSize {};
		Copy<AudioSize>::assign(copied, instance);

		// --

		CHECK ( copied.frames() == 253038 );
	}

	SECTION ("Move assignment is correct")
	{
		instance.set_frames(253038);

		REQUIRE ( instance.frames() == 253038 );

		auto moved = AudioSize {};
		Move<AudioSize>::assign(moved, instance);

		// --

		CHECK ( moved.frames() == 253038 );
	}
}


TEST_CASE ( "AudioSize default constructed instance",
		"[audiosize] [metadata] [metadata]" )
{
	const auto instance = arcstk::AudioSize{};

	SECTION ( "is zero()")
	{
		CHECK ( instance.zero() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !instance );
	}
}


TEST_CASE ( "AudioSize property", "[audiosize] [metadata] [metadata]" )
{
	using arcstk::AudioSize;
	using arcstk::UNIT;

	//auto defaulted = AudioSize {};

	auto size1 = AudioSize { 253038,    UNIT::FRAMES };  // equal to size1 and size2
	auto size2 = AudioSize { 253038,    UNIT::FRAMES };  // equal to size1 and size2
	auto size3 = AudioSize { 253038,    UNIT::FRAMES };  // equal to size1 and size2
	auto size4 = AudioSize { 148786344, UNIT::SAMPLES }; // equal to size1 and size2
	auto size5 = AudioSize { 595145376, UNIT::BYTES };   // equal to size1 and size2

	auto different_size = AudioSize { 14827, UNIT::FRAMES };

	SECTION ("Equality operator == is correct")
	{
		// equal to self

		CHECK ( size1 == size1 );
		CHECK ( size2 == size2 );
		CHECK ( size3 == size3 );
		CHECK ( size4 == size4 );
		CHECK ( size5 == size5 );

		// arithmetically equal

		CHECK ( size1 == size2 );
		CHECK ( size2 == size1 );

		CHECK ( size2 == size3 );
		CHECK ( size3 == size2 );

		CHECK ( size3 == size4 );
		CHECK ( size4 == size3 );

		CHECK ( size4 == size5 );
		CHECK ( size5 == size4 );

		CHECK ( size1 == size3 );
		CHECK ( size1 == size4 );
		CHECK ( size1 == size5 );
		CHECK ( size2 == size4 );
		CHECK ( size2 == size5 );
		CHECK ( size3 == size5 );

		CHECK ( different_size != size1 );
		CHECK ( different_size != size2 );
		CHECK ( different_size != size4 );
		CHECK ( different_size != size5 );
	}

	SECTION ("Less-than operator <= is correct")
	{
		CHECK ( different_size < size1 );
		CHECK ( different_size < size2 );
		CHECK ( different_size < size3 );
		CHECK ( different_size < size4 );
		CHECK ( different_size < size5 );

		CHECK ( !(different_size > size1) );
		CHECK ( !(different_size > size2) );
		CHECK ( !(different_size > size3) );
		CHECK ( !(different_size > size4) );
		CHECK ( !(different_size > size5) );

		CHECK ( AudioSize {} < different_size );
	}

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
}


TEST_CASE ( "AudioSize functions", "[audiosize] [metadata] [metadata]" )
{
	using arcstk::AudioSize;
	using arcstk::UNIT;

	auto instance = AudioSize {};


	// SECTION ("frames is correct")
	// {
	// 	FAIL ("frames test is missing");
	// }

	SECTION ("set_frames is correct")
	{
		instance.set_frames(253038);

		CHECK ( instance.frames()  ==    253038 );
		CHECK ( instance.samples() == 148786344 );
		CHECK ( instance.bytes()   == 595145376 );
	}

	// SECTION ("samples is correct")
	// {
	// 	FAIL ("samples test is missing");
	// }

	SECTION ("set_samples is correct")
	{
		instance.set_samples(148786344);

		CHECK ( instance.frames()  ==    253038 );
		CHECK ( instance.samples() == 148786344 );
		CHECK ( instance.bytes()   == 595145376 );
	}

	// SECTION ("bytes is correct")
	// {
	// 	FAIL ("bytes test is missing");
	// }

	SECTION ("set_bytes is correct")
	{
		instance.set_bytes(595145376);

		// --

		CHECK ( instance.frames()  ==    253038 );
		CHECK ( instance.samples() == 148786344 );
		CHECK ( instance.bytes()   == 595145376 );
	}

	SECTION ("zero is correct")
	{
		auto zero_size = AudioSize { 0, UNIT::BYTES };

		// --

		CHECK ( instance.zero() );
		CHECK ( zero_size.zero() );
	}

	// SECTION ("operator bool is correct")
	// {
	// 	FAIL ("operator bool test is missing");
	// }

	SECTION ("swap is correct")
	{
		auto size1 = AudioSize {};
		size1.set_frames(253038);

		auto size2 = AudioSize {};
		size2.set_frames(14827);

		auto empty_size = AudioSize {};

		instance.set_frames(253038);

		using std::swap;

		swap(empty_size, size1);

		CHECK (      0 == size1.frames() );
		CHECK ( 253038 == empty_size.frames() );

		swap(instance, size2);

		CHECK (  14827 == instance.frames() );
		CHECK ( 253038 == size2.frames() );
	}

	/*
	SECTION ("equals is correct")
	{
		FAIL ("equals test is missing");
	}

	SECTION ("to_string is correct")
	{
		FAIL ("to_string test is missing");
	}
	*/
}

