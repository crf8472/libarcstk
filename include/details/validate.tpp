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
#include <initializer_list>
#include <sstream>
#include <type_traits>
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
inline namespace v_1_0_0
{

/// \internal \addtogroup id
/// @{

/**
 * \brief Require T being either signed or unsigned integral "arithmetic"
 * (non-ref, non-pointer) type with at least 32 bit width.
 *
 * An "LBA type" holds amounts of LBA frames, that means it must be able to
 * express CDDA.MAX_OFFSET which excludes types with less than 32 bits width.
 *
 * Furthermore, it should be possible to perform at least addition of the types.
 * (Ref-types and pointer types would have to be dereferenced for that. This
 * may be added in the future.)
 *
 * The intention is to include exactly the non-ref, non-pointer variants of
 * short, int, long and long long.
 */
template <typename T>
using IsLBAType = std::enable_if_t<
	std::is_integral<T>::value   && // TODO C++17: is_integral_v
	std::is_arithmetic<T>::value && // TODO C++17: is_arithmetic_v
	sizeof(T) >= 4,
void>;


/**
 * \brief Abstracted YES/NO values for SFINAE
 */
struct sfinae_values
{
	using yes = char;
	using no  = yes[2];
};


/**
 * \brief Helper: check for a \c value_type with at least 32 bit width that is
 * of integer type and as well arithmetic.
 *
 * \tparam T Input type to inspect
 */
template <typename T>
struct has_lba_value_type : private sfinae_values
{
private:

	// choose to return "yes" in case value_type is defined, integral,
	// arithmetic and at least 32 bit wide
	template<typename S, typename = IsLBAType<typename S::value_type>>
	static yes & test(typename S::value_type*);

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
 * \brief Require T having a signed or unsigned value_type of int, short or long
 */
template <typename T>
using HasLBAValueType =
	std::enable_if_t<has_lba_value_type<std::remove_reference_t<T>>::value>;


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
 * \brief Require a const_iterator for T
 */
template <typename T>
using HasConstInterator =
	std::enable_if_t<has_const_iterator<std::remove_reference_t<T>>::value>;


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


template <typename T>
using HasSize = std::enable_if_t<has_size <std::remove_reference_t<T>>::value>;


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
 * \brief Require T to have <tt>begin() const</tt>
 */
template <typename T>
using HasBegin = std::enable_if_t<has_begin<std::remove_reference_t<T>>::value>;


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
 * \brief Require T to have <tt>end() const</tt>
 */
template <typename T>
using HasEnd = std::enable_if_t<has_end<std::remove_reference_t<T>>::value>;


/**
 * \brief Require T to be a const-iterable container type with a size
 */
template <typename T, typename T_noref = std::remove_reference_t<T>>
struct is_container : public std::integral_constant<bool,
	has_const_iterator <T_noref>::value &&
	has_size           <T_noref>::value &&
	has_begin          <T_noref>::value &&
	has_end            <T_noref>::value >
{
	// empty
};


/**
 * \brief Helper: Check whether a container holds LBA frames.
 *
 * Defined for container types that define const_iterator as well
 * as an integral value_type, a size and have begin() const and end() const.
 *
 * \tparam The type to inspect
 */
template <typename T, typename = HasLBAValueType<T> >
struct is_lba_container : public is_container<T> { /* empty */ };


/**
 * \brief Require T being a const-iterable integral-numeric container of frames
 *
 * This is intended to require a container type holding offsets or lengths.
 */
template <typename T>
using IsLBAContainer = std::enable_if_t<is_lba_container<T>::value>;


/**
 * \brief Indicate whether T is a type for representing filenames.
 *
 * Currently is_filename_type<T> is of std::true_type for T = std::string and
 * of std::false_type for every other type T.
 *
 * Intended to be specialized by client.
 *
 * \tparam T Type to be classified as representing a filename
 */
template <typename T>
struct is_filename_type : std::false_type { /* empty */ };

// TRUE for std::string
template <>
struct is_filename_type<std::string> : std::true_type { /* empty */ };


/**
 * \brief Helper: check for \c value_type  std::string.
 *
 * \tparam T Input type to inspect
 */
template <typename T>
struct has_filename_value_type : private sfinae_values
{
private:

	// choose to return "yes" in case value_type can represent filenames
	template<typename S, typename = is_filename_type<typename S::value_type>>
	static yes & test(typename S::value_type*);

	// "no" otherwise
	template<typename S> static no  & test(...);


public:

	/**
	 * \brief Result value
	 *
	 * Will be TRUE for types whose value_type is std::string, otherwise false.
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
 * \brief Requires T to have a \c value_type of std::string
 */
template <typename T>
using HasFilenameValueType =
std::enable_if_t<has_filename_value_type<std::remove_reference_t<T>>::value>;


/**
 * \brief Requires T to be a filename container
 */
template <typename T, typename = HasFilenameValueType<T> >
struct is_filename_container : public is_container<T> { /* empty */ };


/**
 * \brief Require T being a const-iterable integral-numeric container of frames
 *
 * This is intended to require a container type holding offsets or lengths.
 */
template <typename T>
using IsFilenameContainer = std::enable_if_t<is_filename_container<T>::value>;

/** @} */


/**
 * \internal
 * \ingroup id
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
	 * \tparam Container The container holding offset values
	 *
	 * \param[in] offsets Offsets (in LBA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container, typename = IsLBAContainer<Container>>
	inline void validate_offsets(Container&& offsets) const;

	/**
	 * \copydoc validate_offsets(Container&&) const
	 */
	template <typename T, typename = IsLBAType<T>>
	inline void validate_offsets(std::initializer_list<T> offsets) const;

	// Commented out: alternative to initializer_lists: C-style arrays
	//template <typename T, std::size_t S>
	//inline void validate_offsets(T const (&list)[S]) const;

	/**
	 * \brief Validate offsets and track count.
	 *
	 * It is ensured that the number of offsets matches the track count and that
	 * the offsets are consistent.
	 *
	 * \tparam Container The container holding offset values
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container, typename = IsLBAContainer<Container>>
	inline void validate_offsets(const TrackNo track_count,
			Container&& offsets) const;

	/**
	 * \copydoc validate_offsets(TrackNo, Container&&) const
	 */
	template <typename T, typename = IsLBAType<T>>
	inline void validate_offsets(TrackNo track_count,
			std::initializer_list<T> offsets) const;

	/**
	 * \brief Validate TOC information.
	 *
	 * It is ensured that the number of offsets matches the track count, that
	 * the offsets are consistent and the leadout frame is consistent with the
	 * offsets.
	 *
	 * \tparam Container The container holding offset values
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in CDDA frames) of each track
	 * \param[in] leadout     Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container, typename = IsLBAContainer<Container>>
	inline void validate(const TrackNo track_count,
			Container&& offsets,
			const lba_count leadout) const;

	/**
	 * \copydoc validate(TrackNo, Container&&, const lba_count) const
	 */
	template <typename T, typename = IsLBAType<T>>
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
	 * \tparam Container The container holding offset values
	 *
	 * \param[in] lengths Lengths (in LBA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container, typename = IsLBAContainer<Container>>
	inline void validate_lengths(Container&& lengths) const;

	/**
	 * \copydoc validate_lengths(Container&&) const
	 */
	template <typename T, typename = IsLBAType<T>>
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

/// \cond UNDOC_FUNCTION_BODIES


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

	auto track { std::begin(offsets) };

	if (*track < 0)
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with negative offset for first track: "
			<< std::to_string(offsets[0]);

		throw InvalidMetadataException(ss.str());
	}

	// FIXME Check for max offset exceed before the first track?

	++track;

	if (std::end(offsets) == track) // size == 1
	{
		return;
	}

	// Check whether all subsequent Offsets have minimum distance

	auto previous_track { std::begin(offsets) };
	auto finished       { std::end(offsets)   };

	auto t = 2;
	for (; track != finished; ++previous_track, ++track)
	{
		// Is offset in a CDDA-legal range?

		if (*track > static_cast<int64_t>(CDDA.MAX_OFFSET))
		{
			std::stringstream ss;
			ss << "Offset " << std::to_string(*track)
				<< " for track " << std::to_string(t);

			if (*track > static_cast<int64_t>(MAX_OFFSET_99))
			{
				ss << " exceeds physical range of 99 min ("
					<< std::to_string(MAX_OFFSET_99) << " frames)";
			} else if (*track > static_cast<int64_t>(MAX_OFFSET_90))
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
				static_cast<lba_count>(*previous_track),
				static_cast<lba_count>(*track));

		++t;
	} // for
}


template <typename T, typename>
void TOCValidator::validate_offsets(std::initializer_list<T> offsets) const
{
	// FIXME Works, but performance hurts. Just pass list?
	this->validate_offsets(std::vector<T>{offsets});
}


// Commented out: alternative to initializer_lists: C-style arrays
//template <typename T, std::size_t S>
//inline void TOCValidator::validate_offsets(T const (&list)[S]) const
//{
//	this->validate_offsets<T[S]>(list); // FIXME: Size??
//}


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

	this->validate_offsets(std::forward<Container>(offsets));
}


template <typename T, typename>
void TOCValidator::validate_offsets(const TrackNo track_count,
		std::initializer_list<T> offsets) const
{
	// FIXME Works, but performance hurts. Just pass list?
	this->validate_offsets(track_count, std::vector<T>{offsets});
}


template <typename Container, typename>
void TOCValidator::validate(const TrackNo track_count,
		Container&& offsets, const lba_count leadout) const
{
	this->validate_leadout(leadout);

	// Validation: Leadout in Valid Distance after Last Offset?

	auto last_track { std::end(offsets) - 1 };

	if (leadout < *last_track + static_cast<int64_t>(CDDA.MIN_TRACK_LEN_FRAMES))
	{
		std::stringstream ss;
		ss << "Leadout frame " << leadout
			<< " is too near to last offset " << *last_track
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
	// FIXME Works, but performance hurts. Just pass list?
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

	if (lengths.size() >
			static_cast<decltype(lengths.size())>(CDDA.MAX_TRACKCOUNT))
	{
		std::stringstream ss;
		ss << "Lengths are only possible for at most "
			<< CDDA.MAX_TRACKCOUNT << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// Length values are valid?

	lba_count sum_lengths = 0;

	auto last { std::end(lengths) - 1 };
	auto t { 1 };

	if (*last > 0) { ++last; } // If last length is actually known, validate it
	for (auto track { std::begin(lengths) }; track != last; ++track)
	{
		if (*track < static_cast<int64_t>(CDDA.MIN_TRACK_LEN_FRAMES))
		{
			std::stringstream ss;
			ss << "Cannot construct TOC with illegal length "
				<< std::to_string(*track) << " for track "
				<< std::to_string(t);

			throw InvalidMetadataException(ss.str());
		}

		sum_lengths += static_cast<lba_count>(*track);
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

			throw NonstandardMetadataException(ss.str());

		} else // more than redbook originally defines? => info
		{
			ss << " exceeds redbook maximum of "
				<< std::to_string(CDDA.MAX_OFFSET);

			throw NonstandardMetadataException(ss.str());
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
		std::stringstream ss;
		ss << "Leadout " << leadout << " exceeds redbook maximum";

		throw NonstandardMetadataException(ss.str());
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
		ss << "Track with offset " << prev_track
			<< " is too short. Next track starts at " << next_track
			<< " but minimal distance is " << CDDA.MIN_TRACK_LEN_FRAMES
			<< " frames." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}
}

/// \endcond

} // namespace v_1_0_0

} // namespace details

} // namespace arcstk

#endif

