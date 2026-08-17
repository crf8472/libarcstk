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
#include <memory>         // for make_unique, unique_ptr, swap
#include <string>         // for string
#include <utility>        // for pair

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
	 * \brief Current multiplier.
	 */
	uint_fast64_t multiplier  { 1 };

	/**
	 * \brief Current subtotal for ARCSv1.
	 */
	uint_fast32_t subtotal_v1 { 0 };

	/**
	 * \brief Current subtotal for ARCSv2.
	 */
	uint_fast32_t subtotal_v2 { 0 };

	/**
	 * \copydoc SNPT_nf_swap
	 */
	friend void swap(Subtotals& lhs, Subtotals& rhs) noexcept
	{
		using std::swap;

		swap(lhs.multiplier,  rhs.multiplier);
		swap(lhs.subtotal_v1, rhs.subtotal_v1);
		swap(lhs.subtotal_v2, rhs.subtotal_v2);
	}
};


/**
 * \brief Return Checksum value type.
 *
 * \param[in] v Subtotal
 *
 * \return Result as Checksum value
 */
inline Checksum::value_type to_value(const uint_fast32_t v)
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
inline AudioSize track_size(const uint_fast64_t m)
{
	using arcstk::UNIT;

	// cast is save for valid input data
	return { static_cast<int32_t>(m - 1), UNIT::SAMPLES };
}


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
{
	using type = checksum::type;

public:

	std::string id_string() const
	{
		return "v1";
	}

	template <class B, class E>
	void operator()(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			st.subtotal_v1 += st.multiplier * (*pos) & LOWER_32_BITS_;
		}
	}

	ChecksumSet value(const Subtotals& st) const
	{
		return {
			track_size(st.multiplier),
			{{ type::ARCS1, Checksum { to_value(st.subtotal_v1) } }}
		};
	}
};


// AccurateRip v2
template <>
class Update<checksum::type::ARCS2>
{
	/**
	 * \brief Current update factor.
	 */
	mutable uint_fast64_t update_ { 0 };

	using type = checksum::type;

public:

	std::string id_string() const
	{
		return "v2";
	}

	template <class B, class E>
	void operator()(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			update_ = st.multiplier * (*pos);

			st.subtotal_v2 += (update_ & LOWER_32_BITS_) + (update_ >> 32u);
		}
	}

	ChecksumSet value(const Subtotals& st) const
	{
		return {
			track_size(st.multiplier),
			{{ type::ARCS2, Checksum { to_value(st.subtotal_v2) } }}
		};
	}
};


// AccurateRip v1+2
template <>
class Update<checksum::type::ARCS1, checksum::type::ARCS2>
{
	/**
	 * \brief Current update factor.
	 */
	mutable uint_fast64_t update_ { 0 };

	using type = checksum::type;

public:

	std::string id_string() const
	{
		return "v1+2";
	}

	template <class B, class E>
	void operator()(const B& start, const E& stop, Subtotals& st) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		for (auto pos = start; pos != stop; ++pos, ++st.multiplier)
		{
			update_ = st.multiplier * (*pos);

			st.subtotal_v1 += update_ & LOWER_32_BITS_;
			st.subtotal_v2 += (update_ >> 32u);
		}
	}

	ChecksumSet value(const Subtotals& st) const
	{
		return {
			track_size(st.multiplier),
			{
				{ type::ARCS1, Checksum { to_value(st.subtotal_v1) } },
				{ type::ARCS2, Checksum { to_value(
						st.subtotal_v1 + st.subtotal_v2) } },
			}
		};
	}
};


// /**
//  * \brief Set of specified Checksum types.
//  *
//  * \tparam T1 First Checksum type
//  * \tparam T2 Trailing Checksum types
//  *
//  * \return Set of Checksum types
//  */
// template <enum checksum::type T1, enum checksum::type... T2>
// inline ChecksumtypeSet types_set()
// {
// 	return { T1, T2... };
// }


// /**
//  * \brief AccurateRip algorithm name string.
//  *
//  * \tparam T1 First Checksum type
//  * \tparam T2 Trailing Checksum types
//  *
//  * \return Name of the Algorithm computing the specified Checksum types
//  */
// template <enum checksum::type T1, enum checksum::type... T2>
// inline std::string name_string()
// {
// 	auto ss = std::ostringstream {};
//
// 	#pragma GCC diagnostic push
// 	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
//
// 	const auto append = [&ss](const auto& type_val)
// 	{
// 		ss << ", " << type_val;
// 	};
//
// 	#pragma GCC diagnostic pop
//
// 	ss <<  "AccurateRip " << T1;
// 	(append(T2), ...);
//
// 	return ss.str();
// }


/**
 * \brief Determine the legal range of samples for the AccurateRip calculation.
 *
 * \param[in] ctx    The Context for calculation
 * \param[in] size   The input size of samples to process
 * \param[in] points The offset points in number of PCM samples
 *
 * \return Input range of 1-based sample indices to use for calculation
 */
inline std::pair<int32_t, int32_t> legal_range(const Context ctx,
		const AudioSize& size, const Points& points)
{
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
	 * \brief Internal subtotals.
	 */
	Subtotals st_ {};

	/**
	 * \brief Internal update strategy.
	 */
	Update<T1, T2...> update_ {};

public:

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
	 * \brief Update the instance by a sequence of samples.
	 *
	 * \tparam B Type of the begin iterator
	 * \tparam E Type of the end iterator
	 *
	 * \param[in] start The start position
	 * \param[in] stop  The stop position
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
	ChecksumSet value() const
	{
		return update_.value(st_);
	}

	/**
	 * \brief Reset the instance to its initial state.
	 */
	void reset()
	{
		st_.subtotal_v1 = 0;
		st_.subtotal_v2 = 0;
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

		swap(this->st_,     rhs.st_);
		swap(this->update_, rhs.update_);
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
	 * \brief Algorithm state.
	 */
	UpdateableSubtotals<T1, T2...> state_ {};

	/**
	 * \brief Current result of performing the algorithm.
	 */
	ChecksumSet current_result_ {};

	/**
	 * \brief Non-virtual implementation of do_setup() for constructor.
	 */
	void do_setup_impl(const Context c)
	{
		ARCS_LOG(DEBUG1) << "Context for Algorithm: " << to_string(c);

		// Adjust multiplier only for Context FIRST_TRACK
		if (any(Context::FIRST_TRACK & c))
		{
			state_.set_multiplier(NUM_SKIP_SAMPLES::FRONT + 1);
		}

		ARCS_LOG(DEBUG1) << "Initialize multiplier to: " << state_.multiplier();
	}

	// Algorithm

	void do_setup(const Context c) final
	{
		this->do_setup_impl(c);
	}

	std::string do_name() const final
	{
		return "AccurateRip " + state_.id_string();
	}

	ChecksumtypeSet do_types() const final
	{
		return state_.types();
	}

	std::pair<int32_t, int32_t> do_range(const AudioSize& size,
			const Points& points) const final
	{
		return legal_range(this->context(), size, points);
	}

	ChecksumSet do_result() const final
	{
		return current_result_;
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
		this->do_setup_impl(c);
	}

	/**
	 * \copydoc SNPT_sm_default_dtor
	 */
	~ARCSAlgorithm() final = default;

	/**
	 * \brief Pass samples coming before the actual range of the algorithm.
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
	void perform_pre_range(B /* start */, E /* stop */)
	{
		//this->skip(start, stop, pre_);
	}

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
	 * \brief Pass samples coming after the actual range of the algorithm.
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
	void perform_post_range(B /* start */, E /* stop */)
	{
		//this->skip(start, stop, post_);
	}

	/**
	 * \brief Finish current track.
	 *
	 * Save the subtotals for current track and start next track.
	 *
	 * \param[in] t       Track number (ignored)
	 * \param[in] length  Track length as calculated
	 */
	void perform_finish_track(const int /*t*/, const AudioSize& length)
	{
		current_result_ = state_.value();
		current_result_.set_length(length);

		state_.reset();
		state_.set_multiplier(1);
	}

	/**
	 * \copydoc SNPT_mf_swap
	 */
	void swap(ARCSAlgorithm& rhs) noexcept
	{
		this->swap_base(rhs);

		using std::swap;

		swap(this->state_,          rhs.state_);
		swap(this->current_result_, rhs.current_result_);
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

