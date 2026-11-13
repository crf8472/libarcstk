#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for TrackSelector.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"
#endif
#ifndef LIBARCSTK_VERIFY_DETAILS_HPP_
#include "verify_details.hpp"      // TO BE TESTED
#endif

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"                // for DBAR
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"        // for Copy, Move
#endif


// TEST_CASE ( "TrackSelector", "[trackselector][verify][details]" )
// {
// 	using arcstk::testing::Copy;
// 	using arcstk::testing::Move;
//
// 	using arcstk::TrackSelector;
//
// 	auto instance = TrackSelector {};
//
//
// 	SECTION ( "Parametized construction is correct" )
// 	{
// 		FAIL ( "Parametized construction test is missing" );
// 	}
//
// 	SECTION ( "Copy construction is correct" )
// 	{
// 		const TrackSelector copied { Copy<TrackSelector>::construct(instance) };
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy construction test is missing" );
// 	}
//
// 	SECTION ( "Move construction is correct" )
// 	{
// 		const TrackSelector moved { Move<TrackSelector>::construct(instance) };
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move construction test is missing" );
// 	}
//
// 	SECTION ( "Copy assignment is correct" )
// 	{
// 		auto copied = TrackSelector {};
// 		Copy<TrackSelector>::assign(copied, instance);
//
// 		// --
//
//         //CHECK ( copied.value() == instance.value() );
//         //CHECK ( copied.name()  == instance.name() );
// 		FAIL ( "Copy assignment test is missing" );
// 	}
//
// 	SECTION ( "Move assignment is correct" )
// 	{
// 		auto moved = TrackSelector {};
// 		Move<TrackSelector>::assign(moved, instance);
//
// 		// --
//
//         //CHECK ( moved.value() == instance.value() );
//         //CHECK ( moved.name()  == instance.name() );
// 		FAIL ( "Move assignment test is missing" );
// 	}
// }


TEST_CASE ( "TrackSelector default constructed instance",
	"[trackselector][verify][details]" )
{
	const auto defaulted = arcstk::details::TrackSelector {};

	// SECTION ( "is empty()" )
	// {
	// 	CHECK ( defaulted.empty() );
	// }
	//
	// SECTION ( "converts to FALSE" )
	// {
	// 	CHECK ( !defaulted );
	// }
}


TEST_CASE ( "TrackSelector property", "[trackselector][verify][details]" )
{
	const auto dbar = arcstk::DBAR {
		// 0:
		{
			/* id */
			{ 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			/* triplets */
			{
				{ 0xB89992E5,  6, 0xC89192E5 },
				{ 0x4F77EB03,  8, 0xB9653BEA },
				{ 0x56582282,  7, 0x6952EC4A },
				{ 0x9E2187F9,  9, 0x1B113449 },
				{ 0x6BE71E50,  2, 0xC5567E10 },
				{ 0x01E7235F,  1, 0x86ECAA48 },
				{ 0xD8F7763C,  0, 0xC3F25BE1 },
				{ 0x8480223E, 13, 0xEBDB4CE2 },
				{ 0x42C5061C, 16, 0xE4BA8570 },
				{ 0x47A70F02, 17, 0x259A8119 },
				{ 0xBABF08CC, 18, 0x527A81E7 },
				{ 0x563EDCCB, 21, 0x0D2496E5 },
				{ 0xAB123C7C, 14, 0xACD78A48 },
				{ 0xC65C20E4, 26, 0x6BD26848 },
				{ 0x58FC3C3E, 28, 0x45A9F67E }
			}
		}
	};

	const auto source = arcstk::DBARSource { &dbar };

	auto instance = arcstk::details::TrackSelector {};

	REQUIRE ( instance.get(source, 0, 0) == 0xB89992E5 );

	SECTION ( "clone() is correct" )
	{
		const auto cloned = instance.clone();

		CHECK ( cloned->get(source, 0, 0) == 0xB89992E5 );
	}
}


TEST_CASE ( "TrackSelector functions", "[trackselector][verify][details]" )
{
	const auto dbar = arcstk::DBAR {
		// 0:
		{
			/* id */
			{ 15, 0x001B9178, 0x014BE24E, 0xB40D2D0F },
			/* triplets */
			{
				{ 0xB89992E5,  6, 0xC89192E5 },
				{ 0x4F77EB03,  8, 0xB9653BEA },
				{ 0x56582282,  7, 0x6952EC4A },
				{ 0x9E2187F9,  9, 0x1B113449 },
				{ 0x6BE71E50,  2, 0xC5567E10 },
				{ 0x01E7235F,  1, 0x86ECAA48 },
				{ 0xD8F7763C,  0, 0xC3F25BE1 },
				{ 0x8480223E, 13, 0xEBDB4CE2 },
				{ 0x42C5061C, 16, 0xE4BA8570 },
				{ 0x47A70F02, 17, 0x259A8119 },
				{ 0xBABF08CC, 18, 0x527A81E7 },
				{ 0x563EDCCB, 21, 0x0D2496E5 },
				{ 0xAB123C7C, 14, 0xACD78A48 },
				{ 0xC65C20E4, 26, 0x6BD26848 },
				{ 0x58FC3C3E, 28, 0x45A9F67E }
			}
		}
	};

	const auto source = arcstk::DBARSource { &dbar };

	auto instance = arcstk::details::TrackSelector {};

	SECTION ( "get() is correct" )
	{
		CHECK ( instance.get(source, 0, 0) == 0xB89992E5 );
		CHECK ( instance.get(source, 1, 0) == 0x4F77EB03 );
	}
}

