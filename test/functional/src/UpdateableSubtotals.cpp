#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Functional tests for UpdateableSubtotals.
 */

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#define LIBARCSTK_ALGORITHMS_HPP_ // allow accuraterip.hpp
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"        // TO BE TESTED
#endif

#include <cstdint>                // for uint32_t
#include <numeric>                // for accumulate

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type
#endif

#include "ref_data.hpp"           // for standard_data


TEST_CASE ( "UpdateableSubtotals caches values correctly",
		"[updateablesubtotals] [calc] [accuraterip]" )
{
	using arcstk::accuraterip::details::UpdateableSubtotals;
	using arcstk::checksum::type;

	using std::cbegin;
	using std::cend;

	auto st12 = UpdateableSubtotals<type::ARCS1,type::ARCS2> {};

	REQUIRE ( st12.cache_index() == 0 );

	const auto& st = st12.subtotals();

	REQUIRE ( st.subtotals_v1[0] == 0 );
	REQUIRE ( st.subtotals_v2[0] == 0 );
	REQUIRE ( st.sums[0]         == 0 );
	REQUIRE ( st.multiplier      == 1 );


	// init data

	using arcstk::testing::data::standard_data;

	const auto sdata = standard_data(3052896);
	// length of Bach, Organ Concertos, Track 1

	REQUIRE ( sdata[      0] ==       1 );
	REQUIRE ( sdata[3052895] == 3052896 );

	REQUIRE ( *cbegin(sdata) == 1 );
	REQUIRE ( *cbegin(sdata) + 2939 - 1 == 2939 );

	// 3049957
	REQUIRE ( *(cbegin(sdata) + (3052896 - 2940)) == 3052896 - 2940 + 1 );
	REQUIRE ( *(cend(sdata) - 1) == 3052896 );


	// sum of first 2939 samples

	const auto sum_first_2939 =
		std::accumulate(cbegin(sdata), cbegin(sdata) + 2939, 0u);

	REQUIRE (sum_first_2939 == 4320330 );


	// sum of last 2940 samples

	auto j = uint64_t { 0 };
	for (int i = 3052896; i > 3052896 - 2940; --i)
	{
		j += static_cast<uint32_t>(i);
	}
	auto sum_last_2940 = j;

	REQUIRE (sum_last_2940 == 8971193910 );


	// subtotal_v1 of first 2939 samples

	const uint64_t v1_first_2939 = std::inner_product(
		cbegin(sdata),
		cbegin(sdata) + 2939,
		cbegin(sdata), // multiply with itself, no overflows in this range
		static_cast<uint64_t>(0)
	);

	REQUIRE ( v1_first_2939 == 8466406690 );


	// subtotal_v1 of last 2940 samples

	j = 0;
	for (uint64_t i = 3052896; i > 3052896 - 2940; --i)
	{
		j += (i * static_cast<uint32_t>(i) & 0xFFFFFFFF);
	}
	const auto v1_last_2940 = j;

	REQUIRE ( v1_last_2940 == 6477333279138 );



	SECTION ( "Cache first 2939 values is correct" )
	{
		st12.cache(cbegin(sdata), cbegin(sdata) + 2940);
		st12.finalize();

		// Index is correct after caching

		CHECK ( st12.cache_index() == 2940 );
		CHECK ( st.current_subtotal_v1 == 0 );
		CHECK ( st.current_subtotal_v2 == 0 );
		CHECK ( st.current_cs_sum      == 0 );

		// Check accumulated simple sums

		CHECK ( st.sums[   0] == 1 );
		CHECK ( st.sums[   1] == 3 );
		CHECK ( st.sums[   2] == 6 );
		// ...
		CHECK ( st.sums[2936] == sum_first_2939 - 2939 - 2938 );
		CHECK ( st.sums[2937] == sum_first_2939 - 2939 );
		CHECK ( st.sums[2938] == sum_first_2939 );

		CHECK ( st.sums[2939] == sum_first_2939 + 2940 );

		// Check accumulated subtotals for ARCSv1

		CHECK ( st.subtotals_v1[0] ==  1 );
		CHECK ( st.subtotals_v1[1] ==  5 );
		CHECK ( st.subtotals_v1[2] == 14 );
		// ...
		CHECK ( st.subtotals_v1[2937] == v1_first_2939 - (2939 * 2939) );
		CHECK ( st.subtotals_v1[2938] == v1_first_2939 );

		CHECK ( st.subtotals_v1[2939] == v1_first_2939 + (2940 * 2940 &
				0xFFFFFFFF) );

		// Check accumulated subtotals for ARCSv2

		CHECK ( st.subtotals_v2[0] == 0 );
		CHECK ( st.subtotals_v2[1] == 0 );
		CHECK ( st.subtotals_v2[2] == 0 );
		//
		CHECK ( st.subtotals_v2[2938] == 0 );
		CHECK ( st.subtotals_v2[2939] == 0 );

		CHECK ( st.subtotals_v2[2940] == 0 );
		CHECK ( st.subtotals_v2[2941] == 0 );
	}

	SECTION ( "Caches first 2939 and last 2940 values correctly" )
	{
		st12.cache(cbegin(sdata), cbegin(sdata) + 2940);

		REQUIRE ( st.sums[2939] == 2940 );

		// Index is correct after caching
		REQUIRE ( st12.cache_index() == 2940 );

		st12.set_multiplier(3052896 - 2940 + 1);
		st12.cache(cbegin(sdata) + (3052896 - 2940), cend(sdata));

		REQUIRE ( st.sums[2939] == 2940 );

		// Index is correct after caching
		REQUIRE ( st12.cache_index() == 5880 );

		// Values for sums are correctly stored

		//REQUIRE ( st.sums[5880] == 0 );

		REQUIRE ( st.sums[5879] == 3052896 );
		REQUIRE ( st.sums[5878] == 3052896 - 1 ); // 2940 - 2939
		REQUIRE ( st.sums[5877] == 3052896 - 2 ); // 2940 - 2938
		// ...
		REQUIRE ( st.sums[2942] == 3052896 - 2940 + 3 );
		REQUIRE ( st.sums[2941] == 3052896 - 2940 + 2 );
		REQUIRE ( st.sums[2940] == 3052896 - 2940 + 1 );

		REQUIRE ( st.sums[2939] == 2940 );
		REQUIRE ( st.sums[2938] == 2939 );
		// ...
		REQUIRE ( st.sums[   3] == 4 );
		REQUIRE ( st.sums[   2] == 3 );
		REQUIRE ( st.sums[   1] == 2 );

		// Values subtotals v1 are correctly stored

		//REQUIRE ( st.subtotals_v1[5880] == 0 );

		REQUIRE ( st.subtotals_v1[5879] == (3052896u * 3052896u & 0xFFFFFFFF) );
		REQUIRE ( st.subtotals_v1[5878] == (3052895u * 3052895u & 0xFFFFFFFF) );
		REQUIRE ( st.subtotals_v1[5877] == (3052894u * 3052894u & 0xFFFFFFFF) );
		// ...
		REQUIRE ( st.subtotals_v1[2942] == ((static_cast<uint64_t>(3052896 - 2940 + 3) * static_cast<uint32_t>(3052896 - 2940 + 3)) & 0xFFFFFFFFu) );
		REQUIRE ( st.subtotals_v1[2941] == ((static_cast<uint64_t>(3052896 - 2940 + 2) * static_cast<uint32_t>(3052896 - 2940 + 2)) & 0xFFFFFFFFu) );
		REQUIRE ( st.subtotals_v1[2940] == ((static_cast<uint64_t>(3052896 - 2940 + 1) * static_cast<uint32_t>(3052896 - 2940 + 1)) & 0xFFFFFFFFu) );

		REQUIRE ( st.subtotals_v1[2939] == 2940 * 2940 );
		REQUIRE ( st.subtotals_v1[2938] == 2939 * 2939 );
		// ...
		REQUIRE ( st.subtotals_v1[   2] == 3 * 3 );
		REQUIRE ( st.subtotals_v1[   1] == 2 * 2 );
		REQUIRE ( st.subtotals_v1[   0] == 1 * 1 );

		// Perform accumulation

		st12.finalize();


		// Check accumulated simple sums

		//CHECK ( st.sums[5880] == 0 );

		CHECK ( st.sums[5879] == 3052896 );
		CHECK ( st.sums[5878] == 3052896 + 3052895 );
		CHECK ( st.sums[5877] == 3052896 + 3052895 + 3052894 );
		// ...
		CHECK ( st.sums[2942] == sum_last_2940 - (3052896 - 2940 + 1) - (3052896 - 2940 + 2) );
		CHECK ( st.sums[2941] == sum_last_2940 - (3052896 - 2940 + 1) );
		CHECK ( st.sums[2940] == sum_last_2940 );

		CHECK ( st.sums[2939] == sum_first_2939 + 2940 );
		CHECK ( st.sums[2938] == sum_first_2939 );
		CHECK ( st.sums[2937] == sum_first_2939 - 2939 );
		CHECK ( st.sums[2936] == sum_first_2939 - 2939 - 2938 );
		// ...
		CHECK ( st.sums[   2] == 6 );
		CHECK ( st.sums[   1] == 3 );
		CHECK ( st.sums[   0] == 1 );


		// TODO Check accumulated subtotals for ARCSv1

		//CHECK ( st.subtotals_v1[5880] == 0 );

		CHECK ( st.subtotals_v1[5879] == (3052896u * 3052896u & 0xFFFFFFFF) );
		CHECK ( st.subtotals_v1[5878] == (3052896u * 3052896u & 0xFFFFFFFF)
			+ (3052895u * 3052895u & 0xFFFFFFFF));
		CHECK ( st.subtotals_v1[5877] == (3052896u * 3052896u & 0xFFFFFFFF)
			+ (3052895u * 3052895u & 0xFFFFFFFF)
			+ (3052894u * 3052894u & 0xFFFFFFFF));
		// ...
		// CHECK ( st.subtotals_v1[2942] == v1_last_2940 - (2939 * 2939) );
		// CHECK ( st.subtotals_v1[2941] == v1_last_2940 - (2939 * 2939) );
		CHECK ( st.subtotals_v1[2940] == v1_last_2940 );

		CHECK ( st.subtotals_v1[2939] == v1_first_2939 + (2940 * 2940) );
		CHECK ( st.subtotals_v1[2938] == v1_first_2939 );
		CHECK ( st.subtotals_v1[2937] == v1_first_2939 - (2939 * 2939) );
		CHECK ( st.subtotals_v1[2936] == v1_first_2939 - (2939 * 2939) - (2938 * 2938) );
		// ...
		CHECK ( st.subtotals_v1[   2] == 3 * 3 + 2 * 2 + 1 * 1 );
		CHECK ( st.subtotals_v1[   1] == 2 * 2 + 1 * 1 );
		CHECK ( st.subtotals_v1[   0] == 1 * 1 );

		// Check subtotals for ARCSv2

		CHECK ( st.subtotals_v2[5879] == 2170 );
		CHECK ( st.subtotals_v2[5878] == 4340 );
		CHECK ( st.subtotals_v2[5877] == 6510 );
		// ...
		CHECK ( st.subtotals_v2[2942] == 6367887 );
		CHECK ( st.subtotals_v2[2941] == 6370052 );
		CHECK ( st.subtotals_v2[2940] == 6372217 );
		//
		CHECK ( st.subtotals_v2[2939] == 0 );
		CHECK ( st.subtotals_v2[2938] == 0 );
		CHECK ( st.subtotals_v2[2937] == 0 );
		// ...
		CHECK ( st.subtotals_v2[   3] == 0 );
		CHECK ( st.subtotals_v2[   2] == 0 );
		CHECK ( st.subtotals_v2[   1] == 0 );

		//

		CHECK ( st.current_subtotal_v1 == 0 );
		CHECK ( st.current_subtotal_v2 == 0 );
		CHECK ( st.current_cs_sum      == 0 );
	}



	SECTION ( "Update over first 2940 values is correct" )
	{
		CHECK ( st12.cache_index() == 0 );
		CHECK ( *cbegin(sdata) == 1 );
		CHECK ( *cbegin(sdata) + 2939 - 1 == 2939 );

		st12.update(cbegin(sdata), cbegin(sdata) + 2939, 2939);

		// Index is correct after caching

		//CHECK ( st12.cache_index() == 2941 );
		CHECK ( st.current_cs_sum == sum_first_2939 );

		// Check simple sums

		// CHECK ( st.sums[   1] == 1 );
		// CHECK ( st.sums[   2] == 3 );
		// CHECK ( st.sums[   3] == 6 );
		// // ...
		// CHECK ( st.sums[2938] == sum_first_2939 - 2940 - 2939 );
		// CHECK ( st.sums[2939] == sum_first_2939 - 2940 );
		// CHECK ( st.sums[2940] == sum_first_2939 );

		// Check subtotals for ARCSv1

		// CHECK ( st.subtotals_v1[1] ==  1 );
		// CHECK ( st.subtotals_v1[2] ==  5 );
		// CHECK ( st.subtotals_v1[3] == 14 );
		// // ...
		// CHECK ( st.subtotals_v1[2939] == v1_first_2939 - (2940 * 2940) );
		// CHECK ( st.subtotals_v1[2940] == v1_first_2939 );

		// Check subtotals for ARCSv2

		// CHECK ( st.subtotals_v2[1] == 0 );
		// CHECK ( st.subtotals_v2[2] == 0 );
		// CHECK ( st.subtotals_v2[2] == 0 );
		// //
		// CHECK ( st.subtotals_v2[2939] == 0 );
		// CHECK ( st.subtotals_v2[2940] == 0 );

		//

		CHECK ( st.current_subtotal_v1 == v1_first_2939 );
		CHECK ( st.current_subtotal_v2 == 0 );

		// CHECK ( st.subtotals_v1[0] ==  607434128 ); // ?
		// CHECK ( st.subtotals_v2[0] == 2206772158 ); // ?
		// CHECK ( st.sums[0]         ==   49003696 ); // ?
	}
}

