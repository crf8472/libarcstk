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
 * \internal
 * \brief Type to represent a 32 bit PCM stereo sample.
 *
 * This type must be defined assignable-to arcstk::sample_t. It has therefore
 * at most the size of arcstk::sample_t.
 *
 * \attention
 * Do not use this type in your code, use only arcstk::sample_t.
 *
 * \see arcstk::sample_t
 */
using sample_type = uint32_t;

/**
 * \brief A sequence of samples represented by 16 or 32 bit integral types.
 *
 * Calculation expects an update represented by two
 * iterators that enumerate the audio input as a sequence of 32 bit unsigned
 * integers of which each represents a pair 16-bit stereo PCM samples.
 * SampleSequence is a read-only compatibility wrapper for passing sample
 * buffers of an integral sample format with 16 or 32 bit width to
 * Calculation::update() in this expected update format.
 *
 * The use of a SampleSequence for providing the updates is optional, the caller
 * may decide to provide the required sample format completely without using
 * SampleSequence.
 *
 * When wrapping the original audio data in a SampleSequence, it must be
 * correctly declared as either interleaved or planar. It is furthermore
 * required to know the size of the input and its channel ordering. If no
 * channel ordering is specified, the default is LEFT/RIGHT.
 *
 * A SampleSequence can wrap integer buffers of type T by member function
 * wrap_int_buffer(). Regardless for which T the SampleSequence is instantiated,
 * it will always be able to also wrap byte buffers by function
 * wrap_byte_buffer(). While wrap_int_buffer() expects samples of type T,
 * wrap_byte_buffer() accepts a uint8_t typed buffer and converts those bytes to
 * T.
 *
 * A SampleSequence instance can be reused by just calling wrap_int_buffer() or
 * wrap_byte_buffer() on the existing instance. While channel ordering, size
 * and the actual sample buffers can be changed, it is not possible to correctly
 * wrap samples that are not represented by type T.
 *
 * Random reading access is provided by operator[] (without bounds check) or
 * at() (providing bounds check). A SampleSequence provides also access via
 * iterators.
 *
 * The caller is responsible for the lifetime of the wrapped sample buffer:
 * SampleSequence will only provide a compatibility layer, it will not erase
 * the wrapped buffers. A SampleSequence can therefore safely be destroyed
 * without affecting the wrapped buffer.
 *
 * \attention
 * For convenience, this template is not intended to be used directly. Instead,
 * use one of the templates PlanarSamples or InterleavedSamples.
 *
 * \tparam T          The sample type to read
 * \tparam is_planar  \c TRUE indicates two planar buffers while \c FALSE
 *                    indicates an interleaved buffer
 *
 * \see PlanarSamples
 * \see InterleavedSamples
 */
template <typename T, bool is_planar>
class SampleSequence;

namespace details
{

/**
 * \internal
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
 * \internal
 * \brief An \c input_iterator for samples in SampleSequence instances.
 *
 * Provides a representation of the 16 bit stereo samples for each channel as
 * a single integer of an unsigned integer type assignable to \c sample_t.
 *
 * Equality between a \c const_iterator and an \c iterator works as expected.
 *
 * Although tagged as \c input_iterator, SampleIterator provides some additional
 * functionality as there is
 * <ul>
 *   <li>prefix- and postfix decrement,</li>
 *   <li>operators add-assign (+=) and subtract-assign (-=),</li>
 *   <li>binary operators for addition and substraction of values, and</li>
 *   <li>binary operators for addition and substraction of positions.</li>
 * </ul>
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

	SampleIterator& operator = (const SampleIterator &rhs) = default;
	// User-declare this to avoid -Wdeprecated-copy firing


	using iterator_category = std::bidirectional_iterator_tag;

	using value_type        = sample_type;

	using difference_type   = std::ptrdiff_t;
	// Must be at least as wide as SampleSequence::size_type

	using pointer           = const value_type*;

	using reference         = const value_type&;

	/**
	 * \brief Default constructor.
	 *
	 * Inititalizes a SampleIterator pointing to \c nullptr on position \c 0.
	 */
	SampleIterator()
		: seq_ { nullptr }
		, pos_ { 0 }
	{
		// empty
	}

	/**
	 * \internal
	 * \brief Construct a constant SampleIterator from a non-constant
	 * SampleIterator.
	 *
	 * \param[in] rhs The non-constant SampleIterator
	 */
	SampleIterator(const SampleIterator<T, is_planar, false> &rhs)
		: seq_ { rhs.seq_ } // works due to friendship
		, pos_ { rhs.pos_ }
	{
		// empty
	}

	/**
	 * \internal
	 * \brief Return a pointer to the SampleSequence.
	 *
	 * Pointer to the SampleSequence the iterator relates to.
	 *
	 * \return A pointer to the SampleSequence.
	 */
	const SampleSequence<T, is_planar>* sequence() const
	{
		return seq_;
	}

	/**
	 * \internal
	 * \brief Return the iterator position.
	 *
	 * Beginning of the sequence is 0 and a legal position is a positive integer
	 * that is smaller than the size() of the sequence.
	 *
	 * \return Iterator position relative to the beginning.
	 */
	difference_type pos() const
	{
		return pos_;
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
	 * \brief Private constructor.
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

/**
 * \internal
 * \brief Abstract base class for SampleSequences.
 *
 * \details
 *
 * Provides the iterators, size(), bounds check and a service method to
 * combine to 16bit integer to a 32bit integer.
 */
template <typename T, bool is_planar, typename>
class SampleSequenceImplBase
{
public:

	using value_type = sample_type;

	using size_type = std::size_t;

	using iterator = SampleIterator<T, is_planar, false>;

	using const_iterator = SampleIterator<T, is_planar, true>;


	/**
	 * \brief Obtain an iterator pointing to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	const_iterator cbegin() const
	{
		return const_iterator(*this->sequence(), 0);
	}

	/**
	 * \brief Obtain an iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	const_iterator cend() const
	{
		return const_iterator(*this->sequence(), static_cast<
				typename const_iterator::difference_type>(this->size()));
	}

	/**
	 * \brief Obtain an iterator pointing to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	iterator begin()
	{
		return iterator(*this->sequence(), 0);
	}

	/**
	 * \brief Obtain an iterator pointing behind the end.
	 *
	 * \return Iterator pointing behind the end of the SampleSequence
	 */
	iterator end()
	{
		return iterator(*this->sequence(),
			static_cast<typename iterator::difference_type>(this->size()));
	}

	/**
	 * \brief Obtain an iterator pointing to the beginning.
	 *
	 * \return Iterator pointing to the beginning of the SampleSequence
	 */
	const_iterator begin() const
	{
		return this->cbegin();
	}

	/**
	 * \brief Obtain an iterator pointing behind the end.
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
	 * \brief Get number of left channel (0 or 1).
	 *
	 * \return Number of the left channel
	 */
	size_type left_channel() const
	{
		return left_;
	}

	/**
	 * \brief Get number of right channel (0 or 1).
	 *
	 * \return Number of the right channel
	 */
	size_type right_channel() const
	{
		return right_;
	}

	/**
	 * \brief Get channel ordering flag, \c TRUE means left is 0 and right is 1.
	 *
	 * \return Channel ordering flag.
	 */
	bool channel_ordering() const
	{
		return left_ == 0 and right_ == 1;
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
	 *
	 * Establishes a size of 0 and a LEFT_RIGHT channel ordering.
	 */
	SampleSequenceImplBase()
		: SampleSequenceImplBase{ true }
	{
		// empty
	}

	/**
	 * \brief Protected constructor.
	 *
	 * \param[in] left0_right1 Channel ordering
	 */
	SampleSequenceImplBase(const bool left0_right1)
		: size_  { 0 }
		, left_  { get_left_channel(left0_right1)  }
		, right_ { get_right_channel(left0_right1) }
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
	 * \brief Combine two 16 bit integers to a PCM 32 bit sample.
	 *
	 * \param[in] higher The higher 16 bit
	 * \param[in] lower  The lower 16 bit
	 *
	 * \return A PCM 32 bit sample with the higher and lower bits as passed
	 */
	sample_type combine(const T higher, const T lower) const
	{
		return (static_cast<sample_type>(higher) << 16) |
			(static_cast<sample_type>(lower) & 0x0000FFFF);

		// NOTE: This works because T cannot be anything but only signed or
		// unsigned integers of either 32 or 64 bit length. Those variants can
		// all be handled correctly by just casting them to sample_type.
	}

	/**
	 * \brief Return amount that \c index exceeds <tt>size() - 1</tt>.
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
	 * \brief Perform bounds check.
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
	 * \brief Obtain the number of the left channel from the ordering flag.
	 *
	 * \param[in] channel_ordering The channel ordering flag
	 *
	 * \return 0 in case of \c TRUE, otherwise 1
	 */
	size_type get_left_channel(const bool channel_ordering) const
	{
		return channel_ordering ? 0 : 1;
	}

	/**
	 * \brief Obtain the number of the right channel from the ordering flag.
	 *
	 * \param[in] channel_ordering The channel ordering flag
	 *
	 * \return 1 in case of \c TRUE, otherwise 0
	 */
	size_type get_right_channel(const bool channel_ordering) const
	{
		return channel_ordering ? 1 : 0;
	}

	/**
	 * \brief Set the channel ordering according to the ordering flag.
	 *
	 * \param[in] channel_ordering The channel ordering flag
	 */
	void set_channel_ordering(const bool channel_ordering)
	{
		left_  = this->get_left_channel(channel_ordering);
		right_ = this->get_right_channel(channel_ordering);
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

	/**
	 * \brief State: Number of the left channel
	 */
	size_type left_;

	/**
	 * \brief State: Number of the right channel
	 */
	size_type right_;
};

} // namespace details


// SampleSequence: Full Specialization for planar sequences (is_planar == true)


/**
 * \internal
 * \brief A planar sequence of samples.
 *
 * \details
 *
 * This class is intended to be used by its alias PlanarSamples<T>.
 *
 * SampleSequence<T,true> is movable but not copyable.
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

	//Skip copy-ctor   SampleSequence(const SampleSequence &)

	//Skip copy-asgnmt SampleSequence& operator = (const SampleSequence &)

	/**
	 * \brief Constructor for a sequence with specified channel ordering.
	 *
	 * \param[in] left0_right1 The channel ordering
	 */
	SampleSequence(const bool left0_right1)
		: Base(left0_right1)
		, buffer_ {}
	{
		// empty
	}

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
	 * Channel ordering: \c TRUE indicates that left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer0      Buffer for channel 0
	 * \param[in] buffer1      Buffer for channel 1
	 * \param[in] size         Number of T's per buffer
	 * \param[in] left0_right1 Channel ordering
	 */
	SampleSequence(const T* buffer0, const T* buffer1, const size_type size,
			const bool left0_right1)
		: Base(left0_right1)
		, buffer_ {}
	{
		this->wrap_int_buffer(buffer0, buffer1, size, left0_right1);
	}

	/**
	 * \brief Constructor.
	 *
	 * Uses default channel ordering in which left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer0      Buffer for channel 0
	 * \param[in] buffer1      Buffer for channel 1
	 * \param[in] size         Number of T's per buffer
	 */
	SampleSequence(const T* buffer0, const T* buffer1, const size_type size)
		: Base(true)
		, buffer_ {}
	{
		this->wrap_int_buffer(buffer0, buffer1, size, true);
	}

	/**
	 * \brief Constructor.
	 *
	 * Channel ordering: \c TRUE indicates that left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer0      Buffer for channel 0
	 * \param[in] buffer1      Buffer for channel 1
	 * \param[in] size         Number of bytes per buffer
	 * \param[in] left0_right1 Channel ordering
	 */
	SampleSequence(const uint8_t *buffer0, const uint8_t *buffer1,
			const size_type size, const bool left0_right1)
		: Base(left0_right1)
		, buffer_ {}
	{
		this->wrap_byte_buffer(buffer0, buffer1, size, left0_right1);
	}

	/**
	 * \brief Constructor.
	 *
	 * Uses default channel ordering in which left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer0      Buffer for channel 0
	 * \param[in] buffer1      Buffer for channel 1
	 * \param[in] size         Number of bytes per buffer
	 */
	SampleSequence(const uint8_t *buffer0, const uint8_t *buffer1,
			const size_type size)
		: Base(true)
		, buffer_ {}
	{
		this->wrap_byte_buffer(buffer0, buffer1, size, true);
	}

	/**
	 * \brief Rewrap the specified integer buffers into this sample sequence.
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of T's per buffer
	 * \param[in] left0_right1 Channel ordering
	 */
	void wrap_int_buffer(const T* buffer0, const T* buffer1,
			const size_type size, const bool left0_right1)
	{
		buffer_[this->left_channel()]  = buffer0;
		buffer_[this->right_channel()] = buffer1;

		this->set_size(size);
		this->set_channel_ordering(left0_right1);
	}

	/**
	 * \brief Rewrap the specified integer buffers into this sample sequence.
	 *
	 * The current channel ordering is used.
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of T's per buffer
	 */
	void wrap_int_buffer(const T* buffer0, const T* buffer1,
			const size_type size)
	{
		this->wrap_int_buffer(buffer0, buffer1, size, this->channel_ordering());
	}

	/**
	 * \brief Rewrap the specified byte buffers into this sample sequence.
	 *
	 * This function does essentially the same as wrap_int_buffer() but converts
	 * a sequence of uint8_t instances by reinterpreting the samples as
	 * instances of type T. However, wrap_byte_buffer() expects bytes instead of
	 * samples of type T.
	 *
	 * \param[in] buffer0      Buffer for channel 0
	 * \param[in] buffer1      Buffer for channel 1
	 * \param[in] size         Number of bytes per buffer
	 * \param[in] left0_right1 Channel ordering
	 */
	void wrap_byte_buffer(const uint8_t *buffer0, const uint8_t *buffer1,
			const size_type size, const bool left0_right1)
	{
		buffer_[this->left_channel()]  = reinterpret_cast<const T *>(buffer0),
		buffer_[this->right_channel()] = reinterpret_cast<const T *>(buffer1),

		this->set_size((size * sizeof(uint8_t)) / sizeof(T));
		this->set_channel_ordering(left0_right1);
	}

	/**
	 * \brief Rewrap the specified byte buffers into this sample sequence.
	 *
	 * This function does essentially the same as wrap_int_buffer() but converts
	 * a sequence of uint8_t instances by reinterpreting the samples as
	 * instances of type T. However, wrap_byte_buffer() expects bytes instead of
	 * samples of type T.
	 *
	 * The current channel ordering is used.
	 *
	 * \param[in] buffer0 Buffer for channel 0
	 * \param[in] buffer1 Buffer for channel 1
	 * \param[in] size    Number of bytes per buffer
	 */
	void wrap_byte_buffer(const uint8_t *buffer0, const uint8_t *buffer1,
			const size_type size)
	{
		this->wrap_byte_buffer(buffer0, buffer1, size,
				this->channel_ordering());
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
		return this->combine(buffer_[this->right_channel()][index],
				buffer_[this->left_channel()][index]);
		// This returns 0 == 1.0 | 0.0,  1 == 1.1 | 0.1,  2 == 1.2 | 0.2, ...

		// NOTE:
		// Equivalent to, but seemingly not slower than:
		//return (static_cast<sample_type>(buffer_[right_][index]) << 16)
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
};


// SampleSequence: Full Specialization for interleaved sequences


/**
 * \internal
 * \brief An interleaved sequence of samples.
 *
 * This class is intended to be used by its alias InterleavedSamples<T>.
 *
 * SampleSequence<T,false> is movable but not copyable.
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

	//Skip copy-ctor   SampleSequence(const SampleSequence &)

	//Skip copy-asgnmt SampleSequence& operator = (const SampleSequence &)

	/**
	 * \brief Constructor for a sequence with specified channel ordering.
	 *
	 * \param[in] left0_right1 The channel ordering
	 */
	SampleSequence(const bool left0_right1)
		: Base(left0_right1)
		, buffer_ { nullptr }
	{
		// empty
	}

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
	 * Channel ordering: \c TRUE indicates that left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer       Sample buffer
	 * \param[in] size         Number of T's per buffer
	 * \param[in] left0_right1 Channel ordering
	 */
	SampleSequence(const T* buffer, const size_type size,
			const bool left0_right1)
		: Base(left0_right1)
		, buffer_ { nullptr }
	{
		this->wrap_int_buffer(buffer, size, left0_right1);
	}

	/**
	 * \brief Constructor.
	 *
	 * Uses default channel ordering in which left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer       Sample buffer
	 * \param[in] size         Number of T's per buffer
	 */
	SampleSequence(const T* buffer, const size_type size)
		: Base(true)
		, buffer_ { nullptr }
	{
		this->wrap_int_buffer(buffer, size, true);
	}

	/**
	 * \brief Constructor.
	 *
	 * Channel ordering: \c TRUE indicates that left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer       Sample buffer
	 * \param[in] size         Number of bytes per buffer
	 * \param[in] left0_right1 Channel ordering
	 */
	SampleSequence(const uint8_t *buffer, const size_type size,
			const bool left0_right1)
		: Base(left0_right1)
		, buffer_ { nullptr }
	{
		this->wrap_byte_buffer(buffer, size, left0_right1);
	}

	/**
	 * \brief Constructor.
	 *
	 * Uses default channel ordering in which left channel is 0, right channel
	 * is 1.
	 *
	 * \param[in] buffer       Sample buffer
	 * \param[in] size         Number of bytes per buffer
	 */
	SampleSequence(const uint8_t *buffer, const size_type size)
		: Base(true)
		, buffer_ { nullptr }
	{
		this->wrap_byte_buffer(buffer, size, true);
	}

	/**
	 * \brief Rewrap the specified typed buffer into this sample sequence.
	 *
	 * Wraps the specified buffer, that must be of the same type as the
	 * specified sample sequence.
	 *
	 * \param[in] buffer       Interleaved buffer
	 * \param[in] size         Number of T's in the buffer
	 * \param[in] left0_right1 \c TRUE for left == 0, right == 1, otherwise
	 *                         \c FALSE
	 */
	void wrap_int_buffer(const T* buffer, const size_type size,
			bool left0_right1)
	{
		buffer_ = buffer;

		this->set_size(size / 2/* channels */);
		this->set_channel_ordering(left0_right1);
	}

	/**
	 * \brief Rewrap the specified typed buffer into this sample sequence.
	 *
	 * Wraps the specified buffer, that must be of the same type as the
	 * specified sample sequence.
	 *
	 * The current channel ordering is reused.
	 *
	 * \param[in] buffer       Interleaved buffer
	 * \param[in] size         Number of T's in the buffer
	 */
	void wrap_int_buffer(const T *buffer, const size_type size)
	{
		this->wrap_int_buffer(buffer, size, this->channel_ordering());
	}

	/**
	 * \brief Rewrap the specified raw byte buffer into this sample sequence.
	 *
	 * This function does essentially the same as wrap_int_buffer() but converts
	 * a sequence of uint8_t instances by reinterpreting the samples as
	 * instances of type T. However, wrap_byte_buffer() expects bytes instead of
	 * samples of type T.
	 *
	 * \param[in] buffer       Buffer for channel 0
	 * \param[in] size         Number of bytes in buffer
	 * \param[in] left0_right1 \c TRUE for left == 0, right == 1, otherwise
	 *                         \c FALSE
	 */
	void wrap_byte_buffer(const uint8_t *buffer, const size_type size,
			bool left0_right1)
	{
		buffer_ = reinterpret_cast<const T*>(buffer);

		this->set_size((size * sizeof(uint8_t) / 2/* channels */) / sizeof(T));
		this->set_channel_ordering(left0_right1);
	}

	/**
	 * \brief Rewrap the specified raw byte buffer into this sample sequence.
	 *
	 * This function does essentially the same as wrap_int_buffer() but converts
	 * a sequence of uint8_t instances by reinterpreting the samples as
	 * instances of type T. However, wrap_byte_buffer() expects bytes instead of
	 * samples of type T.
	 *
	 * The current channel ordering is reused.
	 *
	 * \param[in] buffer       Buffer for channel 0
	 * \param[in] size         Number of bytes in buffer
	 */
	void wrap_byte_buffer(const uint8_t *buffer, const size_type size)
	{
		this->wrap_byte_buffer(buffer, size, this->channel_ordering());
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
		return this->combine(buffer_[2 * index + this->right_channel()],
				buffer_[2 * index + this->left_channel()]);
		// This returns 0 = 1|0,  1 = 3|2,  2 = 5|4, ...

		// NOTE:
		// Equivalent to, but seemingly not slower than:
		//return (static_cast<sample_type>(buffer_[2 * index + right_]) << 16)
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
};

/**
 * \brief Planar sample sequence with samples of type T.
 *
 * A planar sequence has two separated input buffers, one for each channel.
 *
 * T can only be some signed or unsigned integral type of either 16 or 32 bit
 * width.
 *
 * \see SampleSequence
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
 *
 * \see SampleSequence
 */
template <typename T>
using InterleavedSamples = SampleSequence<T, false>;

/** @} */

} // namespace v_1_0_0

} // namespace arcstk

#endif

