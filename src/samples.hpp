#ifndef __LIBARCS_SAMPLES_HPP__
#define __LIBARCS_SAMPLES_HPP__

/**
 * \file samplesequence.hpp Interface for a templated sample sequence
 *
 */


#include <cstdint>
#include <iterator>
#include <type_traits> // for conditional, is_same
#include <memory>

/// @{
namespace arcs
{


/**
 * A random access iterator for samples.
 *
 * Provides a representation of the 16 bit stereo samples for each channel as
 * a single uint32_t integer.
 */
template <typename T, bool is_planar, bool is_const>
class SampleIterator;


/**
 * A SampleSequence for signed integral types that provides converting access to
 * the samples.
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


// forward declaration required by SampleBlock
template<bool is_const>
class SampleBlockIterator;


/**
 * A block of samples to <tt>update()</tt> a Calculation.
 *
 * This block has its initial capacity fixed and is non-copyable.
 *
 * Samples can only be assigned in two ways: via iterator or via the
 * <tt>front()</tt> pointer. The latter method is useful when using
 * SampleBlock as a target for using the <tt>read()</tt> method of an
 * STL stream.
 */
class SampleBlock final
{

public: /* types */

	using iterator = SampleBlockIterator<false>;

	using const_iterator = SampleBlockIterator<true>;

public: /* methods */

	/**
	 * Constructor.
	 *
	 * Construct the instance with a fixed capacity.
	 *
	 * \param capacity Capacity of the container in number of PCM 32 bit samples
	 */
	explicit SampleBlock(const std::size_t capacity);

	// Non-copyable class
	SampleBlock(const SampleBlock &rhs) = delete;

	/**
	 * Move constructor
	 *
	 * \param[in] rhs The instance to move
	 */
	SampleBlock(SampleBlock&& rhs) noexcept;

	/**
	 * Default destructor
	 */
	~SampleBlock() noexcept;

	/**
	 * Return iterator pointing to the beginning
	 *
	 * \return iterator pointing to the beginning
	 */
	iterator begin();

	/**
	 * Return iterator pointing to the end
	 *
	 * \return iterator pointing to the end
	 */
	iterator end();

	/**
	 * Return const_iterator pointing to the beginning
	 *
	 * \return const_iterator pointing to the beginning
	 */
	const_iterator begin() const;

	/**
	 * Return const_iterator pointing to the end
	 *
	 * \return const_iterator pointing to the end
	 */
	const_iterator end() const;

	/**
	 * Return const_iterator pointing to the beginning
	 *
	 * \return const_iterator pointing to the beginning
	 */
	const_iterator cbegin() const;

	/**
	 * Return const_iterator pointing to the end
	 *
	 * \return const_iterator pointing to the end
	 */
	const_iterator cend() const;

	/**
	 * Actual number of elements in the instance.
	 *
	 * \return Actual number of elements in the container
	 */
	std::size_t size() const;

	/**
	 * Set the capacity to a new value.
	 *
	 * \param[in] num_samples Number of 32 bit PCM samples
	 */
	void set_size(std::size_t num_samples);

	/**
	 * Capacity of this instance.
	 *
	 * \return Capacity of this instance in number of 32 bit PCM samples
	 */
	std::size_t capacity() const;

	/**
	 * Returns TRUE if the instance holds no elements.
	 *
	 * \return TRUE if the instance holds no elements, otherwise FALSE
	 */
	bool empty() const;

	/**
	 * Pointer to the start of the samples.
	 *
	 * \return Raw pointer to the beginning of the buffer
	 */
	uint32_t* front();

	// Non-copyable class
	SampleBlock& operator = (const SampleBlock &rhs) = delete;

	/**
	 * Move assignment
	 *
	 * \param[in] rhs The instance to move
	 */
	SampleBlock& operator = (SampleBlock&& rhs) noexcept;


private:

	// forward declaration
	class Impl;

	/**
	 * Private implementation of SampleBlock
	 */
	std::unique_ptr<SampleBlock::Impl> impl_;
};

} // namespace arcs
/// @}

#ifndef __LIBARCSREAD_SAMPLES_TPP__
#include "details/samples.tpp"
#endif

#endif

