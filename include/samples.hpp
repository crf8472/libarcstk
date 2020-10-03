#ifndef __LIBARCSTK_SAMPLES_HPP__
#define __LIBARCSTK_SAMPLES_HPP__

/**
 * \file
 *
 * \brief Public API for representing unconverted sequences of samples.
 */

#include <array>                // for array
#include <cstdint>              // for uint8_t, uint32_t
#include <iosfwd>               // for size_t
#include <iterator>             // for input_iterator_tag
#include <stdexcept>            // for out_of_range
#include <sstream>              // for stringstream
#include <type_traits>          // for is_same, conditional, enable_if_t

namespace arcstk
{
inline namespace v_1_0_0
{

/**
 * \addtogroup calc
 * @{
 */

/**
 * \brief Type to represent a 32 bit PCM stereo sample.
 *
 * This should be defined identically-as or at least assignable-to
 * arcstk::sample_type.
 *
 * \see arcstk::sample_type
 */
using sample_t = uint32_t;

/**
 * \brief A sequence of samples represented by signed integral types.
 *
 * A SampleSequence is a compatibility wrapper for passing sample sequences of
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
 *
 * The interface of SampleSequence is very simple: an existing amount of samples
 * is adapted by the use of wrap() or wrap_bytes(). Random access is provided by
 * operator[] (without bounds check) or at() (providing bounds check). A
 * SampleSequence provides also access via iterators.
 *
 * For convenience, this template is not intended to be used directly. Instead,
 * use one of the templates PlanarSamples or InterleavedSamples.
 *
 * \see arcstk::SampleSequence<T, true>
 * \see arcstk::SampleSequence<T, false>
 */
template <typename T, bool is_planar>
class SampleSequence;

/** @} */

namespace details
{

/**
 * \brief Defined iff T is a legal sample type, an integral type of 16 or 32 bit
 */
template <typename T>
using IsSampleType = std::enable_if_t<
				std::is_same<T,  int16_t>::value
			or  std::is_same<T,  int32_t>::value
			or  std::is_same<T, uint16_t>::value
			or  std::is_same<T, uint32_t>::value>;

/**
 * \internal
 *
 * \brief Common code base for SampleSequence specializations.
 *
 * This class is not intended for polymorphic use.
 *
 * \tparam T         Actual sample type
 * \tparam is_planar TRUE indicates a planar sequence, FALSE is interleaved
 */
template<typename T, bool is_planar, typename = IsSampleType<T>>
class SampleSequenceImplBase; // IWYU pragma keep
// forward declaration required by friend delcaration in SampleIterator

} // namespace details


/**
 * \brief An input iterator for samples in SampleSequence instances.
 *
 * Provides a representation of the 16 bit stereo samples for each channel as
 * a single integer of type sample_t.
 *
 * Equality between a \c const_iterator and an \c iterator works as expected.
 *
 * Although tagged as \c input_iterator, SampleIterator provides some additional
 * functionality as there is prefix- and postfix decrement, add-assignment,
 * subtract-assignment, binary add and subtract and a binary subtraction
 * operator for positions.
 */
template <typename T, bool is_planar, bool is_const>
class SampleIterator final
{
	// Befriend the converse version of the type: const_iterator can access
	// private members of iterator (and vice versa)
	friend SampleIterator<T, is_planar, not is_const>;

	// Allow use of private constructor
	friend class details::SampleSequenceImplBase<T, is_planar>;

public:

	SampleIterator& operator = (const SampleIterator &rhs) = delete;


	using iterator_category = std::input_iterator_tag;

	using value_type        = sample_t;

	using difference_type   = std::ptrdiff_t;
	// Must be at least as wide as SampleSequence::size_type

	using pointer           = typename std::conditional<is_const,
			const value_type*, value_type*>::type;

	using reference         = typename std::conditional<is_const,
			const value_type&, value_type&>::type;

	/**
	 * \internal
	 * \brief Construct const_iterator from iterator.
	 *
	 * \param[in] rhs The iterator to construct a const_iterator
	 */
	SampleIterator(const SampleIterator<T, is_planar, false> &rhs)
		: seq_ { rhs.seq_ } // works due to friendship
		, pos_ { rhs.pos_ }
	{
		// empty
	}

	/**
	 * \internal
	 * \brief Dereference operator.
	 *
	 * \return The converted PCM 32 bit sample the iterator points to
	 */
	value_type operator * () const
	{
		return seq_->operator[](static_cast<
			typename SampleSequence<T, is_planar>::size_type>(pos_));
	}

	/**
	 * \internal
	 * \brief Prefix increment operator.
	 */
	SampleIterator& operator ++ ()
	{
		++pos_;
		return *this;
	}

	/**
	 * \internal
	 * \brief Postfix increment operator.
	 */
	SampleIterator operator ++ (int)
	{
		SampleIterator prev_val(*this);
		this->operator++();
		return prev_val;
	}

	/**
	 * \internal
	 * \brief Prefix decrement operator.
	 */
	SampleIterator& operator -- ()
	{
		--pos_;
		return *this;
	}

	/**
	 * \internal
	 * \brief Postfix decrement operator.
	 */
	SampleIterator operator -- (int)
	{
		SampleIterator prev_val(*this);
		this->operator--();
		return prev_val;
	}

	/**
	 * \internal
	 * \brief Add-assign amount.
	 */
	SampleIterator& operator += (const difference_type value)
	{
		pos_ += value;
		return *this;
	}

	/**
	 * \internal
	 * \brief Subtract-assign amount.
	 */
	SampleIterator& operator -= (const difference_type value)
	{
		pos_ -= value;
		return *this;
	}

	/**
	 * \internal
	 * \brief Add amount.
	 *
	 * \param[in] lhs   Iterator to add amount
	 * \param[in] value Amount to add
	 *
	 * \return Result of \c lhs + \c value
	 */
	friend SampleIterator operator + (SampleIterator lhs,
			const difference_type value) noexcept
	{
		lhs.pos_ += value;
		return lhs;
	}

	/**
	 * \internal
	 * \brief Add amount.
	 *
	 * \param[in] value Amount to add
	 * \param[in] rhs   Iterator to add amount
	 *
	 * \return Result of \c value + \c rhs
	 */
	friend SampleIterator operator + (const difference_type value,
			SampleIterator rhs) noexcept
	{
		return rhs + value;
	}

	/**
	 * \internal
	 * \brief Subtract amount.
	 *
	 * \param[in] lhs   Iterator to subtract amount from
	 * \param[in] value Amount to subtract
	 *
	 * \return Result of \c lhs - \c value
	 */
	friend SampleIterator operator - (SampleIterator lhs,
			const difference_type value) noexcept
	{
		lhs.pos_ -= value;
		return lhs;
	}

	/**
	 * \internal
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


	friend bool operator == (const SampleIterator &lhs,
			const SampleIterator &rhs) noexcept
	{
		return lhs.seq_ == rhs.seq_ and lhs.pos_ == rhs.pos_;
	}

	friend bool operator != (const SampleIterator &lhs,
			const SampleIterator &rhs) noexcept
	{
		return not(lhs == rhs);
	}

	friend void swap(SampleIterator &lhs, SampleIterator &rhs)
	{
		using std::swap;

		swap(lhs.seq_, rhs.seq_);
		swap(lhs.pos_, rhs.pos_);
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
	SampleIterator(const SampleSequence<T, is_planar> &seq,
			const difference_type pos)
		: seq_ { &seq }
		, pos_ { pos }
	{
		// empty
	}

	/**
	 * \brief The SampleSequence to iterate.
	 */
	const SampleSequence<T, is_planar> *seq_;

	/**
	 * \brief Current index position.
	 */
	difference_type pos_;
};

namespace details
{

template <typename T, bool is_planar, typename>
class SampleSequenceImplBase
{
public:

	using value_type = sample_t;

	using size_type = std::size_t;

	using iterator = SampleIterator<T, is_planar, false>;

	using const_iterator = SampleIterator<T, is_planar, true>;


	/**
	 * \brief Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	const_iterator cbegin() const
	{
		return const_iterator(*this->sequence(), 0);
	}

	/**
	 * \brief Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	const_iterator cend() const
	{
		return const_iterator(*this->sequence(), static_cast<
				typename const_iterator::difference_type>(this->size()));
	}

	/**
	 * \brief Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	iterator begin()
	{
		return iterator(*this->sequence(), 0);
	}

	/**
	 * \brief Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	iterator end()
	{
		return iterator(*this->sequence(),
			static_cast<typename iterator::difference_type>(this->size()));
	}

	/**
	 * \brief Iterator pointing behind to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	const_iterator begin() const
	{
		return this->cbegin();
	}

	/**
	 * \brief Iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	const_iterator end() const
	{
		return this->cend();
	}

	/**
	 * \brief Return the number of 32 bit PCM samples represented by this
	 * sequence.
	 *
	 * \return The number of 32 bit PCM samples represented by this sequence
	 */
	size_type size() const
	{
		return size_;
	}

	/**
	 * \brief Return the size of the template argument type in bytes.
	 *
	 * It is identical to <tt>sizeof(T)</tt> and was added for debugging.
	 *
	 * \return This of the template argument type in bytes.
	 */
	size_type typesize() const
	{
		return sizeof(T);
	}

protected:

	/**
	 * \brief Protected default constructor.
	 */
	SampleSequenceImplBase()
		: size_ { 0 }
	{
		// empty
	}

	/**
	 * \brief Protected default copy constructor.
	 */
	SampleSequenceImplBase(const SampleSequenceImplBase &)
	= default;

	/**
	 * \brief Protected default copy assignment operator.
	 */
	SampleSequenceImplBase& operator = (const SampleSequenceImplBase &)
	= default;

	/**
	 * \brief Protected default move constructor.
	 */
	SampleSequenceImplBase(SampleSequenceImplBase &&) noexcept
	= default;

	/**
	 * \brief Protected default move assignment operator.
	 */
	SampleSequenceImplBase& operator = (SampleSequenceImplBase &&) noexcept
	= default;

	/**
	 * \brief Protected non-virtual default destructor.
	 */
	~SampleSequenceImplBase() noexcept
	= default;

	/**
	 * \brief Set the number 32 bit PCM samples in this buffer.
	 *
	 * \param[in] size number of 32 bit PCM samples in the buffer
	 */
	void set_size(const size_type size)
	{
		size_ = size;
	}

	/**
	 * \brief Convert two integers to a PCM 32 bit sample.
	 *
	 * \param[in] higher The higher 16 bit
	 * \param[in] lower  The lower 16 bit
	 *
	 * \return A PCM 32 bit sample with the higher and lower bits as passed
	 */
	sample_t combine(const T higher, const T lower) const
	{
		return (static_cast<sample_t>(higher) << 16) |
			(static_cast<sample_t>(lower) & 0x0000FFFF);

		// NOTE: This works because T cannot be anything but only signed or
		// unsigned integers of either 32 or 64 bit length. Those variants can
		// all be handled correctly by just casting them to sample_t.
	}

	/**
	 * \brief Returns amount that \c index exceeds <tt>size() - 1</tt>.
	 *
	 * 0 means that \c index is within legal access bounds.
	 *
	 * \param[in] index Index to check
	 *
	 * \return 0 if not out of bounds, otherwise <tt>index - 1 - size()</tt>.
	 */
	int out_of_range(const size_type index) const
	{
		return index > this->size() ? this->size() - 1 - index : 0;
	}

	/**
	 * \brief Performs bounds check.
	 *
	 * \param[in] index Index to check
	 *
	 * \throws std::out_of_range if \c index is out of legal range
	 */
	void bounds_check(const size_type index) const
	{
		if (this->out_of_range(index))
		{
			auto msg = std::stringstream {};
			msg << "Index out of bounds: " << index
				<< ". Size: " << this->size();

			throw std::out_of_range(msg.str());
		}
	}

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

} // namespace details


// SampleSequence: Full Specialization for planar sequences (is_planar == true)


/**
 * \brief A planar sequence of samples.
 *
 * This class is intended to be used by its alias PlanarSamples<T>.
 *
 * \note
 * Instances of this class are non-copyable but movable.
 *
 * \tparam T Actual sample type
 */
template <typename T>
class SampleSequence<T, true> final :
								public details::SampleSequenceImplBase<T, true>
{
	using Base = typename details::SampleSequenceImplBase<T, true>;

public: /* typedefs */

	using typename Base::value_type;

	using typename Base::size_type;

	using typename Base::iterator;

	using typename Base::const_iterator;


public: /* member functions */

	//SampleSequence(const SampleSequence &) = delete;

	//SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * \brief Constructor.
	 *
	 * Indicates channel ordering left:0, right:1. Equivalent to
	 * SampleSequence(true).
	 */
	SampleSequence()
		: SampleSequence<T, true>(true)
	{
		// empty
	}

	/**
	 * \brief Constructor.
	 *
	 * \param[in] left0_right1 TRUE indicates that left channel is 0, right is 1
	 */
	SampleSequence(bool left0_right1)
		: buffer_ {}
		, left_  { static_cast<size_type>(left0_right1 ? 0 : 1) }
		, right_ { static_cast<size_type>(left0_right1 ? 1 : 0) }
	{
		// empty
	}

	/**
	 * \brief Rewrap the specified buffers into this sample sequence.
	 *
	 * This function does essentially the same as wrap() but converts a
	 * sequence of uint8_t values by reinterpreting the samples as instances
	 * of type T. However, wrap() expects samples of type T.
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of bytes per buffer
	 */
	void wrap_bytes(const uint8_t *buffer0, const uint8_t *buffer1,
			const size_type size)
	{
		buffer_[left_ ] = reinterpret_cast<const T *>(buffer0),
		buffer_[right_] = reinterpret_cast<const T *>(buffer1),
		this->set_size((size * sizeof(uint8_t)) / sizeof(T));
	}

	/**
	 * \brief Rewrap the specified buffers into this sample sequence.
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of T's per buffer
	 */
	void wrap(const T* buffer0, const T* buffer1, const size_type size)
	{
		buffer_[left_ ] = buffer0;
		buffer_[right_] = buffer1;
		this->set_size(size);
	}

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
	value_type operator [] (const size_type index) const
	{
		return this->combine(buffer_[right_][index], buffer_[left_][index]);
		// This returns 0 == 1.0 | 0.0,  1 == 1.1 | 0.1,  2 == 1.2 | 0.2, ...
		// Equivalent to, but seemingly not slower than:
		//return (static_cast<sample_t>(buffer_[right_][index]) << 16)
		//	| static_cast<uint16_t>(buffer_[left_][index]);
	}

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
	value_type at(const size_type index) const
	{
		this->bounds_check(index);
		return this->operator[](index);
	}

protected:

	const SampleSequence<T, true> *sequence() const final
	{
		return this;
	}

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


// SampleSequence: Full Specialization for interleaved sequences


/**
 * \brief An interleaved sequence of samples.
 *
 * This class is intended to be used by its alias InterleavedSamples<T>.
 *
 * \tparam T Actual sample type
 */
template <typename T>
class SampleSequence<T, false> final :
								public details::SampleSequenceImplBase<T, false>
{
	using Base = typename details::SampleSequenceImplBase<T, false>;

public: /* typedefs */

	using typename Base::value_type;

	using typename Base::size_type;

	using typename Base::iterator;

	using typename Base::const_iterator;


public: /* member functions */

	//SampleSequence(const SampleSequence &) = delete;

	//SampleSequence& operator = (const SampleSequence &) = delete;

	/**
	 * \brief Constructor.
	 *
	 * Indicates channel ordering left:0, right:1.
	 *
	 * Equivalent to SampleSequence(true).
	 */
	SampleSequence()
		: SampleSequence<T, false>(true)
	{
		// empty
	}

	/**
	 * \brief Constructor.
	 *
	 * \param[in] left0_right1 TRUE indicates that left channel is 0, right is 1
	 */
	SampleSequence(bool left0_right1)
		: buffer_ {}
		, left_  { static_cast<size_type>(left0_right1 ? 0 : 1) }
		, right_ { static_cast<size_type>(left0_right1 ? 1 : 0) }
	{
		// empty
	}

	/**
	 * \brief Rewrap the specified buffer into this sample sequence.
	 *
	 * This function does essentially the same as wrap() but converts a
	 * sequence of uint8_t instances by reinterpreting the samples as instances
	 * of type T. However, wrap() expects samples of type T.
	 *
	 * \param[in] buffer Buffer for channel 0
	 * \param[in] size   Number of bytes in buffer
	 */
	void wrap_bytes(const uint8_t *buffer, const size_type size)
	{
		buffer_ = reinterpret_cast<const T*>(buffer),
		this->set_size((size * sizeof(uint8_t) / 2/* channels */) / sizeof(T));
	}

	/**
	 * \brief Rewrap the specified buffer into this sample sequence.
	 *
	 * \param[in] buffer Interleaved buffer
	 * \param[in] size   Number of T's in the buffer
	 */
	void wrap(const T* buffer, const size_type size)
	{
		buffer_ = buffer;
		this->set_size(size / 2/* channels */);
	}

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
	value_type operator [] (const size_type index) const
	{
		return this->combine(buffer_[2 * index + right_],
				buffer_[2 * index + left_]);
		// This returns 0 = 1|0,  1 = 3|2,  2 = 5|4, ...
		// Equivalent to, but seemingly not slower than:
		//return (static_cast<sample_t>(buffer_[2 * index + right_]) << 16)
		//	| static_cast<uint16_t>(buffer_[2 * index + left_]);
	}

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
	value_type at(const size_type index) const
	{
		this->bounds_check(index);
		return this->operator[](index);
	}

protected:

	const SampleSequence<T, false> *sequence() const final
	{
		return this;
	}

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
using PlanarSamples = SampleSequence<T, true>;


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
using InterleavedSamples = SampleSequence<T, false>;

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

