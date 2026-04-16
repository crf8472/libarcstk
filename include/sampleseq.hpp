#ifndef LIBARCSTK_SAMPLESEQ_HPP_
#define LIBARCSTK_SAMPLESEQ_HPP_

/**
 * \file
 *
 * \brief Represent and manage unconverted \link calc sequences of
 * samples\endlink.
 *
 * \details
 *
 * Part of the API for \link calc calculating AccurateRip checksums\endlink.
 */

#include <array>                // for array
#include <cstddef>              // for ptrdiff_t, size_t
#include <cstdint>              // for int16_t, int32_t, uint8_t, uint32_t,...
#include <iterator>             // for bidirectional_iterator_tag
#include <type_traits>          // for is_same

#ifndef LIBARCSTK_BYTES_HPP_
#include "bytes.hpp"            // for is_sample_type, combine
#endif

#ifndef LIBARCSTK_MIXINS_HPP_
#include "mixins.hpp"           // for Comparable
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

/**
 * \addtogroup calc
 * @{
 */

// redefined as in calculate.hpp, documented there
using sample_t = uint32_t;


namespace details
{

/**
 * \brief Template: sequence of samples of an integral type of 16 or 32 bit.
 *
 * Calculation expects an update represented by two iterators that enumerate the
 * audio input as a sequence of 32 bit unsigned integers of which each
 * represents a pair of 16-bit stereo PCM samples.
 * SampleSequence is a read-only compatibility wrapper for passing sample
 * buffers of an integral sample format with 16 or 32 bit width to
 * Calculation::update() in the appropriate update format.
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
template<typename T, bool is_planar>
class SampleSequence; // IWYU pragma keep
// forward declaration required by friend delcaration in SampleIterator

/**
 * \brief An \c input_iterator for samples in SampleSequence instances.
 *
 * Provides a representation of the 16 bit stereo samples pair for each channel
 * as a single 32 bit integer of an unsigned integer type assignable to
 * \c sample_t.
 *
 * SampleIterator provides the following functionality:
 * - swappable,
 * - prefix- and postfix decrement,
 * - operator == and !=
 * - operator add-assign (+=)
 * - binary operators + for addition of positions.
 */
template <typename T, bool is_planar>
class SampleIterator final : Comparable<SampleIterator<T, is_planar>>
{
	// Allow use of private constructor
	friend class details::SampleSequence<T, is_planar>;

public:

	/**
	 * \brief LegacyInputIterator
	 *
	 * See <A HREF="https://en.cppreference.com/w/cpp/named_req/InputIterator">
	 * LegacyInputIterator</A>
	 */
	using iterator_category = std::input_iterator_tag;

	/**
	 * \copydoc SNPT_tp_value
	 */
	using value_type        = sample_t;

	/**
	 * \copydoc SNPT_tp_reference
	 *
	 * \details Not an actual reference type.
	 */
	using reference         = value_type;

	/**
	 * \copydoc SNPT_tp_pointer
	 */
	using pointer           = const value_type*;

	/**
	 * \copydoc SNPT_tp_difference
	 */
	using difference_type   = std::ptrdiff_t;
	// Must be at least as wide as SampleSequence::size_type

	/**
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
	 * \copydoc SNPT_mf_deref
	 */
	reference operator * () const
	{
		using index_type = typename SampleSequence<T, is_planar>::size_type;

		return seq_->operator[](static_cast<index_type>(pos_));
	}

	/**
	 * \copydoc SNPT_mf_inc_prefix
	 */
	SampleIterator& operator ++ ()
	{
		++pos_;
		return *this;
	}

	/**
	 * \copydoc SNPT_mf_inc_postfix
	 */
	SampleIterator operator ++ (int)
	{
		auto prev_val = SampleIterator { *this };
		this->operator++();
		return prev_val;
	}

	/**
	 * \copydoc SNPT_mf_inc_amount
	 */
	SampleIterator& operator += (const difference_type amount)
	{
		pos_ += amount;
		return *this;
	}

	/**
	 * \copydoc SNPT_nf_inc_amount_lhs
	 */
	friend SampleIterator operator + (SampleIterator lhs,
			const difference_type amount) noexcept
	{
		lhs += amount;
		return lhs;
	}

	/**
	 * \copydoc SNPT_nf_inc_amount_rhs
	 */
	friend SampleIterator operator + (const difference_type amount,
			SampleIterator rhs) noexcept
	{
		return rhs + amount;
	}

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(SampleIterator& lhs, SampleIterator& rhs) noexcept
	{
		using std::swap;

		swap(lhs.seq_,   rhs.seq_);
		swap(lhs.pos_,   rhs.pos_);
	}

	/**
	 * \copydoc SNPT_nf_equality
	 */
	friend bool operator == (const SampleIterator& lhs,
			const SampleIterator& rhs) noexcept
	{
		return lhs.seq_ == rhs.seq_ && lhs.pos_ == rhs.pos_;
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
	SampleIterator(const SampleSequence<T, is_planar>& seq,
			const difference_type pos)
		: seq_   { &seq }
		, pos_   { pos }
	{
		// empty
	}

	/**
	 * \brief The SampleSequence to iterate.
	 */
	const SampleSequence<T, is_planar>* seq_;

	/**
	 * \brief Current index position.
	 */
	difference_type pos_;
};

/**
 * \brief Interpret a buffer as of another type.
 *
 * Transparent if types are identical.
 *
 * \tparam T  The target/output type
 * \tparam U  The original type of the buffer
 *
 * \param[in] buffer_ptr Buffer to cast
 *
 * \return buffer converted to sample type
 */
template <typename T, typename U = T>
auto get_buffer(const U* buffer_ptr) -> const T*
{
	if constexpr (std::is_same_v<T, U>)
	{
		return buffer_ptr;
	}
	return reinterpret_cast<const T*>(buffer_ptr);
}

/**
 * \brief Provide adopted size, which is the total number virtual values.
 *
 * \param[in] size Total number of Us
 *
 * \return Total number of Ts
 */
template <typename T, typename U = T>
auto adopt_size(const std::size_t size) -> std::size_t
{
	return (size * sizeof(U)) / sizeof(T);
}

/**
 * \brief Reference values for indices for swapped an non-swapped channels.
 */
struct channel final
{
	/**
	 * \brief Return left channel index.
	 *
	 * \param[in] is_swapped If TRUE, return left channel for swapped layout
	 *
	 * \return 1 iff \c is_swapped, otherwise 0
	 */
	static std::size_t left(const bool is_swapped)
	{
		return is_swapped ? 1 : 0;
	}

	/**
	 * \brief Return right channel index.
	 *
	 * \param[in] is_swapped If TRUE, return right channel for swapped layout
	 *
	 * \return 0 iff \c is_swapped, otherwise 1
	 */
	static std::size_t right(const bool is_swapped)
	{
		return ! left(is_swapped);
	}
};


/**
 * \brief Common part of all buffers.
 *
 * - Do not use pointers of this classes type!
 * - Do not inherit from this class!
 */
class BufferBase
{
	/**
	 * \brief Internal number of 16 bit sample pairs in this buffer.
	 */
	std::size_t size_;

	/**
	 * \brief Internal index of the left channel.
	 */
	std::size_t left_;

	/**
	 * \brief Internal index of the right channel.
	 */
	std::size_t right_;

public:

	/**
	 * \brief Total number of 16 bit sample pairs in this buffer.
	 *
	 * \return Total number of 16 bit sample pairs in this buffer
	 */
	std::size_t size() const noexcept
	{
		return size_;
	}

	/**
	 * \brief Index of the left channel.
	 *
	 * \return Index of the left channel.
	 */
	std::size_t left() const noexcept
	{
		return left_;
	}

	/**
	 * \brief Index of the right channel.
	 *
	 * \return Index of the right channel.
	 */
	std::size_t right() const noexcept
	{
		return right_;
	}

	/**
	 * \brief TRUE indicates channels are swapped, FALSE is not swapped.
	 *
	 * \return TRUE for swapped channels, FALSE for default channel order
	 */
	bool channels_swapped() const noexcept
	{
		return left();
	}

protected:

	/**
	 * \brief Constructor.
	 *
	 * \param[in] size Size in total number of 16 bit sample pairs
	 * \param[in] channels_swapped TRUE for swapped, FALSE for not swapped
	 */
	BufferBase(const std::size_t size, const bool channels_swapped)
		: size_  { size }
		, left_  { channel::left (channels_swapped) }
		, right_ { channel::right(channels_swapped) }
	{
		// empty
	}
};


/**
 * \brief Wrap an existing sample buffer for abstraction of access.
 *
 * \tparam T The type to interpret the buffer
 *
 * T can only be some signed or unsigned integral type of either 16 or 32 bit
 * width.
 */
template <typename T, bool is_planar>
class SampleBufferWrapper
{
	// empty
};


// full specialization, part TRUE
template <typename T>
class SampleBufferWrapper<T, true/* PLANAR */> final : public BufferBase
{
public:

	/**
	 * \copydoc SNPT_tp_value
	 */
	using value_type = T;

	/**
	 * \copydoc SNPT_tp_size
	 */
	using size_type  = std::size_t;

private:

	/**
	 * \brief Internal planar buffer of 16 bit samples for two channels.
	 */
	std::array<const T*, 2> buffer_;

public:

	/**
	 * \brief Constructor.
	 *
	 * \tparam U Input type
	 *
	 * U is the input type while T is the type as which we interpret the input.
	 * Hence, U may be uint8_t but T may be int16_t.
	 */
	template <typename U = T>
	SampleBufferWrapper(const U* buffer0, const U* buffer1,
			const size_type size, const bool channels_swapped)
		: BufferBase { adopt_size<T,U>(size), channels_swapped }
		, buffer_    { { get_buffer<T>(buffer0), get_buffer<T>(buffer1) } }
	{
		static_assert(details::supported_sample_type<T>,
				"Type T must be a supported_sample_type");
	}

	/**
	 * \brief Virtual value under index \c index from left channel.
	 *
	 * \param[in] index Virtual index to access
	 *
	 * \return Virtual value from index \c index
	 */
	auto left_channel(const size_type index) const -> T
	{
		return buffer_[left()][index];
	}

	/**
	 * \brief Virtual value under index \c index from right channel.
	 *
	 * \param[in] index Virtual index to access
	 *
	 * \return Virtual value from index \c index
	 */
	auto right_channel(const size_type index) const -> T
	{
		return buffer_[right()][index];
	}

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM).
	 *
	 * - Bits 31-24: Left Channel MSB
	 * - Bits 23-16: Left Channel LSB
	 * - Bits 15-09: Right Channel MSB
	 * - Bits 08-00: Right Channel LSB
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 */
	sample_t operator [] (const size_type index) const
	{
		// This returns 0 == 1.0 | 0.0,  1 == 1.1 | 0.1,  2 == 1.2 | 0.2, ...
		return combine(right_channel(index), left_channel(index));
	}
};


// full specialization, part FALSE
template <typename T>
class SampleBufferWrapper<T, false/* INTERLEAVED */> final : public BufferBase
{
public:

	/**
	 * \copydoc SNPT_tp_value
	 */
	using value_type = T;

	/**
	 * \copydoc SNPT_tp_size
	 */
	using size_type  = std::size_t;

private:

	/**
	 * \brief Internal interleaved buffer of 16 bit samples for two channels.
	 */
	const T* buffer_;

public:

	/**
	 * \brief Constructor.
	 *
	 * Channel ordering: \c TRUE indicates that left channel is 1, right channel
	 * is 0 (channels are swapped).
	 *
	 * \param[in] buffer           Sample buffer
	 * \param[in] size             Physical size of buffer
	 * \param[in] channels_swapped Channel ordering
	 */
	template <typename U>
	SampleBufferWrapper(const U* buffer, const size_type size,
			const bool channels_swapped)
		: BufferBase { adopt_size<T,U>(size) / 2, channels_swapped }
		, buffer_    { get_buffer<T>(buffer) }
	{
		static_assert(details::supported_sample_type<T>,
				"Type T must be a supported_sample_type");
	}

	/**
	 * \brief Virtual value under index \c index from left channel.
	 *
	 * \param[in] index Virtual index to access
	 *
	 * \return Virtual value from index \c index
	 */
	auto left_channel(const size_type index) const -> T
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		return buffer_[left() + 2 * index];
	}

	/**
	 * \brief Virtual value under index \c index from right channel.
	 *
	 * \param[in] index Virtual index to access
	 *
	 * \return Virtual value from index \c index
	 */
	auto right_channel(const size_type index) const -> T
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		return buffer_[right() + 2 * index];
	}

	/**
	 * \brief Provides access to the samples in a uniform format (32 bit PCM).
	 *
	 * \param[in] index Index of a virtual 32 bit PCM sample
	 *
	 * \return The sample value of the virtual 32 bit PCM sample
	 */
	sample_t operator [] (const size_type index) const
	{
		// This returns 0 = 1|0,  1 = 3|2,  2 = 5|4, ...
		return combine(right_channel(index), left_channel(index));
	}
};


// documented above
template<typename T, bool is_planar>
class SampleSequence
{
	/**
	 * \brief Type of the internal sample buffer.
	 */
	using buffer_type = SampleBufferWrapper<T, is_planar>;

	/**
	 * \brief Internal sample buffer.
	 */
	buffer_type buffer_;

public:

	/**
	 * \brief SNPT_tp_size.
	 */
	using size_type      = typename buffer_type::size_type;

	/**
	 * \brief Unspecified forward iterator type.
	 */
	using iterator       = SampleIterator<T, is_planar>;

	/**
	 * \brief Unspecified constant forward iterator type.
	 */
	using const_iterator = SampleIterator<T, is_planar>;

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator cbegin() const
	{
		return const_iterator { *this, 0 };
	}

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator cend() const
	{
		using diff_t = typename const_iterator::difference_type;

		return const_iterator { *this, static_cast<diff_t>(this->size()) };
	}

	/**
	 * \copydoc SNPT_mf_cbegin
	 */
	const_iterator begin() const
	{
		return this->cbegin();
	}

	/**
	 * \copydoc SNPT_mf_cend
	 */
	const_iterator end() const
	{
		return this->cend();
	}

	/**
	 * \brief Total number of virtual values in this sequence.
	 *
	 * \return The number of virtual values in this sequence
	 */
	size_type size() const
	{
		return buffer_.size();
	}

	// some info about the sequence

	/**
	 * \brief TRUE means LEFT/RIGHT, FALSE means RIGHT/LEFT.
	 *
	 * If TRUE, left is 1, right is 0. If FALSE, left is 0, right is 1.
	 *
	 * \return TRUE if channels are swapped
	 */
	bool channels_swapped() const noexcept
	{
		return buffer_.channels_swapped();
	}

	/**
	 * \brief TRUE iff the sequence has planar layout.
	 *
	 * \return TRUE iff sequence has planar layout
	 */
	constexpr bool planar() const noexcept
	{
		return is_planar;
	}

	// TODO operator [] ?
	// TODO at(const size_type) ?
};

} // namespace details


/**
 * \brief Planar sample sequence with samples of type T.
 *
 * A planar sequence has two separated input buffers, one for each channel.
 */
template <typename T>
using PlanarSamples = details::SampleSequence<T, true>;


/**
 * \brief Interleaved sample sequence with samples of type T.
 *
 * An interleaved sequence controls one input buffer, in which the samples for
 * each channel occurr in order.
 */
template <typename T>
using InterleavedSamples = details::SampleSequence<T, false>;

/** @} */

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

