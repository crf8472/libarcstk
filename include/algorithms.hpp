#ifndef __LIBARCSTK_ALGORITHMS_HPP__
#define __LIBARCSTK_ALGORITHMS_HPP__
/**
 * \file
 *
 * \brief Algorithms for calculating AccurateRip checksums.
 */

#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#include "accuraterip.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{
/** \addtogroup calc */
/** @{ */

/**
 * \brief AccurateRip checksum algorithm version 1.
 */
using AccurateRipV1   = accuraterip::V1;

/**
 * \brief AccurateRip checksum algorithm version 2.
 */
using AccurateRipV2   = accuraterip::V2;

/**
 * \brief AccurateRip checksum algorithm version 2 providing also version 1.
 */
using AccurateRipV1V2 = accuraterip::V1and2;

/** @} */ // group calc

} // namespace v_1_0_0
} // namespace arcstk

#endif

