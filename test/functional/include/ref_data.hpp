#ifndef LIBARCSTK_REFDATA_HPP_
#define LIBARCSTK_REFDATA_HPP_

/**
 * \file
 *
 * \brief Test helpers for unit tests: reference data.
 */

#include <cstdint>                 // for int32_t
#include <numeric>                 // for iota
#include <vector>                  // for vector

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"            // for AudioSize, ToC, make_toc, UNIT
#endif

namespace arcstk
{
namespace testing
{
namespace data
{

/**
 * \brief Produce data with size s of the form { 1, 2, ..., s-1 }.
 */
inline auto standard_data(const std::size_t s, const uint32_t i)
	-> std::vector<uint32_t>
{
	using std::cbegin;
	using std::cend;

	auto sdata = std::vector<uint32_t>(s);
	std::iota(begin(sdata), end(sdata), i);

	return sdata;
}

/**
 * \brief Produce data with size s of the form { 1, 2, ..., s-1 }.
 */
inline auto standard_data(const std::size_t s) -> std::vector<uint32_t>
{
	return standard_data(s, 1);
}


inline auto arcs1_over_standard_data(const uint32_t from, const uint32_t upto)
{
	auto arcs_v1 = uint32_t { 0 };

	auto cs = uint32_t { from };
	for (uint64_t i = 1; i <= upto; ++i, ++cs)
	{
		arcs_v1 += (i * cs & 0xFFFFFFFF);
	}

	return arcs_v1;
}


/**
 * \brief Data set 1.
 */
namespace toc1
{
	/* Bach, Organ Concertos, Simon Preston, DGG */

	using arcstk::AudioSize;
	using arcstk::UNIT;

	const auto offsets = std::vector<int32_t> {
		    33,
		  5225,
		  7390,
		 23380,
		 35608,
		 49820,
		 69508,
		 87733,
		106333,
		139495,
		157863,
		198495,
		213368,
		225320,
		234103
	};

	const auto leadout = int32_t { 253038 };

	const auto points = std::vector<AudioSize> {
		AudioSize {     33 * 588, UNIT::SAMPLES },
		AudioSize {   5225 * 588, UNIT::SAMPLES },
		AudioSize {   7390 * 588, UNIT::SAMPLES },
		AudioSize {  23380 * 588, UNIT::SAMPLES },
		AudioSize {  35608 * 588, UNIT::SAMPLES },
		AudioSize {  49820 * 588, UNIT::SAMPLES },
		AudioSize {  69508 * 588, UNIT::SAMPLES },
		AudioSize {  87733 * 588, UNIT::SAMPLES },
		AudioSize { 106333 * 588, UNIT::SAMPLES },
		AudioSize { 139495 * 588, UNIT::SAMPLES },
		AudioSize { 157863 * 588, UNIT::SAMPLES },
		AudioSize { 198495 * 588, UNIT::SAMPLES },
		AudioSize { 213368 * 588, UNIT::SAMPLES },
		AudioSize { 225320 * 588, UNIT::SAMPLES },
		AudioSize { 234103 * 588, UNIT::SAMPLES }
	};
} // namespace toc1

} // namespace data
} // namespace testing
} // namespace arcstk

#endif

