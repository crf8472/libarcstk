#ifndef __LIBARCSTK_SAMPLES_HPP__
#error "Do not include samples.tpp, include samples.hpp"
#endif
// vim: ft=cpp
#ifndef __LIBARCSTK_SAMPLES_TPP__
#define __LIBARCSTK_SAMPLES_TPP__

/**
 * \file samples.tpp Implementation of SampleSequence and SampleIterator
 */

#include <array>
#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace arcs
{

inline namespace v_1_0_0
{


// forward declaration required by friend delcaration in SampleIterator
template<typename T, bool is_planar>
class SampleSequenceImplBase;


/**
 * Iterator of a SampleSequence
 */
template <typename T, bool is_planar, bool is_const>
class SampleIterator final
{
	// Befriend the converse version of the type: const_iterator can access
	// private members of iterator (and vice versa)
	friend SampleIterator<T, is_planar, not is_const>;

	// Allow use of private constructor
	friend class SampleSequenceImplBase<T, is_planar>;

public:

	using iterator_category = std::random_access_iterator_tag;

	using value_type        = uint32_t;

	using difference_type   = int64_t;

	using pointer           = typename std::conditional<is_const,
			const uint32_t*, uint32_t*>::type;

	using reference         = typename std::conditional<is_const,
			const uint32_t&, uint32_t&>::type;

	/**
	 * Construct const_iterator from iterator
	 *
	 * \param[in] rhs The iterator to construct a const_iterator
	 */
	SampleIterator(const SampleIterator<T, is_planar, false> &rhs);

	/**
	 * Dereference operator
	 *
	 * \return The converted PCM 32 bit sample the iterator points to
	 */
	value_type operator * () const;

	/**
	 * Prefix increment operator
	 */
	SampleIterator& operator ++ ();

	/**
	 * Postfix increment operator
	 */
	SampleIterator operator ++ (int);

	/**
	 * Prefix decrement operator
	 */
	SampleIterator& operator -- ();

	/**
	 * Postfix decrement operator
	 */
	SampleIterator operator -- (int);

	/**
	 * Add amount
	 */
	SampleIterator operator + (const uint32_t value);

	/**
	 * Subtract amount
	 */
	SampleIterator operator - (const uint32_t value);

	/**
	 * Subtract position
	 */
	friend difference_type operator - (const SampleIterator &lhs,
			const SampleIterator &rhs)
	{
		return lhs.pos_ - rhs.pos_;
	}

	/**
	 * Add-assign amount
	 */
	SampleIterator& operator += (const uint32_t value);

	/**
	 * Subtract-assign amount
	 */
	SampleIterator& operator -= (const uint32_t value);

	/**
	 * Subscript operator
	 */
	uint32_t operator [] (const uint32_t index) const;

	/**
	 * Equality
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator == (const SampleIterator &lhs,
			const SampleIterator &rhs) /* const */
	{
		return lhs.seq_ == rhs.seq_ and lhs.pos_ == rhs.pos_;
	}

	/**
	 * Inequality
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator != (const SampleIterator &lhs,
			const SampleIterator &rhs) /* const */
	{
		return not(lhs == rhs);
	}


private:

	/**
	 * Private Constructor.
	 *
	 * Constructs a SampleIterator for the specified SampleSequence starting
	 * at index \c pos.
	 *
	 * \param[in] seq SampleSequence to iterate
	 * \param[in] pos Start index
	 */
	SampleIterator(const SampleSequence<T, is_planar> &seq, const uint32_t pos);

	/**
	 * The SampleSequence to iterate
	 */
	const SampleSequence<T, is_planar> *seq_;

	/**
	 * Current index position
	 */
	uint32_t pos_;
};


// SampleIterator


template <typename T, bool is_planar, bool is_const>
SampleIterator<T, is_planar, is_const>::SampleIterator(
		const SampleSequence<T, is_planar> &seq, const uint32_t pos)
	: seq_(&seq)
	, pos_(pos)
{
	// empty
}


template <typename T, bool is_planar, bool is_const>
SampleIterator<T, is_planar, is_const>::SampleIterator(
		const SampleIterator<T, is_planar, false> &rhs)
	// works due to friendship
	: seq_(rhs.seq_)
	, pos_(rhs.pos_)
{
	// empty
}


template <typename T, bool is_planar, bool is_const>
typename SampleIterator<T, is_planar, is_const>::value_type
	SampleIterator<T, is_planar, is_const>::operator * () const
{
	return seq_->operator[](pos_);
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator ++ ()
	-> SampleIterator<T, is_planar, is_const>&
{
	++pos_;
	return *this;
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator ++ (int)
	-> SampleIterator<T, is_planar, is_const>
{
	SampleIterator prev_val(*this);
	this->operator++();
	return prev_val;
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator -- ()
	-> SampleIterator<T, is_planar, is_const>&
{
	--pos_;
	return *this;
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator -- (int)
	-> SampleIterator<T, is_planar, is_const>
{
	SampleIterator prev_val(*this);
	this->operator--();
	return prev_val;
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator + (const uint32_t value)
	-> SampleIterator<T, is_planar, is_const>
{
	return SampleIterator(*seq_, pos_ + value);
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator - (const uint32_t value)
	-> SampleIterator<T, is_planar, is_const>
{
	return SampleIterator(*seq_, pos_ - value);
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator += (const uint32_t value)
	-> SampleIterator<T, is_planar, is_const>&
{
	pos_ += value;
	return *this;
}


template <typename T, bool is_planar, bool is_const>
auto SampleIterator<T, is_planar, is_const>::operator -= (const uint32_t value)
	-> SampleIterator<T, is_planar, is_const>&
{
	pos_ -= value;
	return *this;
}


template <typename T, bool is_planar, bool is_const>
uint32_t SampleIterator<T, is_planar, is_const>::operator [] (
		const uint32_t index) const
{
	return seq_->operator[](index);
}


// forward delcaration to be used by SampleSequenceImplBase
template<typename T, bool is_planar>
class SampleSequence;


/**
 * Common code base for SampleSequence specializations.
 *
 * This class is not intended for polymorphic use.
 */
template<typename T, bool is_planar>
class SampleSequenceImplBase
{
	static_assert(std::is_same<T,  int16_t>::value
			or    std::is_same<T,  int32_t>::value
			or    std::is_same<T, uint16_t>::value
			or    std::is_same<T, uint32_t>::value,
			"A SampleSequence can only be declared for signed integral types of either 16 or 32 bit width or for unsigned integers of 32 bit width");

public: /* types */

	using iterator = SampleIterator<T, is_planar, false>;

	using const_iterator = SampleIterator<T, is_planar, true>;


public: /* methods */

	/**
	 * Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	iterator begin();

	/**
	 * Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	iterator end();

	/**
	 * Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	const_iterator begin() const;

	/**
	 * Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	const_iterator end() const;

	/**
	 * Return the number of 32 bit PCM samples represented by this sequence.
	 *
	 * \return The number of 32 bit PCM samples represented by this sequence
	 */
	uint32_t size() const;


protected:

	/**
	 * Default constructor
	 */
	SampleSequenceImplBase();

	/**
	 * Protected non-virtual destructor to indicate non-polymorphic use only.
	 */
	virtual ~SampleSequenceImplBase() noexcept;

	/**
	 * Set the number 32 bit PCM samples in this buffer
	 *
	 * \param[in] size number of 32 bit PCM samples in the buffer
	 */
	void set_size(const uint32_t size);

	/**
	 * Convert two integers to a PCM 32 bit sample
	 *
	 * \param[in] higher The higher 16 bit
	 * \param[in] lower  The lower 16 bit
	 *
	 * \return A PCM 32 bit sample with the higher and lower bits as passed
	 */
	uint32_t combine(const uint32_t higher, const uint16_t lower) const;

	/**
	 * Returns 0 if index is within access bounds, otherwise the amount that
	 * \c index exceeds <tt>size()</tt>.
	 *
	 * \param[in] index Index to check
	 *
	 * \return 0 if not out of bounds, otherwise <tt>index - 1 - size()</tt>.
	 */
	int out_of_range(const uint32_t index) const;

	/**
	 * Performs bounds check.
	 *
	 * \param[in] index Index to check
	 *
	 * \throws std::out_of_range if \c index is out of legal range
	 */
	void bounds_check(const uint32_t index) const;

	/**
	 * Pointer to actual SampleSequence
	 */
	virtual const SampleSequence<T, is_planar> *sequence() const
	= 0;


private:

	/**
	 * State: Number of 16 bit samples in this sequence
	 */
	uint32_t size_;
};


// SampleSequenceImplBase


template<typename T, bool is_planar>
SampleSequenceImplBase<T, is_planar>::SampleSequenceImplBase()
	: size_(0)
{
	// empty
}


template<typename T, bool is_planar>
SampleSequenceImplBase<T, is_planar>::~SampleSequenceImplBase() noexcept
= default;


template<typename T, bool is_planar>
auto SampleSequenceImplBase<T, is_planar>::begin()
		-> SampleSequenceImplBase<T, is_planar>::iterator
{
	return SampleIterator<T, is_planar, false>(*this->sequence(), 0);
}


template<typename T, bool is_planar>
auto SampleSequenceImplBase<T, is_planar>::end()
		-> SampleSequenceImplBase<T, is_planar>::iterator
{
	return SampleIterator<T, is_planar, false>(*this->sequence(), this->size());
}


template<typename T, bool is_planar>
auto SampleSequenceImplBase<T, is_planar>::begin() const
		-> SampleSequenceImplBase<T, is_planar>::const_iterator
{
	return SampleIterator<T, is_planar, true>(*this->sequence(), 0);
}


template<typename T, bool is_planar>
auto SampleSequenceImplBase<T, is_planar>::end() const
		-> SampleSequenceImplBase<T, is_planar>::const_iterator
{
	return SampleIterator<T, is_planar, true>(*this->sequence(), this->size());
}


template<typename T, bool is_planar>
uint32_t SampleSequenceImplBase<T, is_planar>::size() const
{
	return size_;
}


template<typename T, bool is_planar>
void SampleSequenceImplBase<T, is_planar>::set_size(const uint32_t size)
{
	size_ = size;
}


template<typename T, bool is_planar>
uint32_t SampleSequenceImplBase<T, is_planar>::combine(const uint32_t higher,
		const uint16_t lower) const
{
	return (higher << 16) | lower;
}


template<typename T, bool is_planar>
int SampleSequenceImplBase<T, is_planar>::out_of_range(const uint32_t index)
	const
{
	return index > this->size() ? this->size() - 1 - index : 0;
}


template<typename T, bool is_planar>
void SampleSequenceImplBase<T, is_planar>::bounds_check(const uint32_t index)
	const
{
	if (this->out_of_range(index))
	{
		std::stringstream msg;
		msg << "Index out of bounds: " << index << ". Size: " << this->size();
		throw std::out_of_range(msg.str());
	}
}


// SampleSequence: Full Specialization for planar sequences (is_planar == true)


/**
 * SampleSequence specialization for planar sequences
 */
template <typename T>
class SampleSequence<T, true> : public SampleSequenceImplBase<T, true>
{

public:

	SampleSequence(const SampleSequence &) = delete;

	SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * Constructor
	 *
	 * \param[in] left0_right1 TRUE indicates that left channel is 0, right is 1
	 */
	SampleSequence(bool left0_right1 = true);

	/**
	 * Rewrap the specified buffers into this sample sequence
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of bytes per buffer
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

	/**
	 * Return the size of the template argument type in bytes.
	 *
	 * It is identical to <tt>sizeof(T)</tt> and was added for debugging.
	 *
	 * \return This of the template argument type in bytes.
	 */
	std::size_t typesize() const;


protected:

	const SampleSequence<T, true> *sequence() const final;


private:

	/**
	 * Internal planar buffer of 16 bit samples for two channels.
	 */
	std::array<const T*, 2> buffer_;

	/**
	 * Number of the left channel
	 */
	const int left_;

	/**
	 * Number of the right channel
	 */
	const int right_;
};


template <typename T>
SampleSequence<T, true>::SampleSequence(bool left0_right1)
	: buffer_()
	, left_ (left0_right1 ? 0 : 1)
	, right_(left0_right1 ? 1 : 0)
{
	// empty
}


template <typename T>
void SampleSequence<T, true>::wrap(const uint8_t * buffer0,
		const uint8_t * buffer1, const uint32_t &size)
{
	buffer_[left_ ] = reinterpret_cast<const T *>(buffer0),
	buffer_[right_] = reinterpret_cast<const T *>(buffer1),
	this->set_size((size * sizeof(uint8_t)) / sizeof(T));
}


template <typename T>
void SampleSequence<T, true>::reset(const T* buffer0, const T* buffer1,
		const uint32_t &size)
{
	buffer_[left_ ] = buffer0;
	buffer_[right_] = buffer1;
	this->set_size(size);
}


template <typename T>
uint32_t SampleSequence<T, true>::operator [] (const uint32_t index) const
{
	return this->combine(buffer_[right_][index], buffer_[left_][index]);
	// This returns 0 == 1.0 | 0.0,  1 == 1.1 | 0.1,  2 == 1.2 | 0.2, ...
	// Equivalent to, but seemingly not slower than:
	//return (static_cast<uint32_t>(buffer_[right_][index]) << 16)
	//	| static_cast<uint16_t>(buffer_[left_][index]);
}


template <typename T>
uint32_t SampleSequence<T, true>::at(const uint32_t index) const
{
	this->bounds_check(index);
	return this->operator[](index);
}


template <typename T>
std::size_t SampleSequence<T, true>::typesize() const
{
	return sizeof(T);
}


template <typename T>
const SampleSequence<T, true>* SampleSequence<T, true>::sequence() const
{
	return this;
}


// SampleSequence: Full Specialization for interleaved sequences


/**
 * SampleSequence specialization for interleaved sequences
 */
template <typename T>
class SampleSequence<T, false> : public SampleSequenceImplBase<T, false>
{

public:

	SampleSequence(const SampleSequence &) = delete;

	SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * Constructor
	 *
	 * \param[in] left0_right1 TRUE indicates that left channel is 0, right is 1
	 */
	SampleSequence(bool left0_right1 = true);

	/**
	 * Rewrap the specified buffers into this sample sequence
	 *
	 * \param[in] buffer Buffer for channel 0
	 * \param[in] size   Number of bytes in buffer
	 */
	void wrap(const uint8_t *buffer, const uint32_t &size);

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
	 * Rewrap the specified buffer into this sample sequence
	 *
	 * \param[in] buffer Interleaved buffer
	 * \param[in] size   Number of T's in the buffer
	 */
	void reset(const T* buffer, const uint32_t &size);

	/**
	 * Return the size of the template argument type in bytes.
	 *
	 * It is identical to <tt>sizeof(T)</tt> and was added for debugging.
	 *
	 * \return This of the template argument type in bytes.
	 */
	std::size_t typesize() const;


protected:

	const SampleSequence<T, false> *sequence() const final;


private:

	/**
	 * Internal interleaved buffer of 16 bit samples for two channels.
	 */
	const T * buffer_;

	/**
	 * Number of the left channel
	 */
	const int left_;

	/**
	 * Number of the right channel
	 */
	const int right_;
};


template <typename T>
SampleSequence<T, false>::SampleSequence(bool left0_right1)
	: buffer_()
	, left_ (left0_right1 ? 0 : 1)
	, right_(left0_right1 ? 1 : 0)
{
	// empty
}


template <typename T>
void SampleSequence<T, false>::wrap(const uint8_t * buffer,
		const uint32_t &size)
{
	buffer_ = reinterpret_cast<const T*>(buffer),
	this->set_size((size * sizeof(uint8_t) / 2 /* channels */ ) / sizeof(T));
}


template <typename T>
void SampleSequence<T, false>::reset(const T* buffer0, const uint32_t &size)
{
	buffer_ = buffer0;
	this->set_size(size / 2 /* channels */);
}


template <typename T>
uint32_t SampleSequence<T, false>::operator [] (const uint32_t index) const
{
	return this->combine(buffer_[2 * index + right_],
			buffer_[2 * index + left_]);
	// This returns 0 = 1|0,  1 = 3|2,  2 = 5|4, ...
	// Equivalent to, but seemingly not slower than:
	//return (static_cast<uint32_t>(buffer_[2 * index + right_]) << 16)
	//	| static_cast<uint16_t>(buffer_[2 * index + left_]);
}


template <typename T>
uint32_t SampleSequence<T, false>::at(const uint32_t index) const
{
	this->bounds_check(index);
	return this->operator[](index);
}


template <typename T>
std::size_t SampleSequence<T, false>::typesize() const
{
	return sizeof(T);
}


template <typename T>
const SampleSequence<T, false>* SampleSequence<T, false>::sequence() const
{
	return this;
}

} // namespace v_1_0_0

} // namespace arcs

#endif

