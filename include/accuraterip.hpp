#ifndef LIBARCSTK_ACCURATERIP_HPP_
#define LIBARCSTK_ACCURATERIP_HPP_

/**
 * \internal
 *
 * \file
 *
 * \brief AccurateRip implementations details.
 *
 * \details
 *
 * Do not include this file directly, include algorithms.hpp instead.
 *
 * Part of the API for \link calc calculating AccurateRip checksums\endlink.
 */

#include <algorithm>      // for fill
#include <array>          // for array
#include <cmath>          // for abs
#include <cstddef>
#include <cstdint>        // for uint_fast32_t, uint_fast64_t, int32_t
#include <memory>         // for make_unique, unique_ptr, swap
#include <string>         // for string
#include <vector>         // for vector

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"    // for Algorithm, Updateable
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"     // for checksum::type, ChecksumSet
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"     // for AudioSize
#endif
#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"
#endif

namespace arcstk
{
                                                  /** \cond NAMESPACE_v_1_0_0 */
inline namespace v_1_0_0
{
                                                                 /** \endcond */

/** \addtogroup calc */
/** @{ */

/**
 * \internal
 *
 * \brief Calculating AccurateRip checksums and ids.
 *
 * URL prefix can be read by function ACCURATERIP::request_url_prefix() and
 * modified by function ACCURATERIP::set_request_url_prefix(). After setting the
 * global URL prefix to a new value, every call of ARId::url() and
 * ARId::prefix() in client code will reflect this updated value. The global
 * value can be reset to its default by function
 * ACCURATERIP::reset_request_url_prefix().
 */
namespace accuraterip
{

/**
 * \internal
 *
 * \brief Implementation details of namespace accuraterip.
 */
namespace details
{

// Checksum calculation

/**
 * \brief Number of samples to skip at back and front.
 */
struct NUM_SKIP_SAMPLES final
{
	/**
	 * \brief Number of samples to be skipped before end of the last track.
	 *
	 * There are 5 frames to be skipped, i.e. 5 frames * 588 samples/frame
	 * = 2940 samples. We derive the number of samples to be skipped at the
	 * start of the first track by just subtracting 1 from this constant.
	 */
	constexpr static int32_t BACK  = 5/*frames*/ * 588/*samples/frame*/;

	/**
	 * \brief Number of samples to be skipped after start of the first track.
	 *
	 * There are 5 frames - 1 sample to be skipped, i.e.
	 * 5 frames * 588 samples/frame - 1 sample = 2939 samples.
	 */
	constexpr static int32_t FRONT = NUM_SKIP_SAMPLES::BACK - 1;
};


/**
 * \brief Helper for masking the lower 32 bits of a sample.
 */
constexpr static uint_fast32_t LOWER_32_BITS_ { 0xFFFFFFFF };


/**
 * \brief Values of a calculation state.
 */
struct Subtotals final
{
	/**
	 * \brief Total number of subtotals.
	 */
	static constexpr std::size_t SIZE { 2939 + 2940 + 1 };

	//            0 : current (k == 0)
	//    1 -  2939 : first i samples of the track (in order from i == 1)
	// 2940 -  5879 : last 2940-i samples of the track (in order from i == 0)

	// 5879+
	//    1 -  2939 : segment from 1st sample of frame 445 to last sample of 449
	// 2940 -  5879 : segment from 1st sample of frame 451 to last sample of 455

	/**
	 * \brief Type of subtotals buffer.
	 */
	using storage_type = std::array<uint_fast32_t, SIZE>;

	/**
	 * \brief Actual subtotals for required indices.
	 *
	 * Aka S_A for v1, contains lower bits of i * sample_i.
	 */
	storage_type subtotals_v1 {/* all 0 */};

	/**
	 * \brief Actual subtotals for required indices.
	 *
	 * Higher bits required for S_A for v2, or maybe higher and lower bits
	 * of i * sample_i.
	 */
	storage_type subtotals_v2 {/* all 0 */};

	/**
	 * \brief Unweighted samples sums for required indices.
	 *
	 * Aka S_B, just sum of sample_i from 0 to i.
	 */
	storage_type sums {/* all 0 */};

	/**
	 * \brief Current multiplier.
	 */
	uint_fast64_t multiplier { 1 };

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(Subtotals& lhs, Subtotals& rhs) noexcept
	{
		using std::swap;

		swap(lhs.subtotals_v1, rhs.subtotals_v1);
		swap(lhs.subtotals_v2, rhs.subtotals_v2);
		swap(lhs.sums,         rhs.sums);
		swap(lhs.multiplier,   rhs.multiplier);
	}
};


/**
 * \brief Provide service functions for all AccessSt<> specializations.
 */
struct AccessSt
{
	/**
	 * \brief Return Checksum value type.
	 *
	 * \param[in] v Subtotal
	 *
	 * \return Result as Checksum value
	 */
	static Checksum::value_type to_value(const uint_fast32_t v)
	{
		return static_cast<Checksum::value_type>(v);
	}

	/**
	 * \brief Convert multiplier to AudioSize.
	 *
	 * \param[in] m Multiplier to convert
	 *
	 * \return AudioSize of track
	 */
	static AudioSize track_size(const uint_fast64_t m)
	{
		using arcstk::UNIT;

		// cast is save for valid input data
		return { static_cast<int32_t>(m - 1), UNIT::SAMPLES };
	}

	/**
	 * \brief Array index for the first k > 0 samples.
	 *
	 * \param[in] k Drive offset
	 *
	 * \return Index of the first k samples value
	 */
	static std::size_t idx_front(const int k)
	{
		return static_cast<std::size_t>(std::abs(k));
	}

	/**
	 * \brief Array index for accessing value for the last <tt>k > 0</tt>
	 * samples.
	 *
	 * \param[in] k Drive offset
	 *
	 * \return Index of the k last samples value
	 */
	static std::size_t idx_back(const int k)
	{
		return 5879u - static_cast<std::size_t>(std::abs(k)) + 1;
	}
};


/**
 * \brief Access actual Subtotals
 *
 * \tparam T1 First checksum type
 * \tparam T2 More checksum types
 */
template <enum checksum::type T>
struct Access;


// AccurateRip v1
template <>
struct Access<checksum::type::ARCS1>
{
	static inline Checksum::value_type subtotal(const Subtotals& st,
			const std::size_t i)
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        return st.subtotals_v1[i];
    }
};


// AccurateRip v2
template <>
struct Access<checksum::type::ARCS2>
{
	static inline Checksum::value_type subtotal(const Subtotals& st,
			const std::size_t i)
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
		return st.subtotals_v1[i]/* == 0 */ + st.subtotals_v2[i];
    }
};


/**
 * \brief Current subtotal of checksum type \c TYPE.
 *
 * \tparam TYPE Checksum type to acquire value of
 *
 * \param[in] st Subtotals to get checksum of
 *
 * \return Value of checksum type \c TYPE
 */
template <enum checksum::type T>
auto checksum(const Subtotals& st) -> Checksum::value_type
{
	return Access<T>::subtotal(st, 0);
}


/**
 * \brief Current sum of combined samples.
 *
 * \param[in] st Subtotals to get sum of
 *
 * \return Current sum of combined samples
 */
inline auto current_cs_sum(const Subtotals& st) -> Checksum::value_type
{
	return st.sums[0];
}


/**
 * \brief Current subtotals of type \c TYPE for the first \c k values.
 *
 * For <tt>i == 0</tt> this function returns \c 0.
 *
 * \tparam TYPE Checksum type to acquire value of
 *
 * \param[in] k  Offset value in range [-2939,2940] (unchecked)
 * \param[in] st Subtotals to access
 *
 * \return Current subtotal of first \c k values for type \c TYPE
 */
template <enum checksum::type T>
auto first(const std::size_t k, const Subtotals& st) -> Checksum::value_type
{
	return (k) ? Access<T>::subtotal(st, k) : 0;
}


/**
 * \brief Current subtotals of type \c TYPE for the last \c k values.
 *
 * For <tt>i == 0</tt> this function returns \c 0.
 *
 * \tparam TYPE Checksum type to acquire value of
 *
 * \param[in] k  Offset value in range [-2939,2940] (unchecked)
 * \param[in] st Subtotals to access
 *
 * \return Current subtotal of last \c k values for type \c TYPE
 */
template <enum checksum::type T>
auto last(const std::size_t k, const Subtotals& st) -> Checksum::value_type
{
	return (k) ? Access<T>::subtotal(st, 5879u - k + 1u) : 0;
}


/**
 * \brief Simple sum of combined samples on index <tt>i &gt; 0</tt> and value
 * <tt>0</tt> for index <tt>i == 0</tt>.
 *
 * This function only accesses sums for subtotals, not for the current checksum.
 *
 * \param[in] i  Index position
 * \param[in] st Subtotals to access
 *
 * \return Sum of combined samples on index \c i
 */
inline const uint_fast32_t& cs_sum(const std::size_t i, const Subtotals& st)
{
	static constexpr uint_fast32_t ZERO = 0;

	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	return (i) ? st.sums[i] : ZERO;
}


/**
 * \brief Worker: reset every value in a Subtotals cache to 0.
 *
 * \param[in] s Storage instance to reset
 */
inline void set_to_zero(Subtotals::storage_type& s)
{
	using std::begin;
	using std::end;

	std::fill(begin(s), end(s), 0);
}


/**
 * \brief Provide operator() for all Update<> specializations.
 */
template <class Derived>
class UpdateBase // NOLINT(bugprone-crtp-constructor-accessibility)
{
protected:

	/**
	 * \brief Next index of vectors.
	 */
	// NOLINTNEXTLINE(misc-non-private-member-variables-in-classes,cppcoreguidelines-non-private-member-variables-in-classes)
	mutable std::size_t idx_ { 1 };

public:

	/**
	 * \brief Current caching index.
	 *
	 * \return Index position to get next cache value
	 */
	std::size_t cache_index() const
	{
		return idx_;
	}

	/**
	 * \brief Set cache index.
	 *
	 * \param[in] idx New cache index
	 */
	void set_cache_index(const std::size_t idx) const
	{
		idx_ = idx;
	}

	/**
	 * \brief Call operator.
	 *
	 * Perform a full update on the current subtotals.
	 *
	 * Caching can be either turned on or off for the entire input.
	 *
	 * \param[in] start The start position
	 * \param[in] stop  The stop position
	 * \param[in] st    Subtotals to update
	 * \param[in] c     TRUE requests caching values, FALSE discards caching
	 */
	template <class B, class E>
	void operator()(const B& start, const E& stop, Subtotals& st, bool c) const
	{
		const auto* self = static_cast<const Derived*>(this);

		if (c)
		{
			self->template calloperator_impl<true>(start, stop, st);
		} else
		{
			self->template calloperator_impl<false>(start, stop, st);
		}
	}

	/**
	 * \brief Accumulate values from 1 to 2939 and from (n - 2940) to n.
	 *
	 * \param[in] st Subtotals to accumulate
	 */
	void accumulate(Subtotals& st) const
	{
		// accumulate first 2939 in track
		for (auto i = std::size_t { 2 }; i <= 2939; ++i)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[i] += st.subtotals_v1[i - 1];
			st.sums[i]         += st.sums[i - 1];
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
		}

		// accumulate last 2940 in track
		for (auto i = std::size_t { 5879 }; i > 2940; --i)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[i - 1] += st.subtotals_v1[i];
			st.sums[i - 1]         += st.sums[i];
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
		}
	}
};


/**
 * \brief Functor for performing the actual update.
 *
 * \tparam T1 First checksum type
 * \tparam T2 More checksum types
 */
template <enum checksum::type T1, enum checksum::type... T2>
class Update;


// AccurateRip v1
template <>
class Update<checksum::type::ARCS1>
	: public UpdateBase<Update<checksum::type::ARCS1>>
{
	using type = checksum::type; // convenience

	friend class UpdateBase<Update<type::ARCS1>>;

	/**
	 * \brief Current update factor.
	 */
	mutable uint_fast64_t update_ { 0 };

	/**
	 * \brief Calculate ARCSv1.
	 *
	 * Also known as ARCF ("AccurateRip Checksum Flawed"),
	 */
	uint32_t arcs_v1(const uint_fast64_t multiplier, const uint32_t csample)
		const
	{
		return multiplier * csample & LOWER_32_BITS_;
	}

	/**
	 * \brief Implementation of update operation.
	 */
	template <bool KEEP, class B, class E>
	void calloperator_impl(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			update_ = arcs_v1(st.multiplier, *pos);

			st.subtotals_v1[0] += update_;
			st.sums[0]         += *pos;

			if constexpr (KEEP)
			{
				// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
				st.subtotals_v1[idx_] = update_;
				st.sums[idx_]         = *pos;
				// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
				++idx_;
			}
		}
	}

public:

	/**
	 * \brief ID string of this update.
	 */
	std::string id_string() const
	{
		return "v1";
	}

	/**
	 * \brief Cache this sequence.
	 */
	template <class B, class E>
	void cache(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[idx_] = arcs_v1(st.multiplier, *pos);
			st.sums[idx_]         = *pos;
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
			++idx_;
		}
	}

	/**
	 * \brief Reset the subtotals.
	 */
	void reset(Subtotals& st) const
	{
		set_to_zero(st.subtotals_v1);
		set_to_zero(st.sums);
	}
};


// AccurateRip v2
template <>
class Update<checksum::type::ARCS2>
	: public UpdateBase<Update<checksum::type::ARCS2>>
{
	using type = checksum::type; // convenience

	friend class UpdateBase<Update<type::ARCS2>>;

	/**
	 * \brief Current update factor.
	 */
	mutable uint_fast64_t update_ { 0 };

	/**
	 * \brief Calculate ARCSv2.
	 */
	uint32_t arcs_v2(const uint_fast64_t multiplier, const uint32_t csample)
		const
	{
		update_ = multiplier * csample; // TODO Make update_ local?

		return (update_ & LOWER_32_BITS_) + (update_ >> 32u);
	}

	/**
	 * \brief Implementation of update operation.
	 */
	template <bool KEEP, class B, class E>
	void calloperator_impl(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			update_ = arcs_v2(st.multiplier, *pos);

			st.subtotals_v2[0] += update_;
			st.sums[0]         += *pos;

			if constexpr (KEEP)
			{
				// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
				st.subtotals_v2[idx_] = update_;
				st.sums[idx_]         = *pos;
				// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
				++idx_;
			}
		}
	}

public:

	/**
	 * \brief ID string of this update.
	 */
	std::string id_string() const
	{
		return "v2";
	}

	/**
	 * \brief Cache this sequence.
	 */
	template <class B, class E>
	void cache(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v2[idx_] = arcs_v2(st.multiplier, *pos);
			st.sums[idx_]         = *pos;
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
			++idx_;
		}
	}

	/**
	 * \brief Reset the subtotals.
	 */
	void reset(Subtotals& st) const
	{
		set_to_zero(st.subtotals_v2);
		set_to_zero(st.sums);
	}
};


// AccurateRip v1+2
template <>
class Update<checksum::type::ARCS1, checksum::type::ARCS2>
	: public UpdateBase<Update<checksum::type::ARCS1, checksum::type::ARCS2>>
{
	using type = checksum::type; // convenience

	friend class UpdateBase<Update<type::ARCS1, type::ARCS2>>;

	/**
	 * \brief Current update factor.
	 */
	mutable uint_fast64_t update_ { 0 };

	/**
	 * \brief Implementation of update operation.
	 */
	template <bool KEEP, class B, class E>
	void calloperator_impl(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			update_ = st.multiplier * (*pos);

			st.subtotals_v1[0] += update_ & LOWER_32_BITS_;
			st.subtotals_v2[0] += (update_ >> 32u);
			st.sums[0]         += *pos;

			if constexpr (KEEP)
			{
				// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
				st.subtotals_v1[idx_] = update_ & LOWER_32_BITS_;
				st.subtotals_v2[idx_] = (update_ >> 32u);
				st.sums[idx_]         = *pos;
				// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
				++idx_;
			}
		}
	}

public:

	/**
	 * \brief ID string of this update.
	 */
	std::string id_string() const
	{
		return "v1+2";
	}

	/**
	 * \brief Cache this sequence.
	 */
	template <class B, class E>
	void cache(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			update_ = st.multiplier * (*pos);

			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[idx_] = update_ & LOWER_32_BITS_;
			st.subtotals_v2[idx_] = (update_ >> 32u);
			st.sums[idx_]         = *pos;
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
			++idx_;
		}
	}

	/**
	 * \brief Reset the subtotals.
	 */
	void reset(Subtotals& st) const
	{
		set_to_zero(st.subtotals_v1);
		set_to_zero(st.subtotals_v2);
		set_to_zero(st.sums);
	}
};


/**
 * \brief Determine the legal range of samples for the AccurateRip calculation.
 *
 * \param[in] ctx    The Context for calculation
 * \param[in] size   The input size of samples to process
 * \param[in] points The offset points in number of PCM samples
 *
 * \return Input range of 1-based sample indices to use for calculation
 */
inline SampleRange legal_range(const Context ctx,
		const AudioSize& size, const Points& points)
{   // required inline since used in ARCSAlgorithm::do_range()
	ARCS_LOG(DEBUG2) << "Get legal range for context " << to_string(ctx);

	auto from = int32_t { 0 };
	auto to   = int32_t { size.samples() - 1 };

	if (!points.empty())
	{
		from += points[0].samples(); // start on first offset

		ARCS_LOG(DEBUG2) << "Skip first " << from
			<< " samples due to offset";
	}

	if (any(Context::FIRST_TRACK & ctx))
	{
		from += NUM_SKIP_SAMPLES::FRONT;

		ARCS_LOG(DEBUG2) << "Skip " << NUM_SKIP_SAMPLES::FRONT
			<< " samples after beginning";
	}

	if (any(Context::LAST_TRACK & ctx))
	{
		to -= NUM_SKIP_SAMPLES::BACK;

		ARCS_LOG(DEBUG2) << "Skip last " << NUM_SKIP_SAMPLES::BACK
			<< " samples";
	}

	ARCS_LOG(DEBUG2) << "Legal range is: " << from << " - " << to;

	return { from, to };
}


/**
 * \brief Interface and base class for updatable subtotals.
 *
 * \tparam T1 First Checksum type
 * \tparam T2 Trailing Checksum types
 */
template <enum checksum::type T1, enum checksum::type... T2>
class UpdateableSubtotals final
{
	/**
	 * \brief Anticipated length of the current track.
	 */
	std::size_t current_length_ {};

	/**
	 * \brief Internal subtotals.
	 */
	Subtotals st_ {};

	/**
	 * \brief Internal update strategy.
	 */
	Update<T1, T2...> update_ {};

public:

	/**
	 * \brief Index of next cache position.
	 *
	 * \return Current cache pointer
	 */
	std::size_t cache_index() const
	{
		return update_.cache_index();
	}

	/**
	 * \brief Current subtotal of checksum type \c TYPE.
	 *
	 * \tparam TYPE Checksum type to acquire value of
	 *
	 * \return Value of checksum type \c TYPE
	 */
	template <enum checksum::type TYPE>
	Checksum::value_type value() const // TODO Rename to checksum()
	{
		return checksum<TYPE>(st_);
	}

	/**
	 * \brief Return the inner subtotals of this instance.
	 *
	 * \return Subtotals of this instance.
	 */
	const Subtotals& subtotals() const
	{
		return st_;
	}

	/**
	 * \brief Current Multiplier of this instance.
	 *
	 * \return Current multiplier
	 */
	uint_fast64_t multiplier() const
	{
		return st_.multiplier;
	}

	/**
	 * \brief Set multiplier to a new value.
	 *
	 * \param[in] m New value for multiplier
	 */
	void set_multiplier(const uint_fast64_t m)
	{
		st_.multiplier = m;
	}

	/**
	 * \brief Return the current track length (in samples).
	 *
	 * \return Current track length
	 */
	std::size_t current_length() const
	{
		return current_length_;
	}

	/**
	 * \brief Set the length of the current track.
	 *
	 * \param[in] samples Total amount of samples
	 */
	void set_current_length(const std::size_t samples)
	{
		current_length_ = samples;
	}

	/**
	 * \brief Update the instance by a sequence of samples.
	 *
	 * \tparam B Type of the begin iterator
	 * \tparam E Type of the end iterator
	 *
	 * \param[in] start The start position
	 * \param[in] stop  The stop position
	 * \param[in] size  Size of the partition
	 */
	template <class B, class E>
	void update(B start, E stop, const std::size_t size)
	{
		// leading samples to be cached
		const auto f_remaining = std::size_t
			{ st_.multiplier <= 2939 ? 2939 - (st_.multiplier - 1) : 0 };

		const auto todo = current_length_ - (st_.multiplier - 1);

		// trailing samples to be cached
		const auto b_remaining = std::size_t
			{ todo - size <= 2940 ? 2940 - (todo - size) : 0 };

		// cache everything
		if (size <= f_remaining || size <= b_remaining)
		{
			ARCS_LOG(DEBUG3) << "Entire update goes to cache";

			// if f_remaining == 0 then there is nothing to cache in front
			update_(start, stop, st_, true);
			return;
		}

		ARCS_LOG(DEBUG3) << "First " << f_remaining << " samples go to cache";
		ARCS_LOG(DEBUG3) << "Last "  << b_remaining << " samples go to cache";

		auto end_front  = start + static_cast<int>(f_remaining);
		auto begin_back = start + static_cast<int>(size - b_remaining);

		// cache remaining part in front
		update_(start,      end_front,  st_, true );
		// TODO Frame 450
		update_(end_front,  begin_back, st_, false);
		update_(begin_back, stop,       st_, true );
	}

	/**
	 * \brief Set cache index position to start caching a track suffix.
	 */
	void set_cache_start_suffix()
	{
		update_.set_cache_index(2940);
	}

	/**
	 * \brief Cache a sequence of samples.
	 *
	 * \tparam B Type of the begin iterator
	 * \tparam E Type of the end iterator
	 *
	 * \param[in] start The start position
	 * \param[in] stop  The stop position
	 */
	template <class B, class E>
	void cache(B start, E stop)
	{
		update_.cache(start, stop, st_);
	}

	/**
	 * \brief Finalize this track.
	 */
	void finalize() // TODO Do this only if shifting was requested
	{
		update_.accumulate(st_);
	}

	/**
	 * \brief Reset the instance to its initial state.
	 */
	void reset()
	{
		update_.reset(st_);
		//st_.multiplier = 1; // TODO Why not?
	}

	/**
	 * \brief Get the ID string from the Updatable.
	 *
	 * \return String representing the type of this instance.
	 */
	std::string id_string() const
	{
		return update_.id_string();
	}

	/**
	 * \brief Return the checksum types this instance calculates.
	 *
	 * \return Set of types calculated by this instance
	 */
	ChecksumtypeSet types() const
	{
		return { T1, T2... };
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(UpdateableSubtotals& rhs) noexcept
	{
		using std::swap;

		swap(this->current_length_, rhs.current_length_);
		swap(this->st_,             rhs.st_);
		swap(this->update_,         rhs.update_);
	}

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(UpdateableSubtotals& lhs, UpdateableSubtotals& rhs)
		noexcept
	{
		lhs.swap(rhs);
	}
};


/**
 * \brief AccurateRip algorithm variants.
 *
 * \tparam T1 First Checksum type
 * \tparam T2 Trailing Checksum types
 */
template <enum checksum::type T1, enum checksum::type... T2>
class ARCSAlgorithm final : public Algorithm
{
	/**
	 * \brief Internal subtotals type.
	 */
	using subtotals_t = UpdateableSubtotals<T1, T2...>;

	/**
	 * \brief Track subtotals.
	 */
	mutable std::vector<subtotals_t> tracks_ { {/*first*/} };

	/**
	 * \brief Current track subtotals.
	 *
	 * \return Current track
	 */
	const subtotals_t& current_track() const
	{
		return tracks_.back();
	}

	/**
	 * \brief Current track subtotals.
	 *
	 * \return Current track
	 */
	subtotals_t& current_subtotals() const
	{
		return tracks_.back();
	}

	/**
	 * \brief Non-virtual implementation of do_setup() for constructor.
	 */
	void setup_impl(const Context c)
	{
		ARCS_LOG(DEBUG1) << "Context for Algorithm: " << to_string(c);

		// Adjust multiplier only for Context FIRST_TRACK

		// Commented out: wrong since pre_range() is implemented
		// if (any(Context::FIRST_TRACK & c))
		// {
		// 	tracks_[0].set_multiplier(NUM_SKIP_SAMPLES::FRONT + 1);
		// }
		// Context LAST_TRACK is correctly handled by do_range()

		ARCS_LOG(DEBUG1) << "Initialize multiplier to: "
			<< current_track().multiplier();
	}

	/**
	 * \brief Get checksum for track \c track_no, shifted by offset \c k.
	 *
	 * \param[in] track_no Track number from [1,99] (checked)
	 * \param[in] k        Drive offset from [-2939,2940] (checked)
	 *
	 * \return Checksum value for track \c track_no, shifted by value \c k
	 */
	template <enum checksum::type TYPE>
	Checksum::value_type shift(const TrackNo track_no, const int k) const
	{
		if (k < -2939 || k > 2940)
		{
			throw std::runtime_error("Illegal value for k");
		}

		if (track_no > CDDA::MAX_TRACKCOUNT)
		{
			throw std::runtime_error("Illegal track number");
		}

		const auto t  = static_cast<std::size_t>(track_no - 1);
		const auto st = tracks_[t].subtotals();

		if (k == 0) // do not shift
		{
			return checksum<TYPE>(st);
		}

		const auto k_abs = static_cast<std::size_t>(std::abs(k));

		auto wsum   = checksum<TYPE>(st); // weighted sum of persistent part
		auto ssum   = current_cs_sum(st); // simple sum of persistent part
		//auto ssum   = tracks_[t].sum(0);  // simple sum of persistent part
		auto factor = std::abs(k);        // factor for persistent part

		auto a_wsum   = uint32_t { 0 };   // weighted sum of added correction
		auto a_ssum   = uint32_t { 0 };   // simple sum of added correction
		auto a_factor =  int32_t { 0 };   // factor for added correction

		if (k < 0) // shift "leftwards": remove k highest, add k lower indices
		{
			wsum -= last<TYPE>(k_abs, st);
			ssum -= cs_sum(5879 - k_abs + 1, st);

			if (t == 0) // first
			{
				// for k == -2939, second call of first() gets 0
				a_wsum = first<TYPE>(2939, st) - first<TYPE>(2939 - k_abs, st);
				a_ssum = cs_sum(2939, st) - cs_sum(2939 - k_abs, st);
				a_factor = -k;
			} else // other than first
			{
				const auto prev = tracks_[t - 1].subtotals();
				const auto sz = AccessSt::track_size(prev.multiplier).samples();

				a_wsum   = last<TYPE>(k_abs, prev);
				a_ssum   = cs_sum(5879 - k_abs + 1, prev);
				a_factor = -(sz - static_cast<int>(k_abs));
			}
		}

		if (k > 0) // shift "rightwards": remove k lowest, add k higher indices
		{
			wsum  -= first<TYPE>(k_abs, st);
			ssum  -= cs_sum(k_abs, st);
			factor = -factor;

			if (t == tracks_.size() - 1 - 1) // last
			{
				// for k == 2940, second call of last() gets 0
				a_wsum   = last<TYPE>(2940, st) - last<TYPE>(2940 - k_abs, st);
				a_ssum   = cs_sum(2940, st) - cs_sum(2940 + k_abs, st);
				a_factor = -k;
			} else // other than last
			{
				const auto next = tracks_[t + 1].subtotals();
				const auto sz = AccessSt::track_size(st.multiplier).samples();

				a_wsum   = first<TYPE>(k_abs, next);
				a_ssum   = cs_sum(k_abs, next);
				a_factor = sz - static_cast<int>(k_abs);
			}
		}

		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wsign-conversion"

		// Shift the part persistent in unshifted as well as shifted checksum.
		// Add shifted correction of incoming samples.

		return wsum + factor * ssum + (a_wsum + a_factor * a_ssum);

		// Note that the silent overflows are necessary since we ignore the
		// overflows also deliberately on the original checksum.

		#pragma GCC diagnostic pop
	}

	/**
	 * \brief Add checksum of type \c TYPE and drive offset \c k to a set.
	 *
	 * \tparam TYPE Checksum type
	 *
	 * \param[in] t Track number
	 * \param[in] k Drive offset
	 * \param[in] s ChecksumSet to add track checksum to
	 */
	template <enum checksum::type TYPE>
	void add_checksum(const int t, const int k, ChecksumSet& s) const
	{
		const auto [ it, success ] =
			s.insert(TYPE, Checksum { shift<TYPE>(t, k) });

		if (!success)
		{
			// TODO Throw?
			ARCS_LOG_ERROR << "Could not insert Checksum for type " << TYPE;
			//throw std::runtime_error("Could not insert Checksum");
		}
	}

	// Algorithm

	void do_setup(const Context c) final
	{
		this->setup_impl(c);
	}

	std::string do_name() const final
	{
		return "AccurateRip " + current_track().id_string();
	}

	ChecksumtypeSet do_types() const final
	{
		return ChecksumtypeSet { T1, T2... };
	}

	SampleRange do_range(const AudioSize& size, const Points& points) const
		final
	{
		return legal_range(this->context(), size, points);
	}

	std::unique_ptr<Partitioner> do_partitioner(
			const Points& offsets, const AudioSize& leadout) const final
	{
		return std::make_unique<arcstk::details::TrackPartitioner>(
				offsets, leadout, range(leadout, offsets));
	}

	std::size_t do_total_tracks() const final
	{
		return tracks_.size();
	}

	ChecksumSet do_track(const TrackNo t, const int k) const final
	{
        auto result = ChecksumSet {};

		if (t < 1 || t > CDDA::MAX_TRACKCOUNT)
		{
			return result;
		}

		add_checksum<T1>(t, k, result);
		(..., add_checksum<T2>(t, k, result));

		result.set_length(AccessSt::track_size(
					tracks_[static_cast<std::size_t>(t - 1)].multiplier()));

        return result;
	}

	std::unique_ptr<Algorithm> do_clone() const final
	{
		return std::make_unique<ARCSAlgorithm>(*this);
	}

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	ARCSAlgorithm() = default;

	/**
	 * \brief Constructor.
	 *
	 * \param[in] c Context for this Algorithm
	 */
	explicit ARCSAlgorithm(const Context c)
		: Algorithm { c }
	{
		this->setup_impl(c);
	}

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~ARCSAlgorithm() final = default;

	/**
	 * \brief Pass track samples before the actual range of the algorithm.
	 *
	 * Implements Algorithm::pre_range().
	 *
	 * \tparam B Type of iterator pointing to the begin of the sample sequence
	 * \tparam E Type of iterator pointing to the end   of the sample sequence
	 *
	 * \param[in] start Iterator pointing to the begin of the sample sequence
	 * \param[in] stop  Iterator pointing to the end   of the sample sequence
	 */
	template <class B, class E>
	void perform_pre_range(B start, E stop)
	{
		current_subtotals().cache(start, stop);

		ARCS_LOG(DEBUG2) << "Multiplier after caching is: "
			<< current_track().multiplier();
	}

	/**
	 * \brief Called when starting a new track.
	 *
	 * \param[in] trackno Track number
	 * \param[in] length  Track length as calculated
	 */
	void perform_start_track(const int /*trackno*/, const AudioSize& length)
	{
		current_subtotals().set_current_length(
				static_cast<std::size_t>(length.samples()));

		ARCS_LOG(DEBUG2) << "Set current length of this track: "
			<< current_subtotals().current_length();
	}

	/**
	 * \brief Update the instance by a sequence of samples.
	 *
	 * \tparam B Type of the begin iterator
	 * \tparam E Type of the end iterator
	 *
	 * \param[in] start The start position (part of update)
	 * \param[in] stop  The stop position (not part of update)
	 * \param[in] size  The input size of the update
	 */
	template <class B, class E>
	void perform_update(B start, E stop, const std::size_t size)
	{
		const auto m { current_track().multiplier() };

		ARCS_LOG(DEBUG3) << "First multiplier: " << m;

		current_subtotals().update(start, stop, size);

		ARCS_LOG(DEBUG3) << "Last multiplier:  "
			<< (current_track().multiplier() - 1);
		// -1 because multiplier_ has already been updated to next input
	}

	/**
	 * \brief Pass track samples after the actual range of the algorithm.
	 *
	 * Implements Algorithm::post_range().
	 *
	 * \tparam B Type of iterator pointing to the begin of the sample sequence
	 * \tparam E Type of iterator pointing to the end   of the sample sequence
	 *
	 * \param[in] start Iterator pointing to the begin of the sample sequence
	 * \param[in] stop  Iterator pointing to the end   of the sample sequence
	 */
	template <class B, class E>
	void perform_post_range(B start, E stop)
	{
		// Note that update() wouldn't have known that it saw the last track.
		// We will have to overwrite the last 2940 cache values.
		// TODO Can we avoid the useless caching of update() on the last track?
		current_subtotals().set_cache_start_suffix();

		current_subtotals().cache(start, stop);

		ARCS_LOG(DEBUG1) << "After caching post-range samples, multiplier is: "
			<< current_track().multiplier();
	}

	/**
	 * \brief Finalize current track.
	 *
	 * Save the subtotals for current track and start next track.
	 *
	 * \param[in] t       Track number (ignored)
	 * \param[in] updated Track length as calculated (only legal range)
	 */
	void perform_finalize_track(const int /*t*/, const AudioSize& /*updated*/)
	{
		current_subtotals().finalize();
		tracks_.emplace_back(subtotals_t {});
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(ARCSAlgorithm& rhs) noexcept
	{
		this->swap_base(rhs);

		using std::swap;

		swap(this->tracks_, rhs.tracks_);
	}

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(ARCSAlgorithm& lhs, ARCSAlgorithm& rhs) noexcept
	{
		lhs.swap(rhs);
	}
};

} // namespace details

/**
 * \internal
 *
 * \brief AccurateRip checksum calculation algorithms.
 */
namespace algorithm
{

// The following using declaratives are intended for testing.
// For regular use, include header algorithms.hpp.

/**
 * \brief AccurateRip checksum algorithm version 1.
 */
using Version1 = details::ARCSAlgorithm<checksum::type::ARCS1>;

/**
 * \brief AccurateRip checksum algorithm version 2.
 */
using Version2 = details::ARCSAlgorithm<checksum::type::ARCS2>;

/**
 * \brief AccurateRip checksum algorithm version 2 providing also version 1.
 */
using Versions1and2 =
		details::ARCSAlgorithm<checksum::type::ARCS1,checksum::type::ARCS2>;

} // namespace algorithm


/**
 * \internal
 *
 * \brief AccurateRip Id, URL, and filename calculation.
 */
namespace id
{

/**
 * \brief Service function: Compute the disc id 1 from offsets and leadout.
 *
 * \param[in] offsets Offsets (in LBA frames) of each track
 * \param[in] leadout Leadout LBA frame
 *
 * \return AccurateRip disc id 1
 */
uint32_t disc_id_1(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept;

/**
 * \brief Service function: Compute the disc id 2 from offsets and leadout.
 *
 * \param[in] offsets Offsets (in LBA frames) of each track
 * \param[in] leadout Leadout LBA frame
 *
 * \return AccurateRip disc id 2
 */
uint32_t disc_id_2(const std::vector<int32_t>& offsets, const int32_t leadout)
	noexcept;

/**
 * \brief Service function: Compute the CDDB id from offsets and leadout.
 *
 * The CDDB id is a 32bit unsigned integer, formed of a concatenation of
 * the following 3 numbers:
 * first chunk (8 bits):   checksum (sum of digit sums of offset secs + 2)
 * second chunk (16 bits): total seconds count
 * third chunk (8 bits):   total number of tracks
 *
 * \param[in] offsets     Offsets (in LBA frames) of each track
 * \param[in] leadout     Leadout LBA frame
 *
 * \return CDDB id
 */
uint32_t cddb_id(const std::vector<int32_t>& offsets, const int32_t leadout);

/**
 * \brief Service function: Compute the AccurateRip response filename
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip response filename
 */
std::string construct_filename(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Service function: Compute the AccurateRip request URL
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 * \param[in] prefix        URL prefix
 *
 * \return AccurateRip request URL
 */
std::string construct_url(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id,
		const std::string& prefix) noexcept;

/**
 * \brief Service function: Compute the AccurateRip request URL
 *
 * The URL is constructed using current_request_url_prefix().
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip request URL
 */
std::string construct_url(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Service function: Compute the AccurateRip request ID
 *
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 *
 * \return AccurateRip request URL
 */
std::string construct_id(const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id) noexcept;

/**
 * \brief Service function: Print an ARId by its ids.
 *
 * \param[in] out           Stream to print to
 * \param[in] total_tracks  Number of tracks in this medium
 * \param[in] id_1          Id 1 of this medium
 * \param[in] id_2          Id 2 of this medium
 * \param[in] cddb_id       CDDB id of this medium
 */
void print(std::ostream& out, const unsigned total_tracks,
		const uint32_t id_1,
		const uint32_t id_2,
		const uint32_t cddb_id);

} // namespace id

} // namespace accuraterip


/**
 * \brief Constants for the AccurateRip service.
 */
class ACCURATERIP final
{
	/**
	 * \brief Current request URL prefix.
	 */
	static std::string request_url_prefix_;

	// ... may contain more constants

public:

	/**
	 * \brief The current URL prefix to construct request URLs.
	 *
	 * \return Current prefix to construct request URLs.
	 */
	static std::string request_url_prefix() noexcept;

	/**
	 * \brief The default URL prefix to construct request URLs.
	 *
	 * \return Default prefix to construct request URLs.
	 */
	static std::string default_request_url_prefix() noexcept;

	/**
	 * \brief Set the global URL prefix for AccurateRip request URLs.
	 *
	 * \param[in] prefix URL prefix to use for constructing ARId URLs
	 */
	static void set_request_url_prefix(const std::string& prefix) noexcept;

	/**
	 * \brief Set the global URL prefix for AccurateRip request URLs to its
	 * default value.
	 *
	 * The default value is defined by
	 * ACCURATERIP::default_request_url_prefix().
	 */
	static void reset_request_url_prefix() noexcept;

	/**
	 * \brief Format an unsigned 32bit integer as an ARCS in the default format.
	 *
	 * The default format is the format in which ARCSs are printed in most
	 * client applications.
	 *
	 * The ARCS default format entails:
	 * - hexadecimal representation
	 * - base (like "0x") is not represented
	 * - always 8 digits wide, possibly with leading zeros
	 * - digits A-F are always uppercase
	 *
	 * \param[in] number The number to format
	 *
	 * \return Default-ARCS-formatted representation of the input number
	 */
	static std::string default_arcs_format(const uint32_t number);
};

/** @} */ // group calc

                                                  /** \cond NAMESPACE_v_1_0_0 */
} // namespace v_1_0_0
                                                                 /** \endcond */
} // namespace arcstk

#endif

