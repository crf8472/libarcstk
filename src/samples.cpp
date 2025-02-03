/**
 * \file
 *
 * \brief Instantiations of SampleSequence and SampleIterator
 */

#include <cstdint>

#ifndef __LIBARCSTK_SAMPLES_HPP__
#include "samples.hpp"
#endif

namespace arcstk
{
inline namespace v_1_0_0
{

// Instantiate all legal SampleSequence cases to ensure they are compiled.
// This avoids compilation on the client side.

template class SampleSequence< int16_t, true>;
template class SampleSequence< int32_t, true>;
template class SampleSequence<uint16_t, true>;
template class SampleSequence<uint32_t, true>;

template class SampleSequence< int16_t, false>;
template class SampleSequence< int32_t, false>;
template class SampleSequence<uint16_t, false>;
template class SampleSequence<uint32_t, false>;

} // namespace v_1_0_0
} // namespace arcstk

