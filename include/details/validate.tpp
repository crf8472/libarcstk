// vim:ft=cpp
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#error "Do not include validate.tpp directly, include identifier.hpp instead"
#endif

#ifndef __LIBARCSTK_VALIDATE_TPP__
#define __LIBARCSTK_VALIDATE_TPP__

/**
 * \file
 *
 * \internal
 *
 * \brief TOC data validation
 */


#include <cstdint>
#include <sstream>
#include <vector>

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__
#include "logging.hpp"
#endif

namespace arcstk
{
namespace details
{

inline namespace V_1_0_0
{


/**
 * \brief Abstracted YES/NO values for SFINAE
 */
struct sfinae_values
{
	using yes = char;
	using no  = yes[2];
};


/**
 * \brief Helper: check for an integral value_type
 *
 * \tparam T Input type to inspect
 */
template <typename T>
struct has_integral_value_type : private sfinae_values
{
private:

	// choose to return "yes" in case value_type is defined + integral
	template<typename S> static yes & test(
		typename std::enable_if<
			std::is_integral<typename S::value_type>::value, void>::type*
	);

	// "no" otherwise
	template<typename S> static no  & test(...);


public:

	/**
	 * \brief Result value
	 *
	 * Will be TRUE for types with an integral value_type, otherwise false.
	 */
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);

	/**
	 * \brief Input type
	 */
	using type = T;


	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of a const_iterator
 *
 * \tparam T Input type to inspect
 */
template <typename T>
struct has_const_iterator : private sfinae_values
{
private:

	// choose to return "yes" in case const_iterator is defined
	template <typename S> static yes & test(typename S::const_iterator*);

	// "no" otherwise
	template <typename S> static no  & test(...);


public:

	/**
	 * \brief Result value
	 *
	 * Will be TRUE for types with a const_iterator, otherwise false.
	 */
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);

	/**
	 * \brief Input type
	 */
	using type = T;


	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of a size
 *
 * \tparam T Input type to inspect
 */
template <typename T>
struct has_size : private sfinae_values
{
private:

	// choose to return "yes" in case size is defined
	template <typename S> static yes & test(
		typename std::enable_if<std::is_same<
			decltype(static_cast<typename S::size_type (S::*)() const>
				(&S::size)),
			typename S::size_type(S::*)() const>::value,
			void>::type*
	);

	// "no" otherwise
	template <typename S> static no  & test(...);


public:

	/**
	 * \brief Result value
	 *
	 * Will be TRUE for types with a size, otherwise false.
	 */
	static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);

	/**
	 * \brief Input type
	 */
	using type = T;


	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of begin() const
 *
 * \tparam T Input type to inspect
 */
template <typename T>
struct has_begin : private sfinae_values
{
private:

	// begin(): choose to return "yes" in case begin() const is defined
	template <typename S> static yes & test(
		typename std::enable_if<std::is_same<
			decltype(static_cast<typename S::const_iterator (S::*)() const>
				(&S::begin)),
			typename S::const_iterator(S::*)() const>::value,
			void>::type*
	);

	// "no" otherwise
	template <typename S> static no  & test(...);


public:

	/**
	 * \brief Result value
	 *
	 * Will be TRUE for types with begin() const, otherwise false.
	 */
	static bool const value = sizeof(test<T>(nullptr)) == sizeof(yes);

	/**
	 * \brief Input type
	 */
	using type = T;


	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of end() const
 *
 * \tparam T Input type to inspect
 */
template <typename T>
struct has_end : private sfinae_values
{
private:

	// end(): choose to return "yes" in case end() const is defined
	template <typename S> static yes & test(
		typename std::enable_if<std::is_same<
			decltype(static_cast<typename S::const_iterator (S::*)() const>
				(&S::end)),
			typename S::const_iterator(S::*)() const>::value,
			void>::type*
	);

	// "no" otherwise
	template <typename S> static no & test(...);


public:

	/**
	 * \brief Result value
	 *
	 * Will be TRUE for types with end() const, otherwise false.
	 */
	static bool const value = sizeof(test<T>(nullptr)) == sizeof(yes);

	/**
	 * \brief Input type
	 */
	using type = T;


	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: Check whether a container holds LBA frames.
 *
 * Defined for container types that define const_iterator as well
 * as an integral value_type, a size and have begin() const and end() const.
 *
 * \tparam The type to inspect
 */
template <typename T, typename T_noref = std::remove_reference_t<T>>
struct is_lba_container : public std::integral_constant<bool,
	has_integral_value_type<T_noref>::value &&
	has_const_iterator     <T_noref>::value &&
	has_size               <T_noref>::value &&
	has_begin              <T_noref>::value &&
	has_end                <T_noref>::value >
{
	// empty
};


// Example usage:
//
//template<typename Container>
//static typename std::enable_if<details::is_lba_container<Container>::value,
//	void>::type
//append(Container& to, const Container& from)
//{
//    using std::begin;
//    using std::end;
//    to.insert(end(to), begin(from), end(from));
//}

} // namespace details::v_1_0_0

} // namespace details


inline namespace v_1_0_0
{


/**
 * \brief Type for representing amounts of LBA frames
 */
using lba_count = uint32_t;


/**
 * \internal
 *
 * \brief Validates offsets, leadout and track count of a compact disc toc.
 *
 * TOCBuilder and ARIdBuilder use TOCValidator to validate
 * the input for building their respective instances.
 *
 * This class is considered an implementation detail.
 */
class TOCValidator final
{

public:

	/**
	 * \brief Validate offsets.
	 *
	 * It is ensured that the offsets are consistent, which means they all are
	 * within a CDDA conforming range, ordered in ascending order with a legal
	 * distance between any two subsequent offsets and their number is a valid
	 * track count.
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container,
		typename = typename std::enable_if_t<details::is_lba_container<Container>::value>
	>
	inline void validate_offsets(Container&& offsets) const;

	/**
	 * \copydoc validate_offsets(Container&&) const
	 */
	template <typename T,
		typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	inline void validate_offsets(std::initializer_list<T> offsets) const;

	/**
	 * \brief Validate offsets and track count.
	 *
	 * It is ensured that the number of offsets matches the track count and that
	 * the offsets are consistent.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container,
		typename = typename std::enable_if_t<details::is_lba_container<Container>::value>
	>
	inline void validate_offsets(const TrackNo track_count,
			Container&& offsets) const;

	/**
	 * \copydoc validate_offsets(TrackNo, Container&&) const
	 */
	template <typename T,
		typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	inline void validate_offsets(TrackNo track_count,
			std::initializer_list<T> offsets) const;

	/**
	 * \brief Validate TOC information.
	 *
	 * It is ensured that the number of offsets matches the track count, that
	 * the offsets are consistent and the leadout frame is consistent with the
	 * offsets.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] leadout     Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container,
		typename = typename std::enable_if_t<details::is_lba_container<Container>::value>
	>
	inline void validate(const TrackNo track_count,
			Container&& offsets,
			const lba_count leadout) const;

	/**
	 * \copydoc validate_offsets(TrackNo, Container&&, const lba_count) const
	 */
	template <typename T,
		typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	inline void validate(TrackNo track_count,
			std::initializer_list<T> offsets,
			const lba_count leadout) const;

	/**
	 * \brief Validate lengths.
	 *
	 * It is ensured that the lengths are consistent, which means they all have
	 * have a CDDA conforming minimal lengths, their sum is within a CDDA
	 * conforming range and their number is a valid track count. An
	 * InvalidMetadataException is thrown if the validation fails.
	 *
	 * \param[in] lengths Lengths (in LBA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container,
		typename = typename std::enable_if_t<details::is_lba_container<Container>::value>
	>
	inline void validate_lengths(Container&& lengths) const;

	/**
	 * \copydoc validate_lengths(Container&&) const
	 */
	template <typename T,
		typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	inline void validate_lengths(std::initializer_list<T> lengths) const;

	/**
	 * \brief Validate leadout frame.
	 *
	 * It is ensured that the leadout is within a CDDA conforming range. An
	 * InvalidMetadataException is thrown if the validation fails.
	 *
	 * \param[in] leadout Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	inline void validate_leadout(const lba_count leadout) const;

	/**
	 * \brief Validate leadout frame.
	 *
	 * Equivalent to validate_leadout(), but furthermore ensures that \c leadout
	 * is not 0.
	 *
	 * \param[in] leadout Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	inline void validate_nonzero_leadout(const lba_count leadout) const;

	/**
	 * \brief Validate track count.
	 *
	 * It is ensured that the track count is within a CDDA conforming range. An
	 * InvalidMetadataException is thrown if the validation fails.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	inline void validate_trackcount(const TrackNo track_count) const;

	/**
	 * \brief Validate TOC information and leadout.
	 *
	 * It is ensured that the leadout frame is consistent with the offsets.
	 *
	 * \param[in] toc     TOC information
	 * \param[in] leadout Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	inline void validate(const TOC &toc, const lba_count leadout) const;


protected:

	/**
	 * \brief Ensure that \c prev_track and \c next_track offsets have at least minimal
	 * distance
	 *
	 * \param[in] prev_track Previous track offset
	 * \param[in] next_track Next track offset
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	inline void have_min_dist(const lba_count prev_track,
			const lba_count next_track) const;

	/**
	 * \brief Maximal valid offset value for a non-redbook 90 min CD (in CDDA frames).
	 *
	 * Non-redbook 90-min CD has 89:59.74 which is equivalent to 405.000 frames.
	 */
	static constexpr lba_count MAX_OFFSET_90 = (89 * 60 + 59) * 75 + 74;

	/**
	 * \brief Maximal valid offset value for a non-redbook 99 min CD (in CDDA frames).
	 *
	 * Non-redbook 99-min CD has 98:59.74 which is equivalent to 445.500 frames.
	 */
	static constexpr lba_count MAX_OFFSET_99 = (98 * 60 + 59) * 75 + 74;
};


// TOCValidator


template <typename Container, typename>
void TOCValidator::validate_offsets(Container&& offsets) const
{
	// Number of offsets in legal range?

	if (offsets.size() == 0)
	{
		std::stringstream ss;
		ss << "No offsets were given. Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	if (offsets.size() >
			static_cast<decltype(offsets.size())>(CDDA.MAX_TRACKCOUNT))
	{
		std::stringstream ss;
		ss << "Offsets are only possible for at most "
			<< CDDA.MAX_TRACKCOUNT << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// Explicitly allow the offset of the first track to be 0

	if (offsets[0] < 0)
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with negative offset for first track: "
			<< std::to_string(offsets[0]);

		throw InvalidMetadataException(ss.str());
	}

	// Check whether all subsequent Offsets have minimum distance

	for (std::size_t i = 1; i < offsets.size(); ++i)
	{
		// Is offset in a CDDA-legal range?

		if (offsets[i] > static_cast<int64_t>(CDDA.MAX_OFFSET))
		{
			std::stringstream ss;
			ss << "Offset " << std::to_string(offsets[i])
				<< " for track " << std::to_string(i);

			if (offsets[i] > static_cast<int64_t>(MAX_OFFSET_99))
			{
				ss << " exceeds physical range of 99 min ("
					<< std::to_string(MAX_OFFSET_99) << " frames)";
			} else if (offsets[i] > static_cast<int64_t>(MAX_OFFSET_90))
			{
				ss << " exceeds physical range of 90 min ("
					<< std::to_string(MAX_OFFSET_90) << " frames)";
			} else
			{
				ss << " exceeds redbook maximum duration of "
					<< std::to_string(CDDA.MAX_OFFSET);
			}

			throw InvalidMetadataException(ss.str());
		}

		// Has offset for current track at least minimum distance after
		// offset for last track?

		this->have_min_dist(
				static_cast<lba_count>(offsets[i-1]),
				static_cast<lba_count>(offsets[i]));
	} // for
}


template <typename T, typename>
void TOCValidator::validate_offsets(std::initializer_list<T> offsets) const
{
	// TODO works, but performance hurts
	this->validate_offsets(std::vector<T>{offsets});
}


template <typename Container, typename>
void TOCValidator::validate_offsets(const TrackNo track_count,
		Container&& offsets) const
{
	this->validate_trackcount(track_count);

	// Validation: Track count consistent with Number of Offsets?

	auto offsets_count = offsets.size();

	if (offsets_count != static_cast<decltype(offsets_count)>(track_count))
	{
		std::stringstream ss;
		ss << "Track count does not match offset count." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	this->validate_offsets(offsets);
}


template <typename T, typename>
void TOCValidator::validate_offsets(const TrackNo track_count,
		std::initializer_list<T> offsets) const
{
	// TODO works, but performance hurts
	this->validate_offsets(track_count, std::vector<T>{offsets});
}


template <typename Container, typename>
void TOCValidator::validate(const TrackNo track_count,
		Container&& offsets, const lba_count leadout) const
{
	this->validate_leadout(leadout);

	// Validation: Leadout in Valid Distance after Last Offset?

	if (leadout <
			offsets.back() + static_cast<int64_t>(CDDA.MIN_TRACK_LEN_FRAMES))
	{
		std::stringstream ss;
		ss << "Leadout frame " << leadout
			<< " is too near to last offset " << offsets.back()
			<< ". Minimal distance is " << CDDA.MIN_TRACK_LEN_FRAMES
			<< " frames." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	this->validate_offsets(track_count, offsets);
}


template <typename T, typename>
void TOCValidator::validate(const TrackNo track_count,
		std::initializer_list<T> offsets, const lba_count leadout) const
{
	// TODO works, but performance hurts
	this->validate(track_count, std::vector<T>{offsets}, leadout);
}


template <typename Container, typename>
void TOCValidator::validate_lengths(Container&& lengths) const
{
	// Number of lengths in legal range?

	if (lengths.size() == 0)
	{
		std::stringstream ss;
		ss << "No lengths were given. Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	if (static_cast<TrackNo>(lengths.size()) > CDDA.MAX_TRACKCOUNT)
	{
		std::stringstream ss;
		ss << "Lengths are only possible for at most "
			<< CDDA.MAX_TRACKCOUNT << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// Length values are valid?

	lba_count sum_lengths = 0;

	// Skip last length, if it is not known (e.g. 0 or -1)
	int tracks = (lengths.back() < 1) ? lengths.size() - 1 : lengths.size();

	for (std::size_t i = 0; i < static_cast<std::size_t>(tracks); ++i)
	{
		if (lengths[i] < static_cast<int64_t>(CDDA.MIN_TRACK_LEN_FRAMES))
		{
			std::stringstream ss;
			ss << "Cannot construct TOC with illegal length "
				<< std::to_string(lengths[i]) << " for track "
				<< std::to_string(i+1);

			throw InvalidMetadataException(ss.str());
		}

		sum_lengths += static_cast<lba_count>(lengths[i]);
	}

	// Sum of all lengths in legal range ?

	if (sum_lengths > CDDA.MAX_OFFSET)
	{
		std::stringstream ss;
		ss << "Total length " << std::to_string(sum_lengths);

		if (sum_lengths > MAX_OFFSET_99) // more than 99 min? => throw
		{
			ss << " exceeds physical range of 99 min ("
				<< std::to_string(MAX_OFFSET_99) << " frames)";

			throw InvalidMetadataException(ss.str());

		} else if (sum_lengths > MAX_OFFSET_90) // more than 90 min? => warn
		{
			ss << " exceeds redbook maximum of "
				<< std::to_string(MAX_OFFSET_90) << " frames (90 min)";

			ARCS_LOG_WARNING << ss.str();
			// TODO NonstandardMetadataException

		} else // more than redbook originally defines? => info
		{
			ss << " exceeds redbook maximum of "
				<< std::to_string(CDDA.MAX_OFFSET);

			ARCS_LOG_INFO << ss.str();
			// TODO NonstandardMetadataException
		}
	}
}


template <typename T, typename>
void TOCValidator::validate_lengths(std::initializer_list<T> lengths) const
{
	this->validate_lengths(std::vector<T>{lengths});
}


void TOCValidator::validate_leadout(const lba_count leadout) const
{
	// Greater than Minimum ?

	if (static_cast<int64_t>(leadout) < CDDA.MIN_TRACK_OFFSET_DIST)
	{
		std::stringstream ss;
		ss << "Leadout " << leadout
			<< " is smaller than minimum track length";

		throw InvalidMetadataException(ss.str());
	}

	// Less than Maximum ?

	if (leadout > CDDA.MAX_BLOCK_ADDRESS)
	{
		std::stringstream ss;
		ss << "Leadout " << leadout << " exceeds physical maximum";

		throw InvalidMetadataException(ss.str());
	}

	// Warning ?

	if (leadout > CDDA.MAX_OFFSET)
	{
		ARCS_LOG_WARNING << "Leadout " << leadout
			<< " exceeds redbook maximum";

		// TODO NonstandardMetadataException
	}
}


void TOCValidator::validate_nonzero_leadout(const lba_count leadout) const
{
	// Not zero ?

	if (leadout == 0)
	{
		std::stringstream ss;
		ss << "Leadout must not be 0";

		throw InvalidMetadataException(ss.str());
	}

	this->validate_leadout(leadout);
}


void TOCValidator::validate_trackcount(const TrackNo track_count) const
{
	if (track_count < 1 or track_count > 99)
	{
		std::stringstream ss;
		ss << "Cannot construct TOC from invalid track count: "
			<< std::to_string(track_count);

		throw InvalidMetadataException(ss.str());
	}
}


void TOCValidator::validate(const TOC &toc, const lba_count leadout) const
{
	this->validate_leadout(leadout);

	auto last_offset { toc.offset(toc.track_count()) };
	this->have_min_dist(last_offset, leadout);
}


void TOCValidator::have_min_dist(const lba_count prev_track,
		const lba_count next_track) const
{
	if (next_track < prev_track + CDDA.MIN_TRACK_OFFSET_DIST)
	{
		std::stringstream ss;
		ss << "Track " << next_track
			<< " is too near to last track offset " << prev_track
			<< ". Minimal distance is " << CDDA.MIN_TRACK_LEN_FRAMES
			<< " frames." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}
}

} // namespace v_1_0_0

} // namespace arcstk

#endif

