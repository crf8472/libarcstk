#ifndef __LIBARCSTK_SAMPLES_HPP__
#define __LIBARCSTK_SAMPLES_HPP__

/**
 * \file
 *
 * \brief Public API for representing unconverted sequences of samples.
 */

#include <cstdint>              // for uint8_t, uint32_t
#include <iosfwd>               // for size_t

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \addtogroup calc
 * @{
 */


/**
 * \brief Type for representing a 32 bit PCM stereo sample.
 *
 * This should be defined identically-as or at least assignable-to
 * arcstk::sample_type.
 *
 * \see arcstk::sample_type
 */
using sample_t = uint32_t;


/**
 * \brief An input iterator for samples in SampleSequence instances.
 *
 * Provides a representation of the 16 bit stereo samples for each channel as
 * a single integer of type sample_t.
 *
 * Equality between a const_iterator and an iterator works as expected.
 *
 * Although tagged as an input_iterator, SampleIterator provides some additional
 * functionality as there is prefix- and postfix decrement, subtract-assignment
 * and a binary subtraction operator.
 */
template <typename T, bool is_planar, bool is_const>
class SampleIterator; // IWYU pragma keep


/**
 * \brief A sequence of samples represented by signed integral types.
 *
 * A SampleSequence is compatibility wrapper for passing sample sequences of
 * virtually any signed integral format to Calculation::update(). SampleSequence
 * instances provide converting access to the samples. The wrapped data is not
 * altered.
 *
 * The sequence is compatible to sample sequences that represent their samples
 * by integral types of either 16 or 32 bit that may or may not be signed.
 * When wrapping the original data in a SampleSequence, it must be correctly
 * declared as either interleaved or planar.
 *
 * The channel ordering can be either left/right or right/left.
 */
template <typename T, bool is_planar>
class SampleSequence;

/** @} */

} // namespace v_1_0_0

} // namespace arcstk


#ifndef __LIBARCSTK_SAMPLES_TPP__
#include "details/samples.tpp"
#endif


namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \addtogroup calc
 * @{
 */

/**
 * \brief Planar sample sequence with samples of type T.
 *
 * A planar sequence has two separated input buffers, one for each channel.
 *
 * T can only be some signed or unsigned integral type of either 16 or 32 bit
 * width.
 */
template <typename T>
using PlanarSampleSequence = SampleSequence<T, true>;


/**
 * \brief Interleaved sample sequence with samples of type T.
 *
 * An interleaved sequence has one input buffer, in which the samples for each
 * channel occurr in order.
 *
 * T can only be some signed or unsigned integral type of either 16 or 32 bit
 * width.
 */
template <typename T>
using InterleavedSampleSequence = SampleSequence<T, false>;

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

