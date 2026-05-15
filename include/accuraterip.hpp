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

#include <cstdint>        // for uint_fast32_t, uint_fast64_t, int32_t
#include <memory>         // for unique_ptr, swap
#include <string>         // for string
#include <unordered_set>  // for unordered_set

#ifndef LIBARCSTK_ALGORITHM_HPP_
#include "algorithm.hpp"    // for Algorithm, Updateable
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"     // for checksum::type, ChecksumSet
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
struct Subtotals
{
	/**
	 * \brief Current multiplier.
	 */
	uint_fast64_t multiplier  = 1;

	/**
	 * \brief Current update factor.
	 */
	uint_fast64_t update      = 0;

	/**
	 * \brief Current subtotal for ARCSv1.
	 */
	uint_fast32_t subtotal_v1 = 0;

	/**
	 * \brief Current subtotal for ARCSv2.
	 */
	uint_fast32_t subtotal_v2 = 0;

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(Subtotals& lhs, Subtotals& rhs) noexcept
	{
		using std::swap;

		swap(lhs.multiplier,  rhs.multiplier);
		swap(lhs.update,      rhs.update);
		swap(lhs.subtotal_v1, rhs.subtotal_v1);
		swap(lhs.subtotal_v2, rhs.subtotal_v2);
	}
};


/**
 * \brief Functor for performing the actual update.
 *
 * \tparam T1 First checksum type
 * \tparam T2 More checksum types
 */
template <enum checksum::type T1, enum checksum::type... T2>
struct Update;


// AccurateRip v1
template <>
struct Update<checksum::type::ARCS1>
{
	template <class B, class E>
	void operator()(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			st.subtotal_v1 += st.multiplier * (*pos) & LOWER_32_BITS_;
		}
	}

	ChecksumSet value(const Subtotals& st) const;
	std::string id_string() const;
};


// AccurateRip v2
template <>
struct Update<checksum::type::ARCS2>
{
	template <class B, class E>
	void operator()(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			st.update = st.multiplier * (*pos);

			st.subtotal_v2 +=
				(st.update & LOWER_32_BITS_) + (st.update >> 32u);
		}
	}

	ChecksumSet value(const Subtotals& st) const;
	std::string id_string() const;
};


// AccurateRip v1+2
template <>
struct Update<checksum::type::ARCS1, checksum::type::ARCS2>
{
	template <class B, class E>
	void operator()(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			st.update       = st.multiplier * (*pos);
			st.subtotal_v1 += st.update & LOWER_32_BITS_;
			st.subtotal_v2 += (st.update >> 32u);
		}
	}

	ChecksumSet value(const Subtotals& st) const;
	std::string id_string() const;
};


/**
 * \brief Set of specified checksum types.
 */
template <enum checksum::type T1, enum checksum::type... T2>
std::unordered_set<checksum::type> types_set()
{
	return { T1, T2... };
}


/**
 * \brief Interface and base class for updatable subtotals.
 */
template<enum checksum::type T1, enum checksum::type... T2>
class UpdateableSubtotals final
{
	/**
	 * \brief Internal subtotals.
	 */
	Subtotals st_;

	/**
	 * \brief Internal update strategy.
	 */
	Update<T1, T2...> update_;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	UpdateableSubtotals();

	/**
	 * \brief Current Multiplier of this instance.
	 *
	 * \return Current multiplier
	 */
	uint_fast64_t multiplier() const;

	/**
	 * \brief Set multiplier to a new value.
	 *
	 * \param[in] m New value for multiplier
	 */
	void set_multiplier(const uint_fast64_t m);

	/**
	 * \brief Update the instance by a sequence of samples.
	 *
	 * \tparam B Type of the begin iterator
	 * \tparam E Type of the end iterator
	 *
	 * \param[in] start The start position (part of update)
	 * \param[in] stop  The stop position (not part of update)
	 */
	template <class B, class E>
	void update(B start, E stop)
	{
		update_(start, stop, st_);
	}

	/**
	 * \brief Get the current updated value from the Updatable.
	 *
	 * The length is the actual length based on the total number of samples the
	 * instance has been updated.
	 *
	 * \return The current subtotal
	 */
	ChecksumSet value() const;

	/**
	 * \brief Reset the instance to its initial state.
	 */
	void reset();

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
	std::unordered_set<checksum::type> types() const
	{
		return types_set<T1, T2...>();
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(UpdateableSubtotals& rhs) noexcept;

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
 */
template<enum checksum::type T1, enum checksum::type... T2>
class ARCSAlgorithm final : public Updateable<ARCSAlgorithm<T1, T2...>>
{
	/**
	 * \brief Algorithm state..
	 */
	UpdateableSubtotals<T1, T2...> state_;

	/**
	 * \brief Current result of performing the algorithm.
	 */
	ChecksumSet current_result_;

	// Algorithm

	void do_setup(const Context s) final;

	void do_track_finished(const int t, const AudioSize& length) final;

	ChecksumSet do_result() const final;

	std::unordered_set<checksum::type> do_types() const final;

	std::pair<int32_t, int32_t> do_range(const AudioSize& size,
			const Points& points) const final;

	std::unique_ptr<Algorithm> do_clone() const final;

public:

	/**
	 * \copydoc SNPT_sm_default_ctor
	 */
	ARCSAlgorithm();

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~ARCSAlgorithm() final = default;

	/**
	 * \brief Update the instance by a sequence of samples.
	 *
	 * \tparam B Type of the begin iterator
	 * \tparam E Type of the end iterator
	 *
	 * \param[in] start The start position (part of update)
	 * \param[in] stop  The stop position (not part of update)
	 */
	template <class B, class E>
	void perform_update(B start, E stop)
	{
		ARCS_LOG(DEBUG3) << "First multiplier: " << state_.multiplier();

		state_.update(start, stop);

		ARCS_LOG(DEBUG3) << "Last multiplier:  " << state_.multiplier() - 1;
		// -1 because multiplier_ has already been updated to next input
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(ARCSAlgorithm& rhs) noexcept;

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

