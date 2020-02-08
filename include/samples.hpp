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
 * \addtogroup calc
 * @{
 */

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

public: /* types */

	/**
	 * \brief Unsigned integer of at least 32 bit size.
	 */
	using value_type = uint32_t;

	using size_type = std::size_t;

	using iterator = SampleIterator<T, is_planar, false>;

	using const_iterator = SampleIterator<T, is_planar, true>;


public: /* functions */

	SampleSequence() = default;

	SampleSequence(const SampleSequence &) = delete;

	SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * \brief Iterator pointing to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	SampleIterator<T, is_planar, false> begin();

	/**
	 * \brief Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	SampleIterator<T, is_planar, false> end();

	/**
	 * \brief Iterator pointing to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	SampleIterator<T, is_planar, true> begin() const;

	/**
	 * \brief Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	SampleIterator<T, is_planar, true> end() const;

	/**
	 * \brief Rewrap the specified buffers into this sample sequence
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size  Number of 16bit samples of genuine format in each buffer
	 */
	void wrap(const uint8_t *buffer0, const uint8_t *buffer1,
			const size_type size);

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM).
	 *
	 * \par Bits 31-24: Left Channel MSB
	 * \par Bits 23-16: Left Channel LSB
	 * \par Bits 15-09: Right Channel MSB
	 * \par Bits 08-00: Right Channel LSB
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 */
	value_type operator [] (const size_type index) const;

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM)
	 *
	 * \par Bits 31-24: Left Channel MSB
	 * \par Bits 23-16: Left Channel LSB
	 * \par Bits 15-09: Right Channel MSB
	 * \par Bits 08-00: Right Channel LSB
	 *
	 * Does bounds checking before accessing the sample.
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 *
	 * \throw std::out_of_range if \c index is out of range
	 */
	value_type at(const size_type index) const;

	/**
	 * \brief Rewrap the specified buffers into this sample sequence
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of T's per buffer
	 */
	void reset(const T* buffer0, const T* buffer1, const size_type size);
};

/** @} */

} // namespace v_1_0_0

} // namespace arcstk


#ifndef __LIBARCSTK_SAMPLES_TPP__
#include "details/samples.tpp"
#endif

#endif

