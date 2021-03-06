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

// Include only what is used HERE but NOT in identifier.hpp
#include <iterator>         // for begin, end
#include <sstream>          // for stringstream

// requires InvalidMetadataException, NonstandardMetadataException, TrackNo,
// TOC and CDDA from identifier.hpp


namespace arcstk
{

inline namespace v_1_0_0
{
namespace details
{

/// \internal \addtogroup id
/// @{


/**
 * \brief std::true_type iff T can represent LBA frame amounts, otherwise
 * std::false_type.
 *
 * An "LBA type" holds amounts of LBA frames, that means it must be a numeric
 * type that is able to express CDDA::MAX_OFFSET which excludes types with less
 * than 32 bits width.
 *
 * Furthermore, it should be possible to perform at least addition of the types.
 * (References and pointers to compatible base types would have to be
 * dereferenced for that and are therefore excluded.)
 *
 * The intention is to include exactly the non-ref, non-pointer variants of
 * at least int, long and long long and their respective unsigned types.
 *
 * \tparam T Type to test for LBA facility
 */
template <typename T>
struct is_lba_type : public std::integral_constant<bool,
	std::is_integral<T>::value   && // TODO C++17: is_integral_v
	sizeof(T) >= 4>
{
	// empty
};


/**
 * \brief Abstracted YES/NO values for SFINAE
 */
struct sfinae
{
	using yes = char;
	using no  = yes[2]; // yes and no must differ in size
};


/**
 * \brief Helper: check for a \c value_type with at least 32 bit width that is
 * of integer type and as well arithmetic.
 *
 * \tparam T Input type to test
 */
template <typename T>
struct has_lba_value_type
{
private:

	// choose this in case is_lba_type<S::value_type>
	template<typename S,
		std::enable_if_t<is_lba_type<typename S::value_type>::value, int> = 0>
	static sfinae::yes& test(typename S::value_type*);

	// choose this otherwise
	template<typename S>
	static sfinae::no& test(...);

public:

	/**
	 * \brief TRUE for types with is_lba_type<value_type>, otherwise false.
	 */
	static const bool value =
		! std::is_same<decltype(test<T>(nullptr)), sfinae::no&>::value;
	// Commented out: The "old" C++98 way without decltype
	//	sizeof(test<T>(nullptr)) == sizeof(sfinae::yes);

	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of a const_iterator
 *
 * \tparam T Input type to test
 */
template <typename T>
struct has_const_iterator
{
private:

	// choose this in case S::const_iterator is defined
	template <typename S>
	static sfinae::yes& test(typename S::const_iterator*);

	// choose this otherwise
	template <typename S>
	static sfinae::no& test(...);

public:

	/**
	 * \brief TRUE for types with has_const_iterator<T>, otherwise false.
	 */
	static const bool value =
		! std::is_same<decltype(test<T>(nullptr)), sfinae::no&>::value;

	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of a size
 *
 * \tparam T Input type to test
 */
template <typename T>
struct has_size
{
private:

	// choose this in case S::size is defined
	template <typename S>
	static sfinae::yes& test(
		typename std::enable_if<std::is_same<
			decltype(
				static_cast<typename S::size_type(S::*)() const>(&S::size)),
			typename S::size_type(S::*)() const>::value,
		void>::type*
	);

	// choose this otherwise
	template <typename S>
	static sfinae::no& test(...);

public:

	/**
	 * \brief TRUE for types with has_size<T>, otherwise false.
	 */
	static const bool value =
		! std::is_same<decltype(test<T>(nullptr)), sfinae::no&>::value;

	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of begin() const
 *
 * \tparam T Input type to test
 */
template <typename T>
struct has_begin
{
private:

	// choose this in case S::begin const is defined
	template <typename S>
	static sfinae::yes& test(
		typename std::enable_if<std::is_same<
			decltype(static_cast<typename S::const_iterator(S::*)() const>
				(&S::begin)),
			typename S::const_iterator(S::*)() const>::value,
		void>::type*
	);

	// choose this otherwise
	template <typename S> static sfinae::no& test(...);

public:

	/**
	 * \brief TRUE for types with has_begin<T>, otherwise false.
	 */
	static const bool value =
		! std::is_same<decltype(test<T>(nullptr)), sfinae::no&>::value;

	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief Helper: check for the presence of end() const
 *
 * \tparam T Input type to test
 */
template <typename T>
struct has_end
{
private:

	// choose this in case S::end const is defined
	template <typename S>
	static sfinae::yes& test(
		typename std::enable_if<std::is_same<
			decltype(static_cast<typename S::const_iterator(S::*)() const>
				(&S::end)),
			typename S::const_iterator(S::*)() const>::value,
		void>::type*
	);

	// choose this otherwise
	template <typename S>
	static sfinae::no& test(...);

public:

	/**
	 * \brief TRUE for types with has_end<T>, otherwise false.
	 */
	static const bool value =
		! std::is_same<decltype(test<T>(nullptr)), sfinae::no&>::value;

	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief std::true_type iff T is a const-iterable container type with a size
 * otherwise std::false_type.
 *
 * \tparam T The type to test
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
 * \brief std::true_type iff is_container<T> and T::value_type is an lba type,
 * otherwise std::false_type.
 *
 * \tparam The type to test
 */
template <typename T>
struct is_lba_container : public std::integral_constant<bool,
	is_container<T>::value &&
	has_lba_value_type<std::remove_reference_t<T>>::value >
{
	/* empty */
};


/**
 * \brief Indicate whether T is a type for representing filenames.
 *
 * Currently \c is_filename_type<T> is of \c std::true_type only for
 * <tt>T = std::string</tt> and of \c std::false_type for every other type T.
 *
 * Intended to be specialized by client.
 *
 * \tparam T Type to be classified as representing a filename
 */
template <typename T>
struct is_filename_type : public std::false_type { /* empty */ };

// std::string: TRUE
template <>
struct is_filename_type<std::string> : public std::true_type { /* empty */ };

// std::wstring: TRUE
template <>
struct is_filename_type<std::wstring> : public std::true_type { /* empty */ };


/**
 * \brief Helper: check for \c value_type  std::string.
 *
 * \tparam T Input type to test
 */
template <typename T>
struct has_filename_value_type
{
private:

	// choose this in case S::value_type can represent filenames
	template<typename S,
		std::enable_if_t<is_filename_type<typename S::value_type>::value, int>
			= 0>
	static sfinae::yes& test(typename S::value_type*);

	// choose this otherwise
	template<typename S>
	static sfinae::no& test(...);

public:

	/**
	 * \brief TRUE for types with has_end<T>, otherwise false.
	 */
	static const bool value =
		! std::is_same<decltype(test<T>(nullptr)), sfinae::no&>::value;

	// ignore
	void gcc_suppress_warning_wctor_dtor_privacy();
};


/**
 * \brief std::true_type iff is_container<T> and T::value_type can represent
 * filenames, otherwise std::false_type.
 *
 * \tparam The type to test
 */
template <typename T>
struct is_filename_container : public std::integral_constant<bool,
	is_container<T>::value &&
	has_filename_value_type<std::remove_reference_t<T>>::value >
{
	/* empty */
};

/**
 * \brief Defined iff T is an lba type
 */
template <typename T>
using LBAType = std::enable_if_t<is_lba_type<T>::value>;

/**
 * \brief Defined iff T is an lba container
 */
template <typename T>
using LBAContainer = std::enable_if_t<is_lba_container<T>::value>;


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
	template <typename Container, typename = LBAContainer<Container>>
	inline static void validate_offsets(Container&& offsets);

	/**
	 * \copydoc validate_offsets(Container&&) const
	 */
	template <typename T, typename = LBAType<T>>
	inline static void validate_offsets(std::initializer_list<T> offsets);

	/**
	 * \brief Validate offsets and track count.
	 *
	 * It is ensured that the number of offsets matches the track count and that
	 * the offsets are consistent.
	 *
	 * \tparam Container The container holding offset values
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container, typename = LBAContainer<Container>>
	inline static void validate_offsets(const TrackNo track_count,
			Container&& offsets);

	/**
	 * \copydoc validate_offsets(TrackNo, Container&&) const
	 */
	template <typename T, typename = LBAType<T>>
	inline static void validate_offsets(const TrackNo track_count,
			std::initializer_list<T> offsets);

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
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] leadout     Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	template <typename Container, typename = LBAContainer<Container>>
	inline static void validate(const TrackNo track_count,
			Container&& offsets,
			const lba_count_t leadout);

	/**
	 * \copydoc validate(TrackNo, Container&&, const lba_count_t) const
	 */
	template <typename T, typename = LBAType<T>>
	inline static void validate(const TrackNo track_count,
			std::initializer_list<T> offsets,
			const lba_count_t leadout);

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
	 * \throw InvalidMetadataException If total length exceeds physical maximum
	 * \throw NonstandardMetadataException If total length exceeds CDDA::MAX_OFFSET
	 */
	template <typename Container, typename = LBAContainer<Container>>
	inline static void validate_lengths(Container&& lengths);

	/**
	 * \copydoc validate_lengths(Container&&) const
	 */
	template <typename T, typename = LBAType<T>>
	inline static void validate_lengths(std::initializer_list<T> lengths);

	/**
	 * \brief Validate leadout frame.
	 *
	 * It is ensured that the leadout is within a CDDA conforming range. An
	 * InvalidMetadataException is thrown if the validation fails.
	 *
	 * \param[in] leadout Leadout frame of the medium
	 *
	 * \throw InvalidMetadataException If leadout exceeds physical maximum
	 * \throw NonstandardMetadataException If leadout exceeds CDDA::MAX_OFFSET
	 */
	inline static void validate_leadout(const lba_count_t leadout);

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
	inline static void validate_trackcount(const TrackNo track_count);

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
	inline static void validate(const TOC &toc, const lba_count_t leadout);


protected:

	/**
	 * \brief Ensure that \c offset does not exceed legal range.
	 *
	 * \param[in] offset The offset value to validate
	 * \param[in] track  The track to check
	 *
	 * \throw InvalidMetadataException If offset exceeds physical maximum
	 * \throw NonstandardMetadataException If offset exceeds CDDA::MAX_OFFSET
	 */
	inline static void check_legal_range(const lba_count_t offset,
			const TrackNo track);

	/**
	 * \brief Ensure that \c prev_track and \c next_track offsets have at least
	 * minimal distance
	 *
	 * \param[in] prev_track Previous track offset
	 * \param[in] next_track Next track offset
	 *
	 * \throw InvalidMetadataException If the validation fails
	 */
	inline static void have_min_dist(const lba_count_t prev_track,
			const lba_count_t next_track);

	/**
	 * \brief Maximal valid offset value for a non-redbook 90 min CD (in LBA
	 * frames).
	 *
	 * Non-redbook 90-min CD has 89:59.74 which is equivalent to 405.000 frames.
	 */
	static constexpr lba_count_t MAX_OFFSET_90 { (89 * 60 + 59) * 75 + 74 };

	/**
	 * \brief Maximal valid offset value for a non-redbook 99 min CD (in LBA
	 * frames).
	 *
	 * Non-redbook 99-min CD has 98:59.74 which is equivalent to 445.500 frames.
	 */
	static constexpr lba_count_t MAX_OFFSET_99 { (98 * 60 + 59) * 75 + 74 };
};


// TOCValidator


template <typename Container, typename>
void TOCValidator::validate_offsets(Container&& offsets)
{
	// Number of offsets in legal range?

	if (offsets.size() == 0)
	{
		auto ss = std::stringstream {};
		ss << "No offsets were given. Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	if (offsets.size() >
			static_cast<decltype(offsets.size())>(CDDA::MAX_TRACKCOUNT))
	{
		auto ss = std::stringstream {};
		ss << "Offsets are only possible for at most "
			<< CDDA::MAX_TRACKCOUNT << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// Skip check for 0 for first track

	auto track { std::begin(offsets) };

	check_legal_range(*track, 1);

	++track;

	if (std::end(offsets) == track) // size == 1
	{
		return;
	}

	// Check whether all subsequent Offsets have minimum distance

	auto previous_track { std::begin(offsets) };
	auto finished       { std::end(offsets)   };

	auto t { 2 };
	for (; track != finished; ++previous_track, ++track)
	{
		// Is offset in a CDDA-legal range?

		if (*track == 0)
		{
			auto ss = std::stringstream {};
			ss << "Offset for track " << std::to_string(t)
				<< " is not allowed to be 0";

			throw InvalidMetadataException(ss.str());
		}

		check_legal_range(*track, t);

		// Has offset for current track at least minimum distance after
		// offset for last track?

		TOCValidator::have_min_dist(
				static_cast<lba_count_t>(*previous_track),
				static_cast<lba_count_t>(*track));

		++t;
	} // for
}


template <typename T, typename>
void TOCValidator::validate_offsets(std::initializer_list<T> offsets)
{
	TOCValidator::validate_offsets(std::vector<T>{offsets});
}


template <typename Container, typename>
void TOCValidator::validate_offsets(const TrackNo track_count,
		Container&& offsets)
{
	TOCValidator::validate_trackcount(track_count);

	// Validation: Track count consistent with Number of Offsets?

	auto offsets_count { offsets.size() };

	if (offsets_count != static_cast<decltype(offsets_count)>(track_count))
	{
		auto ss = std::stringstream {};
		ss << "Track count does not match offset count." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	TOCValidator::validate_offsets(std::forward<Container>(offsets));
}


template <typename T, typename>
void TOCValidator::validate_offsets(const TrackNo track_count,
		std::initializer_list<T> offsets)
{
	TOCValidator::validate_offsets(track_count, std::vector<T>{offsets});
}


template <typename Container, typename>
void TOCValidator::validate(const TrackNo track_count,
		Container&& offsets, const lba_count_t leadout)
{
	TOCValidator::validate_leadout(leadout);

	// Validation: Leadout in Valid Distance after Last Offset?

	const auto last_track { --std::end(offsets) };

	if (leadout - *last_track < CDDA::MIN_TRACK_LEN_FRAMES)
	{
		auto ss = std::stringstream {};
		ss << "Leadout frame " << leadout
			<< " is too near to last offset " << *last_track
			<< ". Minimal distance is " << CDDA::MIN_TRACK_LEN_FRAMES
			<< " frames." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	TOCValidator::validate_offsets(track_count, offsets);
}


template <typename T, typename>
void TOCValidator::validate(const TrackNo track_count,
		std::initializer_list<T> offsets, const lba_count_t leadout)
{
	TOCValidator::validate(track_count, std::vector<T>{offsets}, leadout);
}


template <typename Container, typename>
void TOCValidator::validate_lengths(Container&& lengths)
{
	// Number of lengths in legal range?

	if (lengths.size() == 0)
	{
		auto ss = std::stringstream {};
		ss << "No lengths were given. Bail out.";

		throw InvalidMetadataException(ss.str());
	}

	if (lengths.size() >
			static_cast<decltype(lengths.size())>(CDDA::MAX_TRACKCOUNT))
	{
		auto ss = std::stringstream {};
		ss << "Lengths are only possible for at most "
			<< CDDA::MAX_TRACKCOUNT << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// Length values are valid?

	auto sum_lengths = lba_count_t { 0 };

	auto last { --std::end(lengths) };
	auto t { 1 };

	if (*last > 0) { ++last; } // If last length is actually known, validate it
	for (auto track { std::begin(lengths) }; track != last; ++track)
	{
		if (*track < static_cast<int64_t>(CDDA::MIN_TRACK_LEN_FRAMES))
		{
			auto ss = std::stringstream {};
			ss << "Illegal length " << std::to_string(*track)
				<< " for track " << std::to_string(t);

			throw InvalidMetadataException(ss.str());
		}

		sum_lengths += static_cast<lba_count_t>(*track);
	}

	// Sum of all lengths in legal range ?

	if (sum_lengths > CDDA::MAX_OFFSET)
	{
		auto ss = std::stringstream {};
		ss << "Total length " << std::to_string(sum_lengths);

		if (sum_lengths > MAX_OFFSET_99)
		{
			ss << " exceeds physical range of 99 min ("
				<< std::to_string(MAX_OFFSET_99) << " frames)";

			throw InvalidMetadataException(ss.str());

		} else if (sum_lengths > MAX_OFFSET_90)
		{
			ss << " exceeds "
				<< std::to_string(MAX_OFFSET_90) << " frames (90 min)";

			throw NonstandardMetadataException(ss.str());

		} else // More than redbook originally defines?
		{
			ss << " exceeds redbook maximum of "
				<< std::to_string(CDDA::MAX_OFFSET);

			throw NonstandardMetadataException(ss.str());
		}
	}
}


template <typename T, typename>
void TOCValidator::validate_lengths(std::initializer_list<T> lengths)
{
	TOCValidator::validate_lengths(std::vector<T>{lengths});
}


void TOCValidator::validate_leadout(const lba_count_t leadout)
{
	// Greater than Minimum ?

	if (static_cast<int64_t>(leadout) < CDDA::MIN_TRACK_OFFSET_DIST)
	{
		auto ss = std::stringstream {};
		ss << "Leadout " << leadout
			<< " is smaller than minimum track length";

		throw InvalidMetadataException(ss.str());
	}

	// Less than Maximum ?

	if (leadout > CDDA::MAX_BLOCK_ADDRESS)
	{
		auto ss = std::stringstream {};
		ss << "Leadout " << leadout << " exceeds physical maximum";

		throw InvalidMetadataException(ss.str());
	}

	// Warning ?

	if (leadout > CDDA::MAX_OFFSET)
	{
		auto ss = std::stringstream {};
		ss << "Leadout " << leadout << " exceeds redbook maximum";

		throw NonstandardMetadataException(ss.str());
	}
}


void TOCValidator::validate_trackcount(const TrackNo track_count)
{
	if (track_count < 1 or track_count > 99)
	{
		auto ss = std::stringstream {};
		ss << "Invalid track count: "
			<< std::to_string(static_cast<int>(track_count));

		throw InvalidMetadataException(ss.str());
	}
}


void TOCValidator::validate(const TOC &toc, const lba_count_t leadout)
{
	TOCValidator::validate_leadout(leadout);

	auto last_offset { toc.offset(toc.total_tracks()) };
	TOCValidator::have_min_dist(last_offset, leadout);
}


void TOCValidator::check_legal_range(const lba_count_t offset, const TrackNo t)
{
	if (offset < 0)
	{
		auto ss = std::stringstream {};
		ss << "Negative offset: " << std::to_string(offset);

		throw InvalidMetadataException(ss.str());
	}

	if (offset > static_cast<int64_t>(CDDA::MAX_OFFSET))
	{
		auto ss = std::stringstream {};
		ss << "Offset " << std::to_string(offset)
			<< " for track " << std::to_string(static_cast<int>(t));

		if (offset > static_cast<int64_t>(MAX_OFFSET_99))
		{
			ss << " exceeds physical range of 99 min ("
				<< std::to_string(MAX_OFFSET_99) << " frames)";

			throw NonstandardMetadataException(ss.str());
		} else if (offset > static_cast<int64_t>(MAX_OFFSET_90))
		{
			ss << " exceeds physical range of 90 min ("
				<< std::to_string(MAX_OFFSET_90) << " frames)";

			throw NonstandardMetadataException(ss.str());
		} else
		{
			ss << " exceeds redbook maximum duration of "
				<< std::to_string(CDDA::MAX_OFFSET);
		}

		throw InvalidMetadataException(ss.str());
	}
}


void TOCValidator::have_min_dist(const lba_count_t prev_track,
		const lba_count_t next_track)
{
	if (next_track - prev_track < CDDA::MIN_TRACK_OFFSET_DIST)
	{
		auto ss = std::stringstream {};
		ss << "Track with offset " << prev_track
			<< " is too short. Next track starts at " << next_track
			<< " but minimal distance is " << CDDA::MIN_TRACK_LEN_FRAMES
			<< " frames." << " Bail out.";

		throw InvalidMetadataException(ss.str());
	}
}

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif

