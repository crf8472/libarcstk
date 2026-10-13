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

#include <cstddef>                // for ptrdiff_t
#include <cstdint>                // for uint32_t

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type
#endif

#include "ref_data.hpp"           // for standard_data

//TODO no linting for cppcoreguidelines-avoid-do-while

TEST_CASE ( "UpdateableSubtotals caches values correctly",
		"[updateablesubtotals] [calc] [accuraterip]" )
{
	using arcstk::accuraterip::details::UpdateableSubtotals;
	using arcstk::checksum::type;

	using std::cbegin;
	using std::cend;

	auto st12 = UpdateableSubtotals<
		type::ARCS1, type::ARCS2, type::F4501, type::F4502> {};

	const auto& st = st12.subtotals();

	REQUIRE ( st.subtotals_v1[0] == 0 );
	REQUIRE ( st.subtotals_v2[0] == 0 );
	REQUIRE ( st.sums[0]         == 0 );
	REQUIRE ( st.multiplier      == 1 );


	auto to_ptrdiff = [](const int64_t amount) -> std::ptrdiff_t
	{
		return static_cast<std::ptrdiff_t>(amount);
	};

	// init data

	using arcstk::testing::data::standard_data;

	const auto tsize  = uint64_t { 3052896 };
	// length of Bach, Organ Concertos, Track 1

	const auto fskip  = uint64_t { 2939 };
	const auto bskip  = uint64_t { 2940 };

	const auto sdata = standard_data(tsize);

	REQUIRE ( sdata[      0] ==     1 );
	REQUIRE ( sdata[3052895] == tsize );

	// first equals 1
	REQUIRE ( *cbegin(sdata) == 1 );

	// last equals tsize
	REQUIRE ( *(cend(sdata) - to_ptrdiff(1)) == tsize );

	// 2939
	REQUIRE ( *cbegin(sdata) + to_ptrdiff(fskip - 1) == fskip );

	// 3049957
	REQUIRE ( *(cbegin(sdata) + to_ptrdiff(tsize - bskip)) == tsize - bskip + 1 );

	using multiplier_t = uint_fast64_t; // type of the multiplier
	using cachevar_t   = uint_fast32_t; // type of the accumulating variable

	// NOTE: type uint_fast32_t is allowed to be bigger than 32 bit.
	// The width of the caching type that is used to accumulate the products
	// is therefore platform dependent: it is at least 32 bit but may be bigger.
	// If we accept this, we MUST normalize the access to ensure deterministic
	// overflows in the result. We just have to cast values that are bigger than
	// 2^{32} - 1 to 32 bit.

	// same types as in Update<>
	auto arcs_v1 = [](const multiplier_t m, const uint32_t v) -> uint32_t
	{
		return m * v & 0xFFFFFFFF;
	};

	// convert to uint32_t (in case uint_fast32_t is not 32 bit wide)
	auto v32 = [](const cachevar_t v) -> uint32_t
	{
		return static_cast<uint32_t>(v);
	};

	// reuse this
	auto cachevar = cachevar_t { 0 };

	// turn multiplier to index
	auto index = [](const multiplier_t m) -> std::size_t
	{
		return static_cast<std::size_t>(m);
	};

	// simple sum of first 2939 samples

	cachevar = 0;
	for (multiplier_t i = 1; i <= fskip/*2939*/; ++i)
	{
		cachevar += sdata[index(i - 1)];
	}
	const auto sum_first_2939 = cachevar;

	REQUIRE (sum_first_2939 == 4320330 );


	// simple sum of last 2940 samples

	cachevar = 0;
	for (multiplier_t i = 3052896; i > (3052896 - bskip); --i)
	{
		cachevar += sdata[index(i - 1)];
	}
	const auto sum_last_2940 = cachevar;

	//REQUIRE (sum_last_2940 == 381259318 ); // with overflow
	REQUIRE (v32(sum_last_2940) == v32(8971193910) ); // -Woverflow


	// subtotal_v1 of first 2939 samples

	cachevar = 0;
	for (multiplier_t i = 1; i <= fskip/*2939*/; ++i)
	{
		cachevar += arcs_v1(i, sdata[index(i - 1)]);
	}
	const auto v1_first_2939 = cachevar;

	//REQUIRE ( v1_first_2939 == 4171439394 ); // with overflow
	REQUIRE ( v32(v1_first_2939) == v32(8466406690) ); // -Woverflow


	// subtotal_v1 of last 2940 samples

	cachevar = 0;
	for (multiplier_t i = 3052896; i > (3052896 - bskip); --i)
	{
		cachevar += arcs_v1(i, sdata[index(i - 1)]);
	}
	const auto v1_last_2940 = cachevar;

	//REQUIRE ( v1_last_2940 == 522596770 ); // with overflow
	REQUIRE ( v32(v1_last_2940) == v32(6477333279138) ); // -Woverflow


	SECTION ( "Store and accumulate first 2939 and last 2940 simple sums"
			" correctly" )
	{
		std::size_t idx = 0;

		// cache first 2940 values
		st12.cache(cbegin(sdata), cbegin(sdata) + to_ptrdiff(2940), idx);


		REQUIRE ( st.sums[2939] == 2940 );

		// Index is correct after caching
		REQUIRE ( idx == 2940 );

		st12.set_multiplier(3052896 - 2940 + 1);

		// cache last 2940 values
		st12.cache(cbegin(sdata) + to_ptrdiff(3052896 - 2940), cend(sdata), idx);

		REQUIRE ( st.sums[2939] == 2940 );

		// Index is correct after caching
		REQUIRE ( idx == 5880 );

		// Values for sums are correctly stored (before accumulation)

		REQUIRE ( st.sums[5879] == 3052896 );
		REQUIRE ( st.sums[5878] == 3052896 - 1 ); // 2940 to 2939
		REQUIRE ( st.sums[5877] == 3052896 - 2 ); // 2940 to 2938
		// ...
		REQUIRE ( st.sums[2942] == 3052896 - 2940 + 3 );
		REQUIRE ( st.sums[2941] == 3052896 - 2940 + 2 );
		REQUIRE ( st.sums[2940] == 3052896 - 2940 + 1 );
		// first part begins
		REQUIRE ( st.sums[2939] == 2940 );
		REQUIRE ( st.sums[2938] == 2939 );
		// ...
		REQUIRE ( st.sums[   3] == 4 );
		REQUIRE ( st.sums[   2] == 3 );
		REQUIRE ( st.sums[   1] == 2 );

		st12.finalize(); // accumlation

		// Check accumulated simple sums

		CHECK ( st.sums[5879] == 3052896 );
		CHECK ( st.sums[5878] == 3052896 + 3052895 );
		CHECK ( st.sums[5877] == 3052896 + 3052895 + 3052894 );
		// ...
		// for overflowing values, test 32bit version
		CHECK ( v32(st.sums[2942]) == v32(sum_last_2940 - (3052896 - 2940 + 1) - (3052896 - 2940 + 2)) );
		CHECK ( v32(st.sums[2941]) == v32(sum_last_2940 - (3052896 - 2940 + 1)) );
		CHECK ( v32(st.sums[2940]) == v32(sum_last_2940) );
		// first part begins
		CHECK ( st.sums[2939] == sum_first_2939 + 2940 );
		CHECK ( st.sums[2938] == sum_first_2939 );
		CHECK ( st.sums[2937] == sum_first_2939 - 2939 );
		CHECK ( st.sums[2936] == sum_first_2939 - 2939 - 2938 );
		// ...
		CHECK ( st.sums[   2] == 6 );
		CHECK ( st.sums[   1] == 3 );
		CHECK ( st.sums[   0] == 1 );

		// TODO Check 5580 - 11759
	}

	SECTION ( "Caches first 2939 simple sums correctly" )
	{
		std::size_t idx = 0;

		//NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
		st12.cache(cbegin(sdata), cbegin(sdata) + to_ptrdiff(bskip), idx);
		st12.finalize();

		// Index and current sums are correct after caching

		CHECK ( st.current_subtotal_v1 == 0 );
		CHECK ( st.current_subtotal_v2 == 0 );
		CHECK ( st.current_cs_sum      == 0 );

		CHECK ( idx == 2940 );

		// simple sums 0 - 2939

		CHECK ( st.sums[   0] == 1 );
		CHECK ( st.sums[   1] == 3 );
		CHECK ( st.sums[   2] == 6 );
		// ...
		CHECK ( st.sums[2936] == sum_first_2939 - 2939 - 2938 );
		CHECK ( st.sums[2937] == sum_first_2939 - 2939 );
		CHECK ( st.sums[2938] == sum_first_2939 );
		CHECK ( st.sums[2939] == sum_first_2939 + 2940 );
	}

	SECTION ( "Caches first 2939 ARCSv1 subtotals correctly" )
	{
		std::size_t idx = 0;

		//NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
		st12.cache(cbegin(sdata), cbegin(sdata) + to_ptrdiff(bskip), idx);
		st12.finalize();

		// Index and current sums are correct after caching

		CHECK ( st.current_subtotal_v1 == 0 );
		CHECK ( st.current_subtotal_v2 == 0 );
		CHECK ( st.current_cs_sum      == 0 );

		CHECK ( idx == 2940 );

		// Check accumulated subtotals for ARCSv1

		CHECK ( st.subtotals_v1[0] ==  1 );
		CHECK ( st.subtotals_v1[1] ==  5 );
		CHECK ( st.subtotals_v1[2] == 14 );
		// ...
		// for overflowing values, test 32bit version
		CHECK ( v32(st.subtotals_v1[2937]) == v32(v1_first_2939 - arcs_v1(2939, 2939)) );
		CHECK ( v32(st.subtotals_v1[2938]) == v32(v1_first_2939) );
		CHECK ( v32(st.subtotals_v1[2939]) == v32(v1_first_2939 + arcs_v1(2940, 2940)) );
		//NOLINTEND(cppcoreguidelines-avoid-do-while)
	}

	SECTION ( "Caches first 2939 ARCSv2 subtotals correctly" )
	{
		std::size_t idx = 0;

		//NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
		st12.cache(cbegin(sdata), cbegin(sdata) + to_ptrdiff(bskip), idx);
		st12.finalize();

		// Index and current sums are correct after caching

		CHECK ( st.current_subtotal_v1 == 0 );
		CHECK ( st.current_subtotal_v2 == 0 );
		CHECK ( st.current_cs_sum      == 0 );

		CHECK ( idx == 2940 );

		// Check accumulated subtotals for ARCSv2

		CHECK ( st.subtotals_v2[0] == 0 );
		CHECK ( st.subtotals_v2[1] == 0 );
		CHECK ( st.subtotals_v2[2] == 0 );
		//
		CHECK ( st.subtotals_v2[2938] == 0 );
		CHECK ( st.subtotals_v2[2939] == 0 );

		CHECK ( st.subtotals_v2[2940] == 0 );
		CHECK ( st.subtotals_v2[2941] == 0 );
		//NOLINTEND(cppcoreguidelines-avoid-do-while)
	}

	SECTION ( "Store and accumulate first 2939 and last 2940 ARCSv1 and ARCSv2"
			" subtotals correctly" )
	{
		std::size_t idx = 0;

		// cache first 2940 values
		st12.cache(cbegin(sdata), cbegin(sdata) + to_ptrdiff(2940), idx);

		REQUIRE ( st.sums[2939] == 2940 );

		// Index is correct after caching
		CHECK ( idx == 2940 );

		st12.set_multiplier(3052896 - 2940 + 1);

		// cache last 2940 values
		st12.cache(cbegin(sdata) + to_ptrdiff(3052896 - 2940), cend(sdata), idx);

		REQUIRE ( st.sums[2939] == 2940 );

		// Index is correct after caching
		CHECK ( idx == 5880 );

		// Values subtotals v1 are correctly stored

		REQUIRE ( st.subtotals_v1[   0] == 1 * 1 );
		REQUIRE ( st.subtotals_v1[   1] == 2 * 2 );
		REQUIRE ( st.subtotals_v1[   2] == 3 * 3 );
		// ...
		REQUIRE ( st.subtotals_v1[2938] == 2939l * 2939u );
		REQUIRE ( st.subtotals_v1[2939] == 2940l * 2940u );
		// last part begins
		// for overflowing values, test 32bit version
		REQUIRE ( v32(st.subtotals_v1[2940]) == arcs_v1(3052896 - 2940 + 1, 3052896 - 2940 + 1) );
		REQUIRE ( v32(st.subtotals_v1[2941]) == arcs_v1(3052896 - 2940 + 2, 3052896 - 2940 + 2) );
		REQUIRE ( v32(st.subtotals_v1[2942]) == arcs_v1(3052896 - 2940 + 3, 3052896 - 2940 + 3) );
		// ...
		REQUIRE ( v32(st.subtotals_v1[5877]) == arcs_v1(3052894, 3052894) );
		REQUIRE ( v32(st.subtotals_v1[5878]) == arcs_v1(3052895, 3052895) );
		REQUIRE ( v32(st.subtotals_v1[5879]) == arcs_v1(3052896, 3052896) );

		// TODO Values subtotals v2 are correctly stored

		st12.finalize(); // accumulation

		CHECK ( st.current_subtotal_v1 == 0 );
		CHECK ( st.current_subtotal_v2 == 0 );
		CHECK ( st.current_cs_sum      == 0 );


		// Check accumulated subtotals for ARCSv1

		// for overflowing values, test 32bit version
		CHECK ( v32(st.subtotals_v1[5879]) == arcs_v1(3052896, 3052896) );
		CHECK ( v32(st.subtotals_v1[5878]) == arcs_v1(3052896, 3052896)
				+ arcs_v1(3052896 - 1, 3052896 - 1) );
		CHECK ( v32(st.subtotals_v1[5877]) == arcs_v1(3052896, 3052896)
				+ arcs_v1(3052896 - 1, 3052896 - 1)
				+ arcs_v1(3052896 - 2, 3052896 - 2) );
		// ...
		CHECK ( v32(st.subtotals_v1[2940]) == v32(v1_last_2940) );

		CHECK ( v32(st.subtotals_v1[2939]) == v32(v1_first_2939 + (2940l * 2940u)) );
		CHECK ( v32(st.subtotals_v1[2938]) == v32(v1_first_2939) );
		CHECK ( v32(st.subtotals_v1[2937]) == v32(v1_first_2939 - (2939l * 2939u)) );
		CHECK ( v32(st.subtotals_v1[2936]) == v32(v1_first_2939 - (2939l * 2939u) - (2938l * 2938u)) );
		// ...
		CHECK ( st.subtotals_v1[   2] == 3 * 3 + 2 * 2 + 1 * 1 );
		CHECK ( st.subtotals_v1[   1] == 2 * 2 + 1 * 1 );
		CHECK ( st.subtotals_v1[   0] == 1 * 1 );

		// Check accumulated subtotals for ARCSv2

		CHECK ( st.subtotals_v2[5879] == 2170 );
		CHECK ( st.subtotals_v2[5878] == 4340 );
		CHECK ( st.subtotals_v2[5877] == 6510 );
		// ...
		CHECK ( st.subtotals_v2[2942] == 6367887 );
		CHECK ( st.subtotals_v2[2941] == 6370052 );
		CHECK ( st.subtotals_v2[2940] == 6372217 );
		// first part begins
		CHECK ( st.subtotals_v2[2939] == 0 );
		CHECK ( st.subtotals_v2[2938] == 0 );
		CHECK ( st.subtotals_v2[2937] == 0 );
		// ...
		CHECK ( st.subtotals_v2[   3] == 0 );
		CHECK ( st.subtotals_v2[   2] == 0 );
		CHECK ( st.subtotals_v2[   1] == 0 );
	}

	SECTION ( "Update over first 2940 values is correct" )
	{
		auto idx = std::size_t { 0 };
		auto idx_f450 = std::size_t { 5880 };

		CHECK ( *cbegin(sdata) == 1 );
		CHECK ( *cbegin(sdata) + to_ptrdiff(2939) == 2940 );

		st12.update(cbegin(sdata), cbegin(sdata) + to_ptrdiff(2940), 2939, idx,
				idx_f450);


		// Cache index is correct after updating

		CHECK ( idx == 2940 );

		// Checksums are correct

		CHECK ( v32(st.current_subtotal_v1) == v32(v1_first_2939 + arcs_v1(2940, 2940)) );
		CHECK ( v32(st.current_subtotal_v1) == v32(8475050290) );

		CHECK ( st.current_subtotal_v2 == 0 ); // no bits higher than 31

		CHECK ( st.current_cs_sum      == sum_first_2939 + 2940 );

		// update() stored correctly (not accumulated):

		// Simple sums

		CHECK ( st.sums[   0] == 1 );
		CHECK ( st.sums[   1] == 2 );
		CHECK ( st.sums[   2] == 3 );
		CHECK ( st.sums[   3] == 4 );
		// ...
		CHECK ( st.sums[2937] == 2938 );
		CHECK ( st.sums[2938] == 2939 );
		CHECK ( st.sums[2939] == 2940 );

		// Subtotals for ARCSv1

		CHECK ( st.subtotals_v1[0] == 1 );
		CHECK ( st.subtotals_v1[1] == 4 );
		CHECK ( st.subtotals_v1[2] == 9 );
		// ...
		CHECK ( v32(st.subtotals_v1[2937]) == arcs_v1(2938, 2938) );
		CHECK ( v32(st.subtotals_v1[2938]) == arcs_v1(2939, 2939) );
		CHECK ( v32(st.subtotals_v1[2939]) == arcs_v1(2940, 2940) );

		// Subtotals for ARCSv2

		CHECK ( st.subtotals_v2[1] == 0 );
		CHECK ( st.subtotals_v2[2] == 0 );
		CHECK ( st.subtotals_v2[2] == 0 );
		// ...
		CHECK ( st.subtotals_v2[2939] == 0 );
		CHECK ( st.subtotals_v2[2940] == 0 );
	}

	SECTION ( "sections() provides correct split points" )
	{
		REQUIRE ( st12.cache_size()  == 11760 );
		REQUIRE ( st12.do_frame450() == true );

		// frames 445 - 454
		st12.set_current_length(sdata.size());
		st12.set_multiplier((445 * 588) + 1);

		REQUIRE ( st12.current_length() == 3052896 );
		REQUIRE ( st12.multiplier()     == 261661 );

		// only frame 445 - frame 455
		const auto secs1 = st12.sections(261661 /*multiplier*/,
				5880 /*usize*/, 3052896 /*tsize*/,
				false /* no regular cache */, true /* but do frame 450 */);

		CHECK ( secs1[0] == -1 ); // no first 2940
		CHECK ( secs1[1] == -1 );
		CHECK ( secs1[2] == 2940 );
		CHECK ( secs1[3] == 3528 );
		CHECK ( secs1[4] == -1 ); // no drive offstes, no end of frame 455
		CHECK ( secs1[5] == -1 ); // no last 2940

		// only frame 445 - frame 455
		const auto secs2 = st12.sections(261661 /*multiplier*/,
				5880 /*usize*/, 3052896 /*tsize*/,
				true /* do drive offsets */, true /* but do frame 450 */);

		CHECK ( secs2[0] == -258720 ); // no first 2940
		CHECK ( secs2[1] == 0 ); // start here
		CHECK ( secs2[2] == 2940 );
		CHECK ( secs2[3] == 3528 );
		CHECK ( secs2[4] == 5880 ); // no end of frame 455
		CHECK ( secs2[5] == -1 ); // no last 2940
	}

	SECTION ( "Update over frame 450 values is correct" )
	{
		REQUIRE ( st12.cache_size()  == 11760 );
		REQUIRE ( st12.do_frame450() == true );

		// frames 445 - 454
		st12.set_current_length(sdata.size());
		st12.set_multiplier((445 * 588) + 1);

		REQUIRE ( st12.current_length() == 3052896 );
		REQUIRE ( st12.multiplier()     == 261661 ); // start of frame 445


		auto idx = std::size_t { 0 };
		auto idx_f450 = std::size_t { 5880 };

		// only frame 445 - frame 455
		st12.update(cbegin(sdata) + to_ptrdiff(445 * 588),
				cbegin(sdata) + to_ptrdiff(455 * 588),
				5880, idx, idx_f450);

		CHECK ( idx_f450 == 11760 );
		CHECK ( idx == 0 );


		// upwards
		CHECK ( st.subtotals_v1[ 5880] == v32(68466478921) ); // (445 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[ 6468] == v32(68774538001) ); // (446 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[ 7056] == v32(69083288569) ); // (447 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[ 7644] == v32(69392730625) ); // (448 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[ 8232] == v32(69702864169) ); // (449 * 588 + 1) ^ 2
		//
		CHECK ( st.subtotals_v1[ 8819] == v32(70013160000) ); // (450 * 588) ^ 2

		// downwards
		CHECK ( st.subtotals_v1[ 8820] == v32(70013689201) ); // (450 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[ 9408] == v32(70325205721) ); // (451 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[ 9996] == v32(70637413729) ); // (452 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[10584] == v32(70950313225) ); // (453 * 588 + 1) ^ 2
		CHECK ( st.subtotals_v1[11172] == v32(71263904209) ); // (454 * 588 + 1) ^ 2
		//
		CHECK ( st.subtotals_v1[11759] == v32(71577651600) ); // (455 * 588) ^ 2


		// ARCSv1 of frame 450

		cachevar = 0;
		for (multiplier_t i = 450 * 588 + 1; i <= (451 * 588); ++i)
		{
			cachevar += arcs_v1(i, sdata[index(i - 1)]);
		}
		const auto v1_f450 = cachevar;

		REQUIRE ( v32(v1_f450) == v32(852393145226) );
		REQUIRE ( v32(v1_f450) == 0x7697338A );
		// Note: this is with multiplier 450 * 588 + 1, but we require 1

		st12.finalize();

		// TODO Check accumulated values for frame 450

		CHECK ( st12.value<type::F4501>() == 0xAF1E293A );
	}
}

