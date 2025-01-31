#ifndef __LIBARCSTK_ALGORITHMS_HPP__
#define __LIBARCSTK_ALGORITHMS_HPP__
/**
 * \file
 *
 * \brief Concrete algorithms for calculating checksums.
 */

#ifndef __LIBARCSTK_ACCURATERIP_HPP__
#include "accuraterip.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// AccurateRip algorithms

using AccurateRipV1   = accuraterip::V1;
using AccurateRipV2   = accuraterip::V2;
using AccurateRipV1V2 = accuraterip::V1and2;

} // namespace v_1_0_0
} // namespace arcstk

#endif

