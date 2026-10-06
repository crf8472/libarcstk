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

#include <algorithm>      // for fill, any_of
#include <array>          // for array
#include <cmath>          // for abs
#include <cstddef>        // for ptrdiff_t
#include <cstdint>        // for uint_fast32_t, uint_fast64_t, int32_t
#include <memory>         // for make_unique, unique_ptr, swap
#include <string>         // for string
#include <vector>         // for vector

#include <iostream>

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
 * \brief Available cache sizes.
 */
enum class CacheSize : std::size_t //NOLINT(performance-enum-size)
{
	ZERO     =          0,
	SHIFTING =       5880,
	FRAME450 = 2ul * 5880u
};

//    0 -  2939 : first i samples of the track (in order from i == 1)
// 2940 -  5879 : last 2940-i samples of the track (in order from i == 0)

// 5880 -  8819 : from 1st sample of frame 445 to last sample of 449
// 8820 - 11759 : from 1st sample of frame 450 to last sample of 454

/**
 * \brief Values of a calculation state.
 */
struct Subtotals final
{
	/**
	 * \brief Type for the multiplier.
	 *
	 * An unsigned type of at least 64 bit.
	 */
	using factor_type = uint_fast64_t;

	/**
	 * \brief Type for variables accumulating weighted and simple sums.
	 *
	 * An unsigned type of at least 32 bit.
	 *
	 * Note that this type may be in fact bigger than 32 bit and hence must
	 * be casted to uint32_t to before representing a checksum value.
	 */
	using cache_type = uint_fast32_t;

	/**
	 * \brief Type of subtotals buffer.
	 */
	using storage_type = std::vector<cache_type>;

	/**
	 * \brief Next index of vectors.
	 */
	std::size_t idx_ { 0 };

	/**
	 * \brief Next index of vectors for frame 450.
	 */
	std::size_t f_idx_ { 0 };

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
	factor_type multiplier { 1 };

	/**
	 * \brief Current subtotal for ARCSv1.
	 */
	cache_type current_subtotal_v1 { 0 };

	/**
	 * \brief Current subtotal for ARCSv2.
	 */
	cache_type current_subtotal_v2 { 0 };

	/**
	 * \brief Current sum of subtotals.
	 */
	cache_type current_cs_sum { 0 };

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(Subtotals& lhs, Subtotals& rhs) noexcept
	{
		using std::swap;

		swap(lhs.subtotals_v1, rhs.subtotals_v1);
		swap(lhs.subtotals_v2, rhs.subtotals_v2);
		swap(lhs.sums,         rhs.sums);

		swap(lhs.multiplier,          rhs.multiplier);
		swap(lhs.current_subtotal_v1, rhs.current_subtotal_v1);
		swap(lhs.current_subtotal_v2, rhs.current_subtotal_v2);
	}
};


/**
 * \brief Helper for masking the lower 32 bits of a sample.
 */
constexpr static Subtotals::cache_type LOWER_32_BITS_ { 0xFFFFFFFF };


/**
 * \brief Provide service functions for all AccessSt<> specializations.
 */
struct AccessSt
{
	/**
	 * \brief Constant \c 0.
	 */
	static constexpr Subtotals::cache_type ZERO = 0;

	/**
	 * \brief Return Checksum value type from a value of cache_type.
	 *
	 * \param[in] v Subtotal
	 *
	 * \return Result as Checksum value
	 */
	static Checksum::value_type to_value(const Subtotals::cache_type v)
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
	static AudioSize track_size(const Subtotals::factor_type m)
	{
		using arcstk::UNIT;

		// cast is save for valid input data
		return { static_cast<int32_t>(m - 1), UNIT::SAMPLES };
	}

	/**
	 * \brief Index for representing first \c k positions.
	 *
	 * \param[in] k Amount in [1,2940]
	 *
	 * \return Subtotals index for first \c k positions
	 */
	static std::size_t idx_front(const std::size_t k)
	{
		return k - 1u; // 0 - 2939
	}

	/**
	 * \brief Index for representing last \c k positions.
	 *
	 * \param[in] k Amount in [1,2940]
	 *
	 * \return Subtotals index for first \c k positions
	 */
	static std::size_t idx_back(const std::size_t k)
	{
		return 5879u - k + 1u; // 2940 - 5879
	}

	/**
	 * \brief TRUE iff \c k is in <tt>[1,2940]</tt>.
	 *
	 * \param[in] k Amount in [1,2940]
	 *
	 * \return TRUE iff \c k is valid
	 */
	static bool valid(const std::size_t k)
	{
		return !(k == 0 || k > 2940u);
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
        return AccessSt::to_value(st.subtotals_v1[i]);
    }

	static inline Checksum::value_type checksum(const Subtotals& st)
	{
		return AccessSt::to_value(st.current_subtotal_v1);
	}
};


// AccurateRip v2
template <>
struct Access<checksum::type::ARCS2>
{
	static inline Checksum::value_type subtotal(const Subtotals& st,
			const std::size_t i)
	{
		// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
		return AccessSt::to_value(st.subtotals_v1[i])/* == 0 */
			+  AccessSt::to_value(st.subtotals_v2[i]);
		// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
    }

	static inline Checksum::value_type checksum(const Subtotals& st)
	{
		return AccessSt::to_value(st.current_subtotal_v1)
			+  AccessSt::to_value(st.current_subtotal_v2);
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
	return Access<T>::checksum(st);
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
	return AccessSt::to_value(st.current_cs_sum);
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
	if (!AccessSt::valid(k)) { return AccessSt::ZERO; }

	return Access<T>::subtotal(st, AccessSt::idx_front(k));
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
	if (!AccessSt::valid(k)) { return AccessSt::ZERO; }

	return Access<T>::subtotal(st, AccessSt::idx_back(k));
}


/**
 * \brief Simple sum of first \c k combined samples with \c k in
 * <tt>[1,2940]</tt> and value \c 0 otherwise.
 *
 * \param[in] k  Amount of values
 * \param[in] st Subtotals to access
 *
 * \return Sum of first \c k combined samples
 */
inline auto cs_sum_first(const std::size_t k, const Subtotals& st)
	-> Checksum::value_type
{
	if (!AccessSt::valid(k)) { return AccessSt::ZERO; }

	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	return AccessSt::to_value(st.sums[AccessSt::idx_front(k)]);
}


/**
 * \brief Simple sum of last \c k combined samples with \c k in
 * <tt>[1,2940]</tt> and value \c 0 otherwise.
 *
 * \param[in] k  Amount of values
 * \param[in] st Subtotals to access
 *
 * \return Sum of last \c k combined samples
 */
inline auto cs_sum_last(const std::size_t k, const Subtotals& st)
	-> Checksum::value_type
{
	if (!AccessSt::valid(k)) { return AccessSt::ZERO; }

	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
	return AccessSt::to_value(st.sums[AccessSt::idx_back(k)]);
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
	/**
	 * \brief Accumulate values from index \c from up to index \c upto.
	 *
	 * \param[in] from Start index
	 * \param[in] upto Included end index
	 * \param[in] st   Subtotals to accumulate
	 */
	void accumulate_upwards(const std::size_t from, const std::size_t upto,
			Subtotals& st) const
	{
		for (auto i = std::size_t { from }; i <= upto; ++i)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[i] += st.subtotals_v1[i - 1];
			st.subtotals_v2[i] += st.subtotals_v2[i - 1];
			st.sums[i]         += st.sums[i - 1];
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
		}
	}

	/**
	 * \brief Accumulate values from index \c from down to index \c downto.
	 *
	 * \param[in] from   Start index
	 * \param[in] downto Not-included stop index
	 * \param[in] st     Subtotals to accumulate
	 */
	void accumulate_downwards(const std::size_t from, const std::size_t downto,
			Subtotals& st) const
	{
		for (auto i = std::size_t { from }; i > downto; --i)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[i - 1] += st.subtotals_v1[i];
			st.subtotals_v2[i - 1] += st.subtotals_v2[i];
			st.sums[i - 1]         += st.sums[i];
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
		}
	}

public:

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
		for (auto i = std::size_t { 1 }; i <= 2939; ++i)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[i] += st.subtotals_v1[i - 1];
			st.subtotals_v2[i] += st.subtotals_v2[i - 1];
			st.sums[i]         += st.sums[i - 1];
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
		}
		// TODO accumulate_upwards(1, 2939, st);

		// accumulate last 2940 in track
		for (auto i = std::size_t { 5879 }; i > 2940; --i)
		{
			// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
			st.subtotals_v1[i - 1] += st.subtotals_v1[i];
			st.subtotals_v2[i - 1] += st.subtotals_v2[i];
			st.sums[i - 1]         += st.sums[i];
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
		}
		// TODO accumulate_downwards(5879, 2940, st);
	}

	/**
	 * \brief Accumulate values from 5880 to 8819 and from (n - 2940) to n.
	 *
	 * \param[in] st Subtotals to accumulate
	 */
	void accumulate_f450(Subtotals& st) const
	{
		accumulate_upwards  ( 5880, 8819, st);
		accumulate_downwards(11759, 8820, st);
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
	mutable Subtotals::factor_type update_ { 0 };

	/**
	 * \brief Calculate ARCSv1.
	 *
	 * Also known as ARCF ("AccurateRip Checksum Flawed"),
	 */
	uint32_t arcs_v1(const Subtotals::factor_type multiplier,
			const uint32_t csample) const
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

			st.current_subtotal_v1 += update_;
			st.current_cs_sum      += *pos;

			if constexpr (KEEP)
			{
				// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
				st.subtotals_v1[st.idx_] = update_;
				st.sums[st.idx_]         = *pos;
				// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
				++st.idx_;
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
			st.subtotals_v1[st.idx_] = arcs_v1(st.multiplier, *pos);
			st.sums[st.idx_]         = *pos;
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
			++st.idx_;
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
	mutable Subtotals::factor_type update_ { 0 };

	/**
	 * \brief Calculate ARCSv2.
	 */
	uint32_t arcs_v2(const Subtotals::factor_type multiplier,
			const uint32_t csample) const
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

			st.current_subtotal_v2 += update_;
			st.current_cs_sum      += *pos;

			if constexpr (KEEP)
			{
				// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
				st.subtotals_v2[st.idx_] = update_;
				st.sums[st.idx_]         = *pos;
				// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
				++st.idx_;
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
			st.subtotals_v2[st.idx_] = arcs_v2(st.multiplier, *pos);
			st.sums[st.idx_]         = *pos;
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
			++st.idx_;
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
	mutable Subtotals::factor_type update_ { 0 };

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

			st.current_subtotal_v1 += update_ & LOWER_32_BITS_;
			st.current_subtotal_v2 += (update_ >> 32u);
			st.current_cs_sum      += *pos;

			if constexpr (KEEP)
			{
				// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
				st.subtotals_v1[st.idx_] = update_ & LOWER_32_BITS_;
				st.subtotals_v2[st.idx_] = (update_ >> 32u);
				st.sums[st.idx_]         = *pos;
				// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
				++st.idx_;
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
			st.subtotals_v1[st.idx_] = update_ & LOWER_32_BITS_;
			st.subtotals_v2[st.idx_] = (update_ >> 32u);
			st.sums[st.idx_]         = *pos;
			// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
			++st.idx_;
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
	 * \brief Internal subtotals for track.
	 */
	Subtotals st_ {};

	/**
	 * \brief Internal update strategy for subtotals.
	 */
	Update<T1, T2...> update_ {};

	/**
	 * \brief Initialize subtotals size.
	 *
	 * \param[in] st Subtotals
	 */
	void init_cache_size(Subtotals& st)
	{
		auto size = CacheSize::ZERO;

		if (do_frame450())
		{
			size = CacheSize::FRAME450;
		} else if (do_drive_offsets())
		{
			size = CacheSize::SHIFTING;
		}

		st.subtotals_v1.resize(static_cast<size_t>(size));
		st.subtotals_v2.resize(static_cast<size_t>(size));
		st.sums.resize(static_cast<size_t>(size));
	}

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	UpdateableSubtotals()
	{
		init_cache_size(st_);
	}

	/**
	 * \brief Index of next cache position.
	 *
	 * \return Current cache pointer
	 */
	std::size_t cache_index_subtotals() const
	{
		return st_.idx_;
	}

	/**
	 * \brief Current subtotal of checksum type \c TYPE.
	 *
	 * \tparam TYPE Checksum type to acquire value of
	 *
	 * \return Value of checksum type \c TYPE
	 */
	template <enum checksum::type TYPE>
	Checksum::value_type value() const
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
	Subtotals::factor_type multiplier() const
	{
		return st_.multiplier;
	}

	/**
	 * \brief Set multiplier to a new value.
	 *
	 * \param[in] m New value for multiplier
	 */
	void set_multiplier(const Subtotals::factor_type m)
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
	 * \brief TRUE iff this instance calculates drive offsets, otherwise FALSE.
	 *
	 * \return TRUE iff drive offsets are calculated
	 */
	bool do_drive_offsets() const
	{
		return true;
	}

	/**
	 * \brief TRUE iff this instance calculates checksums for frame 450,
	 * otherwise FALSE.
	 *
	 * \return TRUE iff checksums for frame450 are calculated
	 */
	bool do_frame450() const
	{
		return true;
	}

	// Frame 450:
	// This index is 0-based, so it is the 451. frame counted
	// 450 * 588 == 264.600 Samples  ->  264.601 is 1st sample in f450
	// 451 * 588 == 265.188 Samples  ->  265.188 is last sample in f450
	// To shift -2939 we need the 5 frames before: 445 - 449
	// To shift +2940 we need the 4 frames after:  451 - 454
	// 445 * 588 == 261.660 Samples  ->  261.661 is 1st sample in f445
	// 454 * 588 == 266.952 Samples  ->  266.952 is last sample in f454

	// static constexpr auto f445_1st  = std::size_t { 261661u };
	// static constexpr auto f450_1st  = std::size_t { 264601u };
	// static constexpr auto f450_last = std::size_t { 265188u };
	// static constexpr auto f454_last = std::size_t { 266952u };

	/**
	 * \brief Update sections for caching and optional frame450.
	 *
	 * \param[in] m             Current 1-based track-relative index
	 * \param[in] usize         Total number of samples in the current update
	 * \param[in] tsize         Total number of samples in the current track
	 * \param[in] drive_offsets Add sections for caching subtotals iff TRUE
	 * \param[in] frame450      Add sections for frame450 iff TRUE
	 */
	auto sections(const Subtotals::factor_type m, const std::size_t usize,
			const std::size_t tsize,
			const bool drive_offsets, const bool frame450)
	{
		using std::ptrdiff_t;
		using std::size_t;

		// return type
		using data_t = std::array<ptrdiff_t, 6>;

		if (!drive_offsets && !frame450) // no sections required
		{
			return data_t { /* 0 */ };
		}

		const auto ssize = static_cast<ptrdiff_t>(usize);

		// samples already processed in the track
		const auto shift_m = ptrdiff_t { static_cast<uint32_t>(m) - 1 };

		// collect potential split points: offsets to the start iterator
		auto points = data_t { /* 0 */ };


		if (drive_offsets) // cache subtotals for first + last 2940 samples
		{
			// end of first 2939: cache before, don't cache afterwards
			points[0] = ptrdiff_t { 2940 - shift_m };

			// samples left to process in the track
			const auto track_remain = static_cast<ptrdiff_t>(tsize) - shift_m;

			ARCS_LOG(DEBUG4) << "samples remain in track: " << track_remain;

			// samples left to process in the track after this update
			const auto remainder = track_remain - ssize;

			ARCS_LOG(DEBUG4) << "samples remain in update: " << remainder;

			if (remainder > 0) // update does not end track
			{
				// a part of the last 2940 samples is in this update
				if (remainder < 2940)
				{
					// begin of last 2940: don't cache before, cache afterwards
					points[5] = ptrdiff_t { ssize - (2940 - remainder) };
				}
			} else // update ends track
			{
				// last 2940: don't cache before, cache afterwards
				points[5] = ptrdiff_t { ssize - 2940 };
			}

			ARCS_LOG(DEBUG4) << "m: " << m << ", ssize: " << ssize
				<< ", f: " << points[0] << ", b: " << points[5];

			// Shortcut: entire update must go to cache
			if (ssize <= points[0] || (ssize <= points[5] && ssize <= 2940) )
			{
				ARCS_LOG(DEBUG4) << "Parition goes to cache entirely";

				return data_t { 0, 0, 0, 0, 0, 0 };
				// frame 445 is beyond this update
			}
		}


		if (frame450) // cache frames 445-454 + update frame 450
		{
			// static constexpr auto f445s { 261661 };
			// static constexpr auto f450s { 264601 };
			// static constexpr auto f450e { 265188 };
			// static constexpr auto f454e { 266952 };

			// begin of frame 445: don't cache before, cache afterwards
			const auto sc = ptrdiff_t { 445 * 588 - shift_m };

			// begin of frame 450: don't update before, update afterwards
			const auto s4 = ptrdiff_t { 450 * 588 - shift_m };

			// end of frame 450: update before, don't update afterwards
			const auto e4 = ptrdiff_t { 451 * 588 - shift_m };

			// end of frame 454: cache before, don't cache afterwards
			const auto ec = ptrdiff_t { 455 * 588 - shift_m };

			points[1] = (drive_offsets && sc < ssize) ? sc : 0;
			points[2] = (s4 < ssize) ? s4 : 0;
			points[3] = (e4 < ssize) ? e4 : 0;
			points[4] = (drive_offsets && ec < ssize) ? ec : 0;
		}

		// Now, every point that is > 0 is a split point,
		// before points 1+5 no caching is done
		return points;
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
		update_impl<B, E>(start, stop, size);
		//update_legacy<B, E>(start, stop, size);
	}

	template <class B, class E>
	void update_impl(B start, E stop, const std::size_t size)
	{
		using std::ptrdiff_t;
		using std::size_t;
		using std::cbegin;
		using std::cend;

		const auto cache_requested = bool { do_drive_offsets() };

		const auto points = sections(multiplier(), size, current_length(),
				cache_requested, do_frame450());

		ARCS_LOG(DEBUG4) << "split points: 0:" << points[0]
			<< ", 1:" << points[1] << ", 2:" << points[2]
			<< ", 3:" << points[3] << ", 4:" << points[4]
			<< ", 5:" << points[5];

		const auto has_points { std::any_of(cbegin(points), cend(points),
                               [](int x) { return x > 0; }) };

		// flag to turn caching on/off (while updating)
		auto is_cache_point = bool { false };

		if (has_points)
		{

			auto last = ptrdiff_t { 0 }; // last split point, 0 == start
			auto i    = size_t    { 0 }; // counter

			// use the positive points for splitting the update
			for (; i < points.size(); ++i)
			{
				if (points[i] > 0)
				{
					// don't cache for points 1 and 5 (as "to")
					is_cache_point = ((i - 1) * (i - 5)) > 0;

					ARCS_LOG(DEBUG4) << "update " << i
						<< ": from " << last << " to " << points[i]
						<< " (" << (points[i] - last) << "),"
						<< " cache: " << std::boolalpha << is_cache_point;

					if (i >= 2 && i <= 4)
					{
						ARCS_LOG(DEBUG4) << "(frame450)";

						// update the cache values for frame 450
						update_(start + last, start + points[i], st_, false);

					} else
					{
						update_(start + last, start + points[i], st_,
								cache_requested && is_cache_point);
					}

					last = points[i];
				}
			}

			// section between point 5 and the end of the update
			if (static_cast<size_t>(last) < size)
			{
				// cache iff this is the section from points[5] to end
				is_cache_point = (points.size() == i) && (points.back() > 0);

				const auto amount = size - static_cast<size_t>(last);

				// TODO If (amount == 2940 && is_last_track) { return };

				ARCS_LOG(DEBUG4) << "update _: from "
						<< last << " to " << size << " (" << amount << ")"
						<< ", cache: " << std::boolalpha << is_cache_point;

				update_(start + last, stop, st_,
						cache_requested && is_cache_point);
			}
		} else
		{
			// without points, the update has to be either entirely cached or
			// entirely uncached

			if (cache_requested)
			{
				// drive offsets requested but no actual split points:
				// cache iff the update is entirely in the first or last 2940
				// samples.

				const auto track_remain { current_length_ - (multiplier() - 1) };
				const auto remainder    { track_remain - size };

				is_cache_point =
					remainder <= 2940 || remainder >= track_remain - 2940;

				ARCS_LOG(DEBUG4) << "update is for cache: "
					<< (do_drive_offsets() && is_cache_point)
					<< " (" << size << " samples)";
			}

			if (do_frame450())
			{
				// TODO Must this case be handled?
			}

			update_(start, stop, st_, cache_requested && is_cache_point);
		}
	}

	/**
	 * \brief Set cache index position to start caching a track suffix.
	 */
	void set_cache_start_suffix()
	{
		// first index position where a part of the suffix is to be stored
		st_.idx_ = 2940;
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
	 *
	 * This function must be called exactly once after the last update
	 * contributing to the current track.
	 */
	void finalize()
	{
		ARCS_LOG(DEBUG4) << "Finalize track";

		if (do_drive_offsets())
		{
			update_.accumulate(st_); // do this exactly 1x
		}

		if (do_frame450())
		{
			update_.accumulate_f450(st_); // do this exactly 1x
		}
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
	 * \param[in] track_no     Track number from [1,99] (checked)
	 * \param[in] drive_offset Drive offset from [-2939,2940] (checked)
	 *
	 * \return Checksum value for track \c track_no, shifted by value
	 * \c drive_offset
	 */
	template <enum checksum::type TYPE>
	Checksum::value_type shift(const TrackNo track_no,
			const int drive_offset) const
	{
		if (drive_offset < -2939 || drive_offset > 2940)
		{
			throw std::runtime_error("Illegal drive offset requested");
		}

		const auto t = static_cast<std::size_t>(track_no - 1);

		if (t >= total_tracks() || t > CDDA::MAX_TRACKCOUNT)
		{
			throw std::runtime_error("Illegal track number requested");
		}

		const auto st = tracks_[t].subtotals();

		if (drive_offset == 0) // no actual shift required
		{
			return checksum<TYPE>(st);
		}

		// persistent part: present in previous and shifted sum
		auto wsum   = checksum<TYPE>(st); // weighted sum
		auto ssum   = current_cs_sum(st); // simple sum
		auto factor = int64_t { std::abs(drive_offset) }; // multiplier (signed)

		// added correction: present only in shifted sum
		auto a_wsum   = uint32_t { 0 };   // weighted sum
		auto a_ssum   = uint32_t { 0 };   // simple sum
		auto a_factor =  int64_t { 0 };   // multiplier (signed)

		// absolute (unsigned) amount of drive_offset
		const auto k = static_cast<std::size_t>(std::abs(drive_offset));

		if (drive_offset < 0)
		{
			// shift "leftwards": remove k highest indices, add k lower indices

			wsum -= last<TYPE> (k, st);
			ssum -= cs_sum_last(k, st);

			if (t == 0) // first track
			{
				a_wsum = first<TYPE> (2939, st) - first<TYPE> (2939 - k, st);
				a_ssum = cs_sum_first(2939, st) - cs_sum_first(2939 - k, st);
				a_factor = -drive_offset;
			} else // other than first
			{
				const auto prev = tracks_[t - 1].subtotals();
				const auto sz = AccessSt::track_size(prev.multiplier).samples();

				a_wsum   = last<TYPE> (k, prev);
				a_ssum   = cs_sum_last(k, prev);
				a_factor = -(sz - static_cast<int>(k));
			}
		}

		if (drive_offset > 0)
		{
			// shift "rightwards": remove k lowest indices, add k higher indices

			wsum  -= first<TYPE> (k, st);
			ssum  -= cs_sum_first(k, st);
			factor = -factor;

			if (t == this->total_tracks() - 1) // last track
			{
				a_wsum = last<TYPE> (2940, st) - last<TYPE> (2940 - k, st);
				a_ssum = cs_sum_last(2940, st) - cs_sum_last(2940 - k, st);
				a_factor = -drive_offset;
			} else // other than last
			{
				const auto next = tracks_[t + 1].subtotals();
				const auto sz = AccessSt::track_size(st.multiplier).samples();

				a_wsum   = first<TYPE> (k, next);
				a_ssum   = cs_sum_first(k, next);
				a_factor = sz - static_cast<int>(k);
			}
		}

		// Shift the part persistent in unshifted as well as shifted checksum.
		// Add shifted correction of incoming samples.
		return wsum + factor * ssum + (a_wsum + a_factor * a_ssum);

		// Note that the silent overflows are correct since we ignore the
		// overflows also deliberately on the original checksum.
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
		return tracks_.size() - 1;
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
		// FIXME For the last track, this will be 2940 samples too less

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
		current_subtotals().cache(start, stop); // increases multiplier

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
		// TODO Avoid useless caching of update() by publishing total samples
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

