#include "accuraterip.hpp"
#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for classes in module calculate
 */

#include <type_traits>

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_IMPL_HPP__
#include "calculate_impl.hpp"
#endif


// AudioSize


TEST_CASE ( "AudioSize", "[calculate] [audiosize]" )
{
	using arcstk::AudioSize;
	using UNIT = arcstk::AudioSize::UNIT;

	AudioSize empty_size{};

	REQUIRE ( empty_size.zero() );

	//

	AudioSize size1;

	REQUIRE ( size1.zero() );

	size1.set_total_frames(253038);

	//

	AudioSize size2; // equals size1
					 //
	REQUIRE ( size2.zero() );

	size2.set_total_frames(253038);

	//

	AudioSize size3(253038,    UNIT::FRAMES);  // equal to size1 and size2
	AudioSize size4(148786344, UNIT::SAMPLES); // equal to size1 and size2
	AudioSize size5(595145376, UNIT::BYTES);   // equal to size1 and size2

	//

	AudioSize different_size; // not equal to size1-5

	REQUIRE ( different_size.zero() );

	different_size.set_total_frames(14827);

	//

	SECTION ("Default construction is as declared")
	{
		CHECK ( std::is_default_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_default_constructible<AudioSize>::value );
	}

	SECTION ("Parametized construction is as declared")
	{
		CHECK ( std::is_constructible<AudioSize,
				const int32_t, const AudioSize::UNIT>::value );

		CHECK ( std::is_nothrow_constructible<AudioSize,
				const int32_t, const AudioSize::UNIT>::value );
	}

	SECTION ("Parametized construction is correct")
	{
		CHECK ( empty_size.zero() );
		CHECK ( 0 == empty_size.total_pcm_bytes() );
		CHECK ( 0 == empty_size.leadout_frame() );

		// constructed with frames
		CHECK ( size1.leadout_frame()   ==    253038 );
		CHECK ( size1.total_frames()    ==    253038 );
		CHECK ( size1.total_samples()   == 148786344 );
		CHECK ( size1.total_pcm_bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size2.leadout_frame()   ==    253038 );
		CHECK ( size2.total_frames()    ==    253038 );
		CHECK ( size2.total_samples()   == 148786344 );
		CHECK ( size2.total_pcm_bytes() == 595145376 );

		// constructed with frames too
		CHECK ( size3.leadout_frame()   ==    253038 );
		CHECK ( size3.total_frames()    ==    253038 );
		CHECK ( size3.total_samples()   == 148786344 );
		CHECK ( size3.total_pcm_bytes() == 595145376 );

		// constructed with samples
		CHECK ( size4.leadout_frame()   ==    253038 );
		CHECK ( size4.total_frames()    ==    253038 );
		CHECK ( size4.total_samples()   == 148786344 );
		CHECK ( size4.total_pcm_bytes() == 595145376 );

		// constructed with bytes
		CHECK ( size5.leadout_frame()   ==    253038 );
		CHECK ( size5.total_frames()    ==    253038 );
		CHECK ( size5.total_samples()   == 148786344 );
		CHECK ( size5.total_pcm_bytes() == 595145376 );

		// different size, constructed with frames
		CHECK ( different_size.leadout_frame()   ==    14827 );
		CHECK ( different_size.total_frames()    ==    14827 );
		CHECK ( different_size.total_samples()   ==  8718276 );
		CHECK ( different_size.total_pcm_bytes() == 34873104 );
	}

	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_copy_constructible<AudioSize>::value );
	}

	// Copy construction correct?

	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<AudioSize>::value );

		CHECK ( std::is_nothrow_move_constructible<AudioSize>::value );
	}

	SECTION ("Maximum values are correct")
	{
		CHECK (     449999 == empty_size.max(UNIT::FRAMES)  );
		CHECK (  264599412 == empty_size.max(UNIT::SAMPLES) );
		CHECK ( 1058397648 == empty_size.max(UNIT::BYTES)   );
	}

	SECTION ("Equality operator is correct")
	{
		CHECK ( size1 == size1 );
		CHECK ( size2 == size2 );
		CHECK ( size3 == size3 );
		CHECK ( size4 == size4 );
		CHECK ( size5 == size5 );

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

	SECTION ("Less-than operator is correct")
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

		CHECK ( empty_size < different_size );
	}

	SECTION ("Swap operation is correct")
	{
		using std::swap;

		swap(size2, different_size);

		CHECK (  14827 == size2.total_frames() );
		CHECK ( 253038 == different_size.total_frames() );

		swap(empty_size, size3);

		CHECK (      0 == size3.total_frames() );
		CHECK ( 253038 == empty_size.total_frames() );
	}
}


// Calculation


TEST_CASE ( "Calculation", "[calculate] [calculation]" )
{
	using arcstk::Calculation;
	using arcstk::Algorithm;
	using arcstk::TOC;
	using arcstk::AudioSize;
	using arcstk::accuraterip::AccurateRipV1V2;
	using arcstk::checksum::type;


	const auto toc = arcstk::details::TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	const auto size { AudioSize { 253038, AudioSize::UNIT::FRAMES } };

	auto algorithm { std::make_unique<AccurateRipV1V2>() };

	const auto calculation { Calculation { std::move(algorithm), *toc, size }};

	const auto result { calculation.result() };

	//

	SECTION ("Default construction is as declared")
	{
		CHECK ( not std::is_default_constructible<Calculation>::value );
		CHECK ( not std::is_trivially_default_constructible<Calculation>::value );
		CHECK ( not std::is_nothrow_default_constructible<Calculation>::value );
	}

	SECTION ("Parametized construction is as declared")
	{
		CHECK ( std::is_constructible<Calculation,
				std::unique_ptr<Algorithm>, const TOC&, const AudioSize&>::value );

		CHECK ( not std::is_trivially_constructible<Calculation,
				std::unique_ptr<Algorithm>, const TOC&, const AudioSize&>::value );
		CHECK ( not std::is_nothrow_constructible<Calculation,
				std::unique_ptr<Algorithm>, const TOC&, const AudioSize&>::value );
	}


	SECTION ("Parametized construction is correct")
	{
		CHECK ( calculation.algorithm()->types() ==
				std::vector<type> { type::ARCS1, type::ARCS2 } );

		CHECK ( calculation.samples_expected() == 148786344 );

		CHECK ( calculation.samples_processed() == 0 );

		CHECK ( calculation.samples_todo() == 148786344 );
		CHECK ( calculation.samples_todo() == calculation.samples_expected() );

		CHECK ( calculation.proc_time_elapsed().count() == 0 );

		CHECK ( not calculation.complete() );

		CHECK ( result.empty() );
	}


	SECTION ("Copy construction is prohibited")
	{
		CHECK ( not std::is_copy_constructible<Calculation>::value );
	}


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<Calculation>::value );

		CHECK ( std::is_nothrow_move_constructible<Calculation>::value );
	}
}

