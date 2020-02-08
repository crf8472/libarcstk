#ifndef __LIBARCSTK_SAMPLES_HPP__
#error "Do not include samples.tpp, include samples.hpp"
#endif
// vim: ft=cpp
#ifndef __LIBARCSTK_SAMPLES_TPP__
#define __LIBARCSTK_SAMPLES_TPP__

/**
 * \dir details
 *
 * \internal
 *
 * \brief Implementation details
 */

/**
 * \file
 *
 * \internal
 *
 * \brief Implementation of SampleSequence and SampleIterator
 */

#include <array>
#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace arcstk
{
inline namespace v_1_0_0
{


// forward declaration required by friend delcaration in SampleIterator
template<typename T, bool is_planar>
class SampleSequenceImplBase;


/**
 * \internal
 *
 * \brief Iterator of a SampleSequence.
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
			const value_type*, value_type*>::type;

	using reference         = typename std::conditional<is_const,
			const value_type&, value_type&>::type;

	/**
	 * \brief Construct const_iterator from iterator.
	 *
	 * \param[in] rhs The iterator to construct a const_iterator
	 */
	SampleIterator(const SampleIterator<T, is_planar, false> &rhs);

	/**
	 * \brief Dereference operator.
	 *
	 * \return The converted PCM 32 bit sample the iterator points to
	 */
	value_type operator * () const;

	/**
	 * \brief Prefix increment operator.
	 */
	SampleIterator& operator ++ ();

	/**
	 * \brief Postfix increment operator.
	 */
	SampleIterator operator ++ (int);

	/**
	 * \brief Prefix decrement operator.
	 */
	SampleIterator& operator -- ();

	/**
	 * \brief Postfix decrement operator.
	 */
	SampleIterator operator -- (int);

	/**
	 * \brief Add-assign amount.
	 */
	SampleIterator& operator += (const uint32_t value);

	/**
	 * \brief Subtract-assign amount.
	 */
	SampleIterator& operator -= (const uint32_t value);

	/**
	 * \brief Subscript operator.
	 */
	uint32_t operator [] (const uint32_t index) const;

	// Binary non-assignment operators are friends:
	// 1.) Makes the operator a non-member to the class. (Makes type
	// conversion for normal non-member + non-template functions work.)
	// 2.) Makes operator invisible to normal lookup, it is only found by ADL.

	/**
	 * \brief Add amount.
	 *
	 * \param[in] lhs   Iterator to add amount
	 * \param[in] value Amount to add
	 *
	 * \return Result of \c lhs + \c value
	 */
	friend SampleIterator operator + (SampleIterator lhs, const uint32_t value)
		noexcept
	{
		lhs.pos_ += value;
		return lhs;
	}

	/**
	 * \brief Add amount.
	 *
	 * \param[in] value Amount to add
	 * \param[in] rhs   Iterator to add amount
	 *
	 * \return Result of \c value + \c rhs
	 */
	friend SampleIterator operator + (const uint32_t value, SampleIterator rhs)
		noexcept
	{
		return rhs + value;
	}

	/**
	 * \brief Subtract amount.
	 *
	 * \param[in] lhs   Iterator to subtract amount from
	 * \param[in] value Amount to subtract
	 *
	 * \return Result of \c lhs - \c value
	 */
	friend SampleIterator operator - (SampleIterator lhs, const uint32_t value)
		noexcept
	{
		lhs.pos_ -= value;
		return lhs;
	}

	/**
	 * \brief Subtract position.
	 *
	 * \param[in] lhs Iterator to subtract from
	 * \param[in] rhs Iterator to be subtracted
	 *
	 * \return Arithmetical difference between \c lhs and \c rhs
	 */
	friend difference_type operator - (const SampleIterator &lhs,
			const SampleIterator &rhs) noexcept
	{
		return lhs.pos_ - rhs.pos_;
	}

	/**
	 * \brief Equality.
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator == (const SampleIterator &lhs,
			const SampleIterator &rhs) noexcept
	{
		return lhs.seq_ == rhs.seq_ and lhs.pos_ == rhs.pos_;
	}

	/**
	 * \brief Inequality.
	 *
	 * \param[in] lhs Left hand side of the operation
	 * \param[in] rhs Right hand side of the operation
	 *
	 * \return TRUE if lhs equals rhs, otherwise FALSE
	 */
	friend bool operator != (const SampleIterator &lhs,
			const SampleIterator &rhs) noexcept
	{
		return not(lhs == rhs);
	}


private:

	/**
	 * \brief Private Constructor.
	 *
	 * Constructs a SampleIterator for the specified SampleSequence starting
	 * at index \c pos.
	 *
	 * \param[in] seq SampleSequence to iterate
	 * \param[in] pos Start index
	 */
	SampleIterator(const SampleSequence<T, is_planar> &seq, const uint32_t pos);

	/**
	 * \brief The SampleSequence to iterate.
	 */
	const SampleSequence<T, is_planar> *seq_;

	/**
	 * \brief Current index position.
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
 * \internal
 *
 * \brief Common code base for SampleSequence specializations.
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

	using size_type = std::size_t;


public: /* methods */

	/**
	 * \brief Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	iterator begin();

	/**
	 * \brief Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	iterator end();

	/**
	 * \brief Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	const_iterator begin() const;

	/**
	 * \brief Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	const_iterator end() const;

	/**
	 * \brief Return the number of 32 bit PCM samples represented by this
	 * sequence.
	 *
	 * \return The number of 32 bit PCM samples represented by this sequence
	 */
	size_type size() const;


protected:

	/**
	 * \brief Default constructor.
	 */
	SampleSequenceImplBase();

	/**
	 * \brief Protected non-virtual destructor to indicate non-polymorphic use
	 * only.
	 */
	virtual ~SampleSequenceImplBase() noexcept;

	/**
	 * \brief Set the number 32 bit PCM samples in this buffer.
	 *
	 * \param[in] size number of 32 bit PCM samples in the buffer
	 */
	void set_size(const size_type size);

	/**
	 * \brief Convert two integers to a PCM 32 bit sample.
	 *
	 * \param[in] higher The higher 16 bit
	 * \param[in] lower  The lower 16 bit
	 *
	 * \return A PCM 32 bit sample with the higher and lower bits as passed
	 */
	uint32_t combine(const T higher, const T lower) const;

	/**
	 * \brief Returns amount that \c index exceeds <tt>size() - 1</tt>.
	 *
	 * 0 means that \c index is within legal access bounds.
	 *
	 * \param[in] index Index to check
	 *
	 * \return 0 if not out of bounds, otherwise <tt>index - 1 - size()</tt>.
	 */
	int out_of_range(const size_type index) const;

	/**
	 * \brief Performs bounds check.
	 *
	 * \param[in] index Index to check
	 *
	 * \throws std::out_of_range if \c index is out of legal range
	 */
	void bounds_check(const size_type index) const;

	/**
	 * \brief Pointer to actual SampleSequence.
	 */
	virtual const SampleSequence<T, is_planar> *sequence() const
	= 0;


private:

	/**
	 * \brief State: Number of 16 bit samples in this sequence.
	 */
	size_type size_;
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
auto SampleSequenceImplBase<T, is_planar>::size() const
		-> SampleSequenceImplBase<T, is_planar>::size_type
{
	return size_;
}


template<typename T, bool is_planar>
void SampleSequenceImplBase<T, is_planar>::set_size(
		const SampleSequenceImplBase<T, is_planar>::size_type size)
{
	size_ = size;
}


template<typename T, bool is_planar>
uint32_t SampleSequenceImplBase<T, is_planar>::combine(const T higher,
		const T lower) const
{
	return (static_cast<uint32_t>(higher) << 16) |
		(static_cast<uint32_t>(lower) & 0x0000FFFF);

	// NOTE: This works because T cannot be anything but only signed or
	// unsigned integers of either 32 or 64 bit length. Those variants can
	// all be handled correctly by just casting them to uint32_t.
}


template<typename T, bool is_planar>
int SampleSequenceImplBase<T, is_planar>::out_of_range(
		const SampleSequenceImplBase<T, is_planar>::size_type index) const
{
	return index > this->size() ? this->size() - 1 - index : 0;
}


template<typename T, bool is_planar>
void SampleSequenceImplBase<T, is_planar>::bounds_check(
		const SampleSequenceImplBase<T, is_planar>::size_type index) const
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
 * \internal
 *
 * \brief SampleSequence specialization for planar sequences.
 */
template <typename T>
class SampleSequence<T, true> : public SampleSequenceImplBase<T, true>
{
public: /* types */

	using typename SampleSequenceImplBase<T, true>::size_type;


public: /* methods */

	SampleSequence(const SampleSequence &) = delete;

	SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] left0_right1 TRUE indicates that left channel is 0, right is 1
	 */
	SampleSequence(bool left0_right1 = true);

	/**
	 * \brief Rewrap the specified buffers into this sample sequence.
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of bytes per buffer
	 */
	void wrap(const uint8_t *buffer0, const uint8_t *buffer1,
			const size_type size);

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM).
	 *
	 * Bits 31-24: Left Channel MSB
	 * Bits 23-16: Left Channel LSB
	 * Bits 15-09: Right Channel MSB
	 * Bits 08-00: Right Channel LSB
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 */
	uint32_t operator [] (const size_type index) const;

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM).
	 *
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
	uint32_t at(const size_type index) const;

	/**
	 * \brief Rewrap the specified buffers into this sample sequence.
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of T's per buffer
	 */
	void reset(const T* buffer0, const T* buffer1, const size_type size);

	/**
	 * \brief Return the size of the template argument type in bytes.
	 *
	 * It is identical to <tt>sizeof(T)</tt> and was added for debugging.
	 *
	 * \return This of the template argument type in bytes.
	 */
	size_type typesize() const;


protected:

	const SampleSequence<T, true> *sequence() const final;


private:

	/**
	 * \brief Internal planar buffer of 16 bit samples for two channels.
	 */
	std::array<const T*, 2> buffer_;

	/**
	 * \brief Number of the left channel
	 */
	const size_type left_;

	/**
	 * \brief Number of the right channel
	 */
	const size_type right_;
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
		const uint8_t * buffer1, const size_type size)
{
	buffer_[left_ ] = reinterpret_cast<const T *>(buffer0),
	buffer_[right_] = reinterpret_cast<const T *>(buffer1),
	this->set_size((size * sizeof(uint8_t)) / sizeof(T));
}


template <typename T>
void SampleSequence<T, true>::reset(const T* buffer0, const T* buffer1,
		const size_type size)
{
	buffer_[left_ ] = buffer0;
	buffer_[right_] = buffer1;
	this->set_size(size);
}


template <typename T>
uint32_t SampleSequence<T, true>::operator [] (
		const size_type index) const
{
	return this->combine(buffer_[right_][index], buffer_[left_][index]);
	// This returns 0 == 1.0 | 0.0,  1 == 1.1 | 0.1,  2 == 1.2 | 0.2, ...
	// Equivalent to, but seemingly not slower than:
	//return (static_cast<uint32_t>(buffer_[right_][index]) << 16)
	//	| static_cast<uint16_t>(buffer_[left_][index]);
}


template <typename T>
uint32_t SampleSequence<T, true>::at(const size_type index) const
{
	this->bounds_check(index);
	return this->operator[](index);
}


template <typename T>
auto SampleSequence<T, true>::typesize() const
	-> typename SampleSequence<T, true>::size_type
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
 * \internal
 *
 * \brief SampleSequence specialization for interleaved sequences.
 */
template <typename T>
class SampleSequence<T, false> : public SampleSequenceImplBase<T, false>
{
public: /* types */

	using typename SampleSequenceImplBase<T, false>::size_type;


public:

	SampleSequence(const SampleSequence &) = delete;

	SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] left0_right1 TRUE indicates that left channel is 0, right is 1
	 */
	SampleSequence(bool left0_right1 = true);

	/**
	 * \brief Rewrap the specified buffers into this sample sequence.
	 *
	 * \param[in] buffer Buffer for channel 0
	 * \param[in] size   Number of bytes in buffer
	 */
	void wrap(const uint8_t *buffer, const size_type size);

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM).
	 *
	 * Bits 31-24: Left Channel MSB
	 * Bits 23-16: Left Channel LSB
	 * Bits 15-09: Right Channel MSB
	 * Bits 08-00: Right Channel LSB
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 */
	uint32_t operator [] (const size_type index) const;

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM).
	 *
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
	uint32_t at(const size_type index) const;

	/**
	 * \brief Rewrap the specified buffer into this sample sequence.
	 *
	 * \param[in] buffer Interleaved buffer
	 * \param[in] size   Number of T's in the buffer
	 */
	void reset(const T* buffer, const size_type size);

	/**
	 * \brief Return the size of the template argument type in bytes.
	 *
	 * It is identical to <tt>sizeof(T)</tt> and was added for debugging.
	 *
	 * \return This of the template argument type in bytes.
	 */
	size_type typesize() const;


protected:

	const SampleSequence<T, false> *sequence() const final;


private:

	/**
	 * \brief Internal interleaved buffer of 16 bit samples for two channels.
	 */
	const T * buffer_;

	/**
	 * \brief Number of the left channel
	 */
	const size_type left_;

	/**
	 * \brief Number of the right channel
	 */
	const size_type right_;
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
		const size_type size)
{
	buffer_ = reinterpret_cast<const T*>(buffer),
	this->set_size((size * sizeof(uint8_t) / 2 /* channels */ ) / sizeof(T));
}


template <typename T>
void SampleSequence<T, false>::reset(const T* buffer0, const size_type size)
{
	buffer_ = buffer0;
	this->set_size(size / 2 /* channels */);
}


template <typename T>
uint32_t SampleSequence<T, false>::operator [] (const size_type index) const
{
	return this->combine(buffer_[2 * index + right_],
			buffer_[2 * index + left_]);
	// This returns 0 = 1|0,  1 = 3|2,  2 = 5|4, ...
	// Equivalent to, but seemingly not slower than:
	//return (static_cast<uint32_t>(buffer_[2 * index + right_]) << 16)
	//	| static_cast<uint16_t>(buffer_[2 * index + left_]);
}


template <typename T>
uint32_t SampleSequence<T, false>::at(const size_type index) const
{
	this->bounds_check(index);
	return this->operator[](index);
}


template <typename T>
auto SampleSequence<T, false>::typesize() const
	-> typename SampleSequence<T, false>::size_type
{
	return sizeof(T);
}


template <typename T>
const SampleSequence<T, false>* SampleSequence<T, false>::sequence() const
{
	return this;
}

} // namespace v_1_0_0

} // namespace arcstk

#endif

