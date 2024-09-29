#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for classes in module calculate
 */

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

	AudioSize size1;
	size1.set_total_frames(253038);

	AudioSize size2; // equals size1
	size2.set_total_frames(253038);

	AudioSize size3(253038,    UNIT::FRAMES);  // equal to size1 and size2
	AudioSize size4(148786344, UNIT::SAMPLES); // equal to size1 and size2
	AudioSize size5(595145376, UNIT::BYTES);   // equal to size1 and size2

	AudioSize different_size; // not equal to size1-5

	REQUIRE ( different_size.zero() );

	different_size.set_total_frames(14827);


	SECTION ("Construction is correct")
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

