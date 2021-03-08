// vim:ft=cpp
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#error "Do not include builders.tpp directly, include identifier.hpp instead"
#endif
#ifndef __LIBARCSTK_VALIDATE_TPP__
#error "builders.tpp requires validate.tpp and must be included after it"
#endif

#ifndef __LIBARCSTK_BUILDERS_TPP__
#define __LIBARCSTK_BUILDERS_TPP__

/**
 * \file
 *
 * \internal
 *
 * \brief Builder for TOCs.
 */

// Include only what is used HERE but NOT in identifier.hpp
#include <iterator>         // for begin, end
#include <sstream>

// requires InvalidMetadataException, NonstandardMetadataException and TrackNo
// from identifier.hpp

// requires LBAType, LBAContainer from validate.tpp

namespace arcstk
{
inline namespace v_1_0_0
{
namespace details
{

// forward declaration
class TOCBuilder;


/**
 * \brief Defined iff T has <tt>size const()</tt>
 */
template <typename T>
using hasSize =
	std::enable_if_t<details::has_size<std::remove_reference_t<T>>::value>;

/**
 * \brief Defined iff T has <tt>begin const()</tt>
 */
template <typename T>
using hasBegin =
	std::enable_if_t<details::has_begin<std::remove_reference_t<T>>::value>;

/**
 * \brief Defined iff T is a const-iterable container of filenames
 */
template <typename T>
using FilenameContainer =
	std::enable_if_t<details::is_filename_container<T>::value>;

/**
 * \brief Uniform access to a (0-based) container by (1-based) track.
 *
 * Uniform range-checked method to access a (0-based) container by using the
 * track number, which is a 1-based index. In fact, track \c t is converted
 * to <tt>t - 1</tt> and then the container is accessed.
 *
 * The Container is required to yield its number of elements by member function
 * size() and to provide begin(). The iterator provided by begin() has to
 * support dereferencing by operator \*.
 *
 * \tparam Container Container type with \c size() and <tt>begin()</tt>
 *
 * \param c Actual container
 * \param t Number of the track to access
 *
 * \return The value for track \c t in the \c Container
 *
 * \throw std::out_of_range If either <tt>t < 1</tt> or
 *                          <tt>t > Container.size()</tt>
 */
template <typename Container,
		typename = hasSize<Container>, typename = hasBegin<Container> >
inline decltype(auto) get_track(Container&& c, const TrackNo t)
{
	const auto index = t - 1;

	if (index < 0 or static_cast<decltype(c.size())>(index) >= c.size())
	{
		auto message = std::stringstream {};
		message << "Track " << t << " is out of range (yields index "
			<< index << " but max index is " << (c.size() - 1) << ")";

		throw std::out_of_range(message.str());
	}

	return *(std::next(std::begin(c), index));
}

/**
 * \brief Calculate leadout from lengths and offsets.
 *
 * No validation is performed except check for non-emptyness, equal size and
 * not exceeding CDDA::MAX_BLOCK_ADDRESS.
 *
 * \tparam Container1 Type of the lengths container
 * \tparam Container2 Type of the offsets container
 *
 * \param[in] lengths The lengths
 * \param[in] offsets The offsets
 *
 * \return The leadout corresponding to the offsets and lengths
 */
template <typename Container1, typename Container2,
		typename = LBAContainer<Container1>,
		typename = LBAContainer<Container2>>
inline lba_count_t calculate_leadout(Container1&& lengths, Container2&& offsets)
{
	// from last offset and last length

	if (offsets.size() == 0)
	{
		throw InvalidMetadataException("Need offsets to calculate leadout");
	}

	if (lengths.size() != offsets.size())
	{
		throw InvalidMetadataException(
			"Requested leadout from inconsistent offsets and lengths");
	}

	auto end_lengths { std::end(lengths) }; // avoid modifying temporaries
	auto last_length { *--end_lengths };

	auto end_offsets { std::end(offsets) }; // avoid modifying temporaries
	auto leadout     { last_length ? *--end_offsets + last_length : 0 };

	if (CDDA::MAX_BLOCK_ADDRESS <
			static_cast<decltype(CDDA::MAX_BLOCK_ADDRESS)>(leadout))
	{
		throw InvalidMetadataException(
			"Calculated leadout is bigger than maximal legal block address");
	}

	// We suppose std::numeric_limits<lba_count_t>::max() > CDDA::MAX_BLOCK_ADDRESS
	return static_cast<lba_count_t>(leadout);
}

} // namespace details


/**
 * \internal
 * \ingroup id
 *
 * \brief Private implementation of TOC.
 *
 * This class is considered an implementation detail.
 *
 * \see TOC
 */
class TOC::Impl final
{
	// TOCBuilder constructs TOC::Impls exclusively

	friend details::TOCBuilder;

public:

	/**
	 * \brief Copy constructor
	 *
	 * \param[in] rhs Object to move
	 */
	inline Impl(const TOC::Impl &rhs);

	/**
	 * \brief Move constructor
	 *
	 * \param[in] rhs Object to move
	 */
	inline Impl(TOC::Impl &&rhs) noexcept;

	/**
	 * \brief Implements TOC::total_tracks()
	 */
	inline int total_tracks() const noexcept;

	/**
	 * \brief Implements TOC::offset(const uint8_t) const
	 */
	inline lba_count_t offset(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::parsed_length(const uint8_t) const
	 */
	inline lba_count_t parsed_length(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::filename(const TrackNo idx) const
	 */
	inline std::string filename(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::leadout()
	 */
	inline lba_count_t leadout() const noexcept;

	/**
	 * \brief Implements TOC::complete()
	 */
	inline bool complete() const noexcept;

	/**
	 * \brief Implements TOC::operator==()
	 */
	inline bool equals(const TOC::Impl &rhs) const noexcept;

	/**
	 * \brief Updates the leadout.
	 *
	 * \param[in] leadout The new leadout to update the TOC with
	 */
	inline void update(const lba_count_t leadout) noexcept;

private:

	/**
	 * \brief Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] leadout     Leadout frame
	 * \param[in] files       File name of each track
	 */
	inline Impl(const TrackNo track_count,
			const std::vector<lba_count_t> &offsets,
			const lba_count_t leadout,
			const std::vector<std::string> &files);

	/**
	 * \brief Implements private constructor of TOC.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] lengths     Lengths (in LBA frames) of each track
	 * \param[in] files       File name of each track
	 */
	inline Impl(const TrackNo track_count,
			const std::vector<lba_count_t> &offsets,
			const std::vector<lba_count_t> &lengths,
			const std::vector<std::string> &files);

	/**
	 * \brief TRUE iff \c t is a track in this TOC, i.e. 1 <= t <= track_count,
	 * otherwise FALSE
	 *
	 * \param[in] track Track number to check
	 *
	 * \return TRUE iff \c t is a track in this TOC
	 */
	inline bool has_track(const TrackNo track) const noexcept;

	/**
	 * \brief Number of tracks
	 */
	int track_count_;

	/**
	 * \brief Track offsets (in frames)
	 */
	std::vector<lba_count_t> offsets_;

	/**
	 * \brief Track lengths (in frames)
	 */
	std::vector<lba_count_t> lengths_;

	/**
	 * \brief Leadout frame
	 */
	lba_count_t leadout_;

	/**
	 * \brief Audio file names
	 */
	std::vector<std::string> files_;
};


TOC::Impl::Impl(const TOC::Impl &rhs) = default;


TOC::Impl::Impl(TOC::Impl &&rhs) noexcept = default;


TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<lba_count_t> &offsets,
		const lba_count_t leadout,
		const std::vector<std::string> &files)
	: track_count_ { track_count }
	, offsets_ { offsets }
	, lengths_ {}
	, leadout_ { leadout }
	, files_ { files }
{
	// empty
}


TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<lba_count_t> &offsets,
		const std::vector<lba_count_t> &lengths,
		const std::vector<std::string> &files)
	: track_count_ { track_count }
	, offsets_ { offsets }
	, lengths_ { lengths }
	, leadout_ { arcstk::details::calculate_leadout(lengths, offsets) }
	, files_ { files }
{
	// empty
}


bool TOC::Impl::has_track(const TrackNo track) const noexcept
{
	return track >= 1 and track <= this->total_tracks();
}


int TOC::Impl::total_tracks() const noexcept
{
	return track_count_;
}


lba_count_t TOC::Impl::offset(const TrackNo track) const
{
	return details::get_track(offsets_, track);

	// While length or filename may be missing, an offset _MUST_ be present,
	// thus we do not catch anything for a normalized return value.
}


lba_count_t TOC::Impl::parsed_length(const TrackNo track) const
{
	try {

		return details::get_track(lengths_, track);
	} catch (const std::out_of_range &e)
	{
		// If track is valid, don't throw, even if lengths_ is empty
		if (has_track(static_cast<int>(track)))
		{
			return 0;
		} else {
			throw;
		}
	}
}


std::string TOC::Impl::filename(const TrackNo track) const
{
	try {

		return details::get_track(files_, track);
	} catch (const std::out_of_range &e)
	{
		// If track is valid, don't throw, even if files_ is empty
		if (has_track(track))
		{
			return std::string{};
		} else {
			throw;
		}
	}
}


lba_count_t TOC::Impl::leadout() const noexcept
{
	return leadout_;
}


bool TOC::Impl::complete() const noexcept
{
	return leadout_ != 0;
}


bool TOC::Impl::equals(const TOC::Impl &rhs) const noexcept
{
	return this == &rhs
		or  (track_count_ == rhs.track_count_
		and  offsets_     == rhs.offsets_
		and  lengths_     == rhs.lengths_
		and  leadout_     == rhs.leadout_
		and  files_       == rhs.files_);
}


void TOC::Impl::update(const lba_count_t leadout) noexcept
{
	leadout_ = leadout;
}


namespace details
{


/**
 * \internal
 * \ingroup id
 *
 * \brief Constructs a TOC from metadata of the compact disc.
 *
 * TOCBuilder will refuse to construct invalid TOCs from any data and throw
 * an InvalidMetadataException instead. Hence, if no exception was thrown
 * and an actual TOC is returned, this TOC is guaranteed to be at
 * least minimally consistent.
 *
 * The only allowed relaxation is that the TOC may be incomplete, which
 * means that the leadout may be unknown. This relaxation is allowed because the
 * length of the last track or the leadout cannot be deduced from certain
 * TOC file formats. However, it is required to construct TOC from
 * files in any TOC file format, e.g. CUEsheets.
 *
 * This class is considered an implementation detail.
 */
class TOCBuilder final
{

public:

	/**
	 * \brief Build a TOC object from the specified information.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) for each track
	 * \param[in] leadout     Leadout frame
	 * \param[in] files       File name of each track
	 *
	 * \return A TOC object representing the specified information
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	template <typename Container, typename Files,
			typename = LBAContainer<Container>,
			typename = FilenameContainer<Files> >
	inline static std::unique_ptr<TOC> build(const TrackNo track_count,
			Container&& offsets,
			const lba_count_t leadout,
			Files&& files);

	/**
	 * \brief Build a TOC object from the specified information.
	 *
	 * Intended for testing.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) for each track
	 * \param[in] leadout     Leadout frame
	 *
	 * \return A TOC object representing the specified information
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	template <typename T, typename = LBAType<T> >
	inline static std::unique_ptr<TOC> build(const TrackNo track_count,
			std::initializer_list<T> offsets,
			const lba_count_t leadout);

	/**
	 * \brief Build a TOC object from the specified information.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] lengths     Lengths (in LBA frames) of each track
	 * \param[in] files       File name of each track
	 *
	 * \return A TOC object representing the specified information
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	template <typename Container1, typename Container2, typename Files,
		typename = LBAContainer<Container1>,
		typename = LBAContainer<Container2>,
		typename = FilenameContainer<Files> >
	inline static std::unique_ptr<TOC> build(const TrackNo track_count,
			Container1&& offsets,
			Container2&& lengths,
			Files&& files);

	/**
	 * \brief Build a TOC object from the specified information.
	 *
	 * Intended for testing.
	 *
	 * \param[in] track_count Number of tracks in this medium
	 * \param[in] offsets     Offsets (in LBA frames) of each track
	 * \param[in] lengths     Lengths (in LBA frames) of each track
	 *
	 * \return A TOC object representing the specified information
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	template <typename T1, typename T2,
		typename = LBAType<T1>,
		typename = LBAType<T2> >
	inline static std::unique_ptr<TOC> build(const TrackNo track_count,
			std::initializer_list<T1> offsets,
			std::initializer_list<T2> lengths);

	/**
	 * \copydoc build(const TrackNo, Container1&&, Container2&&) const
	 */
	template <typename T, typename Container,
		typename = LBAType<T>,
		typename = LBAContainer<Container> >
	inline static std::unique_ptr<TOC> build(const TrackNo track_count,
		std::initializer_list<T> offsets,
		Container&& lengths);

	/**
	 * \copydoc build(const TrackNo, Container1&&, Container2&&) const
	 */
	template <typename Container, typename T,
		typename = LBAContainer<Container>,
		typename = LBAType<T> >
	inline static std::unique_ptr<TOC> build(const TrackNo track_count,
		Container&& offsets,
		std::initializer_list<T> lengths);

	/**
	 * \brief Update a TOC object with a leadout.
	 *
	 * \param[in] toc     The TOC to make complete
	 * \param[in] leadout The leadout to update the TOC with
	 *
	 * \throw InvalidMetadataException If the input data forms no valid TOC
	 */
	inline static void update(TOC &toc, const lba_count_t leadout);


private:

	/**
	 * \brief Service method: Builds a track count for a TOC object.
	 *
	 * \param[in] track_count Number of tracks
	 *
	 * \return The intercepted track_count
	 *
	 * \throw InvalidMetadataException If the track count is not valid
	 */
	inline static int build_track_count(const TrackNo track_count);

	/**
	 * \brief Service method: Builds validated offsets for a TOC object.
	 *
	 * \tparam Container An LBA container type
	 *
	 * \param[in] offsets     Offsets to be validated
	 * \param[in] track_count Number of tracks
	 * \param[in] leadout     Leadout frame
	 *
	 * \return A representation of the validated offsets as unsigned integers
	 *
	 * \throw InvalidMetadataException If the offsets are not valid
	 */
	template <typename Container, typename = LBAContainer<Container> >
	inline static std::vector<lba_count_t> build_offsets(Container&& offsets,
			const TrackNo track_count, const lba_count_t leadout);

	/**
	 * \brief Service method: Builds validated offsets for a TOC object.
	 *
	 * \tparam Container1 An LBA container type
	 * \tparam Container2 An LBA container type
	 *
	 * \param[in] offsets     Offsets to be validated
	 * \param[in] track_count Number of tracks
	 * \param[in] lengths     Lengths to be validated
	 *
	 * \return A representation of the validated offsets as unsigned integers
	 *
	 * \throw InvalidMetadataException If the offsets are not valid
	 */
	template <typename Container1, typename Container2,
		typename = LBAContainer<Container1>,
		typename = LBAContainer<Container2> >
	inline static std::vector<lba_count_t> build_offsets(
			Container1&& offsets,
			const TrackNo track_count,
			Container2&& lengths);

	/**
	 * \brief Service method: Builds validated lengths for a TOC object.
	 *
	 * \tparam Container An LBA container type
	 *
	 * \param[in] lengths Vector of lengths as signed integers to be validated
	 * \param[in] track_count Number of tracks
	 *
	 * \return A representation of the validated lengths as unsigned integers
	 *
	 * \throw InvalidMetadataException If the lengths are not valid
	 */
	template <typename Container, typename = LBAContainer<Container> >
	inline static std::vector<lba_count_t> build_lengths(Container&& lengths,
			const TrackNo track_count);

	/**
	 * \brief Service method: Builds validated leadout for a TOC object.
	 *
	 * \param[in] leadout Leadout to be validated
	 *
	 * \return A representation of the validated leadout
	 *
	 * \throw InvalidMetadataException If the leadout is not valid
	 */
	inline static lba_count_t build_leadout(const lba_count_t leadout);

	/**
	 * \brief Service method: Builds validated audio file list for a TOC object.
	 *
	 * The standard implementation of this function does nothing.
	 * Specialize for intended use.
	 *
	 * \tparam Container Container with filenames
	 *
	 * \param[in] files File list to be validated
	 *
	 * \return A representation of the validated file list
	 *
	 * \throw InvalidMetadataException If the file list is not valid
	 */
	template <typename Container, typename = FilenameContainer<Container> >
	inline static std::vector<std::string> build_files(Container&& files);
};


// TOCBuilder


template <typename Container, typename Files, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		Container&& offsets,
		const lba_count_t leadout,
		Files&& files)
{
	auto impl { std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::forward<Container>(offsets), track_count, leadout),
		build_leadout(leadout),
		build_files(files))
	) };

	return std::make_unique<TOC>(std::move(impl));
}


template <typename T, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		std::initializer_list<T> offsets,
		const lba_count_t leadout)
{
	auto impl { std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::vector<T>{offsets}, track_count, leadout),
		build_leadout(leadout),
		build_files(std::vector<std::string>{}))
	) };

	return std::make_unique<TOC>(std::move(impl));
}


template <typename Container1, typename Container2, typename Files,
	typename, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		Container1&& offsets,
		Container2&& lengths,
		Files&& files)
{
	auto impl { std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::forward<Container1>(offsets), track_count, lengths),
		build_lengths(std::forward<Container2>(lengths), track_count),
		build_files(files))
	) };

	return std::make_unique<TOC>(std::move(impl));
}


template <typename T, typename Container, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		std::initializer_list<T> offsets,
		Container&& lengths)
{
	auto impl { std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::vector<T>{offsets}, track_count,
			std::forward<Container>(lengths)),
		build_lengths(std::forward<Container>(lengths), track_count),
		{/* no filenames */})
	) };

	return std::make_unique<TOC>(std::move(impl));
}


template <typename Container, typename T, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		Container&& offsets,
		std::initializer_list<T> lengths)
{
	auto impl { std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::forward<Container>(offsets), track_count,
			std::vector<T>{lengths}),
		build_lengths(std::vector<T>{lengths}, track_count),
		{/* no filenames */})
	) };

	return std::make_unique<TOC>(std::move(impl));
}


template <typename T1, typename T2, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		std::initializer_list<T1> offsets,
		std::initializer_list<T2> lengths)
{
	auto impl { std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::vector<T1>(offsets), track_count, lengths),
		build_lengths(std::vector<T2>(lengths), track_count),
		{/* no filenames */})
	) };

	return std::make_unique<TOC>(std::move(impl));
}


void TOCBuilder::update(TOC &toc, const lba_count_t leadout)
{
	toc.update(leadout);
}


int TOCBuilder::build_track_count(const TrackNo track_count)
{
	TOCValidator::validate_trackcount(track_count);

	return track_count;
}


template <typename Container, typename>
std::vector<lba_count_t> TOCBuilder::build_offsets(
		Container&& offsets,
		const TrackNo track_count,
		const lba_count_t leadout)
{
	TOCValidator::validate(track_count, offsets, leadout);

	// Convert offsets to lba_count_t

	return std::vector<lba_count_t>(offsets.begin(), offsets.end());
}


template <typename Container1, typename Container2, typename, typename>
std::vector<lba_count_t> TOCBuilder::build_offsets(
		Container1&& offsets, const TrackNo track_count,
		Container2&& lengths)
{
	// Valid number of lengths ?

	if (offsets.size() != lengths.size())
	{
		auto ss = std::stringstream {};
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(offsets.size()) << " offsets";

		throw InvalidMetadataException(ss.str());
	}

	try {

		TOCValidator::validate_offsets(track_count, offsets);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Do not throw NonstandardMetadataException for now
		// since we accept non-standard metadata
	}

	try {

		TOCValidator::validate_lengths(lengths);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Do not throw NonstandardMetadataException for now
		// since we accept non-standard metadata
	}

	// Convert offsets to lba_count_t

	return std::vector<lba_count_t>(offsets.begin(), offsets.end());
}


template <typename Container, typename>
std::vector<lba_count_t> TOCBuilder::build_lengths(Container&& lengths,
		const TrackNo track_count)
{
	// Valid number of lengths ?

	if (lengths.size() != static_cast<std::size_t>(track_count))
	{
		auto ss = std::stringstream {};
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// If params make sense, use TOCValidator

	try {

		TOCValidator::validate_lengths(lengths);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Do not throw NonstandardMetadataException for now
		// since we accept non-standard metadata
	}

	// Convert ints to lba_count_t while normalizing the last length to 0

	std::vector<lba_count_t> uv(lengths.begin(), lengths.end());

	auto end_lengths { std::end(lengths) }; // avoid modifying temporaries
	if (*--end_lengths < 0) // normalize last length to 0
	{
		uv.back() = 0;
	}

	return uv;
}


lba_count_t TOCBuilder::build_leadout(const lba_count_t leadout)
{
	try {

		TOCValidator::validate_leadout(leadout);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Do not throw NonstandardMetadataException for now
		// since we accept non-standard metadata
	}

	return leadout;
}


template <typename Container, typename>
std::vector<std::string> TOCBuilder::build_files(Container&& files)
{
	// No validation for now, just convert to vector<string>

	auto filenames = std::vector<std::string>{};
	filenames.reserve(files.size());
    filenames.insert(std::end(filenames), std::begin(files), std::end(files));

	return filenames;
}

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif

