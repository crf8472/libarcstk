#ifndef __LIBARCSTK_SAMPLES_HPP__
#define __LIBARCSTK_SAMPLES_HPP__

/**
 * \file
 *
 * \brief Public API for representing unconverted sequences of samples.
 */

#include <cstdint>
#include <iterator>
#include <type_traits> // for conditional, is_same
#include <memory>


namespace arcstk
{

inline namespace v_1_0_0
{

/**
 * \brief A random access iterator for samples.
 *
 * Provides a representation of the 16 bit stereo samples for each channel as
 * a single uint32_t integer.
 */
template <typename T, bool is_planar, bool is_const>
class SampleIterator;


/**
 * \brief A sequence of samples represented by signed integral types.
 *
 * A SampleSequence is an adapter for passing sample sequences of virtually any
 * signed integral format to Calculation::update(). SampleSequence instances
 * provide converting access to the samples.
 *
 * The channel ordering can be either left/right or right/left.
 */
template <typename T, bool is_planar = true>
class SampleSequence final
{

public:

	SampleSequence(const SampleSequence &) = delete;

	SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	SampleIterator<T, is_planar, false> begin();

	/**
	 * Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	SampleIterator<T, is_planar, false> end();

	/**
	 * Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	SampleIterator<T, is_planar, true> begin() const;

	/**
	 * Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	SampleIterator<T, is_planar, true> end() const;

	/**
	 * Rewrap the specified buffers into this sample sequence
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size  Number of 16bit samples of genuine format in each buffer
	 */
	void wrap(const uint8_t *buffer0, const uint8_t *buffer1,
			const uint32_t &size);

	/**
	 * Provides access to the samples in a uniform format (32 bit PCM)
	 * Bits 31-24: Left Channel MSB
	 * Bits 23-16: Left Channel LSB
	 * Bits 15-09: Right Channel MSB
	 * Bits 08-00: Right Channel LSB
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 */
	uint32_t operator [] (const uint32_t index) const;

	/**
	 * Provides access to the samples in a uniform format (32 bit PCM)
	 * Bits 31-24: Left Channel MSB
	 * Bits 23-16: Left Channel LSB
	 * Bits 15-09: Right Channel MSB
	 * Bits 08-00: Right Channel LSB
	 *
	 * Does bounds checking before accessing the sample.
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 *
	 * \throw std::out_of_range if \c index is out of range
	 */
	uint32_t at(const uint32_t index) const;

	/**
	 * Rewrap the specified buffers into this sample sequence
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of T's per buffer
	 */
	void reset(const T* buffer0, const T* buffer1, const uint32_t &size);
};

} // namespace v_1_0_0

} // namespace arcstk


#ifndef __LIBARCSTK_SAMPLES_TPP__
#include "details/samples.tpp"
#endif

#endif

