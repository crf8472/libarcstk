#ifndef LIBARCSTK_REFDATA_HPP_
#define LIBARCSTK_REFDATA_HPP_

/**
 * \file
 *
 * \brief Test helpers for unit tests: reference data.
 */

#include <cstdint>  // for int32_t
#include <utility>  // for move
#include <vector>   // for vector

namespace arcstk
{
namespace testing
{
namespace data
{

	const auto toc_1 = make_toc(
		// leadout
		253038,
		// offsets
		std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508,
					87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 }
	);

	const auto points = std::vector<int32_t> {
		/* Bach, Organ Concertos, Simon Preston, DGG */
		    33 * 588,
		  5225 * 588,
		  7390 * 588,
		 23380 * 588,
		 35608 * 588,
		 49820 * 588,
		 69508 * 588,
		 87733 * 588,
		106333 * 588,
		139495 * 588,
		157863 * 588,
		198495 * 588,
		213368 * 588,
		225320 * 588,
		234103 * 588
	};


} // namespace data
} // namespace testing
} // namespace arcstk

#endif

