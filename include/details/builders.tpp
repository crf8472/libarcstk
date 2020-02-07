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


#include <cstdint>
#include <initializer_list>
#include <iterator>         // for begin, end
#include <memory>           // for make_unique, unique_ptr
#include <numeric>          // for accumulate
#include <sstream>
#include <stdexcept>        // for out_of_range
#include <string>
#include <type_traits>      // for enable_if, remove_reference_t
#include <utility>          // for move, forward
#include <vector>

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
 * \brief Uniform access to a container by track
 *
 * Uniform range-checked method to access a container by using the track number,
 * which is a 1-based index.
 *
 * The Container is required to yield its number of elements by member function
 * size() and to provide begin() const.
 *
 * \tparam Container Container type with \c size() and <tt>begin() const</tt>
 *
 * \param c Actual container
 * \param t Number of the track to access
 *
 * \return The value for track \c t in the container \c
 */
template <typename Container,
		typename = hasSize<Container>, typename = hasBegin<Container> >
inline decltype(auto) get_track(Container&& c, const TrackNo t)
{
	auto container_size = c.size();

	// Do the range check
	if (t < 1 or static_cast<decltype(container_size)>(t) > container_size)
	{
		std::stringstream message;
		message << "Track " << t << " is out of range (yields index "
			<< (t - 1) << " but max index is " << (container_size - 1) << ")";

		throw std::out_of_range(message.str());
	}

	auto it = std::begin(c);
	for (auto i = static_cast<int>(t); i > 1; --i, ++it);

	return *it;
}


/**
 * \brief Calculate leadout from lengths and offsets.
 *
 * No validation is performed except check for non-emptyness, equal size and
 * not exceeding CDDA.MAX_BLOCK_ADDRESS.
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
inline lba_count calculate_leadout(Container1&& lengths, Container2&& offsets)
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

	auto last_length { *--std::end(lengths) };
	auto leadout     { last_length ? *--std::end(offsets) + last_length : 0 };

	if (CDDA.MAX_BLOCK_ADDRESS <
			static_cast<decltype(CDDA.MAX_BLOCK_ADDRESS)>(leadout))
	{
		throw InvalidMetadataException(
			"Calculated leadout is bigger than maximal legal block address");
	}

	// We suppose std::numeric_limits<uint32_t>::max() > CDDA.MAX_BLOCK_ADDRESS
	return static_cast<lba_count>(leadout);
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
	 * \brief Implements TOC::track_count()
	 */
	inline TrackNo track_count() const;

	/**
	 * \brief Implements TOC::offset(const uint8_t) const
	 */
	inline uint32_t offset(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::parsed_length(const uint8_t) const
	 */
	inline uint32_t parsed_length(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::filename(const TrackNo idx) const
	 */
	inline std::string filename(const TrackNo idx) const;

	/**
	 * \brief Implements TOC::leadout()
	 */
	inline uint32_t leadout() const;

	/**
	 * \brief Implements TOC::complete()
	 */
	inline bool complete() const;

	/**
	 * \brief Implements TOC::operator==()
	 */
	inline bool equals(const TOC::Impl &rhs) const;


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
			const std::vector<uint32_t> &offsets,
			const uint32_t leadout,
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
			const std::vector<uint32_t> &offsets,
			const std::vector<uint32_t> &lengths,
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
	TrackNo track_count_;

	/**
	 * \brief Track offsets (in frames)
	 */
	std::vector<uint32_t> offsets_;

	/**
	 * \brief Track lengths (in frames)
	 */
	std::vector<uint32_t> lengths_;

	/**
	 * \brief Leadout frame
	 */
	uint32_t leadout_;

	/**
	 * \brief Audio file names
	 */
	std::vector<std::string> files_;
};

/// \cond UNDOC_FUNCTION_BODIES


TOC::Impl::Impl(const TOC::Impl &rhs) = default;


TOC::Impl::Impl(TOC::Impl &&rhs) noexcept = default;


TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<uint32_t> &offsets,
		const uint32_t leadout,
		const std::vector<std::string> &files)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_()
	, leadout_(leadout)
	, files_(files)
{
	// empty
}


TOC::Impl::Impl(const TrackNo track_count,
		const std::vector<uint32_t> &offsets,
		const std::vector<uint32_t> &lengths,
		const std::vector<std::string> &files)
	: track_count_(track_count)
	, offsets_(offsets)
	, lengths_(lengths)
	, leadout_(arcstk::details::calculate_leadout(lengths, offsets))
	, files_(files)
{
	// empty
}


bool TOC::Impl::has_track(const TrackNo track) const noexcept
{
	return track >= 1 and track <= this->track_count();
}


TrackNo TOC::Impl::track_count() const
{
	return track_count_;
}


uint32_t TOC::Impl::offset(const TrackNo track) const
{
	return details::get_track(offsets_, track);
}


uint32_t TOC::Impl::parsed_length(const TrackNo track) const
{
	try {

		return details::get_track(lengths_, track);
	} catch (const std::out_of_range &e)
	{
		// If track is valid, don't throw, even if lengths_ is empty
		if (has_track(track))
		{
			return 0;
		} else throw;
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
			return "";
		} else throw;
	}
}


uint32_t TOC::Impl::leadout() const
{
	return leadout_;
}


bool TOC::Impl::complete() const
{
	return leadout_ != 0;
}


bool TOC::Impl::equals(const TOC::Impl &rhs) const
{
	return this == &rhs
		or  (track_count_ == rhs.track_count_
		and  offsets_     == rhs.offsets_
		and  lengths_     == rhs.lengths_
		and  leadout_     == rhs.leadout_
		and  files_       == rhs.files_);
}

/// \endcond


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
			const uint32_t leadout,
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
			const uint32_t leadout);

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
	inline static void update(TOC &toc, const uint32_t leadout);


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
	inline static TrackNo build_track_count(const TrackNo track_count);

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
	inline static std::vector<uint32_t> build_offsets(Container&& offsets,
			const TrackNo track_count, const uint32_t leadout);

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
	inline static std::vector<uint32_t> build_offsets(
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
	inline static std::vector<uint32_t> build_lengths(Container&& lengths,
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
	inline static uint32_t build_leadout(const uint32_t leadout);

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

/// \cond UNDOC_FUNCTION_BODIES


// TOCBuilder


template <typename Container, typename Files, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		Container&& offsets,
		const uint32_t leadout,
		Files&& files)
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::forward<Container>(offsets), track_count, leadout),
		build_leadout(leadout),
		build_files(files))
	);

	return std::make_unique<TOC>(std::move(impl));
}


template <typename T, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		std::initializer_list<T> offsets,
		const uint32_t leadout)
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::vector<T>{offsets}, track_count, leadout),
		build_leadout(leadout),
		build_files(std::vector<std::string>{}))
	);

	return std::make_unique<TOC>(std::move(impl));
}


template <typename Container1, typename Container2, typename Files,
	typename, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		Container1&& offsets,
		Container2&& lengths,
		Files&& files)
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::forward<Container1>(offsets), track_count, lengths),
		build_lengths(std::forward<Container2>(lengths), track_count),
		build_files(files))
	);

	return std::make_unique<TOC>(std::move(impl));
}


template <typename T, typename Container, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		std::initializer_list<T> offsets,
		Container&& lengths)
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::vector<T>{offsets}, track_count,
			std::forward<Container>(lengths)),
		build_lengths(std::forward<Container>(lengths), track_count),
		{/* no filenames */})
	);

	return std::make_unique<TOC>(std::move(impl));
}


template <typename Container, typename T, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		Container&& offsets,
		std::initializer_list<T> lengths)
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::forward<Container>(offsets), track_count,
			std::vector<T>{lengths}),
		build_lengths(std::vector<T>{lengths}, track_count),
		{/* no filenames */})
	);

	return std::make_unique<TOC>(std::move(impl));
}


template <typename T1, typename T2, typename, typename>
std::unique_ptr<TOC> TOCBuilder::build(const TrackNo track_count,
		std::initializer_list<T1> offsets,
		std::initializer_list<T2> lengths)
{
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
		build_track_count(track_count),
		build_offsets(std::vector<T1>(offsets), track_count, lengths),
		build_lengths(std::vector<T2>(lengths), track_count),
		{/* no filenames */})
	);

	return std::make_unique<TOC>(std::move(impl));
}


void TOCBuilder::update(TOC &toc, const uint32_t leadout)
{
	TOCValidator::validate(toc, leadout);

	// FIXME Copying TOC::Impl manually is inefficient
	auto impl = std::make_unique<TOC::Impl>(TOC::Impl(
			toc.track_count(),
			toc::get_offsets(toc),
			toc::get_parsed_lengths(toc),
			toc::get_filenames(toc))
	);

	impl->leadout_ = leadout;

	toc.update(std::move(impl));
}


TrackNo TOCBuilder::build_track_count(const TrackNo track_count)
{
	TOCValidator::validate_trackcount(track_count);

	return track_count;
}


template <typename Container, typename>
std::vector<uint32_t> TOCBuilder::build_offsets(
		Container&& offsets,
		const TrackNo track_count,
		const uint32_t leadout)
{
	TOCValidator::validate(track_count, offsets, leadout);

	// Convert offsets to uints

	return std::vector<uint32_t>(offsets.begin(), offsets.end());
}


template <typename Container1, typename Container2, typename, typename>
std::vector<uint32_t> TOCBuilder::build_offsets(
		Container1&& offsets, const TrackNo track_count,
		Container2&& lengths)
{
	// Valid number of lengths ?

	if (offsets.size() != lengths.size())
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(offsets.size()) << " offsets";

		throw InvalidMetadataException(ss.str());
	}

	try {

		TOCValidator::validate_offsets(track_count, offsets);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Just swallow it for now
	}

	try {

		TOCValidator::validate_lengths(lengths);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Just swallow it for now
	}

	// Convert offsets to uints

	return std::vector<uint32_t>(offsets.begin(), offsets.end());
}


template <typename Container, typename>
std::vector<uint32_t> TOCBuilder::build_lengths(Container&& lengths,
		const TrackNo track_count)
{
	// Valid number of lengths ?

	if (lengths.size() != static_cast<std::size_t>(track_count))
	{
		std::stringstream ss;
		ss << "Cannot construct TOC with " << std::to_string(lengths.size())
			<< " lengths for " << std::to_string(track_count) << " tracks";

		throw InvalidMetadataException(ss.str());
	}

	// If params make sense, use TOCValidator

	try {

		TOCValidator::validate_lengths(lengths);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Just swallow it for now
	}

	// Convert ints to uints while normalizing the last length to 0

	std::vector<uint32_t> uv(lengths.begin(), lengths.end());

	if (*--std::end(lengths) < 0) // normalize last length to 0
	{
		uv.back() = 0;
	}

	return uv;
}


uint32_t TOCBuilder::build_leadout(const uint32_t leadout)
{
	try {

		TOCValidator::validate_leadout(leadout);

	} catch (const NonstandardMetadataException &nsm)
	{
		// Just swallow it for now
	}

	return leadout;
}


template <typename Container, typename>
std::vector<std::string> TOCBuilder::build_files(Container&& files)
{
	// No validation for now, just convert to vector<string>

	// FIXME This requires the container T to be assignable to std::string
	// but this is not checked by the template specification
	auto filenames = std::vector<std::string>(files.size());
    filenames.insert(std::end(filenames), std::begin(files), std::end(files));

	return filenames;
}

/// \endcond

} // namespace details

} // namespace v_1_0_0

} // namespace arcstk

#endif
