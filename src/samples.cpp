/**
 * \internal
 *
 * \file
 *
 * \brief Instantiations of SampleSequence and SampleIterator
 */

#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"
#endif

#include <cstdint>      // for int16_t, uint16_t, int32_t, uint32_t

namespace arcstk
{
inline namespace v_1_0_0
{

// Instantiate all legal SampleSequence cases to ensure they are compiled.
// This avoids compilation on the client side.

template class details::SampleSequence< int16_t, true>;
template class details::SampleSequence< int32_t, true>;
template class details::SampleSequence<uint16_t, true>;
template class details::SampleSequence<uint32_t, true>;

template class details::SampleSequence< int16_t, false>;
template class details::SampleSequence< int32_t, false>;
template class details::SampleSequence<uint16_t, false>;
template class details::SampleSequence<uint32_t, false>;

} // namespace v_1_0_0
} // namespace arcstk

