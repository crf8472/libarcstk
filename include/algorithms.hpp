#ifndef LIBARCSTK_ALGORITHMS_HPP_
#define LIBARCSTK_ALGORITHMS_HPP_

/**
 * \file
 *
 * \brief Algorithms for \link calc calculating AccurateRip checksums\endlink.
 *
 * \details
 *
 * Provides Algorithms AccurateRip v1, AccurateRip v2 and a third variant that
 * contains both, v1 and v2.
 *
 * Part of the API for \link calc calculating AccurateRip checksums\endlink.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

/** \addtogroup calc */
/** @{ */

/**
 * \brief AccurateRip checksum calculation algorithms.
 */
namespace AccurateRip
{

/**
 * \brief AccurateRip checksum algorithm version 1.
 */
using V1 = accuraterip::details::Version1;

/**
 * \brief AccurateRip checksum algorithm version 2.
 */
using V2 = accuraterip::details::Version2;

/**
 * \brief AccurateRip checksum algorithm version 2 providing also version 1.
 */
using V1andV2 = accuraterip::details::Versions1and2;

} // namespace accuraterip

/** @} */ // group calc

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

