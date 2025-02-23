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

namespace accuraterip
{

/**
 * \brief AccurateRip checksum algorithm version 1.
 */
using V1 = details::Version1;

/**
 * \brief AccurateRip checksum algorithm version 2.
 */
using V2 = details::Version2;

/**
 * \brief AccurateRip checksum algorithm version 2 providing also version 1.
 */
using V1andV2 = details::Versions1and2;

} // namespace accuraterip

/** @} */ // group calc

} // namespace v_1_0_0
} // namespace arcstk

#endif

